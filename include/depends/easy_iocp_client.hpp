/**
* @file easy_iocp_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-30-14-07
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_IOCP_CLIENT
#define EASY_IOCP_CLIENT

#ifdef _WIN32

#include "easy_tcp_client.hpp"
#include "cell_iocp.hpp"

class easy_iocp_client : public easy_tcp_client {
public:
    void on_init_socket();
    void close();
    bool on_run(int microseconds = 1);
protected:
    int do_iocp_net_events(int microseconds);
protected:
    cell_iocp iocp_;
    io_event ioevent_ = {};
};

#endif

#endif // EASY_IOCP_CLIENT