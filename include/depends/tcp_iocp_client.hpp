/**
* @file tcp_iocp_client.hpp
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

#include "tcp_client.hpp"
#include "iocp.hpp"

namespace nemausa {
namespace io {

class tcp_iocp_client : public tcp_client {
public:
    void on_init_socket();
    void close();
    virtual bool on_run(int microseconds = 1);
protected:
    int do_iocp_net_events(int microseconds);
protected:
    iocp iocp_;
    io_event ioevent_ = {};
};

} // namespace io 
} // namespace nemausa

#endif // _WIN32
#endif // EASY_IOCP_CLIENT