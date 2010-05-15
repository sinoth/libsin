#ifndef SINSOCKET_H
#define SINSOCKET_H

#include <pthread.h>
#include <string>
#include <queue>


#define SINSOCKET_ERROR_ZLIB            -4
#define SINSOCKET_ERROR_NOT_READY       -3
#define SINSOCKET_ERROR_UNRECOVERABLE   -2
#define SINSOCKET_ERROR_DISCONNECT      -1
#define SINSOCKET_OK                     0


//////////////////////////////

typedef struct packet_data_s {
    int data_size;
    int data_size_uncompressed;
    int current_loc;
    char *data;
    //
    packet_data_s(void *indata=NULL, int size=0);
    packet_data_s(const std::string &in_string);
    ~packet_data_s();
    void getChunk(void *output, int size);
    void setChunk(void *input, int size);
    void setAll(void* input);
    void compress();
    int size();
} packet_data;

///////////////

struct sinbits {
    static void vector_to_bits( const std::vector<uint8_t> &in_vec, uint8_t **in_data, int &in_size );
    static void vector_to_bits( const std::vector<uint8_t> &in_vec, packet_data &in_packet );
    static void bits_to_vector( const uint8_t *in_data, const int &in_size, std::vector<uint8_t> &in_vec );
    static void bits_to_vector( const packet_data &in_packet, std::vector<uint8_t> &in_vec );
};

//////////////////////////////


class sinsocket
{

public:

    sinsocket(int my_sockd=-1);
    ~sinsocket();

    //server functions
    int listen(const int &port, const int &hint=0); //eventually add sockets_per_thread and threads inits
    sinsocket* accept(char *store_ip=NULL);

    //client functions
    int connect(const char* address, const int &port);

    //shared functions
    int sendRaw( const void *indata, const int inlength ); //blocking
    int sendPacket( packet_data *inpacket, bool compressed = false ); //blocking
    int sendPacket( packet_data &inpacket, bool compressed = false ); //blocking
    int recvRaw( const void *indata, const int inlength ); //blocking
    int recvPacket( packet_data * &outpacket, bool compressed = false ); //blocking
    int recvPacket( packet_data &outpacket, bool compressed = false ); //blocking
    int beginDisconnect();
    int endDisconnect();
    void closeSinsocket();

    //utility functions
    void setUserData(void* in);
    void *getUserData();

private:

    bool ready_for_action;
    int my_socket;
    char my_port[6];

    //to know if the program-wide init has been run
    static bool done_init;
    static int socket_count;

    //to associate data with the socket
    void *user_data;


//////////////////
// thread stuff //
//////////////////

#ifndef SINSOCKET_NO_THREADS

public:

    int spawnThreads();
    void asyncSend( const void *indata, const int inlength );
    void asyncSend( packet_data *inpacket );
    packet_data* asyncRecv();
    packet_data* asyncRecvWait();
    int checkErrors();

    void calculatePercentage(const bool in);
    void calculateSpeed(const bool in);
    int getTransferTotalBytes();
    int getTransferBytes();
    float getTransferPercent();
    int getBps();

private:


    //async thread stuff
    static void *sinRecvThread(void*);
    static void *sinSendThread(void*);
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
    bool spawned_threads;
    bool stop_threads;
    int socket_error;

    bool calculate_percentage;
    bool calculate_speed;
    int bytes_per_second;
    int current_transfer_total_bytes;
    int current_transfer_bytes;
    float current_transfer_percent;
    pthread_mutex_t bps_mutex;
    pthread_mutex_t percent_mutex;

#endif

};



//this class allows you to service multiple sockets simultaneously in one thread
class sinsocket_farm {
public:
private:
};


#endif




/*

struct sinbits {

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    static void vector_to_bits( const std::vector<uint8_t> &in_vec, uint8_t **in_data, int &in_size ) {
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void bits_to_vector( const uint8_t *in_data, const int &in_size, std::vector<uint8_t> &in_vec ) {
            in_vec.resize( in_size * 8 );
            uint8_t bit_count=0, byte_count=0;
            for ( uint32_t i=0; i < in_vec.size(); ++i, ++bit_count ) {
                if ( bit_count == 8 ) { bit_count = 0; ++byte_count; }
                in_vec[i] = (in_data[byte_count] >> bit_count) & 1;
            }
        }

    #ifdef SINSOCKET_H
    //////////////////////////////////////////////////////////////////////////////////////
    static void vector_to_bits( const std::vector<uint8_t> &in, packet_data &in_packet ) {
            vector_to_bits( in, &in_packet.data, in_packet.size );
        }

    static void bits_to_vector( const packet_data &in_packet, std::vector<uint8_t> &in_vec ) {
            bits_to_vector( in_packet.data, in_packet.size, in_vec);
        }
    #endif

};

*/
