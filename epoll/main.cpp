#include<unistd.h> //uni std
#include<arpa/inet.h>
#include<string.h>
#include<sys/epoll.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#include<stdio.h>
#include<vector>
#include<thread>
#include<algorithm>

std::vector<SOCKET> gclients;

bool g_bRun = true;

void cmd_thread() {
    while (true) {
        char cmd[256] = {};
        scanf("%s", cmd);
        if (0 == strcmp(cmd, "exit")) {
            g_bRun = false;
            printf("thread exit\n");
            break;
        } else {
            printf("undefine command\n");
        }
    }

}

int cell_epoll_ctl(int epfd, int op, SOCKET sockfd, uint32_t events) {
    epoll_event ev;
    // 事件类型
    ev.events = events;
    // 事件关联的socket描述符对象
    ev.data.fd = sockfd;
    // 向epoll对象注册需要管理、监听的socket文件描述符
    // 并且说明关心的事件
    // 返回0代表操作成功，返回负值代表失败 -1
    if (epoll_ctl(epfd, op, sockfd, &ev) == -1) {
        printf("error, epoll_ctl\n");
    }

}

char sz_buff[4096] = {};
int glen = 0;
int read_data(SOCKET csock) {
    glen  = (int)recv(csock, sz_buff, 4096, 0);
    return glen;
}

int write_data(SOCKET csock) {
    if (glen > 0) {
        int len = (int)send(csock, sz_buff, glen, 0);
        glen = 0;
        return len;
    }
    return 1;
}

int client_levae(SOCKET csock) {
    close(csock);
    printf("client<%d> exited\n", csock);
    auto itr = std::find(gclients.begin(), gclients.end(), csock);
    gclients.erase(itr);
}

int main() {

    
}