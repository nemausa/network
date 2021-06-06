#include "easy_tcp_server.hpp"

int main() {
    easy_tcp_server server;
    server.init_socket();
    server.bind(nullptr, 4567);
    server.listen(5);
    while (server.is_run()){
        server.on_run();
    }
    server.close();
    printf("exit\n");
    getchar();
    return 0;
}