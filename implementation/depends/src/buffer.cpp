#include "depends/cell.hpp"
#include "depends/buffer.hpp"

namespace nemausa {
namespace io {

buffer::buffer(int size) {
    size_ = size;
    data_ = new char[size];
}

buffer::~buffer() {
    if (data_) {
        delete[] data_;
        data_ = nullptr;
    }
}

char *buffer::data() {
    return data_;
}

bool buffer::push(const char *data, int length) {
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

void buffer::pop(int length) {
    int n = last_ - length;
    if (n > 0) {
        memcpy(data_, data_ + length, n);
    }
    last_ = n;
    if (full_count_ > 0) {
        --full_count_;
    }
}

int buffer::send_to_socket(SOCKET sockfd) {
    int ret  = 0;
    if (last_ > 0 && sockfd != INVALID_SOCKET) {
        ret = send(sockfd, data_, last_, 0);
        if (ret <= 0) {
            SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
            "send_to_socket:sockfd{} size<{}> last<{}> ret<{}>",
            sockfd, size_, last_, ret);
            return SOCKET_ERROR; 
        }
        if (ret == last_) {
            last_ = 0;
        } else {
            last_ -= ret;
            memcpy(data_, data_ + ret, last_);
        }
        full_count_ = 0;
    }
    return ret;
}

int buffer::recv_from_socket(SOCKET sockfd) {
    if (size_ - last_ > 0) {
        char *szrecv = data_ + last_;
        int len = (int)recv(sockfd, szrecv, size_ - last_, 0);
        if (len <= 0) {
            SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
            "recv_from_socket:sockfd{} size<{}> last<{}> len<{}>",
            sockfd, size_, last_, len);
            return SOCKET_ERROR;
        }
        last_ += len;
        return len;
    }
    return 0;
}

bool buffer::has_msg() {
    if (last_ >= sizeof(data_header)) {
        data_header *data = (data_header*)data_;
        return last_ >= data->length;
    }
    return false;
}

bool buffer::need_write() {
    return last_ > 0;
}

#ifdef _WIN32
io_data_base *buffer::make_recv_iodata(SOCKET sockfd) {
    int len = size_ - last_ - 1;
    if (len > 0) {
        iodata_.wsabuff.buf = data_ + last_;
        iodata_.wsabuff.len = len;
        iodata_.sockfd = sockfd;
        return &iodata_;
    }
    return nullptr;
}

io_data_base *buffer::make_send_iodata(SOCKET sockfd) {
    if (last_ > 0) {
        iodata_.wsabuff.buf = data_;
        iodata_.wsabuff.len = last_;
        iodata_.sockfd = sockfd;
        return &iodata_;
    }
    return nullptr;
}

bool buffer::read_for_iocp(int nrecv) {
    if (nrecv > 0 && size_ - last_ >= nrecv) {
        last_ += nrecv;
        return true;
    }
    SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
    "read_for_iocp:socket<{}> size<{}> last<{}> nrecv<{}>", 
    iodata_.sockfd, size_, last_, nrecv);
    return false;
}

bool buffer::write_to_iocp(int nsend) {
    if (last_ < nsend) {
        SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
        "write_to_iocp:sockfd<{}> size<{}> last<{}> nsend<{}>", 
        iodata_.sockfd, size_, last_, nsend);
        return false;
    }
    if (last_ == nsend) {
        last_ = 0;
    } else {
        last_ -= nsend;
        memcpy(data_, data_ + nsend, last_);
    }
    full_count_ = 0;
    return true;
}

#endif

} // namespace io 
} // namespace nemausa
