#include "easy_tcp_client.hpp"

easy_tcp_client::easy_tcp_client() {
    sock_ = INVALID_SOCKET;
}

easy_tcp_client::~easy_tcp_client() {
    close();
}

void easy_tcp_client::init_socket() {
#ifdef _WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif
    if (INVALID_SOCKET != sock_) {
        printf("socket=%d close old connect\n", sock_);
        close();
    }
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock_) {
        printf("error carete socket failed\n");
    } else {
        // printf("create socket=%d success\n", sock_);
    }
}

int easy_tcp_client::connect(const char *ip, unsigned short port) {
    if (INVALID_SOCKET == sock_) {
        init_socket();
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
    int ret = ::connect(sock_, (sockaddr*)&sin, sizeof(sockaddr_in));
    if (SOCKET_ERROR == ret) {
        printf("socket=%d error, connect server<%s:%d> failed\n", sock_, ip, port);
    } else  {
        // printf("socket=%d connect server<%s:%d> success\n", sock_, ip, port);
    }
    return ret;
}

void easy_tcp_client::close() {
    if (sock_ != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(sock_);
        WSACleanup();
#else
        ::close(sock_);
#endif
        sock_ = INVALID_SOCKET;        
    }
}

bool easy_tcp_client::on_run() {
    if (is_run()) {
        fd_set fd_read;
        FD_ZERO(&fd_read);
        FD_SET(sock_, &fd_read);
        timeval t = {0, 0};
        int ret = select(sock_ + 1, &fd_read, 0, 0, &t);
        if (ret < 0) {
            printf("socket=%d select task end1\n");
            close();
            return false;
        }
        if (FD_ISSET(sock_, &fd_read)) {
            FD_CLR(sock_ , &fd_read);
            if (-1 == recv_data(sock_)) {
                printf("socket=%d slect task end1\n", sock_);
                close();
                return false;
            }
        }
        return true;
    }
    return false;
}

bool easy_tcp_client::is_run() {
    return sock_ != INVALID_SOCKET;
}

int easy_tcp_client::recv_data(SOCKET c_sock) {
    int len = (int)recv(c_sock, sz_recv, RECV_BUFF_SIZE, 0);
    if (len <=0) {
        printf("sockt=%d disconnect from server\n");
        return -1;
    }
    memcpy(sz_msg_buf + last_pos, sz_recv, len);
    last_pos += len;
    while (last_pos >= sizeof(data_header)) {
        data_header *header = (data_header*)sz_msg_buf;
        if (last_pos >= header->length) {
            int size = last_pos - header->length;
            on_msg(header);
            memcpy(sz_msg_buf, sz_msg_buf + header->length, size);
            last_pos = size;
        } else {
            break;
        }
    }
    return 0;
}


void easy_tcp_client::on_msg(data_header *header) {
    switch (header->cmd) {
    case CMD_LOGIN_RESULT: {
        // printf("socket=%d CMD_LOGIN_RESULT, data length=%d\n", sock_, header->length);
    }
    break;
    case CMD_LOGOUT_RESULT: {
        // printf("socket=%d CMD_LOGOUT_RESULT, data length=%d\n", sock_, header->length);
    }
    break;
    case CMD_NEW_JOIN: {
        // printf("socket=%d CMD_NEW_JOIN, data length=%d\n", sock_, header->length);
    }
    break;
    case CMD_ERROR: {
        printf("socket=%d CMD_ERROR, data length=%d\n", sock_, header->length);
    }
    break;
    default: {
        printf("socket=%d undefined command, data length=%d\n", sock_, header->length);
    }
    }
}

int easy_tcp_client::send_data(data_header *header) {
    if (is_run() && header) {
        return send(sock_, (const char*)header, header->length, 0);
    }
    return SOCKET_ERROR;
}