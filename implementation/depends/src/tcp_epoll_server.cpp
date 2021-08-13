#include "depends/tcp_epoll_server.hpp"

namespace nemausa {
namespace io {

#if __linux__

void tcp_epoll_server::start(int n) {
    tcp_server::start<epoll_server>(n);
}

void tcp_epoll_server::on_run(cell_thread *pthread) {
    epoll ep;
    ep.create(1);
    ep.ctl(EPOLL_CTL_ADD, sockfd(), EPOLLIN);
    while (pthread->is_run()) {
        time4msg();
        int ret = ep.wait(1);
        if (ret < 0) {
            SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "tcp_epoll_server.on_run.wait exit");
            pthread->exit();
            break;
        } 
        auto events = ep.events();
        for (int i=0; i < ret; i++) {
            // 当服务端socket发生事件时，表示有新客户端连接
            if (events[i].data.fd = sockfd()) {
                if (events[i].events & EPOLLIN) {
                    accept();
                }
            }
        }
    }

}

#endif

} // namespace io 
} // namespace nemausa