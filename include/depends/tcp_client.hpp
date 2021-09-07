/**
* @file tcp_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-07-21-06
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_TCP_CLIENT
#define EASY_TCP_CLIENT

#include "cell.hpp"
#include "network.hpp"
#include "message_header.hpp"
#include "client.hpp"
#include "fdset.hpp"

#include <stdio.h>
#include <atomic>
#include <string>

#include "message_header.hpp"

namespace nemausa {
namespace io {

class tcp_client {
public:
    tcp_client();
    virtual ~tcp_client();
    SOCKET init_socket(int af, int send_size, int redv_size);
    int connect(const char *ip, unsigned short port);
    virtual void close();
    virtual bool on_run(int microseconds = 1) = 0;
    bool is_run();
    int recv_data();
    void do_msg();
    virtual void on_msg(data_header *header) = 0;
    int send_data(data_header *header);
    int send_data(const char *data, int length);
    void set_scope_id_name(std::string scope_id_name);
    virtual client * make_client(SOCKET csock, int send_size, int recv_size);
    virtual void on_init_socket();
    virtual void on_connect();
    virtual void on_disconnect();
protected:
    client* pclient_ = nullptr;
    int af_ = AF_INET;
    std::string scope_id_name_;
    bool is_connect_ = false;
};

} // namespace io 
} // namespace nemausa

#endif // EASY_TCP_CLIENT