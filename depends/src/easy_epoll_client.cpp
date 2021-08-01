#if __linux__

#include "easy_epoll_client.hpp"

void easy_epoll_client::on_init_socket() {
    ep_.create(1);
    ep_.ctl(EPOLL_CTL_ADD, pclient_, EPOLLIN);
}

void easy_epoll_client::close() {
    ep_.destory();
    easy_tcp_client::close();
}

bool easy_epoll_client::on_run(int microseconds) {
    if (is_run()) {
       if (pclient_->need_write()) {
           ep_.ctl(EPOLL_CTL_MOD, pclient_, EPOLLIN | EPOLLOUT);
       } else {
           ep_.ctl(EPOLL_CTL_MOD, pclient_, EPOLLIN); 
       }
       int ret = ep_.wait(microseconds);
       if (ret < 0) {
           LOG_INFO("cell_epoll_client.on_run.wait");
           return false;
       } else if (ret == 0) {
           return true;
       }
       auto events = ep_.events();
       for (int i = 0; i < ret; i++) {
           cell_client *pclient = (cell_client*)events[i].data.ptr;
           if (pclient) {
                if (events[i].events & EPOLLIN) {
                    if (SOCKET_ERROR == recv_data()) {
                        LOG_INFO("socket<%d>on_run.epoll recv_data exit", pclient->sockfd());
                        close();
                        continue;
                    }
                }
                if (events[i].events & EPOLLOUT) {
                    if (SOCKET_ERROR == pclient->send_data_real()) {
                        LOG_INFO("socket<%d>on_run.epoll sen_data_real exit", pclient->sockfd());
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