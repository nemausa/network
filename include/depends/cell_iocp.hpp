/**
* @file cell_iocp.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-28-10-03
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_IOCP
#define CELL_IOCP

#ifdef _WIN32  
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <mswsock.h>
#include <stdio.h>


enum io_type_e {
    ACCEPT = 10,
    RECV,
    SEND
};

// 数据缓冲区空间大小
// #define IO_DATA_BUFF_SIZE 2014
struct io_data_base {
    OVERLAPPED  overlapped;
    SOCKET      sockfd;
    WSABUF      wsabuff;
    io_type_e   io_type;
};

struct io_event {
    union {
        void *ptr;
        SOCKET sockfd;
    }data;
    io_data_base *p_io_data;
    DWORD bytes_trans = 0;
};

class cell_iocp {
public:
    ~cell_iocp();
    bool create();
    void destory();
    bool reg(SOCKET sockfd);
    bool reg(SOCKET sockfd, void *ptr);
    bool post_accept(io_data_base *p_io_data);
    bool post_recv(io_data_base *p_io_data);
    bool post_send(io_data_base *p_io_data);
    int wait(io_event &io_event, int timeout);   
    bool load_accept(SOCKET listen_socket);
private:
    LPFN_ACCEPTEX   acceptex_ = NULL;  
    HANDLE completion_port_ = NULL;
    SOCKET sock_server_ = INVALID_SOCKET;
};

#endif // __WIN32

#endif // CELL_IOCP