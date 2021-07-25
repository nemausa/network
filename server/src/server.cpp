#include <thread>
#include <cstring>
#include "easy_epoll_server.hpp"
#include "cell_log.hpp"
#ifndef VERSION
const char *VERSION_INFO = "version: 0.0.1";
#endif

int main() {
    cell_log::initstance().set_path("server_log.txt", "w");
    easy_epoll_server server;
    server.init_socket();
    server.bind(nullptr, 4567);
    server.listen(64);
    server.start(4);


    while(true) {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) {
            cell_log::info("exit thread\n");
            server.close();
            break;
        } else {
            cell_log::info("undefined commad\n");
        }
    }

    cell_log::info("exit\n");
    getchar();
    return 0;
}