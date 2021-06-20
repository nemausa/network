/**
* @file cell_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-20-17-54
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_SERVER
#define CELL_SERVER
#ifdef _WIN32
    #define FD_SETSIZE      2506
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
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include "message_header.hpp"
#include "obersver.hpp"
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 102400
#endif


class client_socket {
public:
    client_socket(SOCKET sockfd = INVALID_SOCKET) {
        sockfd_ = sockfd;
        memset(sz_msg_buf, 0, sizeof(sz_msg_buf));
        last_pos_ = 0;
    }

    SOCKET sockfd() {
        return sockfd_;
    }

    char *msg_buf() {
        return sz_msg_buf;
    }

    int get_pos() {
        return last_pos_;
    }

    void set_pos(int pos) {
        last_pos_ = pos;
    }
private:
    SOCKET sockfd_;
    char sz_msg_buf[RECV_BUFF_SIZE * 10];
    int last_pos_;
};

class cell_server : public subject {
private:
    SOCKET sockfd_;
    std::vector<client_socket*> clients_;
    std::vector<client_socket*> clients_buff_;
    std::mutex mutex_;
    std::thread thread_;
    observer *observer_;
    char sz_recv_[RECV_BUFF_SIZE];
public:
    cell_server(SOCKET sockfd = INVALID_SOCKET, observer *ob = nullptr);
    virtual ~cell_server();
    virtual void on_msg(SOCKET c_sock, data_header *header);
    void close();
    bool on_run();
    bool is_run();
    int recv_data(client_socket *client);
    void add_client(client_socket* client);
    void start();
    size_t count();
};

#endif // CELL_SERVER