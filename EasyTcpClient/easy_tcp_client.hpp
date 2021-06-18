/**
* @file easy_tcp_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-07-21-06
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_TCP_CLIENT
#define EASY_TCP_CLIENT
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <winsock2.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <string.h>
    #define SOCKET int
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR    (SOCKET)(-1)
#endif

#include <stdio.h>
#include "message_header.hpp"
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 102400
#endif
class easy_tcp_client {
private:
    SOCKET sock_;
    int last_pos = 0;
    char sz_recv[RECV_BUFF_SIZE] = {};
    char sz_msg_buf[RECV_BUFF_SIZE * 10] = {};
public:
    easy_tcp_client();
    virtual ~easy_tcp_client();
    void init_socket();
    int connect(const char *ip, unsigned short port);
    void close();
    bool on_run();
    bool is_run();
    int recv_data(SOCKET c_sock);
    virtual void on_msg(data_header *header);
    int send_data(data_header *header);
};

#endif // EASY_TCP_CLIENT