#include "depends/easy_tcp_server.hpp"
#include "depends/cell_network.hpp"
#include "depends/cell_config.hpp"

easy_tcp_server::easy_tcp_server() {
    sockfd_ = INVALID_SOCKET;
    recv_count_ = 0;
    message_count_ = 0;
    client_count_ = 0;
    send_buffer_size_ = cell_config::instance().get_int("send_buffer_szie", SEND_BUFF_SIZE);
    recv_buffer_size_ = cell_config::instance().get_int("recv_buffer_szie", RECV_BUFF_SIZE);
    max_client_ = cell_config::instance().get_int("max_client", 102400);
}

easy_tcp_server::~easy_tcp_server() {
    close();
}

SOCKET easy_tcp_server::init_socket() {
    cell_network::init();
    if (INVALID_SOCKET != sockfd_) {
        LOG_WARN("<socket=%d>close old socket...\n", (int)sockfd_);
        close();
    }
    sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sockfd_) {
        LOG_PERROR("error, create socket failed\n");
    } else {
        cell_network::make_reuseadd(sockfd_);
        LOG_INFO("create socket=<%d> succes\n", (int)sockfd_);
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
        LOG_PERROR("error, bind port<%d> failed\n", port);
    } else {
        LOG_INFO("bind port<%d> success\n", port);
    }
    return ret;
}

int easy_tcp_server::listen(int n) {
    int ret = ::listen(sockfd_, n);

    if (SOCKET_ERROR == ret) {
        LOG_PERROR("socket=<%d> error, listen port failed\n", sockfd_);
    } else {
        LOG_INFO("socket=<%d> listen port success\n", sockfd_);
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
        LOG_PERROR("socket=<%d> error, accept invalid socket\n",(int)sockfd_);
    } else {
        if (client_count_ < max_client_) {
            cell_network::make_reuseadd(c_sock);
            add_client_to_server(new cell_client(c_sock));
        } else {
            cell_network::destory_socket(c_sock);;
            LOG_WARN("accept to max_client");
        }
    }
    return c_sock;
}

void easy_tcp_server::add_client_to_server(cell_client *client) {
    auto min_server =  cell_servers_[0];
    for (auto server : cell_servers_) {
        if (min_server->client_count() > server->client_count()) {
            min_server = server;
        }
    }
    min_server->add_client(client);
}

void easy_tcp_server::close() {
    LOG_INFO("easy_tcp_server close begin");
    thread_.close();
    if (sockfd_ != INVALID_SOCKET) {
        for (auto s : cell_servers_) {
            delete s;
        }
        cell_servers_.clear();
        cell_network::destory_socket(sockfd_);
        sockfd_ = INVALID_SOCKET;
    }
    LOG_INFO("easy_tcp_server close end");
}

void easy_tcp_server::on_join(cell_client *pclient) {
    client_count_++;
}
void easy_tcp_server::on_leave(cell_client *pclient) {
    client_count_--;
}
void easy_tcp_server::on_msg(cell_server *pserver, cell_client *pclient, data_header *header) {
    message_count_++;
}
void easy_tcp_server::on_recv(cell_client *pclient) {
    recv_count_++;
}
void easy_tcp_server::time4msg() {
    auto t1 = time_.second();
    if (t1 >= 1.0f) {
        LOG_INFO("thread<%d>, time<%f>, socket<%d>, client_count<%d>, recv_count<%d>, message<%d>",
            cell_servers_.size(), t1, sockfd_, (int)client_count_, (int)recv_count_, (int)message_count_); 
        recv_count_  = 0;
        message_count_ = 0;
        time_.update();
    }
}

int easy_tcp_server::sockfd() {
    return sockfd_;
}