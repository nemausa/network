/**
* @file tcp_epoll_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-25-13-58
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_EPOLL_CLIENT
#define EASY_EPOLL_CLIENT

#if __linux__

#include "tcp_client.hpp"
#include "epoll.hpp"

namespace nemausa {
namespace io {

class tcp_epoll_client : public tcp_client {
public:
    virtual void on_init_socket();
    void close();
    virtual bool on_run(int microseconds = 1);
protected:
    epoll ep_;
};

} // namespace io 
} // namespace nemausa

#endif // __linux__
#endif // EASY_EPOLL_CLIENT