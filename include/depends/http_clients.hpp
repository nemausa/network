/**
* @file http_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-18-23-05
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_HTTP_CLIENT_HPP_
#define NEMAUSA_IO_HTTP_CLIENT_HPP_

#include <map>

#include "depends/client.hpp"

namespace nemausa {
namespace io {

class http_clients : public client {
public:
    enum request_type_e {
        GET = 10,
        POST,
        UNKNOWN
    };
    http_clients(SOCKET sockfd, int send_size, int recv_size);
    virtual bool has_msg();
    int check_request();
    void split_url_args(char *args);
    bool request_info();
    bool request_args(char *request_line);
    virtual void pop_msg();
    bool can_write(int size);
    void resp_400_bad();
    void resp_400_not_found();
    void resp_200_ok(const char *body_buff, int body_len);
    void write_response(const char *code, const char *body_buff, int body_len); 
    char * url();
    bool url_compare(const char *str);
    bool has_args(const char *key);
    bool has_header(const char *key);
    int args_int(const char *name, int def);
    const char * args_str(const char *agr_name, const char *def);
    const char * header_str(const char *arg_name, const char *def);
    void on_send_complete();
protected:
    int header_len_ = 0;
    int body_len_ = 0;
    std::map<std::string, char*> header_map_;
    std::map<std::string, char*> args_map_;
    request_type_e request_type_ = http_clients::UNKNOWN;
    char *method_;
    char *url_;
    char *url_path_;
    char *url_args_;
    char *http_version_;
    bool keep_alive_ = true;
};

} // namesapce nemausa
} // namespace io

#endif // NEMAUSA_IO_HTTP_CLIENT_HPP_