/**
* @file client_socket.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-25-20-46
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CLIENT_SOCKET
#define CLIENT_SOCKET

#include "cell.hpp"
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#define SEND_BUFF_SIZE RECV_BUFF_SIZE
#endif

class client_socket {
public:
    client_socket(SOCKET sockfd = INVALID_SOCKET);
    SOCKET sockfd();
    char *msg_buf();
    int get_pos();
    void set_pos(int pos);
    int send_data(data_header *header);

private:
    SOCKET sockfd_;
    char sz_msg_buf[RECV_BUFF_SIZE];
    char send_buf_[SEND_BUFF_SIZE];
    int last_pos_;
    int last_send_pos_;
};

#endif // CLIENT_SOCKET