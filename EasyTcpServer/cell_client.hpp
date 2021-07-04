/**
* @file cell_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-30-00-21
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_CLIENT
#define CELL_CLIENT

#include "cell.hpp"
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#define SEND_BUFF_SIZE RECV_BUFF_SIZE
#endif

#define CLIENT_HEART_DEAD_TIME  10000
#define CLIENT_SEND_BUFF_TIME   200

class cell_client {
public:
    cell_client(SOCKET sockfd = INVALID_SOCKET);
    ~cell_client();
    SOCKET sockfd();
    char *msg_buf();
    int get_pos();
    void set_pos(int pos);
    int send_data_real();
    int send_data(data_header *header);
    void reset_heart_time();
    void reset_send_time();
    bool check_heart_time(time_t dt);
    bool check_send_time(time_t dt);
private:
    SOCKET sockfd_;
    char sz_msg_buf[RECV_BUFF_SIZE];
    char send_buf_[SEND_BUFF_SIZE];
    int last_pos_;
    int last_send_pos_;
    time_t heart_time_;
    time_t send_time_;
};

#endif // CELL_CLIENT