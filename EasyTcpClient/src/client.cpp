#include <thread>
#include <atomic>
#include "easy_tcp_client.hpp"
#include "timestamp.hpp"

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

const int client_count = 1;
const int thread_count = 1;
easy_tcp_client *clients[client_count];
std::atomic_int send_count;
std::atomic_int ready_count;
std::atomic_int msg_count_;

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
    ready_count++;
    while (ready_count < thread_count) {
        // 等待其他线程连接服务器
        std::chrono::milliseconds t(10);
        std::this_thread::sleep_for(t);
    }

    login lg[10];
    for (int n=0; n < 10; n++) {
        strcpy(lg[n].user_name, "Nemausa");
        strcpy(lg[n].password, "Nemausa");
    }
    const int length = sizeof(lg);
    while (is_run){
        for (int n = begin; n < end; n++) {
            if (SOCKET_ERROR != clients[n]->send_data(lg, length)) {
                send_count++;
            }
            clients[n]->on_run();
            std::chrono::milliseconds t(100);
            std::this_thread::sleep_for(t);
        }
    }

    for (int n = begin; n < end; n++) {
        clients[n]->close();
        delete clients[n];
    }

    printf("thread<%d> exit\n", id);
    

}

int main() {
    ready_count = 0;
    send_count = 0;
    msg_count_ = 0;
    std::thread t1(cmd_thread);

    for (int n = 0; n < thread_count; n++) {
        std::thread t(send_thread, n+1);
        t.detach();
    }
    timestamp stamp;
    while(is_run) {
        auto t = stamp.second();
        if (t > 1.0) {
            printf("thread<%d>, clients<%d>, time<%lf>, send<%d>, msg<%d>\n", thread_count, client_count, t, (int)(send_count / t), (int)msg_count_);
            send_count = 0;
            msg_count_ = 0;
            stamp.update();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    getchar();
    return 0;
}