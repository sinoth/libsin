

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
#endif

#include "sinsocket.h"
#include "sinzutil.h"


bool sinsocket::done_init = false;
int sinsocket::socket_count = 0;


///////////////////////////////////////////////////////////////////////////////
packet_data_s::packet_data_s(void *indata, int insize)
        : data_size(insize), data_size_compressed(-1), current_loc(0), data((char*)indata) {}
packet_data_s::packet_data_s(const std::string &in_string)
        : data_size(in_string.size()), data_size_compressed(-1), current_loc(0) {
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
    data_size_compressed = 0;
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
                                  , spawned_threads(false), socket_error(0)
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
        pthread_cancel(recv_thread_id);
        void *status;
        pthread_join(send_thread_id, &status);
        pthread_join(recv_thread_id, &status);
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
int sinsocket::send( const void *indata, const int inlength ) {
    int bytes_sent = 0;
    int bytes_left = inlength;
    int temp_sent = 0;

    if ( !ready_for_action ) {
        fprintf(stderr, "ERROR: sinsocket.send: socket not ready to send\n");
        return -1; }

    while ( bytes_sent < inlength ) {
        temp_sent = ::send(my_socket, (char*)indata+bytes_sent, bytes_left, 0);
        if ( temp_sent == -1 ) break; //something bad happened
        bytes_sent += temp_sent;
        bytes_left -= temp_sent;
    }

    if ( temp_sent == -1 ) { perror("ERROR: sinsocket.send"); }

    //return -1 if something messed up, 0 otherwise
    return ( temp_sent==-1?-1:0 );
}


///////////////////////////////////////////////////////////////////////////////
// returns -1 if the peer disconnected, -2 for error, bytes received otherwise
int sinsocket::recv( const void *indata, const int inlength ) {

    //make sure we have a valid connection
    if ( !ready_for_action ) {
        fprintf(stderr, "ERROR: sinsocket.recv: socket not ready to receive\n");
        return -2; }

    int bytes_left = inlength;
    int temp_recv = 0;

    while ( bytes_left ) {
        //printf("SINSOCKET.RECV: bytes_in_buffer: %d, inlength: %d, bytes_left: %d\n", bytes_in_buffer, inlength, bytes_left);
        temp_recv = ::recv(my_socket, (char*)indata+(inlength-bytes_left), bytes_left, 0 ); //MSG_WAITALL = 0x8
        if ( temp_recv == -1 || temp_recv == 0 ) break; //something bad happened or disconnect
        bytes_left -= temp_recv;
    }

    //error
    if ( temp_recv == -1 ) {
        #ifdef _WIN32_WINNT
        printf("WSAError: %d\n", WSAGetLastError());
        #endif
        perror("ERROR: sinsocket.recv");
        return -2;
    }

    //peer disconnected
    if ( temp_recv == 0 ) {
        if (!spawned_threads) fprintf(stderr, "INFO: sinsocket.recv: peer has gracefully disconnected\n");
        ready_for_action = false;
        return -1; }

    //everything seems fine
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::closeSinsocket() {
    close(my_socket);
    ready_for_action = false;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// returns 0 on success, 1 on error
//
int sinsocket::beginDisconnect() {
    //first, see if we've got threads spawned
    if ( spawned_threads ) {
        //if so, wait till they are done sending, then clean em up
        pthread_mutex_lock(&send_mutex);
        if ( sending_data.size() )
            pthread_cond_wait(&empty_condition, &send_mutex);
        //should mean we're in the clear now
        pthread_mutex_unlock(&send_mutex);
    }

    //tell the client we are done
    #ifdef _WIN32_WINNT
    shutdown(my_socket, SD_SEND);
    #else
    shutdown(my_socket, SHUT_WR);
    #endif


    //wait for recv to return a 0, meaning client has disconnected
    int temp_recv = ::recv(my_socket,NULL,0,0);

    if ( temp_recv == 0 ) {
        //we're done here
        //closeSinsocket();
        return 0;
    } else if ( temp_recv == -1 ) {
        //ah well, report it and close anyway
        fprintf(stderr, "ERROR: sinsocket.beginDisconnect: recv returned -1\n");
        //closeSinsocket();
        return 1;
    } else {
        //who knows! lets just report and move on..
        fprintf(stderr, "ERROR: sinsocket.beginDisconnect: recv returned %d\n", temp_recv);
        //closeSinsocket();
        return 1;
    }

    //shouldn't be here..
    return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::endDisconnect() {
    //wait for recv to return a 0, meaning client has disconnected
    int temp_recv = ::recv(my_socket,NULL,0,0);

    if ( temp_recv == 0 ) {
        //tell the client we are done
        #ifdef _WIN32_WINNT
        shutdown(my_socket, SD_SEND);
        #else
        shutdown(my_socket, SHUT_WR);
        #endif

        temp_recv = ::recv(my_socket,NULL,0,0);
        if ( temp_recv == -1 ) {
            //other side is down, let us end as well
            //closeSinsocket();
            return 0;
        }
    } else if ( temp_recv == -1 ) {
        //ah well, close and move on
        fprintf(stderr, "ERROR: sinsocket.endDisconnect: recv returned -1\n");
        //closeSinsocket();
        return 1;
    } else {
        //wtf, who knows!
        fprintf(stderr, "ERROR: sinsocket.endDisconnect: recv returned %d\n",temp_recv);
        //closeSinsocket();
        return 1;
    }

    //shouldn't be here..
    return 1;
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

    return 0;
}



#ifndef SINSOCKET_NO_THREADS

///////////////////////////////////////////////////////////////////////////////
//
void sinsocket::spawnThreads() {

    if ( spawned_threads ) return;

    pthread_mutex_init(&send_mutex, NULL);
    pthread_mutex_init(&recv_mutex, NULL);
    pthread_mutex_init(&error_mutex, NULL);
    pthread_cond_init (&send_condition, NULL);
    pthread_cond_init (&recv_condition, NULL);
    pthread_cond_init (&empty_condition, NULL);

    socket_error = pthread_create(&recv_thread_id, NULL, sinRecvThread, this);
    socket_error = pthread_create(&send_thread_id, NULL, sinSendThread, this);

    spawned_threads = true;

}


///////////////////////////////////////////////////////////////////////////////
//
void *sinsocket::sinRecvThread(void *inself) {
    sinsocket *myself = (sinsocket *)inself;
    int rc;
    int packet_size;
    int packet_compressed_size;
    char *compressed_data;
    packet_data *current_packet;
    uint8_t compression_check;

    while (true) {

        //peek to see if we're compressed.  if so, decomress it
        rc = myself->recv(&compression_check, 1); if ( rc != 0 ) break;

        if ( compression_check ) {
            //decompress this sucker
            rc = myself->recv(&packet_compressed_size, 4); if ( rc != 0 ) break;
            rc = myself->recv(&packet_size, 4); if ( rc != 0 ) break;
            printf("INFO: got compressed packet, size: %d, actual: %d\n", packet_size, packet_compressed_size);
            compressed_data = (char*)malloc(packet_compressed_size);
            current_packet = new packet_data(malloc(packet_size), packet_size);
            rc = myself->recv(compressed_data, packet_compressed_size); if ( rc != 0 ) break;
            rc = sinz::zInf(compressed_data, packet_compressed_size, current_packet->data, packet_size);
            if ( rc ) { printf("* ERROR: zInf in sinRecvThread: %d\n", rc); /*myself->closeSinsocket();*/ break; }
            free(compressed_data);
        } else {
            //not compressed
            rc = myself->recv(&packet_size, 4); if ( rc != 0 ) break;
            //printf("asyncRecvThread: got packet size %d ... ", packet_size);
            current_packet = new packet_data(malloc(packet_size), packet_size);
            rc = myself->recv(current_packet->data, packet_size); if ( rc != 0 ) break;
        }

        pthread_mutex_lock(&myself->recv_mutex);
            //printf("pushing size %d\n", current_packet->data_size);
            myself->received_data.push(current_packet);
            //printf("sinRecvThread: pushing a packet of size %d\n", current_packet->size());
            pthread_cond_signal(&myself->recv_condition);
        pthread_mutex_unlock(&myself->recv_mutex);

    }

    //if we're here, something has gone wrong

    //error
    if ( rc == -2 ) {
        #ifdef _WIN32_WINNT
        printf("WSAError: %d\n", WSAGetLastError());
        #endif
        perror("ERROR: sinsocket.recv");
    }

    //peer disconnected
    if ( rc == -1 ) {
        fprintf(stderr, "INFO: sinsocket.recvThread: peer gracefully disconnected\n");
        myself->endDisconnect();
     }

    pthread_mutex_lock(&myself->error_mutex);
    myself->socket_error = 1;
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

    while (true) {

        pthread_mutex_lock(&myself->send_mutex);
        if ( !myself->sending_data.size() ) {
            pthread_cond_signal(&myself->empty_condition);
            pthread_cond_wait(&myself->send_condition, &myself->send_mutex);
        }

        current_packet = myself->sending_data.front();
        myself->sending_data.pop();
        //printf("sinSendThread: sending a packet of size %d\n", current_packet->size());
        pthread_mutex_unlock(&myself->send_mutex);

        //printf("asyncSendThread of size %d\n", current_packet->data_size);

        if ( current_packet->data_size_compressed == 0 ) {
            //we need to compress it before sending
            char *old_data = current_packet->data;
            current_packet->data_size_compressed = current_packet->data_size+6;
            char *deflated_data = (char*)malloc( current_packet->data_size_compressed );
            sinz::zDef(current_packet->data, current_packet->data_size, deflated_data, current_packet->data_size_compressed);
            current_packet->data = deflated_data;
            free(old_data);
            //now send a 1 indicating we are compressed
            if ( myself->send(&compressed, 1) ) break;
            //send the uncompressed size, followed by the compressed size
            if ( myself->send(&current_packet->data_size_compressed, 4) ) break;
            if ( myself->send(&current_packet->data_size, 4) ) break;
            if ( myself->send(current_packet->data, current_packet->data_size_compressed) ) break;
        } else {
            //not a compressed packet
            if ( myself->send(&not_compressed, 1) ) break;
            //just send the normal size
            if ( myself->send(&current_packet->data_size, 4) ) break;
            if ( myself->send(current_packet->data, current_packet->data_size) ) break;
        }

        delete current_packet;
    }

    //if we're here, something has gone wrong
    pthread_mutex_lock(&myself->error_mutex);
    myself->socket_error = 2;
    pthread_mutex_unlock(&myself->error_mutex);

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

#endif
