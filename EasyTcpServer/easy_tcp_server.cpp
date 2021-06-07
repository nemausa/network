#include "easy_tcp_server.hpp"

#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 102400
#endif

class client_socket {
public:
    client_socket(SOCKET sockfd = INVALID_SOCKET) {
        sockfd_ = sockfd;
        memset(sz_msg_buf, 0, sizeof(sz_msg_buf));
        last_pos_ = 0;
    }

    SOCKET sockfd() {
        return sockfd_;
    }

    char *msg_buf() {
        return sz_msg_buf;
    }

    int get_pos() {
        return last_pos_;
    }

    void set_pos(int pos) {
        last_pos_ = pos;
    }
private:
    SOCKET sockfd_;
    char sz_msg_buf[RECV_BUFF_SIZE * 10];
    int last_pos_;
};


easy_tcp_server::easy_tcp_server() {
    sockfd_ = INVALID_SOCKET;
}

easy_tcp_server::~easy_tcp_server() {
    close();
}

SOCKET easy_tcp_server::init_socket() {
#ifdef _WIN32   
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif

    if (INVALID_SOCKET != sockfd_) {
        printf("<socket=%d>close old socket...\n", (int)sockfd_);
        close();
    }
    sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sockfd_) {
        printf("error, create socket failed\n");
    } else {
        printf("create socket=<%d> succes\n", (int)sockfd_);
    }
    return sockfd_;
}

int easy_tcp_server::bind(const char *ip, unsigned short port) {
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

#ifdef _WIN32
    if (ip) {
        sin.sin_addr.S_un.S_addr = inet_addr(ip);
    } else {
        sin.sin_addr.S_un.S_addr = INADDR_ANY;
    }
#else
    if (ip) {
        sin.sin_addr.s_addr = inet_addr(ip);
    } else {
        sin.sin_addr.s_addr = INADDR_ANY;
    }
#endif

    int ret = ::bind(sockfd_, (sockaddr*)&sin, sizeof(sockaddr));
    if (SOCKET_ERROR == ret) {
        printf("error, bind port<%d> failed\n", port);
    } else {
        printf("bind port<%d> success\n", port);
    }
    return ret;
}

int easy_tcp_server::listen(int n) {
    int ret = ::listen(sockfd_, n);

    if (SOCKET_ERROR == ret) {
        printf("socket=<%d> error, listen port failed\n", sockfd_);
    } else {
        printf("socket=<%d> listen port success\n", sockfd_);
    }
    return ret;
}

SOCKET easy_tcp_server::accept() {
    sockaddr_in client_addr = {};
    int length = sizeof(sockaddr_in);
    SOCKET c_sock = INVALID_SOCKET;
#ifdef _WIN32
    c_sock = ::accept(sockfd_, (sockaddr*)&client_addr, &length);
#else
    c_sock = ::accept(sockfd_, (sockaddr*)&client_addr, (socklen_t*)&length);
#endif
    if (INVALID_SOCKET == c_sock) {
        printf("socket=<%d> error, accept invalid socket\n",(int)sockfd_);
    } else {
        new_join join;
        join.sock = c_sock;
        send_data_to_all(&join);
        clients_.push_back(new client_socket(c_sock));
        printf("socket=<%d> new client socket = %d, ip = %s \n", (int)sockfd_, (int)c_sock, inet_ntoa(client_addr.sin_addr));
    }
    return c_sock;
}

void easy_tcp_server::close() {
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

bool easy_tcp_server::on_run() {
    if (is_run()) {
        fd_set fd_read;
        fd_set fd_write;
        fd_set fd_exp;

        FD_ZERO(&fd_read);
        FD_ZERO(&fd_write);
        FD_ZERO(&fd_exp);

        FD_SET(sockfd_, &fd_read);
        FD_SET(sockfd_, &fd_write);
        FD_SET(sockfd_, &fd_exp);
        SOCKET max_sock = sockfd_;
        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            FD_SET(clients_[n]->sockfd(), &fd_read);
            if (max_sock < clients_[n]->sockfd()) {
                max_sock = clients_[n]->sockfd();
            }
        }

        timeval t = {1, 0};
        int ret = select(max_sock + 1, &fd_read, &fd_write, &fd_exp, &t);
        if (ret < 0) {
            printf("select task end\n");
            close();
            return false;
        }

        if (FD_ISSET(sockfd_, &fd_read)) {
            FD_CLR(sockfd_, &fd_read);
            accept();
        }

        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            if (FD_ISSET(clients_[n]->sockfd(), &fd_read)) {
                if (-1 == recv_data(clients_[n])) {
                    auto iter = clients_.begin() + n;
                    if (iter != clients_.end()) {
                        delete clients_[n];
                        clients_.erase(iter);
                    }
                }
            }
        }
        return true;
    }
    return false;
}

bool easy_tcp_server::is_run() {
    return sockfd_ != INVALID_SOCKET;
}

char sz_recv[RECV_BUFF_SIZE] = {};
int easy_tcp_server::recv_data(client_socket *client) {
    
    int len = (int)recv(client->sockfd(), sz_recv, RECV_BUFF_SIZE, 0);
    if (len <= 0) {
        printf("client<socket=%d> cloes, task end\n", client->sockfd());
        return -1;
    }
    memcpy(client->msg_buf() + client->get_pos(), sz_recv, len);
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

void easy_tcp_server::on_msg(SOCKET c_sock, data_header *header) {
    switch (header->cmd) {
    case CMD_LOGIN: {
        login *l = (login*)header;
        // printf("receive cmd: CMD_LOGIN,data length=%d, username=%s, password=%s\n", l->length, l->user_name, l->password);
        login_result ret;
        send_data(c_sock, &ret);
    }
    break;
    case CMD_LOGOUT: {
        logout *l = (logout*)header;
        // printf("receive cmd: CMD_LOGOUT,data length=%d, username=%s \n", l->length, l->user_name);
        logout_result ret;
        send_data(c_sock, &ret);
    }
    break;
    default: {
        printf("receive cmd: undefined,data length=%d\n", header->length);
        // data_header header = {0, CMD_ERROR};
        // send(c_sock, (char*)&header, sizeof(header), 0);
    }
    break;
    }
}

int easy_tcp_server::send_data(SOCKET c_sock, data_header *header) {
    if (is_run() && header) {
        return send(c_sock, (const char*)header, header->length, 0);
    }
    return SOCKET_ERROR;
}

void easy_tcp_server::send_data_to_all(data_header *header) {
    for (int n = (int)clients_.size() -1; n >= 0; n--) {
        send_data(clients_[n]->sockfd(), header);
    }
}



