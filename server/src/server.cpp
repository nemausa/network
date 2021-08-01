#include <thread>
#include <cstring>
#include "easy_epoll_server.hpp"
#include "cell_log.hpp"
#include "easy_iocp_server.hpp"
#ifndef VERSION
const char *VERSION_INFO = "version: 0.0.1";
#endif

int main() {
    cell_log::instance().set_path("server_log.txt", "w", true);
#ifdef _WIN32
    easy_iocp_server server;
#else
    easy_epoll_server server;
#endif
    server.init_socket();
    server.bind(nullptr, 4567);
    server.listen(64);
    server.start(4);


    while(true) {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) {
            LOG_INFO("exit thread\n");
            server.close();
            break;
        } else {
            LOG_INFO("undefined commad\n");
        }
    }

    LOG_INFO("exit\n");
    getchar();
    return 0;
}