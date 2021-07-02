#include "cell_client.hpp"

cell_client::cell_client(SOCKET sockfd) {
    sockfd_ = sockfd;
    memset(sz_msg_buf, 0, sizeof(sz_msg_buf));
    memset(send_buf_, 0, SEND_BUFF_SIZE);
    last_pos_ = 0;
    last_send_pos_ = 0;
}

SOCKET cell_client::sockfd() {
    return sockfd_;
}

char* cell_client::msg_buf() {
    return sz_msg_buf;
}

int cell_client::get_pos() {
    return last_pos_;
}

void cell_client::set_pos(int pos) {
    last_pos_ = pos;
}

int cell_client::send_data(data_header *data) {
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


void cell_client::reset_heart() {
    heart_ = 0;
}

bool cell_client::check_heart(time_t dt) {
    heart_ += dt;
    if (heart_ >= CLIENT_HEART_DEAD_TIME) {
        printf("check_heart dead: s=%d, time=%d\n", sockfd_, heart_);
        return true;
    }
    return false;
}


