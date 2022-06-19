#include "depends/tcp_epoll_client.hpp"

namespace nemausa {
namespace io {

#if __linux__

void tcp_epoll_client::on_init_socket() {
    ep_.create(1);
    ep_.ctl(EPOLL_CTL_ADD, pclient_, EPOLLIN);
}

void tcp_epoll_client::close() {
    ep_.destory();
    tcp_client::close();
}

bool tcp_epoll_client::on_run(int microseconds) {
    if (is_run()) {
       if (pclient_->need_write()) {
           ep_.ctl(EPOLL_CTL_MOD, pclient_, EPOLLIN | EPOLLOUT);
       } else {
           ep_.ctl(EPOLL_CTL_MOD, pclient_, EPOLLIN); 
       }
       int ret = ep_.wait(microseconds);
       if (ret < 0) {
           //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                    // "epoll_client.on_run.wait");
           return false;
       } else if (ret == 0) {
           return true;
       }
       auto events = ep_.events();
       for (int i = 0; i < ret; i++) {
           client *pclient = (client*)events[i].data.ptr;
           if (pclient) {
                if (events[i].events & EPOLLIN) {
                    if (SOCKET_ERROR == recv_data()) {
                        //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                                // "socket<{}>on_run.epoll recv_data exit", 
                                // pclient->sockfd());
                        close();
                        continue;
                    }
                }
                if (events[i].events & EPOLLOUT) {
                    if (SOCKET_ERROR == pclient->send_data_real()) {
                        //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                                // "socket<{}>on_run.epoll sen_data_real exit", 
                                // pclient->sockfd());
                        close();
                    }
                }
           }
       }
       return true;
    }
    return false;
}

#endif

} // namespace io 
} // namespace nemausa