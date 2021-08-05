#include "depends/cell.hpp"
#include "depends/cell_buffer.hpp"

cell_buffer::cell_buffer(int size) {
    data_ = nullptr;
    last_ = 0;
    size_ = size;
    full_count_ = 0;
    data_ = new char[size];
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
        if (ret <= 0) {
        LOG_INFO("send_to_socket:sockfd%d size<%d> last<%d> ret<%d>",
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

int cell_buffer::recv_from_socket(SOCKET sockfd) {
    if (size_ - last_ > 0) {
        char *szrecv = data_ + last_;
        int len = (int)recv(sockfd, szrecv, size_ - last_, 0);
        if (len <= 0) {
            LOG_INFO("send_to_socket:sockfd%d size<%d> last<%d> len<%d>",
                sockfd, size_, last_, len);
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

bool cell_buffer::need_write() {
    return last_ > 0;
}

#ifdef _WIN32
io_data_base *cell_buffer::make_recv_iodata(SOCKET sockfd) {
    int len = size_ - last_;
    if (len > 0) {
        iodata_.wsabuff.buf = data_ + last_;
        iodata_.wsabuff.len = len;
        iodata_.sockfd = sockfd;
        return &iodata_;
    }
    return nullptr;
}

io_data_base *cell_buffer::make_send_iodata(SOCKET sockfd) {
    if (last_ > 0) {
        iodata_.wsabuff.buf = data_;
        iodata_.wsabuff.len = last_;
        iodata_.sockfd = sockfd;
        return &iodata_;
    }
    return nullptr;
}

bool cell_buffer::read_for_iocp(int nrecv) {
    if (nrecv > 0 && size_ - last_ >= nrecv) {
        last_ += nrecv;
        return true;
    }
    LOG_INFO("read_for_iocp:socket<%d> size<%d> last<%d> nrecv<%d>", iodata_.sockfd, size_, last_, nrecv);
    return false;
}

bool cell_buffer::write_to_iocp(int nsend) {
    if (last_ < nsend) {
        LOG_INFO("write_to_iocp:sockfd<%d> size<%d> last<%d> nsend<%d>", iodata_.sockfd, size_, last_, nsend);
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