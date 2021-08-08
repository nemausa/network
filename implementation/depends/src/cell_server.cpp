#include <functional>

#include "depends/cell.hpp"

#include "depends/cell_client.hpp"

#include "depends/cell_server.hpp"

#include "depends/net_event.hpp"

int observer::static_number_ = 0;

cell_server::cell_server() {

}

cell_server::~cell_server() {
    close();
}

void cell_server::set_id(int id) {
    id_ = id;
    task_server_.service_id_ = id;
}

void cell_server::set_event(net_event *event) {
    p_net_event_ = event;
}

void cell_server::close() {
    task_server_.close();
    thread_.close();
}

void cell_server::start() {
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

void cell_server::on_run(cell_thread *pthread) {
    while (pthread->is_run()) {
        if (clients_buff_.size() > 0) {
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
            old_clock_ = cell_timestamp::now_milliseconds();
            continue;
        }

        check_time();
        if (!do_net_events()) {
            pthread->exit();
            break;
        }
        do_msg();
    }
    LOG_INFO("cell_server%d on run exit", id_);
}

void cell_server::check_time() {
    auto now = cell_timestamp::now_milliseconds();
    auto dt = now -  old_clock_;
    old_clock_ = now;
    cell_client *pclient = nullptr;
    for (auto iter = clients_.begin(); iter != clients_.end(); ) {
        pclient = iter->second;
        if (pclient->check_heart_time(dt)) {
#if _WIN32
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

void cell_server::on_leave(cell_client *pclient) {
    if (p_net_event_)
        p_net_event_->on_leave(pclient);
    client_change_ = true;
    delete pclient;
}

void cell_server::on_join(cell_client *pclient) {

}

void cell_server::on_recv(cell_client *pclient) {
    if (p_net_event_) {
        p_net_event_->on_recv(pclient);
    }
}

void cell_server::do_msg() {
    cell_client *pclient = nullptr;
    for (auto iter : clients_) {
        pclient = iter.second;
        while (pclient->has_msg()) {
            on_msg(pclient, pclient->front_msg());
            pclient->pop_msg();
        }
    }
}

int cell_server::recv_data(cell_client *pclient) {
    int len = pclient->recv_data();
    if (p_net_event_) {
        p_net_event_->on_recv(pclient);
    }
    return len;
}

void cell_server::on_msg(cell_client *pclient, data_header *header) {
    if (p_net_event_) {
        p_net_event_->on_msg(this, pclient, header);
    }
}

void cell_server::add_client(cell_client* pclient) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_buff_.push_back(pclient);
}

void cell_server::clear_client() {
    for (auto iter : clients_) {
        delete iter.second;
    }
    for (auto iter : clients_buff_) {
        delete iter;
    }

    clients_.clear();
    clients_buff_.clear();

}

size_t cell_server::client_count() {
    return clients_.size() + clients_buff_.size();
}
