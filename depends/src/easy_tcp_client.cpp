#include "easy_tcp_client.hpp"
#include "cell_network.hpp"

easy_tcp_client::easy_tcp_client() {
    is_connect_ = false;
}

easy_tcp_client::~easy_tcp_client() {
    close();
}

SOCKET easy_tcp_client::init_socket(int send_size, int recv_size) {
    cell_network::init();
    if (pclient_) {
        cell_log::info("warning close old socket<%d>...", (int)pclient_->sockfd());
        close();
    }
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock) {
        cell_log::info("create socket failed");
    } else {
        cell_network::make_reuseadd(sock);
        pclient_ = new cell_client(sock, send_size, recv_size);
    }
    return sock;
}

int easy_tcp_client::connect(const char *ip, unsigned short port) {
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
    // printf("socket=%d connecting server<%s:%d>\n", sock_, ip, port);
    int ret = ::connect(pclient_->sockfd(), (sockaddr*)&sin, sizeof(sockaddr_in));
    if (SOCKET_ERROR == ret) {
        printf("socket=%d error, connect server<%s:%d> failed\n", (int)pclient_->sockfd(), ip, port);
    } else  {
        // printf("socket=%d connect server<%s:%d> success\n", sock_, ip, port);
        is_connect_ = true;
    }
    return ret;
}

void easy_tcp_client::close() {
    if (pclient_) {
        delete pclient_;
        pclient_ = nullptr;
    }
    is_connect_ = false;
}

bool easy_tcp_client::on_run(int microseconds) {
    if (is_run()) {
        SOCKET _sock = pclient_->sockfd();
        fd_read_.zero();
        fd_read_.add(_sock);
        fd_write_.zero();
        
        timeval t = {0, microseconds};
        int ret = 0;
        if (pclient_->need_write()) {
            fd_write_.add(_sock);
            ret = select(_sock + 1, fd_read_.fdset(), fd_write_.fdset(), nullptr, &t);
        } else {
            ret = select(_sock + 1, fd_read_.fdset(), nullptr, nullptr, &t);
        }
        if (ret < 0) {
            cell_log::info("socke=%d select exit", (int)_sock);
            close();
            return false;
        }

        if (fd_read_.has(_sock)) {
            if (SOCKET_ERROR == recv_data(_sock)) {
                cell_log::info("socket=%d onrun select recv_data exit", (int)_sock);
                close();
                return false;
            }
        }

        if (fd_write_.has(_sock)) {
            if (SOCKET_ERROR == pclient_->send_data_real()) {
                cell_log::info("socket=%d onrun select send_data_real exit", (int)_sock);
                close();
                return false;
            }
        }

        return true;
    }
    return false;
}

bool easy_tcp_client::is_run() {
    return pclient_ && is_connect_;
}

int easy_tcp_client::recv_data(SOCKET c_sock) {
   if (is_run()) {
       int len = pclient_->recv_data();
       if (len > 0) {
           while (pclient_->has_msg()) {
               on_msg(pclient_->front_msg());
               pclient_->pop_msg();
           }
       }
       return len;
   }
   return 0;
}

int easy_tcp_client::send_data(data_header *header) {
    if (is_run()) {
        return pclient_->send_data(header);
    }
    return SOCKET_ERROR;
}


int easy_tcp_client::send_data(const char *data, int length) {
    if (is_run()) {
        return pclient_->send_data(data, length);
    }
    return SOCKET_ERROR;
}