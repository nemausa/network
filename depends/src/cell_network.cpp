#include "cell_network.hpp"


cell_network::cell_network() {
#ifdef _WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA data;
    WSAStartup(ver, &data);
#else
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

int cell_network::make_reuseadd(SOCKET fd) {
    int flag = 1;
    if (SOCKET_ERROR == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(flag))) {
        cell_log::info("setsockopt socket<%d> fail", int(fd));
        return SOCKET_ERROR;
    }
    return 0;
}

