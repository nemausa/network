#include "depends/cell_client.hpp"

cell_client::cell_client(SOCKET sockfd, int send_size, int recv_size):
    send_buffer_(send_size),
    recv_buffer_(recv_size) {
    static int n = 1;
    sockfd_ = sockfd;
    id_ = n++;

    reset_heart_time();
    reset_send_time();
}

cell_client::~cell_client() {
    LOG_INFO("s=%d client%d\n", service_id_, id_);
    destory();
}

void cell_client::destory() {
    if (INVALID_SOCKET != sockfd_) {
        LOG_INFO("cell_client::destory sid=%d id=%d socket=%d", service_id_, id_, (int)sockfd_);
        cell_network::destory_socket(sockfd_);
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
    return send_data((const char*)data, data->length); 
}

int cell_client::send_data(const char *data, int length) {
    if (send_buffer_.push(data, length)) {
        return length;
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
        LOG_INFO("check_heart_time dead: s=%d, time=%d\n", sockfd_, heart_time_);
        return true;
    }
    return false;
}

bool cell_client::check_send_time(time_t dt) {
    send_time_ += dt;
    if (send_time_ >= CLIENT_SEND_BUFF_TIME) {
        LOG_INFO("check_send_time:socket=%d, time=%d\n", sockfd_, send_time_);
        send_data_real();
        reset_send_time();
        return true;
    }
    return false;
}

#ifdef _WIN32
io_data_base *cell_client::make_recv_iodata() {
    if (is_post_recv_)
        return nullptr;
    is_post_recv_ = true;
    return recv_buffer_.make_recv_iodata(sockfd_);
}

void cell_client::recv_for_iocp(int nrecv) {
    if (!is_post_recv_) {
        LOG_INFO("recv_for_iocp is+post_recv_ is false");
    }
    is_post_recv_ = false;
    recv_buffer_.read_for_iocp(nrecv);
}

io_data_base *cell_client::make_send_iodata() {
    if (is_post_send_)
        return nullptr;
    is_post_send_ = true;
    return send_buffer_.make_send_iodata(sockfd_);
}

void cell_client::send_to_iocp(int nsend) {
    if (!is_post_send_) {
        LOG_INFO("send_to_iocp is_post_send_ is false");
    }
    is_post_send_ = false;
    send_buffer_.write_to_iocp(nsend);
}

bool cell_client::is_post_action() {
    return is_post_recv_ || is_post_send_;
}

#endif