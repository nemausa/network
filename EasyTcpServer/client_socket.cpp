#include "client_socket.hpp"

client_socket::client_socket(SOCKET sockfd) {
    sockfd_ = sockfd;
    memset(sz_msg_buf, 0, sizeof(sz_msg_buf));
    memset(send_buf_, 0, SEND_BUFF_SIZE);
    last_pos_ = 0;
    last_send_pos_ = 0;
}

SOCKET client_socket::sockfd() {
    return sockfd_;
}

char* client_socket::msg_buf() {
    return sz_msg_buf;
}

int client_socket::get_pos() {
    return last_pos_;
}

void client_socket::set_pos(int pos) {
    last_pos_ = pos;
}

int client_socket::send_data(data_header *data) {
    int ret = SOCKET_ERROR;
    int length = data->length;
    const char *send_data = (const char*)data;
    
    while (true) {
        if (length + last_send_pos_ >= SEND_BUFF_SIZE) {
            // 可拷贝的数据长度
            int copy_len = SEND_BUFF_SIZE - last_send_pos_;
            memcpy(send_buf_ + last_send_pos_, send_data, copy_len);
            // 剩余的数据量
            send_data += copy_len;
            length -= copy_len;
            ret = send(sockfd_, send_buf_, SEND_BUFF_SIZE, 0);
            last_send_pos_ = 0;
            if (SOCKET_ERROR == ret) {
                return ret;
            }
        } else {
            memcpy(send_buf_ + last_send_pos_, send_data, length);
            last_send_pos_ += length;
            break;
        }

    }
    return ret;
}