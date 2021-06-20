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

const int client_count = 8;
const int thread_count = 4;
easy_tcp_client *clients[client_count];

void send_thread(int id) {
    printf("thread<%d> start\n", id);
    int c = client_count / thread_count;
    int begin = (id -1) * c;
    int end = id * c;
    for (int n = begin; n < end; n++) {
        clients[n] = new easy_tcp_client();
    }
    for (int n = begin; n < end; n++) {
        clients[n]->connect("127.0.0.1", 4567);
    }

    printf("thread<%d> connect begin<%d> end<%d>\n", id, begin, end);
    std::chrono::milliseconds t(3000);
    std::this_thread::sleep_for(t);

    login l;
    strcpy(l.user_name, "Nemausa");
    strcpy(l.password, "Nemausa");
    while (is_run)
    {
        for (int n = begin; n < end; n++) {
            clients[n]->send_data(&l);
            clients[n]->on_run();
        }
    }

    for (int n = begin; n < end; n++) {
        clients[n]->close();
        delete clients[n];
    }

    printf("thread<%d> exit\n", id);
    

}

int main() {
    std::thread t1(cmd_thread);

    for (int n = 0; n < thread_count; n++) {
        std::thread t(send_thread, n+1);
        t.detach();
    }

    while(is_run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    getchar();
    return 0;
}