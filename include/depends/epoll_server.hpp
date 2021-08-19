/**
* @file epoll_server.hpp
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
#ifndef EPOLL_SERVER
#define EPOLL_SERVER

#if __linux__

#include "server.hpp"
#include "epoll.hpp"

namespace nemausa {
namespace io {

class epoll_server : public server {
public:
    epoll_server();
    ~epoll_server() noexcept;
    virtual bool do_net_events();
    void rm_client(client *pclient);
    void on_join(client *pclient);
private:
    epoll ep_;
};

} // namespace io
} // namesapce nemausa

#endif // __linux__
#endif // EPOLL_SERVER
