

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <assert.h>


#ifdef _WIN32_WINNT
 #include <ws2tcpip.h>
#else
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <sys/wait.h>
 #include <unistd.h>
 #include <fcntl.h>
#endif

#include "sinsocket.h"
#include "sinzutil.h"


bool sinsocket::done_init = false;
int sinsocket::socket_count = 0;


///////////////////////////////////////////////////////////////////////////////
packet_data_s::packet_data_s(void *indata, int insize)
        : data_size(insize), data_size_uncompressed(-1), current_loc(0), data((char*)indata) {}
packet_data_s::packet_data_s(const std::string &in_string)
        : data_size(in_string.size()), data_size_uncompressed(-1), current_loc(0) {
        data = (char*)malloc( in_string.size() );
        memcpy(data,in_string.c_str(),in_string.size()); }
packet_data_s::~packet_data_s() { assert(data!=NULL); free(data); }
int packet_data_s::size() { return data_size; }
void packet_data_s::getChunk(void *output, int insize) {
    if (insize==0) return;
    assert(current_loc+insize <= data_size);
    memcpy(output, data+current_loc, insize);
    current_loc += insize; }
void packet_data_s::setChunk(void *input, int insize) {
    if (insize==0) return;
    memcpy(data+data_size, input, insize);
    data_size += insize; }
void packet_data_s::setAll(void *input) {
    memcpy(data, input, data_size); }
