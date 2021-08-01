#include <functional>
#include "cell.hpp"
#include "cell_client.hpp"
#include "cell_server.hpp"

int observer::static_number_ = 0;

cell_server::cell_server() {

}

cell_server::~cell_server() {
    close();
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

void cell_server::close() {
    task_server_.close();
    thread_.close();
}

void cell_server::on_run(cell_thread *pthread) {
    while (pthread->is_run()) {
        if (clients_buff_.size() > 0) {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto pclient : clients_buff_){
                clients_[pclient->sockfd()] = pclient;
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
        if (!do_select()) {
            pthread->exit();
            break;
        }
        do_msg();
    }
    cell_log::info("cell_server%d on run exit", id_);
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
    if (len <= 0) {
        return -1;
    }   
    create_message("recv");
    while (pclient->has_msg()) {
        on_msg(pclient, pclient->front_msg());
        pclient->pop_msg();
    } 
    return 0;
}


void cell_server::on_msg(cell_client *pclient, data_header *header) {
    create_message("msg");
    switch (header->cmd) {
    case CMD_LOGIN: {
        pclient->reset_heart_time();
        login *lg = (login*)header;
        login_result ret ;
        if (SOCKET_ERROR == pclient->send_data(&ret)) {
            cell_log::info("socket=%d\n", pclient->sockfd());
        }
    }
    break;
    case CMD_LOGOUT: {

    }
    break;
    case CMD_C2S_HEART: {
        pclient->reset_heart_time();
        s2c_heart ret;
        pclient->send_data(&ret);
    }
    break;
    default: {
        cell_log::info("socket=%d receive undefine command, length=%d\n", pclient->sockfd(), header->length);
    }
    break;
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



size_t cell_server::count() {
    return clients_.size() + clients_buff_.size();
}

void cell_server::check_time() {
    auto now = timestamp::now_milliseconds();
    auto dt = now -  old_clock_;
    old_clock_ = now;
    for (auto iter = clients_.begin(); iter != clients_.end(); ) {
        if (iter->second->check_heart_time(dt)) {
            on_leave(iter->second);
            auto iterold = iter;
            iter++;
            clients_.erase(iterold);
            continue;
        }
        // iter->second->check_send_time(dt);
        iter++;
    }
}

void on_client_leave(cell_client *pclient) {
    if (p_netevent_)
        p_netevent_->on_leave(pclient);
    client_change_ = true;
    delete pclient;
}
void cell_server::on_leave(cell_client *pclient) {
    create_message("leave");
    client_change_ = true;
    delete pclient;
}

void cell_server::set_id(int id) {
    id_ = id;
    task_server_.service_id_ = id;
}