#include "depends/http_clients.hpp"
#include "depends/split_string.hpp"

namespace nemausa {
namespace io {

http_clients::http_clients(SOCKET sockfd, int send_size, int recv_size)
        : client(sockfd, send_size, recv_size) {

}

bool http_clients::has_msg() {
    if (recv_buffer_.length() < 20) {
        return false;
    }
    int ret = check_request();
    if (ret < 0) {
        resp_400_bad();
    } 
    return ret > 0;
}

int http_clients::check_request() {
    char *temp = strstr(recv_buffer_.data(), "\r\n\r\n");
    if (!temp) {
        return 0;
    }
    // SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), recv_buffer_.data());
    temp += 4;
    header_len_ = temp - recv_buffer_.data();
    temp = recv_buffer_.data();
    if (temp[0] == 'G' &&
        temp[1] == 'E' &&
        temp[2] == 'T') {
        request_type_ = http_clients::GET;
    } else if (temp[0] == 'P' &&
        temp[1] == 'O' &&
        temp[2] == 'S' &&
        temp[3] == 'T') {
        request_type_ = http_clients::POST;
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
        request_type_ = http_clients::UNKNOWN;
        return -1;
    }
    return header_len_;
}

bool http_clients::request_info() {
    if (header_len_ <= 0) {
        return false;
    }

    char *pp = recv_buffer_.data();
    pp[header_len_ - 1] = '\0';

    split_string ss;
    ss.set(recv_buffer_.data());
    // 请求示例"GET /login.php?a=5 HTTP/1.1\r\n"
    char *temp = ss.get("\r\n");
    if (temp) {
        header_map_["request_line"] = temp;
        request_args(temp);
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
        split_url_args(recv_buffer_.data() + header_len_);
    }
    const char *str = header_str("Connection", "");
    keep_alive_ = (0 == strcmp("keep-alive", str) || 0 == strcmp("Keep-Alive", str));

    return true;
}

void http_clients::split_url_args(char *args) {
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

bool http_clients::request_args(char *request_line) {
    split_string ss;
    ss.set(request_line);
    method_ = ss.get(' ');
    if (!method_)
        return false;
    
    url_ = ss.get(' ');
    if (!url_)
        return false;
    
    http_version_ = ss.get(' ');
    if (!http_version_)
        return false;

    ss.set(url_);
    url_path_ = ss.get('?');
    if (!url_path_)
        return false;
    
    url_args_ = ss.get('?');
    if (!url_args_)
        return true;
    
    split_url_args(url_args_);
    
    return true;
}

void http_clients::pop_msg() {    
    if (header_len_ > 0) {
        recv_buffer_.pop(header_len_ + body_len_);
        header_len_ = 0;
        body_len_ = 0;
        args_map_.clear();
        header_map_.clear();
    }
}

bool http_clients::can_write(int size) {
    return send_buffer_.can_write(size);
}

void http_clients::resp_400_bad() {
    write_response("400 Bad Request", "Only support GET or POST.", 25);
}

void http_clients::resp_400_not_found() {
    write_response("404 Not Found", "(^o^):404", 11);
}

void http_clients::resp_200_ok(const char *body_buff, int body_len) {
    write_response("200 OK", body_buff, body_len);
}

void http_clients::write_response(const char *code, const char *body_buff, int body_len) {
    char resp_body_len[32] = {};
    sprintf(resp_body_len, "Content-Length: %d\r\n", body_len);
    char response[256] = {};
    strcat(response, "HTTP/1.1");
    strcat(response, code);
    strcat(response, "\r\n");
    
    strcat(response, "Content-Type: text/html;charset=UFT-8\r\n");
    strcat(response, "Access-Control-Allow-Origin: *\r\n");
	strcat(response, "Connection: Keep-Alive\r\n");
    strcat(response, resp_body_len);
    strcat(response, "\r\n");

    send_data(response, strlen(response));
    send_data(body_buff, body_len);
}

char * http_clients::url() {
    return url_path_;
}

bool http_clients::url_compare(const char *str) {
    return 0 == strcmp(url_path_, str);
}

bool http_clients::has_args(const char *key) {
    return args_map_.find(key) != args_map_.end();
}

bool http_clients::has_header(const char *key) {
    return header_map_.find(key) != header_map_.end();
}

int http_clients::args_int(const char *name, int def) {
    auto itr = args_map_.find(name);
    if (itr == args_map_.end()) {

    } else {
        def = atoi(itr->second);
    }
    return def;
}

const char * http_clients::args_str(const char *arg_name, const char *def) {
    auto itr = args_map_.find(arg_name);
    if (itr != args_map_.end()) {
        return itr->second;
    }
    return def;
}

const char * http_clients::header_str(const char *arg_name, const char *def) {
    auto itr = header_map_.find(arg_name);
    if (itr != header_map_.end()){
        return itr->second;
    }
    return def;
}

void http_clients::on_send_complete() {
    if (!keep_alive_)
        this->on_close();
}

} // namespace nemausa
} // namepsace io