/**
* @file tcp_http_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-26-15-49
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_TCP_HTTP_CLIENT_HPP_
#define NEMAUSA_IO_TCP_HTTP_CLIENT_HPP_

#include <queue>

#include "tcp_mgr.hpp"
#include "http_clientc.hpp"

namespace nemausa {
namespace io {

class tcp_http_client : public tcp_client_mgr {
public:
    tcp_http_client();
protected:
    virtual client * make_client(SOCKET csock, int send_size, int recv_size);
private:
    typedef std::function<void(http_clientc*)> event_call;
    struct event {
        std::string http_url;
        event_call on_resp_call = nullptr;
        bool is_get = true;
        uint8_t count = 0;
        uint8_t max_count = 1;
    };
    std::queue<event> event_queue_;
    bool next_request_  = false;
public:
    virtual void on_msg(data_header *header);
    virtual void on_disconnect();
    void get(const char *http_url, event_call on_resp_call);
    void post(const char *http_url, event_call on_resp_call);
    void post(const char *http_url, const char *data, event_call on_resp_call);
    int hostname_to_ip(const char *hostname, const char *port);
    void next_request();
    void url_to_get(const char *host, const char *path, const char *args);
    void url_to_post(const char *host, const char *paht, const char *args);
    bool connect_to_ip(int af, const char *ip, unsigned short port);
    void deatch_http_url(std::string httpurl);
private:
    std::string http_type_;
    std::string host_;
    std::string port_;
    std::string path_;
    std::string args_;
    std::string host0_;
    unsigned short port0_;
    event_call on_resp_call_ = nullptr; 
};

} // namespace nemausa
} // namespace io

#endif // NEMAUSA_IO_TCP_HTTP_CLIENT_HPP_