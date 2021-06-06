#include "easy_tcp_server.hpp"

easy_tcp_server::easy_tcp_server() {
    sock_ = INVALID_SOCKET;
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

    if (INVALID_SOCKET != sock_) {
        printf("<socket=%d>close old socket...\n", (int)sock_);
        close();
    }
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock_) {
        printf("error, create socket failed\n");
    } else {
        printf("create socket=<%d> succes\n", (int)sock_);
    }
    return sock_;
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

    int ret = ::bind(sock_, (sockaddr*)&sin, sizeof(sockaddr));
    if (SOCKET_ERROR == ret) {
        printf("error, bind port<%d> failed\n", port);
    } else {
        printf("bind port<%d> success\n", port);
    }
    return ret;
}

int easy_tcp_server::listen(int n) {
    int ret = ::listen(sock_, n);

    if (SOCKET_ERROR == ret) {
        printf("socket=<%d> error, listen port failed\n", sock_);
    } else {
        printf("socket=<%d> listen port success\n", sock_);
    }
    return ret;
}

SOCKET easy_tcp_server::accept() {
    sockaddr_in client_addr = {};
    int length = sizeof(sockaddr_in);
    SOCKET c_sock = INVALID_SOCKET;
#ifdef _WIN32
    c_sock = ::accept(sock_, (sockaddr*)&client_addr, &length);
#else
    c_sock = ::accept(sock_, (sockaddr*)&client_addr, (socklen_t*)&length);
#endif
    if (INVALID_SOCKET == c_sock) {
        printf("socket=<%d> error, accept invalid socket\n",(int)sock_);
    } else {
        new_join join;
        join.sock = c_sock;
        send_data_to_all(&join);
        clients_.push_back(c_sock);
        printf("socket=<%d> new client socket = %d, ip = %s \n", (int)sock_, (int)c_sock, inet_ntoa(client_addr.sin_addr));
    }
    return c_sock;
}

void easy_tcp_server::close() {
    if (sock_ == INVALID_SOCKET) {
#ifdef _WIN32
        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            closesocket(clients_[n]);
        }
        closesocket(sock_);
        WSACleanup();
#else
        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            ::close(clients_[n]);
        }
        ::close(sockd_);
#endif
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

        FD_SET(sock_, &fd_read);
        FD_SET(sock_, &fd_write);
        FD_SET(sock_, &fd_exp);
        SOCKET max_sock = sock_;
        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            FD_SET(clients_[n], &fd_read);
            if (max_sock < clients_[n]) {
                max_sock = clients_[n];
            }
        }

        int ret = select(max_sock + 1, &fd_read, &fd_write, &fd_exp, nullptr);
        if (ret < 0) {
            printf("select task end\n");
            close();
        }

        if (FD_ISSET(sock_, &fd_read)) {
            FD_CLR(sock_, &fd_read);
            accept();
        }

        for (int n = (int)clients_.size() - 1; n >= 0; n--) {
            if (FD_ISSET(clients_[n], &fd_read)) {
                if (-1 == recv_data(clients_[n])) {
                    auto iter = clients_.begin() + n;
                    if (iter != clients_.end()) {
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
    return sock_ != INVALID_SOCKET;
}


int easy_tcp_server::recv_data(SOCKET c_sock) {
    char sz_recv[4096] = {};
    int len = (int)recv(c_sock, sz_recv, 409600, 0);
    data_header *header = (data_header*)sz_recv;
    if (len <= 0) {
        printf("client<socket=%d> cloes, task end\n", c_sock);
        return -1;
    }
    recv(sock_, sz_recv + sizeof(data_header), header->length - sizeof(data_header), 0);
    on_msg(c_sock, header);
    return 0;
}

void easy_tcp_server::on_msg(SOCKET c_sock, data_header *header) {
    int header_len = sizeof(data_header);
    switch (header->cmd) {
    case CMD_LOGIN: {
        login *l = (login*)header;
        printf("receive cmd: CMD_LOGIN,data length=%d, username=%s, password=%s\n", l->length, l->user_name, l->password);
        login_result ret;
        send_data(c_sock, &ret);
    }
    break;
    case CMD_LOGOUT: {
        logout *l = (logout*)header;
        printf("receive cmd: CMD_LOGOUT,data length=%d, username=%s \n", l->length, l->user_name);
        logout_result ret;
        send_data(c_sock, &ret);
    }
    break;
    default: {
        data_header header = {0, CMD_ERROR};
        send(c_sock, (char*)&header, sizeof(header), 0);
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
        send_data(clients_[n], header);
    }
}



