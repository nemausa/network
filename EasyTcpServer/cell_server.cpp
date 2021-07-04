#include "cell_server.hpp"
#include <functional>


int observer::static_number_ = 0;

cell_server::cell_server(int id, observer* _observer) {
    observer_ = _observer;
    attach(observer_);
    id_ = id;
    task_server_.service_id_ = id;
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

bool cell_server::on_run(cell_thread *pthread) {
    while (pthread->is_run()) {
        if (clients_buff_.size() > 0) {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto client : clients_buff_){
                clients_[client->sockfd()] = client;
            }
            clients_buff_.clear();
            client_change_ = true;
        }

        if (clients_.empty()) {
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);
            old_clock_ = timestamp::now_milliseconds();
            continue;
        }

        fd_set fd_read;
        fd_set fd_write;
        if (client_change_) {
            client_change_ = false;
            FD_ZERO(&fd_read);
            max_socket_ = clients_.begin()->first;
            for (auto iter : clients_) {
                FD_SET(iter.first, &fd_read);
                if (max_socket_ < iter.first) {
                    max_socket_ = iter.first;
                }
            }
            memcpy(&fd_back_, &fd_read, sizeof(fd_set));
        } else {
            memcpy(&fd_read, &fd_back_, sizeof(fd_set));
        }
        memcpy(&fd_write, &fd_read, sizeof(fd_set));
        timeval t{0, 1};
        int ret = select(max_socket_ + 1, &fd_read, &fd_write, nullptr, &t);
        if (ret < 0) {
            std::cout << "select task end" << std::endl;
            pthread->exit();
            break;
        }

        read_data(fd_read);
        write_data(fd_write);
        check_time();
    }
}


void cell_server::read_data(fd_set &fd_read) {
#if _WIN32
        for (int n = 0; n < fd_read.fd_count; n++) {
            auto iter = clients_.find(fd_read.fd_array[n]);
            if (iter != clients_.end()) {
                if (-1 == recv_data(iter->second)) {
                    on_leave(iter->second);
                    clients_.erase(iter);
                }
            } else {
                printf("error iter != clients_.end()\n");
            }
        }
#else
        for (auto iter = clients_.begin(); iter != clients_.end();) {
            if (FD_ISSET(iter->second->sockfd(), &fd_read)) {
                if (-1 == recv_data(iter->second)) {
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

void cell_server::write_data(fd_set &fd_write) {
#ifdef _WIN32
    for (int n = 0; n < fd_write.fd_count; n++) {
        auto iter = clients_.find(fd_write.fd_array[n]);
        if (iter != clients_.end()) {
            if (-1 == iter->second->send_data_real()) {
                on_leave(iter->second);
                clients_.erase(iter);
            }
        }
    }
#else
    for (auto iter = clients_.begin(); iter != clients_.end();) {
        if (FD_ISSET(iter->second->sockfd(), &fd_write)) {
            if (-1 == iter->second->send_data_real()) {
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

int cell_server::recv_data(cell_client *client) {
    
    char *sz_recv = client->msg_buf() + client->get_pos();
    int len = (int)recv(client->sockfd(), sz_recv, RECV_BUFF_SIZE - client->get_pos(), 0);
    create_message("recv");
    if (len <= 0) {
        // printf("client<socket=%d> cloes, task end\n", client->sockfd());
        return -1;
    }
    client->set_pos(client->get_pos() + len);
    while (client->get_pos() >= sizeof(data_header)) {
        data_header *header = (data_header*)client->msg_buf();
        if (client->get_pos() >= header->length) {
            int size = client->get_pos() - header->length;
            on_msg(client, header);
            memcpy(client->msg_buf(), client->msg_buf() + header->length, size);
            client->set_pos(size);
        } else {
            break;
        }
    }
    return 0;
}

void cell_server::on_msg(cell_client *client, data_header *header) {
    create_message("msg");
    switch (header->cmd) {
    case CMD_LOGIN: {
        client->reset_heart_time();
        login *lg = (login*)header;
        login_result ret ;
        if (SOCKET_ERROR == client->send_data(&ret)) {
            printf("socket=%d\n", client->sockfd());
        }
    }
    break;
    case CMD_LOGOUT: {

    }
    break;
    case CMD_C2S_HEART: {
        client->reset_heart_time();
        s2c_heart ret;
        client->send_data(&ret);
    }
    break;
    default: {
        printf("socket=%d receive undefine command, length=%d\n", client->sockfd(), header->length);
    }
    break;
    }
}

void cell_server::add_client(cell_client* client) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_buff_.push_back(client);
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

void cell_server::on_leave(cell_client *pclient) {
    create_message("levae");
    client_change_ = true;
    delete pclient;
}