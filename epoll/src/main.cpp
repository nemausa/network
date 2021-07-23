#if __linux__

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
            // break;
        } else {
            printf("undefine command\n");
        }
    }

}

void cell_epoll_ctl(int epfd, int op, SOCKET sockfd, uint32_t events) {
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

void client_levae(SOCKET csock) {
    close(csock);
    printf("client<%d> exited\n", csock);
    auto itr = std::find(gclients.begin(), gclients.end(), csock);
    gclients.erase(itr);
}

int main() {
    std::thread t1(cmd_thread);
    t1.detach();
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(4567);
    sin.sin_addr.s_addr = INADDR_ANY;

    int flag = 1;
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(flag))) {
        return SOCKET_ERROR;
    }
    

    if (SOCKET_ERROR == bind(sock, (sockaddr*)&sin, sizeof(sin))) {
        printf("bind error\n");
    } else {
        printf("bind success\n");
    }

    if (SOCKET_ERROR == listen(sock, 64)) {
        printf("listen error\n");
    } else {
        printf("listen success\n");
    }
    
    const int max_client = 60000;
    // linux 2.6.8 后size就么有作用了
    // 由epoll动态管理，理论值为filemax
    // 通过 cat /proc/sys/fs/file-max查询
    int epfd = epoll_create(max_client);

    // 向epoll对象注册需要管理、监听的socket文件描述符
    cell_epoll_ctl(epfd, EPOLL_CTL_ADD, sock, EPOLLIN);

    int msg_count = 0;
    int c_count = 0;
    // 用于接收检测的网络事件组
    epoll_event events[max_client] = {};
    while (g_bRun) {
        // epfd epoll对象的描述符
        // events 用于接收检测到的网络事件的数组
        // maxevents 接收数组的大小，能够接收的事件数量
        // timeout
        //      t = -1 直到有事件发生才返回
        //      t = 0 立即返回 std::map
        //      t > 0 如果没有事件那么等待t毫秒后返回
        int n = epoll_wait(epfd, events, max_client, 1);
        if (n < 0) {
            printf("error, epoll_wait ret=%d\n", n);
            break;
        }

        for (int i = 0; i < n; i++) { 
			//当服务端socket发生事件时，表示有新客户端连接
			if(events[i].data.fd == sock) {
                if (events[i].events & EPOLLIN) {
                    // accept 等待客户端连接
                    sockaddr_in client_addr = {};
                    int addr_len = sizeof(sockaddr_in);
                    SOCKET csock = INVALID_SOCKET;
                    csock = accept(sock, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
                    c_count++;
                    if (INVALID_SOCKET == csock) {
                        printf("error, invalid socket<%d> error code<%d> error info<%s>\n", c_count, errno, strerror(errno));
                    } else {
                        gclients.push_back(csock);
                        cell_epoll_ctl(epfd, EPOLL_CTL_ADD, csock, EPOLLIN);
                        printf("client<%d> ip<%s>\n", (int)csock, inet_ntoa(client_addr.sin_addr));
                    }
                    continue;
            }
        }

            // 当前socket有数据可读，也有可能发生错误
            if (events[i].events & EPOLLIN) {
                auto csock = events[i].data.fd;
                int ret = read_data(csock);
                if (ret <= 0) {
                    client_levae(csock);
                } else {

                }
            }

            if (events[i].events & EPOLLOUT) {
                printf("eEPOLLOUT|%d\n", msg_count);
                auto csock = events[i].data.fd;
                int ret = write_data(csock);
                if (ret < 0) {
                    client_levae(csock);
                }
                if (msg_count < 5 )
                    cell_epoll_ctl(epfd, EPOLL_CTL_MOD, csock, EPOLLIN);
                else 
                    cell_epoll_ctl(epfd, EPOLL_CTL_MOD, csock, 0);
            }

        }
    }

    for (auto client : gclients) {
        close(client);
    }
    close(epfd);
    close(sock);
    printf("exit\n");
    return 0;
}

#endif

#ifndef __linux__
int main() {
    return 0;
}

#endif