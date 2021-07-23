#include "cell_epoll_server.hpp"

cell_epoll_server::cell_epoll_server() {
    ep_.create(10240);
}

cell_epoll_server::~cell_epoll_server() noexcept {
    close();
}

bool cell_epoll_server::do_events() {
    for (auto iter : clients_) {
       if (iter.second->need_write()) {
           ep_.ctl(EPOLL_CTL_MOD, iter.second, EPOLLIN|EPOLLOUT);
       } else {
           ep_.ctl(EPOLL_CTL_MOD, iter.second, EPOLLIN);
       }
    }
    
    int ret = ep_.wait(1);
    if (ret < 0) {
        cell_log::info("cell_epoll_server%d wait", id_);
        return false;
    } else if (ret == 0) {
        return true;
    }
    
    auto events = ep_.events();
    for (int i = 0; i < ret; i++) {
        cell_client *pclient = (cell_client*)events[i].data.ptr;
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

void cell_epoll_server::rm_client(cell_client *pclient) {
    auto iter = clients_.find(pclient->sockfd());
    if (iter != clients_.end()) {
       clients_.erase(iter); 
    }
    on_leave(pclient);
}

void cell_epoll_server::on_join(cell_client *pclient) {
    ep_.ctl(EPOLL_CTL_ADD, pclient, EPOLLIN);
}
