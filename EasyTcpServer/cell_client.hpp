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

class cell_client {
public:
    cell_client(SOCKET sockfd = INVALID_SOCKET);
    SOCKET sockfd();
    char *msg_buf();
    int get_pos();
    void set_pos(int pos);
    int send_data(data_header *header);
    void reset_heart();
    bool check_heart(time_t dt);

private:
    SOCKET sockfd_;
    char sz_msg_buf[RECV_BUFF_SIZE];
    char send_buf_[SEND_BUFF_SIZE];
    int last_pos_;
    int last_send_pos_;
    time_t heart_;
};

#endif // CELL_CLIENT