#include "cell_client.hpp"

cell_client::cell_client(SOCKET sockfd):
    send_buffer_(SEND_BUFF_SIZE),
    recv_buffer_(RECV_BUFF_SIZE) {
    static int n = 1;
    sockfd_ = sockfd;
    last_pos_ = 0;
    last_send_pos_ = 0;
    id_ = n++;
    service_id_ = -1;
}

cell_client::~cell_client() {
    cell_log::info("s=%d client%d\n", service_id_, id_);
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

int cell_client::recv_data() {
    return recv_buffer_.recv_from_socket(sockfd_);
}
bool cell_client::has_msg() {
    return recv_buffer_.has_msg();
}

data_header *cell_client::front_msg() {
    return (data_header*)recv_buffer_.data();
}

void cell_client::pop_msg() {
    if (has_msg()) {
        recv_buffer_.pop(front_msg()->length);
    }
}

bool cell_client::need_write() {
    return send_buffer_.need_write();
}

int cell_client::send_data_real() {
    reset_send_time();
    return send_buffer_.send_to_socket(sockfd_);
}

int cell_client::send_data(data_header *data) {
    if (send_buffer_.push((const char*)data, data->length)) {
        return data->length;
    }
    return SOCKET_ERROR;
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
        cell_log::info("check_heart_time dead: s=%d, time=%d\n", sockfd_, heart_time_);
        return true;
    }
    return false;
}

bool cell_client::check_send_time(time_t dt) {
    send_time_ += dt;
    if (send_time_ >= CLIENT_SEND_BUFF_TIME) {
        cell_log::info("check_send_time:socket=%d, time=%d\n", sockfd_, send_time_);
        send_data_real();
        reset_send_time();
        return true;
    }
    return false;
}

