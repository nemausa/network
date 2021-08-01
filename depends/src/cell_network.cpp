#include "cell_network.hpp"
#ifndef _WIN32
#include <fcntl.h>
#include <stdlib.h>
#endif

cell_network::cell_network() {
#ifdef _WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA data;
    WSAStartup(ver, &data);
#else
    // 忽略异常信号，默认情况会导致进程终止
    signal(SIGPIPE, SIG_IGN);
#endif
}

cell_network::~cell_network() {
#ifdef _WIN32   
    WSACleanup();
#endif
}

void cell_network::init() {
    static cell_network obj;
}

int cell_network::make_nonblock(SOCKET fd) {
#ifdef _WIN32
    unsigned long noblock = 1;
    if (ioctlsocket(fd, FIONBIO, &noblock) == SOCKET_ERROR) {
        LOG_WARN("fcntl(%d, F_GETFL)", (int)fd);
        return -1;
    }
#else
    int flags;
    if ((flags == fcntl(fd, F_GETFL, NULL)) <0) {
        LOG_WARN("fcntl(%d, F_GETFL)", (int)fd);
        return -1;
    }
    if (!(flags & O_NONBLOCK)) {
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            LOG_WARN("fcntl(%d, FSETFL)", fd);
            return -1;
        }
    }
#endif
}
int cell_network::make_reuseadd(SOCKET fd) {
    int flag = 1;
    if (SOCKET_ERROR == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(flag))) {
        LOG_INFO("setsockopt socket<%d> fail", int(fd));
        return SOCKET_ERROR;
    }
    return 0;
}

// int cell_network::make_nodelay(SOCKET fd) {
//     int flag = 1;
//     if (SOCKET_ERROR == setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&flag, sizeof(flag))) {
//         LOG_WARN("setsockopt socket<%d> IPPROTO_TCP TCP_NODELAY failed", (int)fd);
//         return SOCKET_ERROR;
//     }
//     return 0;
// }

int cell_network::destory_socket(SOCKET sockfd) {
#ifdef _WIN32
    int ret = closesocket(sockfd);
#else
    int ret = close(sockfd);
#endif
    if (ret < 0) {
        LOG_INFO("destory sockfd<%d>", int(sockfd));
    }
    return ret;
}