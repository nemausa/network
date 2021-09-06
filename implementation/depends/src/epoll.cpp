#include "depends/epoll.hpp"

namespace nemausa {
namespace io {

#if __linux__

epoll::~epoll() {
    destory();
}

int epoll::create(int max_events) {
    if (epfd_ > 0) {
        destory();
    }
    epfd_ = epoll_create(max_events);
    if (EPOLL_ERROR == epfd_) {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "epoll_create");
        return epfd_;
    }
    pevents_ = new epoll_event[max_events];
    max_events_ = max_events;
    return epfd_;
}

void epoll::destory() {
    if (epfd_ > 0) {
        network::destory_socket(epfd_);
        epfd_ = -1;
    }

    if (pevents_) {
        delete [] pevents_;
        pevents_ = nullptr;
    }
}

// 向epoll对象注册需要管理、监听的socket文件描述符
int epoll::ctl(int op, SOCKET sockfd, uint32_t events) {
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
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "epoll_ctl error");
    }
    return ret;
}

int epoll::ctl(int op, client *pclient, uint32_t events) {
    epoll_event ev;
    // 事件类型
    ev.events = events;
    // 事件关联socket描述对象
    ev.data.ptr = pclient;
    int ret = epoll_ctl(epfd_, op, pclient->sockfd(), &ev);
    if (EPOLL_ERROR == ret) {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "epoll_ctl2");
    }
    return ret;
}

int epoll::wait(int timeout) {
        //epfd epoll对象的描述符
        //events 用于接收检测到的网络事件的数组
        //maxevents 接收数组的大小，能够接收的事件数量
        //timeout
        //		t=-1 直到有事件发生才返回
        //		t= 0 立即返回 std::map
        //		t> 0 如果没有事件那么等待t毫秒后返回
    int ret = epoll_wait(epfd_, pevents_, max_events_, timeout);
    if (EPOLL_ERROR == ret) {
        if (errno == EINTR) {
            SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "epoll_wait EINTR");
            return 0;
        }
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "epoll_wait");
    }
    return ret;
}

epoll_event *epoll::events() {
    return pevents_;
}

#endif

} // namespace io 
} // namespace nemausa
