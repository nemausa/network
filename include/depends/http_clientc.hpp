/**
* @file http_clientc.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-26-15-57
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_HTTP_CLIENTC_HPP_
#define NEMAUSA_IO_HTTP_CLIENTC_HPP_

#include <map>

#include "client.hpp"
#include "split_string.hpp"
#include "key_string.hpp"

namespace nemausa {
namespace io {

class http_clientc : public client {
public:
    http_clientc(SOCKET sockfd = INVALID_SOCKET, 
                 int send_buffer_szie = SEND_BUFF_SIZE, 
                 int recv_buffer_size = RECV_BUFF_SIZE);
    virtual bool has_msg();
    int check_response();
    bool get_response_info();
    void split_url_args(char *args);
    virtual void pop_msg();
    bool has_args(const char *key);
    bool has_header(const char *key);
    int args_int(const char *name, int def);
    const char * args_str(const char *agr_name, const char *def);
    const char * header_str(const char *arg_name, const char *def);
    void on_recv_complete();
    const char *content();
protected:
    int header_len_ = 0;
    int body_len_ = 0;
    std::map<key_string, char*> header_map_;
    std::map<key_string, char*> args_map_;
    bool keep_alive_ = true;
};

} // namespace nemausa
} // namespace io

#endif // NEMAUSA_IO_HTTP_CLIENTC_HPP_