#include "depends/select_server.hpp"

namespace nemausa {
namespace io {

select_server::~select_server() {

}

bool select_server::do_net_events() {
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
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "");
    } else if (ret == 0) {
        return true;
    }
    read_data();
    write_data();
    return true;
}

void select_server::read_data() {
#ifdef _WIN32
    auto pfdset = fd_read_.fdset();
    for (int n = 0; n < pfdset->fd_count; n++) {
        auto iter = clients_.find(pfdset->fd_array[n]);
        if (iter != clients_.end()) {
            if (-1 == recv_data(iter->second)) {
                on_leave(iter->second);
                clients_.erase(iter);
            }
        } else {
            SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                    "error iter != clients_.end()");
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

void select_server::write_data() {
#ifdef _WIN32
    auto pfdset = fd_write_.fdset();
    for (int n = 0; n < pfdset->fd_count; n++) {
        auto iter = clients_.find(pfdset->fd_array[n]);
        if (iter != clients_.end()) {
            if (SOCKET_ERROR == iter->second->send_data_real()) {
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

} // namespace io 
} // namespace nemausa