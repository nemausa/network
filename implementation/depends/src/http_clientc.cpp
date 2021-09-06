#include "depends/http_clientc.hpp"

namespace nemausa {
namespace io {

http_clientc::http_clientc(SOCKET sockfd, 
        int send_size, 
        int recv_size):
        client(sockfd, send_size, recv_size) {
    
}

bool http_clientc::has_msg() {
    if (recv_buffer_.length() < 20)
        return false;
    int ret = check_response();
    if (ret < 0)
        SPDLOG_LOGGER_ERROR(spdlog::get(MULTI_SINKS), "check_reponse error");
    return ret > 0;
}

int http_clientc::check_response() {
    char *temp = strstr(recv_buffer_.data(), "\r\n\r\n");
    if (!temp) {
        return 0;
    }
    SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), recv_buffer_.data());
    temp += 4;
    header_len_ = temp - recv_buffer_.data();
    temp = recv_buffer_.data();
    if (temp[0] == 'H' &&
        temp[1] == 'T' &&
        temp[2] == 'T' &&
        temp[3] == 'P') {
        char *p1 = strstr(recv_buffer_.data(), "Content-Length: ");
        if (!p1)
            return -2;
        p1 += 16;
        char *p2 = strchr(p1, '\r');
        if (!p2)
            return -2;
        int n = p2 - p1;
        if (n > 6)
            return -2;
        char len[7] = {};
        strncpy(len, p1, n);
        body_len_ = atoi(len);
        if (body_len_ < 0)
            return -2;
        if (header_len_ + body_len_ > recv_buffer_.size())
            return -2;
        if (header_len_ + body_len_ > recv_buffer_.length())
            return 0; 
    } else {
        return -1;
    }
    return header_len_;
}

bool http_clientc:: get_response_info() {
    if (header_len_ <= 0) {
        return false;
    }

    char *pp = recv_buffer_.data();
    pp[header_len_ - 1] = '\0';

    split_string ss;
    ss.set(recv_buffer_.data());
    char *temp = ss.get("\r\n");
    if (temp) {
        header_map_["request_line"] = temp;
    }

    while (true) {
        temp = ss.get("\r\n");
        if (temp) {
            split_string ss2;
            ss2.set(temp);
            char *key = ss2.get(": ");
            char *val = ss2.get(": ");
            if (key && val) {
                header_map_[key] = val;
            }
        } else {
            break;
        }
    }
 
    if (body_len_ > 0) {
        args_map_["content"] = recv_buffer_.data() + header_len_;
    }

    const char *str = header_str("Connection", "");
    keep_alive_ = (0 == strcmp("keep-alive", str)); 

    return true;
}

void http_clientc::split_url_args(char *args) {
    split_string ss;
    ss.set(args);
    while (true) {
        char *temp = ss.get('&');
        if (temp) {
            split_string ss2;
            ss2.set(temp);
            char *key = ss2.get('=');
            char *val = ss2.get('=');
            if (key && val)
                args_map_[key] = val;
        } else {
            break;
        }
    }    
}

void http_clientc::pop_msg() {    
    if (header_len_ > 0) {
        recv_buffer_.pop(header_len_ + body_len_);
        header_len_ = 0;
        body_len_ = 0;
        args_map_.clear();
        header_map_.clear();
    }
}
bool http_clientc::has_header(const char *key) {
    return header_map_.find(key) != header_map_.end();
}

int http_clientc::args_int(const char *name, int def) {
    auto itr = args_map_.find(name);
    if (itr == args_map_.end()) {

    } else {
        def = atoi(itr->second);
    }
    return def;
}

const char * http_clientc::args_str(const char *arg_name, const char *def) {
    auto itr = args_map_.find(arg_name);
    if (itr != args_map_.end()) {
        return itr->second;
    }
    return def;
}

const char * http_clientc::header_str(const char *arg_name, const char *def) {
    auto itr = header_map_.find(arg_name);
    if (itr != header_map_.end()){
        return itr->second;
    }
    return def;
}

void http_clientc::on_send_complete() {
    if (!keep_alive_)
        this->on_close();
}

const char * http_clientc::content() {
    return args_str("content", nullptr);
}
} // namespace nemausa
} // namespace io