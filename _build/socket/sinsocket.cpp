

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

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


bool sinsocket::done_init = false;
int sinsocket::socket_count = 0;

packet_data_s::packet_data_s(void *indata, const int &insize) : data_size(insize), data((char*)indata) {}
packet_data_s::~packet_data_s() { free(data); }
int packet_data_s::size() { return data_size; }
void packet_data_s::getChunk(void *output, const int &insize) {
    static int current_loc = 0;
    memcpy(output, data, insize);
    current_loc += insize; }
void packet_data_s::setChunk(void *input, const int &insize) {
    memcpy(data+data_size, input, insize);
    data_size += insize; }

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
sinsocket::sinsocket(int in_fd) : ready_for_action(false), spawned_threads(false),
                                  my_socket(in_fd), socket_error(0), user_data(NULL) {

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

    if ( spawned_threads ) {
        pthread_cancel(send_thread_id);
        pthread_cancel(recv_thread_id);
        void *status;
        pthread_join(send_thread_id, &status);
        pthread_join(recv_thread_id, &status);
    }

    if ( user_data != NULL ) { free(user_data); }
}


///////////////////////////////////////////////////////////////////////////////
//
int sinsocket::listen(const int &inport) {

    struct addrinfo hints, *servinfo, *p;
    char yes=1;
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

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((my_socket = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("ERROR: sinsocket.listen: socket");
            continue;
        }

        if (setsockopt(my_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(char)) == -1) {
            perror("ERROR: sinsocket.listen: setsockopt");
            continue;
        }

        if (bind(my_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(my_socket);
            perror("ERROR: sinsocket.listen: bind");
            continue;
        }

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
sinsocket* sinsocket::accept() {

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
int sinsocket::send( const void *indata, const int &inlength ) {
    int bytes_sent = 0;
    int bytes_left = inlength;
    int temp_sent = 0;

    while ( bytes_sent < inlength ) {
        temp_sent = ::send(my_socket, (char*)indata+bytes_sent, bytes_left, 0);
        if ( temp_sent == -1 ) break; //something bad happened
        bytes_sent += temp_sent;
        bytes_left -= temp_sent;
    }

    if ( temp_sent == -1 ) { perror("ERROR: sinsocket.send"); }

    //return 1 if something messed up, 0 otherwise
    return ( temp_sent==-1?1:0 );
}


///////////////////////////////////////////////////////////////////////////////
// returns -1 if the peer disconnected, -2 for error, bytes received otherwise
int sinsocket::recv( const void *indata, const int &inlength ) {

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
        printf("WSAError: %d\n", WSAGetLastError());
        perror("ERROR: sinsocket.recv");
        return -2; }

    //peer disconnected
    if ( temp_recv == 0 ) {
        fprintf(stderr, "ERROR: sinsocket.recv: peer has disconnected\n");
        ready_for_action = false;
        return -1; }

    //everything seems fine
    return inlength;
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
    //tell the client we are done
    shutdown(my_socket, SD_SEND);

    //wait for recv to return a 0, meaning client has disconnected
    int temp_recv = ::recv(my_socket,NULL,0,0);

    if ( temp_recv == 0 ) {
        //we're done here
        closeSinsocket();
        return 0;
    } else if ( temp_recv == -1 ) {
        //ah well, report it and close anyway
        fprintf(stderr, "ERROR: sinsocket.beginDisconnect: recv returned -1\n");
        closeSinsocket();
        return 1;
    } else {
        //who knows! lets just report and move on..
        fprintf(stderr, "ERROR: sinsocket.beginDisconnect: recv returned %d\n", temp_recv);
        closeSinsocket();
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
        shutdown(my_socket, SD_SEND);

        temp_recv = ::recv(my_socket,NULL,0,0);
        if ( temp_recv == -1 ) {
            //other side is down, let us end as well
            closeSinsocket();
            return 0;
        }
    } else if ( temp_recv == -1 ) {
        //ah well, close and move on
        fprintf(stderr, "ERROR: sinsocket.endDisconnect: recv returned -1\n");
        closeSinsocket();
        return 1;
    } else {
        //wtf, who knows!
        fprintf(stderr, "ERROR: sinsocket.endDisconnect: recv returned %d\n",temp_recv);
        closeSinsocket();
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



///////////////////////////////////////////////////////////////////////////////
//
void sinsocket::spawnThreads() {

    if ( spawned_threads ) return;

    pthread_mutex_init(&send_mutex, NULL);
    pthread_mutex_init(&recv_mutex, NULL);
    pthread_mutex_init(&error_mutex, NULL);
    pthread_cond_init (&send_condition, NULL);
    pthread_cond_init (&recv_condition, NULL);

    socket_error = pthread_create(&recv_thread_id, NULL, sinRecvThread, this);
    socket_error = pthread_create(&send_thread_id, NULL, sinSendThread, this);

    spawned_threads = true;

}


///////////////////////////////////////////////////////////////////////////////
//
void *sinsocket::sinRecvThread(void *inself) {
    sinsocket *myself = (sinsocket *)inself;
    int amount_received;
    int packet_size;
    int bytes_left;
    packet_data *current_packet;
    char *current_data;

    while (true) {

        amount_received = ::recv(myself->my_socket, (char*)&packet_size, 4, 0 ); //MSG_WAITALL = 0x8
        if ( amount_received == 0 || amount_received == -1 ) break;

        current_data = (char*)malloc(packet_size);
        current_packet = new packet_data(current_data, packet_size);

        bytes_left = packet_size;
        while ( bytes_left ) {
            amount_received = ::recv(myself->my_socket, current_data+(packet_size-bytes_left), bytes_left, 0 );
            if ( amount_received == 0 || amount_received == -1 ) break;
            bytes_left -= amount_received;
        }

        if ( amount_received == 0 || amount_received == -1 ) break;

        pthread_mutex_lock(&myself->recv_mutex);
            myself->received_data.push(current_packet);
            pthread_cond_signal(&myself->recv_condition);
        pthread_mutex_unlock(&myself->recv_mutex);

    }

    //if we're here, something has gone wrong

    //error
    if ( amount_received == -1 ) {
        printf("WSAError: %d\n", WSAGetLastError());
        perror("ERROR: sinsocket.recv"); }

    //peer disconnected
    if ( amount_received == 0 ) {
        fprintf(stderr, "ERROR: sinsocket.recv: peer has disconnected\n");
        myself->ready_for_action = false; }

    pthread_mutex_lock(&myself->error_mutex);
    myself->socket_error = 1;
    pthread_mutex_unlock(&myself->error_mutex);

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
void *sinsocket::sinSendThread(void *inself) {
    sinsocket *myself = (sinsocket *)inself;
    packet_data *current_packet;

    while (true) {

        pthread_mutex_lock(&myself->send_mutex);
        if ( !myself->sending_data.size() )
            pthread_cond_wait(&myself->send_condition, &myself->send_mutex);

        current_packet = myself->sending_data.front();
        myself->sending_data.pop();
        pthread_mutex_unlock(&myself->send_mutex);

        if ( myself->send(&current_packet->data_size, 4) )
            //returns 1 if error, so something bad happened
            break;

        if ( myself->send(current_packet->data, current_packet->data_size) )
            //returns 1 if error, so something bad happened
            break;

        delete current_packet;

    }

    //if we're here, something has gone wrong
    pthread_mutex_lock(&myself->error_mutex);
    myself->socket_error = 2;
    pthread_mutex_unlock(&myself->error_mutex);

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
void sinsocket::asyncSend(const void *indata, const int &inlength) {

    packet_data *current_packet = new packet_data((char*)indata, inlength);

    pthread_mutex_lock(&send_mutex);
        sending_data.push(current_packet);
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
        received_data.pop();
    pthread_mutex_unlock(&recv_mutex);

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

void sinsocket::setUserData(void *in) { user_data = in; }
void *sinsocket::getUserData() { return user_data; }