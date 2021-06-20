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
                clients_.push_back(client);
            }
            clients_buff_.clear();
        }

        if (clients_.empty()) {
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);
            continue;
        }

        fd_set fd_read;
        FD_ZERO(&fd_read);
        SOCKET max_socket = clients_[0]->sockfd();
        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            FD_SET(clients_[n]->sockfd(), &fd_read);
            if (max_socket < clients_[n]->sockfd()) {
                max_socket = clients_[n]->sockfd();
            }
        }

        int ret = select(max_socket + 1, &fd_read, nullptr, nullptr, nullptr);
        if (ret < 0) {
            std::cout << "select task end" << std::endl;
            close();
            return false;
        }

        for (int n= (int)clients_.size() - 1; n >= 0; n--) {
            if (FD_ISSET(clients_[n]->sockfd(), &fd_read)) {
                if (-1 == recv_data(clients_[n])) {
                    auto iter = clients_.begin() + n;
                    if (iter != clients_.end()) {
                        create_message("leave");
                        delete clients_[n];
                        clients_.erase(iter);
                    }
                }
            }
        }


    }
}


int cell_server::recv_data(client_socket *client) {
    
    int len = (int)recv(client->sockfd(), sz_recv_, RECV_BUFF_SIZE, 0);
    if (len <= 0) {
        printf("client<socket=%d> cloes, task end\n", client->sockfd());
        return -1;
    }
    memcpy(client->msg_buf() + client->get_pos(), sz_recv_, len);
    client->set_pos(client->get_pos() + len);
    while (client->get_pos() >= sizeof(data_header)) {
        data_header *header = (data_header*)client->msg_buf();
        if (client->get_pos() >= header->length) {
            int size = client->get_pos() - header->length;
            on_msg(client->sockfd(), header);
            memcpy(client->msg_buf(), client->msg_buf() + header->length, size);
            client->set_pos(size);
        } else {
            break;
        }
    }
    return 0;
}

void cell_server::on_msg(SOCKET c_sock, data_header *header) {
    create_message("msg");
}

void cell_server::add_client(client_socket* client) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_buff_.push_back(client);
}

void cell_server::start() {
    thread_ = std::thread(std::mem_fn(&cell_server::on_run), this);
}

size_t cell_server::count() {
    return clients_.size() + clients_buff_.size();
}