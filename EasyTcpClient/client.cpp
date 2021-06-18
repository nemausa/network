#include "easy_tcp_client.hpp"
#include <thread>

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
    const int count =  64-1;
    easy_tcp_client* clients[count];
    for (int n = 0; n < count; n++) {
        clients[n] = new easy_tcp_client();
    }
    for (int n = 0; n < count; n++) {
        clients[n]->connect("127.0.0.1", 4567);
    }
    std::thread t1(cmd_thread);
    t1.detach();

    login l;
    strcpy(l.user_name, "Nemausa");
    strcpy(l.password, "Nemausa");
    while (is_run)
    {
        for (int n = 0; n < count; n++) {
            clients[n]->send_data(&l);
            clients[n]->on_run();
        }
    }
    for (int n = 0; n < count; n++) {
        clients[n]->close();
    }
    getchar();
    return 0;
}