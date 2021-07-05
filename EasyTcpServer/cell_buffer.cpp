#include "cell.hpp"
#include "cell_buffer.hpp"

cell_buffer::cell_buffer(int size) {
    data_ = nullptr;
    last_ = 0;
    size_ = size;
    full_count_ = 0;
}

cell_buffer::~cell_buffer() {
    if (data_) {
        delete[] data_;
        data_ = nullptr;
    }
}

char *cell_buffer::data() {
    return data_;
}

bool cell_buffer::push(const char *data, int length) {
    if(last_ + length <= size_) {
        memcpy(data_ + last_, data, length);
        last_ += length;
        if (last_ == SEND_BUFF_SIZE) {
            ++full_count_;
        }
        return true;
    } else {
        ++full_count_;
    }
    return false;
}

void cell_buffer::pop(int length) {
    int n = last_ - length;
    if (n > 0) {
        memcpy(data_, data_ + length, n);
    }
    last_ = n;
    if (full_count_ > 0) {
        --full_count_;
    }
}

int cell_buffer::send_to_socket(SOCKET sockfd) {
    int ret  = 0;
    if (last_ > 0 && sockfd != INVALID_SOCKET) {
        ret = send(sockfd, data_, last_, 0);
        last_ = 0;
        full_count_ = 0;
    }
    return ret;
}

int cell_buffer::recv_from_socket(SOCKET sockfd) {
    if (size_ - last_ > 0) {
        char *szrecv = data_ + last_;
        int len = (int)recv(sockfd, szrecv, size_ - last_, 0);
        if (len <= 0) {
            return len;
        }
        last_ += len;
        return len;
    }
    return 0;
}

bool cell_buffer::has_msg() {
    if (last_ >= sizeof(data_header)) {
        data_header *data = (data_header*)data_;
        return last_ >= data->length;
    }
    return false;
}