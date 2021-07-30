/**
* @file easy_tcp_client.hpp
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
#include "cell_network.hpp"
#include "message_header.hpp"
#include "cell_client.hpp"
#include "cell_fdset.hpp"

#include <stdio.h>
#include <atomic>

#include "message_header.hpp"
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#endif
class easy_tcp_client {
public:
    easy_tcp_client();
    virtual ~easy_tcp_client();
    SOCKET init_socket(int send_size = SEND_BUFF_SIZE, int redv_size = RECV_BUFF_SIZE);
    int connect(const char *ip, unsigned short port);
    void close();
    bool on_run(int microseconds = 1);
    bool is_run();
    int recv_data();
    void do_msg();
    virtual void on_msg(data_header *header) = 0;
    int send_data(data_header *header);
    int send_data(const char *data, int length);
    virtual void on_init_socket();
protected:
    cell_fdset fd_read_;
    cell_fdset fd_write_;
    cell_client* pclient_ = nullptr;
    bool is_connect_ = false;
};

#endif // EASY_TCP_CLIENT