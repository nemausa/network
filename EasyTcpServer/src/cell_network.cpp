#include "cell_network.hpp"
#include <signal.h>
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

