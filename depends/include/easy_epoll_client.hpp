/**
* @file easy_epoll_client.hpp
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

#include "easy_tcp_client.hpp"
#include "cell_epoll.hpp"

class easy_epoll_client : public easy_tcp_client {
public:
    virtual void on_init_socket();
    void close();
    bool on_run(int microseconds = 1);
protected:
    cell_epoll ep_;
};
#endif

#endif // EASY_EPOLL_CLIENT