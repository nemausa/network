#include "cell_epoll.hpp"


cell_epoll::~cell_epoll() {
    destory();
}

int cell_epoll::create(int max_events) {
    if (epfd_ > 0) {
        destory();
    }
    epfd_ = epoll_create(max_events);
    if (EPOLL_ERROR == epfd_) {
        cell_log::info("epoll_create");
        return epfd_;
    }
    pevents_ = new epoll_event[max_events];
    max_events_ = max_events;
    return epfd_;
}

void cell_epoll::destory() {
    if (epfd_ > 0) {
        cell_network::destory_socket(epfd_);
        epfd_ = -1;
    }

    if (pevents_) {
        delete [] pevents_;
        pevents_ = nullptr;
    }
}

// 向epoll对象注册需要管理、监听的socket文件描述符
int cell_epoll::ctl(int op, SOCKET sockfd, uint32_t events) {
    epoll_event ev;
    // 事件类型
    ev.events = events;
    // 事件关联的socket描述符对象
    ev.data.fd = sockfd;
    // 向epoll对象注册需要管理、监听的socket文件描述符
    // 并且说明关心的事件
    // 返回0代表操作成功，返回负值代表失败 -1
    int ret = epoll_ctl(epfd_, op, sockfd, &ev);
    if (EPOLL_ERROR == ret) {
        cell_log::info("epoll_ctl error");
    }
    return ret;
}

int cell_epoll::ctl(int op, cell_client *pclient, uint32_t events) {
    epoll_event ev;
    // 事件类型
    ev.events = events;
    // 事件关联socket描述对象
    ev.data.ptr = pclient;
    int ret = epoll_ctl(epfd_, op, pclient->sockfd(), &ev);
    if (EPOLL_ERROR == ret) {
        cell_log::info("epoll_ctl2");
    }
    return ret;
}

int cell_epoll::wait(int timeout) {
    int ret = epoll_wait(epfd_, pevents_, max_events_, timeout);
    if (EPOLL_ERROR == ret) {
        if (errno == EINTR) {
            cell_log::info("epoll_wait EINTR");
            return 0;
        }
        cell_log::info("epoll_wait");
    }
}

epoll_event *cell_epoll::events() {
    return pevents_;
}