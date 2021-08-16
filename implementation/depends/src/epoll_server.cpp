#include "depends/epoll_server.hpp"

namespace nemausa {
namespace io {

#if __linux__
epoll_server::epoll_server() {
    ep_.create(10240);
}

epoll_server::~epoll_server() noexcept {
    close();
}

bool epoll_server::do_net_events() {
    for (auto iter : clients_) {
       if (iter.second->need_write()) {
           ep_.ctl(EPOLL_CTL_MOD, iter.second, EPOLLIN|EPOLLOUT);
       } else {
           ep_.ctl(EPOLL_CTL_MOD, iter.second, EPOLLIN);
       }
    }
    
    int ret = ep_.wait(1);
    if (ret < 0) {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                "epoll_server {} wait", id_);
        return false;
    } else if (ret == 0) {
        return true;
    }
    
    auto events = ep_.events();
    for (int i = 0; i < ret; i++) {
        client *pclient = (client*)events[i].data.ptr;
        if (pclient) {
            if (events[i].events & EPOLLIN) {
                if (SOCKET_ERROR == recv_data(pclient)) {
                    rm_client(pclient);
                    continue;
                }
            } 
            
            if (events[i].events & EPOLLOUT) {
                if (SOCKET_ERROR == pclient->send_data_real()) {
                    rm_client(pclient);
                }
            }

        }
    }
    return true;
}

void epoll_server::rm_client(client *pclient) {
    auto iter = clients_.find(pclient->sockfd());
    if (iter != clients_.end()) {
       clients_.erase(iter); 
    }
    on_leave(pclient);
}

void epoll_server::on_join(client *pclient) {
    ep_.ctl(EPOLL_CTL_ADD, pclient, EPOLLIN);
}

#endif

} // namespace io 
} // namespace nemausa
