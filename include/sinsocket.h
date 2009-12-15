#ifndef _SINSOCKET_H
#define _SINSOCKET_H

#include <pthread.h>
#include <queue>

typedef struct packet_data_s {
    int data_size;
    char *data;
    void getChunk(void *output, const int &size);
    void setChunk(void *input, const int &size);
    int size();
    packet_data_s(void *indata, const int &size=0);
    ~packet_data_s();
} packet_data;

///////////////

class sinsocket
{
private:

    bool ready_for_action;
    bool spawned_threads;
    int my_socket;
    char my_port[6];

    //thread goodies
    std::queue<packet_data*> received_data;
    std::queue<packet_data*> sending_data;
    pthread_mutex_t recv_mutex;
    pthread_mutex_t send_mutex;
    pthread_mutex_t error_mutex;
    pthread_cond_t send_condition;
    pthread_cond_t recv_condition;
    pthread_t send_thread_id;
    pthread_t recv_thread_id;
    int socket_error;

    //to know if the program-wide init has been run
    static bool done_init;
    static int socket_count;

    //to associate data with the socket
    void *user_data;


public:

    sinsocket(int my_sockd=-1);
    ~sinsocket();

    //server functions
    int listen(const int &port); //eventually add sockets_per_thread and threads inits
    sinsocket* accept();

    //client functions
    int connect(const char* address, const int &port);

    //shared functions
    int send( const void *indata, const int &inlength ); //blocking
    int recv( const void *indata, const int &inlength ); //blocking
    int beginDisconnect();
    int endDisconnect();
    int closeSinsocket();

    //async thread stuff
    static void *sinRecvThread(void*);
    static void *sinSendThread(void*);
    //
    void spawnThreads();
    void asyncSend(const void *indata, const int &inlength );
    packet_data* asyncRecv();
    packet_data* asyncRecvWait();
    int checkErrors();

    //utility functions
    void setUserData(void* in);
    void *getUserData();

};



//this class allows you to service multiple sockets simultaneously in one thread
class sinsocket_farm {
public:
private:
};


#endif
