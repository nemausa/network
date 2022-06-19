#include <functional>

#include "depends/cell.hpp"
#include "depends/client.hpp"
#include "depends/server.hpp"
#include "depends/net_event.hpp"

namespace nemausa {
namespace io {

server::server() {
}

server::~server() {
    // //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "id={}", id_);
    close();
    // //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "id={}", id_);
}

void server::set_id(int id) {
    id_ = id;
    task_server_.service_id_ = id;
}

void server::set_client_num(int socket_num) {

}

void server::set_event(net_event *event) {
    p_net_event_ = event;
}

void server::close() {
    //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "close id={}", id_);
    task_server_.close();
    thread_.close();
    //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "close id={}", id_);
}

void server::start() {
    task_server_.start();
    thread_.start(nullptr,
        [this](cell_thread *pthread) {
            on_run(pthread);
        },
        [this](cell_thread *pthread) {
            clear_client();
        }
    );
}

void server::on_run(cell_thread *pthread) {
    while (pthread->is_run()) {
        if (!clients_buff_.empty()) {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto pclient : clients_buff_){
                clients_[pclient->sockfd()] = pclient;
                pclient->service_id_ = id_;
                if (p_net_event_) {
                    p_net_event_->on_join(pclient);
                }
                on_join(pclient);
            }
            clients_buff_.clear();
            client_change_ = true;
        }

        if (clients_.empty()) {
            cell_thread::sleep(1);
            old_clock_ = timestamp::now_milliseconds();
            continue;
        }

        check_time();
        if (!do_net_events()) {
            pthread->exit();
            break;
        }
        do_msg();
    }
    //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
            // "server{} on run exit", id_);
}

void server::check_time() {
    auto now = timestamp::now_milliseconds();
    auto dt = now -  old_clock_;
    old_clock_ = now;
    client *pclient = nullptr;
    for (auto iter = clients_.begin(); iter != clients_.end(); ) {
        pclient = iter->second;
        if (pclient->check_heart_time(dt)) {
#ifdef _WIN32
            if (pclient->is_post_action()) {
                pclient->destory();
            } else {
                on_leave(pclient);
            }
#else
            on_leave(pclient);
#endif
            iter = clients_.erase(iter);
            continue;
        }
        iter++;
    }
}

void server::on_leave(client *pclient) {
    if (p_net_event_)
        p_net_event_->on_leave(pclient);
    client_change_ = true;
    delete pclient;
}

void server::on_join(client *pclient) {

}

void server::on_recv(client *pclient) {
    if (p_net_event_) {
        p_net_event_->on_recv(pclient);
    }
}

void server::do_msg() {
    client *pclient = nullptr;
    for (auto iter : clients_) {
        pclient = iter.second;
        while (pclient->has_msg()) {
            on_msg(pclient, pclient->front_msg());
            pclient->pop_msg();
        }
    }
}

int server::recv_data(client *pclient) {
    int len = pclient->recv_data();
    if (p_net_event_) {
        p_net_event_->on_recv(pclient);
    }
    return len;
}

void server::on_msg(client *pclient, data_header *header) {
    if (p_net_event_) {
        p_net_event_->on_msg(this, pclient, header);
    }
}

void server::add_client(client* pclient) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_buff_.push_back(pclient);
}

void server::clear_client() {
    for (auto iter : clients_) {
        delete iter.second;
    }
    for (auto iter : clients_buff_) {
        delete iter;
    }

    clients_.clear();
    clients_buff_.clear();

}

size_t server::client_count() {
    return clients_.size() + clients_buff_.size();
}

} // namespace io 
} // namespace nemausa