void packet_data_s::compress() {
    data_size_uncompressed = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void sinbits::vector_to_bits( const std::vector<uint8_t> &in_vec, uint8_t **in_data, int &in_size ) {
    if ( in_vec.size() < 8 ) in_size = 1;
    else if ( in_vec.size() % 8 == 0 ) in_size = in_vec.size()/8;
    else in_size = in_vec.size()/8+1;
    (*in_data) = (uint8_t*) malloc(in_size);
    uint8_t bit_count = 0, byte_count = 0;
    (*in_data)[0] = 0;
    for ( uint32_t i=0; i < in_vec.size(); ++i,++bit_count ) {
        if ( bit_count == 8 ) { bit_count = 0; ++byte_count; (*in_data)[byte_count]=0; }
        if ( in_vec[i] ) (*in_data)[byte_count] += ( 1 << bit_count );
    }
}
void sinbits::bits_to_vector( const uint8_t *in_data, const int &in_size, std::vector<uint8_t> &in_vec ) {
    in_vec.resize( in_size * 8 );
    uint8_t bit_count=0, byte_count=0;
    for ( uint32_t i=0; i < in_vec.size(); ++i, ++bit_count ) {
        if ( bit_count == 8 ) { bit_count = 0; ++byte_count; }
        in_vec[i] = (in_data[byte_count] >> bit_count) & 1;
    }
}
void sinbits::vector_to_bits( const std::vector<uint8_t> &in_vec, packet_data &in_packet ) {
    sinbits::vector_to_bits( in_vec, (uint8_t**)&in_packet.data, in_packet.data_size ); }
void sinbits::bits_to_vector( const packet_data &in_packet, std::vector<uint8_t> &in_vec ) {
    sinbits::bits_to_vector( (uint8_t*)in_packet.data, in_packet.data_size, in_vec); }



///////////////////////////////////////////////////////////////////////////////
//helper to get the right address
void *get_in_addr(struct sockaddr_storage *sa) {
    if (sa->ss_family == AF_INET) //IPv4
      return &(((struct sockaddr_in*)sa)->sin_addr);
    else //IPv6
      return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#ifdef _WIN32_WINNT
///////////////////////////////////////////////////////////////////////////////
// windows helper functions
const char *inet_ntop(int af, const void *__restrict__ src, char *__restrict__ dst, socklen_t cnt)
{
        if (af == AF_INET)
        {
            struct sockaddr_in in;
            memset(&in, 0, sizeof(in));
            in.sin_family = AF_INET;
            memcpy(&in.sin_addr, src, sizeof(struct in_addr));
            getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
            return dst;
        }
        else if (af == AF_INET6)
        {
            struct sockaddr_in6 in;
            memset(&in, 0, sizeof(in));
            in.sin6_family = AF_INET6;
            memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
            getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
            return dst;
        }
        return NULL;
}
int inet_pton(int af, const char *src, void *__restrict__ dst)
{
        struct addrinfo hints, *res, *ressave;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = af;

        if (getaddrinfo(src, NULL, &hints, &res) != 0)
        {
            fprintf(stderr, "ERROR: inet_pton: Couldn't resolve host %s\n", src);
            return -1;
        }

        ressave = res;

        while (res)
        {
            memcpy(dst, res->ai_addr, res->ai_addrlen);
            res = res->ai_next;
        }

        freeaddrinfo(ressave);
        return 0;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
sinsocket::sinsocket(int in_fd) : ready_for_action(false),
                                  my_socket(in_fd),  user_data(NULL)
                                  #ifndef SINSOCKET_NO_THREADS
                                  , spawned_threads(false), stop_threads(false), socket_error(0)
                                  #endif
{

    if ( in_fd != -1 ) ready_for_action = true;

    if ( !done_init ) {
        #ifdef _DEBUG
        printf("sinsocket: init\n");
        #endif

        #ifdef _WIN32_WINNT
        WSADATA wsaData;   // if this doesn't work try WSAData
        // MAKEWORD(1,1) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0:
        if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
            fprintf(stderr, "sinsocket: WSAStartup failed.\n");
            WSACleanup();
            exit(1); }
        #endif

        done_init = true;
    }

    socket_count++;
}


///////////////////////////////////////////////////////////////////////////////
//
sinsocket::~sinsocket() {

    if ( ready_for_action ) closeSinsocket();

    //if this guy is the last socket, go ahead and uninit
    if ( socket_count == 1 && done_init ) {
        #ifdef _DEBUG
        printf("sinsocket: uninit\n");
        #endif

        #ifdef _WIN32_WINNT
        WSACleanup();
        #endif

        done_init = false;
    }

    socket_count--;

#ifndef SINSOCKET_NO_THREADS
    if ( spawned_threads ) {
        pthread_cancel(send_thread_id);
        pthread_join(send_thread_id, NULL);
        pthread_join(recv_thread_id, NULL);

        pthread_mutex_destroy(&send_mutex);
        pthread_mutex_destroy(&recv_mutex);
        pthread_mutex_destroy(&error_mutex);
        pthread_cond_destroy (&send_condition);
        pthread_cond_destroy (&recv_condition);
    }
#endif

    if ( user_data != NULL ) { free(user_data); }
}


///////////////////////////////////////////////////////////////////////////////

void sinsocket::setUserData(void *in) { user_data = in; }
void *sinsocket::getUserData() { return user_data; }

///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::listen(const int &inport, const int &hint) {

    struct addrinfo hints, *servinfo, *p;
    #ifdef _WIN32_WINNT
    char yes=1;
    #else
    int yes=1;
    #endif
    int return_value;

    sprintf(my_port,"%d",inport);


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((return_value = getaddrinfo(NULL, my_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "ERROR: sinsocket.listen: getaddrinfo: %s\n", gai_strerror(return_value));
        return 1;
    }

    //first skip a few of the interfaces, as per the 'hint'
    p = servinfo; for (int i=0; i<hint; ++i) p = p->ai_next;

    // loop through all the results and bind to the first we can
    for(; p != NULL; p = p->ai_next) {
        if ((my_socket = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("ERROR: sinsocket.listen: socket");
            continue;
        }

        if (setsockopt(my_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            perror("ERROR: sinsocket.listen: setsockopt");
            continue;
        }

        if (bind(my_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(my_socket);
            perror("ERROR: sinsocket.listen: bind");
            continue;
        }

/*
        char connection_name[100];
        inet_ntop(p->ai_family,
                  get_in_addr((struct sockaddr_storage *)p->ai_addr),
                  connection_name,
                  sizeof connection_name);
        printf("Interface: %s\n", connection_name);
*/

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "ERROR: sinsocket.listen: failed to bind\n");
        return 1;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (::listen(my_socket, 10) == -1) { //this value is the 'backlog'
        perror("ERROR: sinsocket.listen: listen");
        return 1;
    }

    #ifndef _WIN32_WINNT
      //force socket to close on exec
      fcntl(my_socket, F_SETFD, FD_CLOEXEC);
    #endif

    ready_for_action = true;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
sinsocket* sinsocket::accept(char *store_ip) {

    int new_fd;  // listen on sock_fd, new connection on new_fd
    struct sockaddr_storage their_addr; // connector's address information
    char connection_name[INET6_ADDRSTRLEN];
    socklen_t sin_size;

    #ifdef _DEBUG
    printf("sinsocket.accept: waiting for connections...\n");
    #endif

    sin_size = sizeof their_addr;
    new_fd = ::accept(my_socket, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        perror("accept");
        return NULL; }

    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr_storage *)&their_addr),
              connection_name,
              sizeof connection_name);

    if ( store_ip != NULL ) {
        strcpy(store_ip, connection_name);
    }

    #ifdef _DEBUG
    printf("sinsocket.accept: got connection from %s\n", connection_name);
    //int optlen = sizeof(int);
    //int optval;
    //getsockopt(new_fd, SOL_SOCKET, SO_MAX_MSG_SIZE, (char *)&optval, &optlen);
    //printf("sinsocket.accept: maximum packet size: %d\n", optval);
    #endif

    return new sinsocket(new_fd);

}



///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::sendRaw( const void *indata, const int inlength ) {

    if ( !ready_for_action ) {
        fprintf(stderr, "ERROR: sinsocket.sendRaw: socket not ready to send\n");
        return SINSOCKET_ERROR_NOT_READY; }

    int bytes_sent = 0;
    int bytes_left = inlength;
    int temp_sent = 0;

    while ( bytes_sent < inlength ) {
        temp_sent = ::send(my_socket, (char*)indata+bytes_sent, bytes_left, 0);
        if ( temp_sent == -1 ) break; //something bad happened
        bytes_sent += temp_sent;
        bytes_left -= temp_sent;
    }

    if ( temp_sent == -1 ) {
        perror("ERROR: sinsocket.sendRaw");
        #ifdef _WIN32_WINNT
        printf("WSAError: %d\n", WSAGetLastError());
        #endif
        closeSinsocket();
        return SINSOCKET_ERROR_UNRECOVERABLE;
    }

    return SINSOCKET_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::sendPacket( packet_data &inpacket, bool compressed ) {
    return sendPacket( &inpacket, compressed);
}
///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::sendPacket( packet_data *inpacket, bool compressed ) {

    if ( !ready_for_action ) {
        fprintf(stderr, "ERROR: sinsocket.sendPacket: socket not ready to send\n");
        return SINSOCKET_ERROR_NOT_READY; }

    if ( inpacket->data_size_uncompressed == 0 ) compressed = true;

    int result = SINSOCKET_OK;

    if ( compressed ) {
        //we need to compress it before sending
        inpacket->data_size_uncompressed = inpacket->data_size;
        char *old_data = inpacket->data;
        int length_compressed = inpacket->data_size+6;
        char *deflated_data = (char*)malloc( length_compressed );
        if ( (result = sinz::zDef(inpacket->data, inpacket->data_size, deflated_data, length_compressed)) )
            { fprintf(stderr, "ERROR: sinsocket.sendPacket: deflate error (%d)\n", result);
              result = SINSOCKET_ERROR_ZLIB; }
        inpacket->data_size = length_compressed;
        inpacket->data = deflated_data;
        free(old_data);
        //send the actual size, uncompressed size, then the data
        if ( !result ) result = sendRaw(&inpacket->data_size, 4);
        if ( !result ) result = sendRaw(&inpacket->data_size_uncompressed, 4);
        if ( !result ) result = sendRaw(inpacket->data, inpacket->data_size);
    } else {
        //just send the actual size, then data
        if ( !result ) result = sendRaw(&inpacket->data_size, 4);
        if ( !result ) result = sendRaw(inpacket->data, inpacket->data_size);
        //printf("[%d:%s]\n", inpacket->data_size, inpacket->data);
    }

    if ( result != 0 ) {
        fprintf(stderr,"ERROR: sinsocket.sendPacket (%d)\n", result);
        return result;
    }

    return SINSOCKET_OK;
}


///////////////////////////////////////////////////////////////////////////////
// returns -1 if the peer disconnected, -2 for error, 0 otherwise
int sinsocket::recvRaw( const void *indata, const int inlength ) {

    if ( !ready_for_action ) {
        fprintf(stderr, "ERROR: sinsocket.recvRaw: socket not ready to receive\n");
        return SINSOCKET_ERROR_NOT_READY; }

    int bytes_left = inlength;
    int temp_recv = 0;

    while ( bytes_left ) {
        temp_recv = ::recv(my_socket, (char*)indata+(inlength-bytes_left), bytes_left, 0 ); //MSG_WAITALL = 0x8
        if ( temp_recv == -1 || temp_recv == 0 ) break; //something bad happened or disconnect
        bytes_left -= temp_recv;
    }

    //error
    if ( temp_recv == -1 ) {
        perror("ERROR: sinsocket.recvRaw");
        #ifdef _WIN32_WINNT
        printf("WSAError: %d\n", WSAGetLastError());
        #endif
        closeSinsocket();
        return SINSOCKET_ERROR_UNRECOVERABLE;
    }

    //peer disconnected
    if ( temp_recv == 0 ) {
        #ifdef _DEBUG
        printf("INFO: sinsocket.recvRaw: peer has gracefully disconnected\n");
        #endif
        return SINSOCKET_ERROR_DISCONNECT;
    }

    //everything seems fine
    return SINSOCKET_OK;
}


///////////////////////////////////////////////////////////////////////////////
// returns -1 if the peer disconnected, -2 for error, 0 otherwise
int sinsocket::recvPacket( packet_data* &outpacket, bool compressed ) {
    outpacket = new packet_data();
    return ( recvPacket( *outpacket, compressed ) );
}
///////////////////////////////////////////////////////////////////////////////
// returns -1 if the peer disconnected, -2 for error, 0 otherwise
int sinsocket::recvPacket( packet_data &outpacket, bool compressed ) {

    if ( !ready_for_action ) {
        fprintf(stderr, "ERROR: sinsocket.recvPacket: socket not ready to receive\n");
        return SINSOCKET_ERROR_NOT_READY; }

    if ( outpacket.data != NULL ) { free( outpacket.data ); }

    int result = SINSOCKET_OK;

    if ( compressed ) {
        //recv the actual size, uncompressed size, then the data
        if ( !result ) result = recvRaw(&outpacket.data_size, 4);
        if ( !result ) result = recvRaw(&outpacket.data_size_uncompressed, 4);
        //now uncompress the data
        char *compressed_data = NULL;
        if ( !result ) {
            compressed_data = (char*)malloc(outpacket.data_size);
            outpacket.data = (char*)malloc(outpacket.data_size_uncompressed);
        }
        if ( !result ) result = recvRaw(compressed_data, outpacket.data_size);
        if ( !result )
            if ( (result = sinz::zInf(compressed_data, outpacket.data_size, outpacket.data, outpacket.data_size_uncompressed)) )
                { fprintf(stderr, "ERROR: zInf in recvPacket: %d\n", result);
                  free( compressed_data );
                  return SINSOCKET_ERROR_ZLIB; }
        outpacket.data_size = outpacket.data_size_uncompressed;
        if ( compressed_data != NULL ) free(compressed_data);
    } else {
        //just recv the actual size, then data
        if ( !result ) result = recvRaw(&outpacket.data_size, 4);
        if ( !result ) outpacket.data = (char*)malloc(outpacket.data_size);
        if ( !result ) result = recvRaw(outpacket.data, outpacket.data_size);
        //printf("[%d:%s]\n", outpacket.data_size, outpacket.data);
    }

    if ( result != 0 ) {
        fprintf(stderr, "ERROR: sinsocket.recvPacket: %d\n", result);
        return result;
    }

    //everything seems fine
    return SINSOCKET_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
void sinsocket::closeSinsocket() {
    close(my_socket);
    ready_for_action = false;
}


///////////////////////////////////////////////////////////////////////////////
// returns 0 on success, 1 on error
//
int sinsocket::beginDisconnect() {

    if ( !ready_for_action ) {
        fprintf(stderr, "ERROR: sinsocket.beginDisconnect: socket not ready\n");
        return SINSOCKET_ERROR_NOT_READY; }

    if ( spawned_threads ) {
        //wait till done sending, then cancel thread
        pthread_mutex_lock(&send_mutex);
        stop_threads = true;
        //signal the send thread so it can die
        pthread_cond_signal(&send_condition);
        pthread_mutex_unlock(&send_mutex);
        pthread_join(send_thread_id, NULL);
    }

    //tell the client we are done
    #ifdef _WIN32_WINNT
    shutdown(my_socket, SD_SEND);
    #else
    shutdown(my_socket, SHUT_WR);
    #endif

    if ( spawned_threads ) {
        pthread_join(recv_thread_id, NULL);
        return checkErrors();
    }

    //wait for recv to return a 0 or -1, meaning client has disconnected
    int temp_recv;
    do { temp_recv = ::recv(my_socket,NULL,0,0);
    } while ( temp_recv != 0 && temp_recv != -1 );

    if ( temp_recv == -1 )
        fprintf(stderr, "ERROR: sinsocket.beginDisconnect: recv returned -1\n");

    closeSinsocket();
    return temp_recv;
}

///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::endDisconnect() {

    if ( !ready_for_action ) {
        fprintf(stderr, "ERROR: sinsocket.endDisconnect: socket not ready\n");
        return SINSOCKET_ERROR_NOT_READY; }

    int temp_recv = 0;
    if ( !spawned_threads ) {
        //wait for recv to return a 0 or -1, meaning client has disconnected
        do { temp_recv = ::recv(my_socket,NULL,0,0);
        } while ( temp_recv != 0 && temp_recv != -1 );
    }

    if ( temp_recv == 0 ) {
        //tell the client we are done
        #ifdef _WIN32_WINNT
        shutdown(my_socket, SD_SEND);
        #else
        shutdown(my_socket, SHUT_WR);
        #endif

        do { temp_recv = ::recv(my_socket,NULL,0,0);
        } while ( temp_recv != -1 && temp_recv != -0);
        //other side is down, let us end as well
        closeSinsocket();
        return 0;
    }

    //ah well, close and move on
    fprintf(stderr, "ERROR: sinsocket.endDisconnect: recv returned -1\n");
    closeSinsocket();
    return -1;
}


///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::connect(const char* inaddress, const int &inport ) {

    struct addrinfo hints, *servinfo, *p;
    int return_value;
    char connection_name[INET6_ADDRSTRLEN];

    sprintf(my_port,"%d",inport);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((return_value = getaddrinfo(inaddress, my_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "ERROR: sinsocket.connect: getaddrinfo: %s\n", gai_strerror(return_value));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((my_socket = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("ERROR: sinsocket.connect: socket");
            continue;
        }

        if (::connect(my_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(my_socket);
            perror("ERROR: sinsocket.connect: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "ERROR: sinsocket.client: failed to connect to %s\n", inaddress);
        return 1;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr_storage*)p->ai_addr),
            connection_name, sizeof connection_name);

    #ifdef _DEBUG
    printf("sinsocket.connect: connected to %s\n", connection_name);
    #endif

    freeaddrinfo(servinfo); // all done with this structure

    ready_for_action = true;

    #ifndef _WIN32_WINNT
      //force socket to close on exec
      fcntl(my_socket, F_SETFD, FD_CLOEXEC);
    #endif

    return 0;
}



#ifndef SINSOCKET_NO_THREADS

///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::spawnThreads() {

    if ( spawned_threads ) return -1;

    pthread_mutex_init(&send_mutex, NULL);
    pthread_mutex_init(&recv_mutex, NULL);
    pthread_mutex_init(&error_mutex, NULL);
    pthread_cond_init (&send_condition, NULL);
    pthread_cond_init (&recv_condition, NULL);

    int result;
    if ( (result = pthread_create(&recv_thread_id, NULL, sinRecvThread, this)) ||
         (result = pthread_create(&send_thread_id, NULL, sinSendThread, this)) ) {
        pthread_mutex_destroy(&send_mutex);
        pthread_mutex_destroy(&recv_mutex);
        pthread_mutex_destroy(&error_mutex);
        pthread_cond_destroy (&send_condition);
        pthread_cond_destroy (&recv_condition);
        return result;
    }

    spawned_threads = true;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
void *sinsocket::sinRecvThread(void *inself) {
    sinsocket *myself = (sinsocket *)inself;
    int result;
    packet_data *current_packet;
    uint8_t compression_check;

    while (true) {

        //peek to see if we're compressed
        if ( (result = myself->recvRaw(&compression_check, 1)) ) break;

        if ( compression_check ) {
            if ( (result = myself->recvPacket( current_packet, true )) ) break;
        } else {
            if ( (result = myself->recvPacket( current_packet, false )) ) break;
        }

        pthread_mutex_lock(&myself->recv_mutex);
            //printf("pushing size %d\n", current_packet->data_size);
            myself->received_data.push(current_packet);
            //printf("sinRecvThread: pushing a packet of size %d\n", current_packet->size());
            pthread_cond_signal(&myself->recv_condition);
        pthread_mutex_unlock(&myself->recv_mutex);

    }

    if ( result == SINSOCKET_ERROR_DISCONNECT ) {
        //finish the disconnect
        #ifdef _DEBUG
          printf("INFO: sinsocket.recvThread: disconnecting\n");
        #endif
        if ( !myself->stop_threads ) myself->endDisconnect();
    } else {
        #ifdef _DEBUG
        fprintf(stderr, "ERROR: sinsocket.recvThread: stopping with result %d\n", result);
        #endif
    }

    pthread_mutex_lock(&myself->error_mutex);
      myself->socket_error = result;
    pthread_mutex_unlock(&myself->error_mutex);

    #ifdef PTW32_STATIC_LIB
    pthread_win32_thread_detach_np();
    #endif

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
void *sinsocket::sinSendThread(void *inself) {
    sinsocket *myself = (sinsocket *)inself;
    packet_data *current_packet;
    uint8_t compressed = 1;
    uint8_t not_compressed = 0;
    int result;

    while (true) {

        pthread_mutex_lock(&myself->send_mutex);
            if ( !myself->sending_data.size() && !myself->stop_threads ) {
                pthread_cond_wait(&myself->send_condition, &myself->send_mutex);
            }
            //exit if we need to, only if queue is empty
            if ( myself->stop_threads && !myself->sending_data.size()) { pthread_mutex_unlock(&myself->send_mutex); break; }
            current_packet = myself->sending_data.front();
            myself->sending_data.pop();
            //printf("sinSendThread: sending a packet of size %d\n", current_packet->size());
        pthread_mutex_unlock(&myself->send_mutex);

        if ( current_packet->data_size_uncompressed == 0 ) {
            //compressed packet
            if ( (result = myself->sendRaw(&compressed, 1)) ) break;
            if ( (result = myself->sendPacket(current_packet, true)) ) break;
        } else {
            //not a compressed packet
            if ( (result = myself->sendRaw(&not_compressed, 1)) ) break;
            if ( (result = myself->sendPacket(current_packet, false)) ) break;
        }

        delete current_packet;
    }

    if ( myself->stop_threads ) {
        #ifdef _DEBUG
          printf("INFO: sinsocket.sendThread: told to stop\n");
        #endif
    } else {
        //if we're here, something has gone wrong
        #ifdef _DEBUG
          printf("INFO: sinsocket.sendThread: stopping with result %d\n", result);
        #endif
    }

    #ifdef PTW32_STATIC_LIB
    pthread_win32_thread_detach_np();
    #endif

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
void sinsocket::asyncSend(const void *indata, const int inlength) {
    asyncSend(new packet_data((char*)indata, inlength));
}
//
void sinsocket::asyncSend(packet_data *inpacket) {

    pthread_mutex_lock(&send_mutex);
        //printf("asyncSend: pushing packet size %d\n", inpacket->data_size);
        sending_data.push(inpacket);
        //printf("asyncSend: pushing a packet of size %d\n", inpacket->size());
        pthread_cond_signal(&send_condition);
    pthread_mutex_unlock(&send_mutex);

}


///////////////////////////////////////////////////////////////////////////////
//
packet_data *sinsocket::asyncRecv() {

    packet_data *current_packet = NULL;

    pthread_mutex_lock(&recv_mutex);
        if ( received_data.size() ) {
            current_packet = received_data.front();
            //printf("asyncRecv: popped a packet of size %d\n", current_packet->size());
            received_data.pop();
        }
    pthread_mutex_unlock(&recv_mutex);

    return current_packet;
}


///////////////////////////////////////////////////////////////////////////////
//
packet_data *sinsocket::asyncRecvWait() {

    packet_data *current_packet;

    pthread_mutex_lock(&recv_mutex);
        if ( !received_data.size() )
            pthread_cond_wait(&recv_condition, &recv_mutex);

        current_packet = received_data.front();
        //printf("asyncRecvWait: popped a packet of size %d\n", current_packet->size());
        received_data.pop();
    pthread_mutex_unlock(&recv_mutex);

    //printf("asyncRecvWait: got packet size %d\n", current_packet->data_size);
    return current_packet;
}


///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::checkErrors() {
    int check_errors;

    pthread_mutex_lock(&error_mutex);
        check_errors = socket_error;
    pthread_mutex_unlock(&error_mutex);

    return check_errors;
}




//////////
// yay stats stuff

void sinsocket::setCalculatePercentage(const bool in) {}
void sinsocket::setCalculateSpeed(const bool in) {}
int sinsocket::getTransferTotalBytes() {}
int sinsocket::getTransferBytes() {}
float sinsocket::getTransferPercent() {}
int sinsocket::getAverageBps() {}
int sinsocket::getPreviousBps() {}










#endif
