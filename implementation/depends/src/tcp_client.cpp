#include "depends/tcp_client.hpp"
#include "depends/network.hpp"

namespace nemausa {
namespace io {

tcp_client::tcp_client() {
    is_connect_ = false;
}

tcp_client::~tcp_client() {
    close();
}

SOCKET tcp_client::init_socket(int af, int send_size, int recv_size) {
    network::init();
    if (pclient_) {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                "warning close old socket<{}>...", (int)pclient_->sockfd());
        close();
    }
    af_ = af;
    SOCKET sock = socket(af_, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock) {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "create socket failed");
    } else {
        network::make_reuseaddr(sock);
        pclient_ = make_client(sock, send_size, recv_size); 
        on_init_socket();
    }
    return sock;
}

int tcp_client::connect(const char *ip, unsigned short port) {
    if (!pclient_) {
        return SOCKET_ERROR;
    }
    int ret = SOCKET_ERROR;
    if (AF_INET == af_) {
        sockaddr_in sin = {};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
    #ifdef _WIN32
        sin.sin_addr.S_un.S_addr = inet_addr(ip);
    #else
        sin.sin_addr.s_addr = inet_addr(ip);
    #endif
        ret = ::connect(pclient_->sockfd(), (sockaddr*)&sin, sizeof(sockaddr_in));

    } else if (AF_INET6 == af_) {
        sockaddr_in6 sin = {};
        sin.sin6_scope_id = if_nametoindex(scope_id_name_.c_str());
        sin.sin6_family = AF_INET6;
        sin.sin6_port = htons(port);
        inet_pton(AF_INET6, ip, &sin.sin6_addr);
        ret = ::connect(pclient_->sockfd(), (sockaddr*)&sin, sizeof(sockaddr_in6));
    } else {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), "connect error");
    }
    if (SOCKET_ERROR == ret) {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), 
                "socket={} error, connect server<{}:{}> failed", 
                (int)pclient_->sockfd(), ip, port);
    } else  {
        is_connect_ = true;
        on_connect();
    }
    return ret;
}

void tcp_client::close() {
    if (pclient_) {
        delete pclient_;
        pclient_ = nullptr;
        is_connect_ = false;
        on_disconnect();
    }
}

bool tcp_client::is_run() {
    return pclient_ && is_connect_;
}

int tcp_client::recv_data() {
   if (is_run()) {
       int len = pclient_->recv_data();
       if (len > 0) {
           do_msg();
       }
       return len;
   }
   return 0;
}

void tcp_client::do_msg() {
    while (pclient_->has_msg()) {
        on_msg(pclient_->front_msg());
        pclient_->pop_msg();
    }
}

int tcp_client::send_data(data_header *header) {
    if (is_run()) {
        return pclient_->send_data(header);
    }
    return SOCKET_ERROR;
}

int tcp_client::send_data(const char *data, int length) {
    if (is_run()) {
        return pclient_->send_data(data, length);
    }
    return SOCKET_ERROR;
}

void tcp_client::set_scope_id_name(std::string scope_id_name) {
    scope_id_name_ = scope_id_name;
}

client * tcp_client::make_client(SOCKET csock, int send_size, int recv_size) {
    return new client(csock, send_size, recv_size);
}

void tcp_client::on_init_socket() {
    
}

void tcp_client::on_connect() {

}

void tcp_client::on_disconnect() {

}

} // namespace io 
} // namespace nemausa
