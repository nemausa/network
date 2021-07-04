#include "cell_server.hpp"
#include <functional>


int observer::static_number_ = 0;

cell_server::cell_server(SOCKET sockfd, observer* _observer) {
    sockfd_ = sockfd;
    observer_ = _observer;
    attach(observer_);
} 

cell_server::~cell_server() {
    close();
    sockfd_ = INVALID_SOCKET;
}

void cell_server::close() {
    if (sockfd_ == INVALID_SOCKET) {
#ifdef _WIN32
        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            closesocket(clients_[n]->sockfd());
            delete clients_[n];
        }
        closesocket(sockfd_);
        WSACleanup();
#else
        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            ::close(clients_[n]->sockfd());
            delete clients_[n];
        }
        ::close(sockfd_);
#endif
        clients_.clear();
    }
}

bool cell_server::is_run() {
    return sockfd_ != INVALID_SOCKET;
}


bool cell_server::on_run() {
    while (is_run()) {
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
            continue;
        }

        fd_set fd_read;
        FD_ZERO(&fd_read);
        if (client_change_) {
            client_change_ = false;
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

        int ret = select(max_socket_ + 1, &fd_read, nullptr, nullptr, nullptr);
        if (ret < 0) {
            std::cout << "select task end" << std::endl;
            close();
            return false;
        }

#if _WIN32
        for (int n = 0; n < fd_read.fd_count; n++) {
            auto iter = clients_.find(fd_read.fd_array[n]);
            if (iter != clients_.end()) {
                if (-1 == recv_data(iter->second)) {
                    create_message("leave");
                    client_change_ = true;
                    delete iter->second;
                    closesocket(iter->first);
                    clients_.erase(iter);
                }
            } else {
                printf("error iter != clients_.end()\n");
            }
        }
#else
        std::vector<cell_client*> temp;
        for (auto iter : clients_) {
            if (FD_ISSET(iter.second->sockfd(), &fd_read)) {
                if (-1 == recv_data(iter.second)) {
                    client_change_ = true;
                    create_message("leave");
                    ::close(iter.first);
                    temp.push_back(iter.second);
                }
            }
        }
        for (auto client : temp) {
            clients_.erase(client->sockfd());
            delete client;
        }
#endif 
        check_time();
    }
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
        // client->send_data(&ret);
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

void cell_server::start() {
    thread_ = std::thread(std::mem_fn(&cell_server::on_run), this);
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
            create_message("leave");
            client_change_ = true;
            delete iter->second;
            auto iterold = iter;
            iter++;
            clients_.erase(iterold);
            continue;
        }
        iter->second->check_send_time(dt);
        iter++;
    }
}