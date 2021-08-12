
#include "depends/tcp_client.hpp"
#include "depends/network.hpp"

// #include "spdlog/spdlog.h"
// #include "spdlog/sinks/stdout_sinks.h"
// #include "spdlog/sinks/daily_file_sink.h"

tcp_client::tcp_client() {
    is_connect_ = false;
}

tcp_client::~tcp_client() {
    close();
}

SOCKET tcp_client::init_socket(int send_size, int recv_size) {
    network::init();
    if (pclient_) {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "warning close old socket<{}>...", (int)pclient_->sockfd());
        close();
    }
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock) {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "create socket failed");
    } else {
        network::make_reuseaddr(sock);
        pclient_ = new client(sock, send_size, recv_size);
        on_init_socket();
    }
    return sock;
}

int tcp_client::connect(const char *ip, unsigned short port) {
    if (!pclient_) {
        if (INVALID_SOCKET == init_socket()) {
            return SOCKET_ERROR;
        }
    }
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
#ifdef _WIN32
    sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
    sin.sin_addr.s_addr = inet_addr(ip);
#endif
    // auto logger = spdlog::get("name");
    // printf("socket={} connecting server<%s:{}>\n", sock_, ip, port);
    int ret = ::connect(pclient_->sockfd(), (sockaddr*)&sin, sizeof(sockaddr_in));
    if (SOCKET_ERROR == ret) {
         SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), "socket={} error, connect server<{}:{}> failed", (int)pclient_->sockfd(), ip, port);
    } else  {
        // printf("socket={} connect server<%s:%d> success\n", sock_, ip, port);
        is_connect_ = true;
        on_connect();
    }
    return ret;
}

void tcp_client::close() {
    if (pclient_) {
        delete pclient_;
        pclient_ = nullptr;
    }
    is_connect_ = false;
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

void tcp_client::on_init_socket() {
    
}

void tcp_client::on_connect() {

}