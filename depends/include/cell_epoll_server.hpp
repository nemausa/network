/**
* @file cell_epoll_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-23-21-06
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_EPOLL_SERVER
#define CELL_EPOLL_SERVER

#include "cell_server.hpp"
#include "cell_epoll.hpp"

class cell_epoll_server : public cell_server {
public:
    cell_epoll_server();
    ~cell_epoll_server() noexcept;
    bool do_events();
    void rm_client(cell_client *pclient);
    void on_join(cell_client *pclient);
private:
    cell_epoll ep_;
};

#endif // CELL_EPOLL_SERVER