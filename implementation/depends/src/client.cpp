#include "depends/client.hpp"

client::client(SOCKET sockfd, int send_size, int recv_size):
    send_buffer_(send_size), recv_buffer_(recv_size) {
    static int n = 1;
    sockfd_ = sockfd;
    id_ = n++;

    reset_heart_time();
    reset_send_time();
}

client::~client() {
    // SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "s={} client{}", service_id_, id_);
    destory();
}

void client::destory() {
    if (INVALID_SOCKET != sockfd_) {
        // SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "client::destory sid={} id={} socket={}", service_id_, id_, (int)sockfd_);
        network::destory_socket(sockfd_);
        sockfd_ = INVALID_SOCKET;
    }
}

SOCKET client::sockfd() {
    return sockfd_;
}

int client::recv_data() {
    return recv_buffer_.recv_from_socket(sockfd_);
}
bool client::has_msg() {
    return recv_buffer_.has_msg();
}

data_header *client::front_msg() {
    return (data_header*)recv_buffer_.data();
}

void client::pop_msg() {
    if (has_msg()) {
        recv_buffer_.pop(front_msg()->length);
    }
}

bool client::need_write() {
    return send_buffer_.need_write();
}

int client::send_data_real() {
    reset_send_time();
    return send_buffer_.send_to_socket(sockfd_);
}

int client::send_data(data_header *data) {
    return send_data((const char*)data, data->length); 
}

int client::send_data(const char *data, int length) {
    if (send_buffer_.push(data, length)) {
        return length;
    }
    return SOCKET_ERROR;
}

void client::reset_heart_time() {
    heart_time_ = 0;
}

void client::reset_send_time() {
    send_time_ = 0;
}

bool client::check_heart_time(time_t dt) {
    heart_time_ += dt;
    if (heart_time_ >= CLIENT_HEART_DEAD_TIME) {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
        "check_heart_time dead: s={}, time={}", 
        sockfd_, heart_time_);
        return true;
    }
    return false;
}

bool client::check_send_time(time_t dt) {
    send_time_ += dt;
    if (send_time_ >= CLIENT_SEND_BUFF_TIME) {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
        "check_send_time:socket={}, time={}", 
        sockfd_, send_time_);
        send_data_real();
        reset_send_time();
        return true;
    }
    return false;
}

#ifdef _WIN32
io_data_base *client::make_recv_iodata() {
    if (is_post_recv_)
        return nullptr;
    is_post_recv_ = true;
    return recv_buffer_.make_recv_iodata(sockfd_);
}

void client::recv_for_iocp(int nrecv) {
    if (!is_post_recv_) {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "recv_for_iocp is+post_recv_ is false");
    }
    is_post_recv_ = false;
    recv_buffer_.read_for_iocp(nrecv);
}

io_data_base *client::make_send_iodata() {
    if (is_post_send_)
        return nullptr;
    is_post_send_ = true;
    return send_buffer_.make_send_iodata(sockfd_);
}

void client::send_to_iocp(int nsend) {
    if (!is_post_send_) {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "send_to_iocp is_post_send_ is false");
    }
    is_post_send_ = false;
    send_buffer_.write_to_iocp(nsend);
}

bool client::is_post_action() {
    return is_post_recv_ || is_post_send_;
}

#endif