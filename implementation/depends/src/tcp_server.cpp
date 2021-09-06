#include "depends/tcp_server.hpp"
#include "depends/network.hpp"
#include "utils/conf.hpp"

namespace nemausa {
namespace io {

tcp_server::tcp_server() {
    sockfd_ = INVALID_SOCKET;
    recv_count_ = 0;
    message_count_ = 0;
    client_join_ = 0;
    client_accept_ = 0;
    send_buffer_size_ = config::instance().get_int_default("send_buffer_szie", 
            SEND_BUFF_SIZE);
    recv_buffer_size_ = config::instance().get_int_default("recv_buffer_szie",
            RECV_BUFF_SIZE);
    max_client_ = config::instance().get_int_default("max_client", 102400);
}

tcp_server::~tcp_server() {
    close();
}

SOCKET tcp_server::init_socket(int af) {
    network::init();
    if (INVALID_SOCKET != sockfd_) {
        SPDLOG_LOGGER_WARN(spdlog::get(MULTI_SINKS), 
                "<socket={}>close old socket...", (int)sockfd_);
        close();
    }
    af_ = af;
    sockfd_ = socket(af, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sockfd_) {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), 
                "error, create socket failed");
    } else {
        network::make_reuseaddr(sockfd_);
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                "create socket=<{}> success", (int)sockfd_);
    }
    return sockfd_;
}

int tcp_server::bind(const char *ip, unsigned short port) {
    int ret = SOCKET_ERROR;
    if (AF_INET == af_) {
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
        ret = ::bind(sockfd_, (sockaddr*)&sin, sizeof(sin));
    } else if(AF_INET6 == af_){
        sockaddr_in6 sin = {};
        sin.sin6_family = AF_INET6;
        sin.sin6_port = htons(port);
        if (ip) {
            inet_pton(AF_INET6, ip, &sin.sin6_addr);
        } else {
            sin.sin6_addr = in6addr_any;
        }
        ret = ::bind(sockfd_, (sockaddr*)&sin, sizeof(sin));
    } else {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), 
                "bind port,addres family{} failed...", af_);
    }

    if (SOCKET_ERROR == ret) {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), 
                "error, bind port<{}> failed", port);
    } else {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                "bind port<{}> success", port);
    }
    return ret;
}

int tcp_server::listen(int n) {
    int ret = ::listen(sockfd_, n);

    if (SOCKET_ERROR == ret) {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), 
            "socket=<{}> error, listen port failed", sockfd_);
    } else {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS),
                "socket=<{}> listen port success", sockfd_);
    }
    return ret;
}

SOCKET tcp_server::accept() {
    if (AF_INET == af_) {
        return accept_ipv4();
    } else {
        return accept_ipv6();
    }
}
SOCKET tcp_server::accept_ipv4() {
    sockaddr_in addr = {};
    int len = sizeof(sockaddr_in);
    SOCKET csock = INVALID_SOCKET;
#ifdef _WIN32
    csock = ::accept(sockfd_, (sockaddr*)&addr, &len);
#else
    csock = ::accept(sockfd_, (sockaddr*)&addr, (socklen_t*)&len);
#endif
    if (INVALID_SOCKET == csock) {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), "accept invalid socket");
    } else {
        char *ip = inet_ntoa(addr.sin_addr);
        accept_client(csock, ip);
    }
    return csock;
}

SOCKET tcp_server::accept_ipv6() {
    sockaddr_in6 addr = {};
    int len = sizeof(sockaddr_in6);
    SOCKET csock = INVALID_SOCKET;
#ifdef _WIN32
    csock = ::accept(sockfd_, (sockaddr*)&addr, &len);
#else
    csock = ::accept(sockfd_, (sockaddr*)&addr, (socklen_t*)&len);
#endif
    if (INVALID_SOCKET == csock) {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), "accept invalid socket...");
    } else {
        static char ip[INET6_ADDRSTRLEN] = {};
        inet_ntop(AF_INET6, &addr.sin6_addr, ip, INET6_ADDRSTRLEN - 1);
        accept_client(csock, ip);
    }
    return csock;
}

void tcp_server::accept_client(SOCKET csock, char *ip) {
    network::make_reuseaddr(csock);
    SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "accpet ip:{} {}", ip, csock);
    if (client_accept_ < max_client_) {
        client_accept_++;
        auto c = make_client(csock);
        c->setip(ip);
        add_client_to_server(c);
    } else {
        network::destory_socket(csock);
        SPDLOG_LOGGER_WARN(spdlog::get(MULTI_SINKS), "accept to max_client");
    }
}

client* tcp_server::make_client(SOCKET csock) {
    return new client(csock, send_buffer_size_, recv_buffer_size_);
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
    SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "tcp_server close begin");
    thread_.close();
    if (sockfd_ != INVALID_SOCKET) {
        for (auto s : servers_) {
            delete s;
        }
        servers_.clear();
        network::destory_socket(sockfd_);
        sockfd_ = INVALID_SOCKET;
    }
    SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "tcp_server close end");
}

void tcp_server::on_join(client *pclient) {
    client_join_++;
}
void tcp_server::on_leave(client *pclient) {
    client_join_--;
    client_accept_--;
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
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
            "cell_thread<{}>, time<{:02.4f}>, socket<{}>, accept<{}>," 
            "client_count<{}>, recv_count<{}>, message<{}>",
            servers_.size(), t1, sockfd_, (int)client_accept_, 
            (int)client_join_, (int)recv_count_, (int)message_count_); 
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