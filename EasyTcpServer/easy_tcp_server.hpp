/**
* @file easy_tcp_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-06-22-09
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_TCP_SERVER
#define EASY_TCP_SERVER


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
#include <vector>
#include <algorithm>
#include "message_header.hpp"

class easy_tcp_server {
private:
    SOCKET sock_;
    std::vector<SOCKET> clients_;
public:
    easy_tcp_server();
    virtual ~easy_tcp_server();
    virtual void on_msg(SOCKET c_sock, data_header *header);
    SOCKET init_socket();
    SOCKET accept();
    int bind(const char *ip, unsigned short port);
    int listen(int n);
    void close();
    bool on_run();
    bool is_run();
    int recv_data(SOCKET c_sock);
    int send_data(SOCKET c_sock, data_header *header);
    void send_data_to_all(data_header *header);
};

#endif // EASY_TCP_SERVER