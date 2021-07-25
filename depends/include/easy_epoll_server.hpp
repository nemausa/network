/**
* @file esay_epoll_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-24-22-42
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef ESAY_EPOLL_SERVER
#define ESAY_EPOLL_SERVER

#if __linux__

#include "easy_tcp_server.hpp"
#include "cell_epoll_server.hpp"
#include "cell_epoll.hpp"

class easy_epoll_server : public easy_tcp_server {
public:
    void start(int n);
protected:
    void on_run(cell_thread *pthread);
};

#endif

#endif // ESAY_EPOLL_SERVER