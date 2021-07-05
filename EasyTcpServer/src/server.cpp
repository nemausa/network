#include <thread>
#include "easy_tcp_server.hpp"

bool is_run = true;

void cmd_thread() {
    while(true) {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) {
            is_run = false;
            printf("exit thread\n");
            break;
        } else {
            printf("undefined commad\n");
        }
    }
}


int main() {
    easy_tcp_server server;
    server.init_socket();
    server.bind(nullptr, 4567);
    server.listen(5);
    server.start(4);
    std::thread t1(cmd_thread);
    t1.detach();

    std::chrono::milliseconds t(10);
    while (is_run){
        std::this_thread::sleep_for(t);
    }
    server.close();
    printf("exit\n");
    getchar();
    return 0;
}