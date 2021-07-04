#include "cell_client.hpp"

cell_client::cell_client(SOCKET sockfd) {
    sockfd_ = sockfd;
    memset(sz_msg_buf, 0, sizeof(sz_msg_buf));
    memset(send_buf_, 0, SEND_BUFF_SIZE);
    last_pos_ = 0;
    last_send_pos_ = 0;
}

cell_client::~cell_client() {
    if (INVALID_SOCKET != sockfd_) {
#ifdef _WIN32
        ::closesocket(sockfd_);
#else
        ::close(sockfd_);
#endif
        sockfd_ = INVALID_SOCKET;
    }
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

int cell_client::send_data_real() {
    int ret = 0;
    if (last_send_pos_ > 0 && INVALID_SOCKET != sockfd_) {
        ret = send(sockfd_, send_buf_, last_send_pos_, 0);
        last_send_pos_ = 0;
        reset_send_time();
    }
    return ret;
}

int cell_client::send_data(data_header *data) {
    int ret = SOCKET_ERROR;
    int length = data->length;
    const char *send_data = (const char*)data;
    
    if (length + last_send_pos_ <= SEND_BUFF_SIZE) {
        memcpy(send_buf_ +last_send_pos_, send_data, length);
        last_send_pos_ += length;
        if (last_pos_ == SEND_BUFF_SIZE) {

        }
        return length;
    } else {

    }
    return ret;
}


void cell_client::reset_heart_time() {
    heart_time_ = 0;
}

void cell_client::reset_send_time() {
    send_time_ = 0;
}

bool cell_client::check_heart_time(time_t dt) {
    heart_time_ += dt;
    if (heart_time_ >= CLIENT_HEART_DEAD_TIME) {
        printf("check_heart_time dead: s=%d, time=%d\n", sockfd_, heart_time_);
        return true;
    }
    return false;
}

bool cell_client::check_send_time(time_t dt) {
    send_time_ += dt;
    if (send_time_ >= CLIENT_SEND_BUFF_TIME) {
        printf("check_send_time:socket=%d, time=%d\n", sockfd_, send_time_);
        send_data_real();
        reset_send_time();
        return true;
    }
    return false;
}

