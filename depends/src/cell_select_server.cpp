#include "cell_select_server.hpp"

cell_select_server::~cell_select_server() {

}

bool cell_select_server::do_net_events() {
    if (client_change_) {
        client_change_ = false;
        fd_read_.zero();
        max_socket_ = clients_.begin()->second->sockfd();
        for (auto iter : clients_) {
            fd_read_.add(iter.second->sockfd());
            if (max_socket_ < iter.second->sockfd()) {
                max_socket_ = iter.second->sockfd();
            }
        }
        fd_read_bak_.copy(fd_read_);
    } else {
        fd_read_.copy(fd_read_bak_);
    }
    bool is_need_write = false;
    fd_write_.zero();
    for (auto iter : clients_) {
        // 需要些数据才加入fd_set检测是否可写
        if (iter.second->need_write()) {
            is_need_write = true;
            fd_write_.add(iter.second->sockfd());
        }
    }
    timeval t{0, 1};
    int ret = 0;
    if (is_need_write) {
        ret = select(max_socket_ + 1, fd_read_.fdset(), fd_write_.fdset(), nullptr, &t);
    } else {
        ret = select(max_socket_ + 1, fd_read_.fdset(), nullptr, nullptr, &t);
    }
    if (ret < 0) {
        LOG_INFO("");
    } else if (ret == 0) {
        return true;
    }
    read_data();
    write_data();
    return true;
}

void cell_select_server::write_data() {
#if _WIN32
    auto pfdset = fd_read_.fdset();
    for (int n = 0; n < pfdset->fd_count; n++) {
        auto iter = clients_.find(pfdset->fd_array[n]);
        if (iter != clients_.end()) {
            if (-1 == recv_data(iter->second)) {
                on_leave(iter->second);
                clients_.erase(iter);
            }
        } else {
            LOG_INFO("error iter != clients_.end()\n");
        }
    }
#else
    auto pfdset = fd_read_.fdset();
    for (auto iter = clients_.begin(); iter != clients_.end();) {
        if (fd_read_.has(iter->second->sockfd())) {
            if (SOCKET_ERROR == recv_data(iter->second)) {
                on_leave(iter->second);
                auto iter_old = iter;
                iter++;
                clients_.erase(iter_old);
                continue;
            }
        }
        iter++;
    }
#endif 
}

void cell_select_server::read_data() {
#ifdef _WIN32
    auto pfdset = fd_read_.fdset();
    for (int n = 0; n < pfdset->fd_count; n++) {
        auto iter = clients_.find(pfdset->fd_array[n]);
        if (iter != clients_.end()) {
            if (-1 == iter->second->send_data_real()) {
                on_leave(iter->second);
                clients_.erase(iter);
            }
        }
    }
#else
    for (auto iter = clients_.begin(); iter != clients_.end();) {
        if (iter->second->need_write() && fd_write_.has(iter->second->sockfd())) {
            if (SOCKET_ERROR == iter->second->send_data_real()) {
                on_leave(iter->second);
                auto iter_old = iter;
                iter++;
                clients_.erase(iter_old);
                continue;
            }
        }
        iter++;
    }
#endif
}