#include "depends/tcp_server.hpp"
#include "depends/network.hpp"
#include "utils/conf.hpp"

namespace nemausa {
namespace io {

tcp_server::tcp_server() {
    sockfd_ = INVALID_SOCKET;
    recv_count_ = 0;
    message_count_ = 0;
    client_count_ = 0;
    send_buffer_size_ = config::instance().get_int_default("send_buffer_szie", 
            SEND_BUFF_SIZE);
    recv_buffer_size_ = config::instance().get_int_default("recv_buffer_szie",
            RECV_BUFF_SIZE);
    max_client_ = config::instance().get_int_default("max_client", 102400);
}

tcp_server::~tcp_server() {
    close();
}

SOCKET tcp_server::init_socket() {
    network::init();
    if (INVALID_SOCKET != sockfd_) {
        SPDLOG_LOGGER_WARN(spdlog::get(LOG_NAME), 
                "<socket={}>close old socket...", (int)sockfd_);
        close();
    }
    sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sockfd_) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "error, create socket failed");
    } else {
        network::make_reuseaddr(sockfd_);
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
                "create socket=<{}> success", (int)sockfd_);
    }
    return sockfd_;
}

int tcp_server::bind(const char *ip, unsigned short port) {
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
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "error, bind port<{}> failed", port);
    } else {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
                "bind port<{}> success", port);
    }
    return ret;
}

int tcp_server::listen(int n) {
    int ret = ::listen(sockfd_, n);

    if (SOCKET_ERROR == ret) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
            "socket=<{}> error, listen port failed", sockfd_);
    } else {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME),
                "socket=<{}> listen port success", sockfd_);
    }
    return ret;
}

SOCKET tcp_server::accept() {
    sockaddr_in client_addr = {};
    int length = sizeof(sockaddr_in);
    SOCKET c_sock = INVALID_SOCKET;
#ifdef _WIN32
    c_sock = ::accept(sockfd_, (sockaddr*)&client_addr, &length);
#else
    c_sock = ::accept(sockfd_, (sockaddr*)&client_addr, (socklen_t*)&length);
#endif
    if (INVALID_SOCKET == c_sock) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "socket=<{}> error, accept invalid socket\n",(int)sockfd_);
    } else {
        if (client_count_ < max_client_) {
            network::make_reuseaddr(c_sock);
            add_client_to_server(new client(c_sock, send_buffer_size_, recv_buffer_size_));
        } else {
            network::destory_socket(c_sock);;
            SPDLOG_LOGGER_WARN(spdlog::get(LOG_NAME), "accept to max_client");
        }
    }
    return c_sock;
}

void tcp_server::add_client_to_server(client *client) {
    auto min_server =  servers_[0];
    for (auto server : servers_) {
        if (min_server->client_count() > server->client_count()) {
            min_server = server;
        }
    }
    min_server->add_client(client);
}

void tcp_server::close() {
    SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "tcp_server close begin");
    thread_.close();
    if (sockfd_ != INVALID_SOCKET) {
        for (auto s : servers_) {
            delete s;
        }
        servers_.clear();
        network::destory_socket(sockfd_);
        sockfd_ = INVALID_SOCKET;
    }
    SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "tcp_server close end");
}

void tcp_server::on_join(client *pclient) {
    client_count_++;
}
void tcp_server::on_leave(client *pclient) {
    client_count_--;
}
void tcp_server::on_msg(server *pserver, client *pclient, data_header *header) {
    message_count_++;
}
void tcp_server::on_recv(client *pclient) {
    recv_count_++;
}
void tcp_server::time4msg() {
    auto t1 = time_.second();
    if (t1 >= 1.0f) {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
            "cell_thread<{}>, time<{:02.4f}>, socket<{}>, client_count<{}>, recv_count<{}>, message<{}>",
            servers_.size(), t1, sockfd_, (int)client_count_, (int)recv_count_, (int)message_count_); 
        recv_count_  = 0;
        message_count_ = 0;
        time_.update();
    }
}

int tcp_server::sockfd() {
    return sockfd_;
}

} // namespace io 
} // namespace nemausa