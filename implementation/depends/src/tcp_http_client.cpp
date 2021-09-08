#include "depends/tcp_http_client.hpp"

namespace nemausa {
namespace io {

tcp_http_client::tcp_http_client() {
    network::init();
}

client * tcp_http_client::make_client(SOCKET csock, 
        int send_size, int recv_size) {
    return new http_clientc(csock, send_size, recv_size);
}

bool tcp_http_client::on_run(int microseconds) {
    if (next_request_)
        next_request();
    return tcp_client_mgr::on_run(microseconds);
}

void tcp_http_client::on_msg(data_header *header) {
    http_clientc *pclient = dynamic_cast<http_clientc*>(pclient_);
    if (!pclient)
        return;
    
    if (!pclient->get_response_info())
        return;
    
    if (on_resp_call_) {
        on_resp_call_(pclient);
        on_resp_call_ = nullptr;
    }

    pclient->on_recv_complete();
    if (!event_queue_.empty())
        event_queue_.pop();
    next_request_ = true;
}

void tcp_http_client::on_disconnect() {
    if (on_resp_call_) {
        on_resp_call_(nullptr);
        on_resp_call_ = nullptr;
    }
    next_request(); 
}

void tcp_http_client::get(const char *http_url, event_call on_resp_call) {
    event e;
    e.http_url = http_url;
    e.on_resp_call = on_resp_call;
    e.is_get = true;
    event_queue_.push(e);
    next_request_ = true;
}

void tcp_http_client::post(const char *http_url, event_call on_resp_call) {
    event e;
    e.http_url = http_url;
    e.on_resp_call = on_resp_call;
    e.is_get = false;
    event_queue_.push(e);
    next_request_ = true;
}

void tcp_http_client::post(const char *http_url, 
        const char *data, 
        event_call on_resp_call) {
    std::string url = http_url;
    url += '?';
    url += data;
    post(url.c_str(), on_resp_call);
}

int tcp_http_client::hostname_to_ip(const char *hostname, const char *port) {
    if (!hostname) {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "");
        return -1;
    }

    if (!port) {
        SPDLOG_LOGGER_WARN(spdlog::get(MULTI_SINKS), "");
        return -1;
    }

    unsigned short port_ = 80;
    if (port, strlen(port) > 0)
        port_ = atoi(port);    
    
    // 主机和端口号不变就不重新连接服务端
    if (is_run() && host0_ == hostname && port_ == port0_)
        return 0;
    
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_IP;
    hints.ai_flags = AI_ALL;
    addrinfo *addr_list = nullptr;
    int ret = getaddrinfo(hostname, nullptr, &hints, &addr_list);
    if (0 != ret) {
        SPDLOG_LOGGER_ERROR(spdlog::get(MULTI_SINKS), "{}", hostname);
        freeaddrinfo(addr_list);
        return ret;
    }

    char ip[256] = {};
    for (auto addr = addr_list; addr != nullptr; addr = addr->ai_next) {
        ret = getnameinfo(addr->ai_addr, addr->ai_addrlen, ip, 255, 
                nullptr, 0, NI_NUMERICHOST);
        if (0 != ret) {
            SPDLOG_LOGGER_ERROR(spdlog::get(MULTI_SINKS), "{}", hostname);
            continue;
        } else {
            if (addr->ai_family == AF_INET6) {
                SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                        "{} ipv6: {}", hostname, ip);
            } else if (addr->ai_family == AF_INET) {
                SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                        "{} ipv4: {}", hostname, ip);
            } else {
                SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                        "{} addr: {}", hostname, ip);
            }

            if (connect_to_ip(addr->ai_family, ip, port_)) {
                host0_ = host_;
                port0_ = port_;
                break;
            }
        }
    }

    freeaddrinfo(addr_list);
    return ret;
}

void tcp_http_client::next_request() {
    next_request_ = false;
    if (!event_queue_.empty()) {
        event &e = event_queue_.front();
        ++e.count;
        if (e.count > e.max_count) {
            event_queue_.pop();
            next_request_ = true;
            return;
        }

        if (e.is_get) {
            deatch_http_url(e.http_url);
            if (0 == hostname_to_ip(host_.c_str(), port_.c_str())) {
                url_to_get(host_.c_str(), path_.c_str(), args_.c_str());
                on_resp_call_ = e.on_resp_call;
            }
        } else {
            deatch_http_url(e.http_url);
            if (0 == hostname_to_ip(host_.c_str(), port_.c_str())) {
                url_to_post(host_.c_str(), path_.c_str(), args_.c_str());
                on_resp_call_ = e.on_resp_call;
            }
        }
    }
}

void tcp_http_client::url_to_get(const char *host, const char *path, const char *args) {
    std::string msg = "GET ";
    
    if (path && strlen(path) > 0)
        msg += path;
    else 
        msg += "/";
    
    if (args && strlen(args) > 0) {
        msg += "?";
        msg += args;
    }

    msg += " HTTP/1.1\r\n";

    msg += "Host: ";
    msg += host;
    msg += "\r\n";

    msg += "Connection: keep-alive\r\n";
    msg += "Accept: */*\r\n";

    msg += "Origin: ";
    msg += host;
    msg += "\r\n";

    msg += "\r\n";

    send_data(msg.c_str(), msg.length());
}

void tcp_http_client::url_to_post(const char *host, 
        const char *path, const char *args) {
    std::string msg = "POST ";
    
    if (path && strlen(path) > 0)
        msg += path;
    else 
        msg += "/";
    
    if (args && strlen(args) > 0) {
        msg += "?";
        msg += args;
    }

    msg += " HTTP/1.1\r\n";

    msg += "Host: ";
    msg += host;
    msg += "\r\n";

    msg += "Connection: keep-alive\r\n";
    msg += "Accept: */*\r\n";

    msg += "Origin: ";
    msg += host;
    msg += "\r\n";

    int body_len = 0;
    if (args)
        body_len = strlen(args);
    
    char sz_body[32] = {};
    sprintf(sz_body, "Content-Length: %d\r\n", body_len);
    msg += sz_body;

    msg += "\r\n";

    if (body_len > 0) 
        msg += args;
    
    send_data(msg.c_str(), msg.length());
    
}

bool tcp_http_client::connect_to_ip(int af, const char *ip, unsigned short port) {
    if (!ip) 
        return false;
    
    if (INVALID_SOCKET == init_socket(af, 10240, 102400))
        return false;
    
    if (SOCKET_ERROR == connect(ip, port))
        return false;
    
    SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "ip {} port {}", ip, port);
    return true;
}
void tcp_http_client::deatch_http_url(std::string http_url) {
    http_type_.clear();
    host_.clear();
    port_.clear();
    path_.clear();
    args_.clear();

    auto pos1 = http_url.find("://");
    if (pos1 != std::string::npos) {
        http_type_ = http_url.substr(0, pos1);
        pos1 += 3;
    } else {
        pos1 = 0;
    }

    auto pos2 = http_url.find('/', pos1);
    if (pos2 != std::string::npos) {
        host_ = http_url.substr(pos1, pos2 - pos1);
        path_ = http_url.substr(pos2);

        pos1 = path_.find('?');
        if (pos1 != std::string::npos) {
            args_ = path_.substr(pos1 + 1);
            path_ = path_.substr(0, pos1);
        }
    } else {
        host_ = http_url.substr(pos1);
    }

    pos1 = host_.find(':');
    if (pos1 != std::string::npos) {
        port_ = host_.substr(pos1 + 1);
        host_ = host_.substr(0, pos1);
    }
}
} // namespace nemausa
} // namespace io