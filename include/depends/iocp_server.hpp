/**
* @file iocp_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-29-18-40
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef IOCP_SERVER
#define IOCP_SERVER

#ifdef _WIN32

#include "server.hpp"
#include "iocp.hpp"

namespace nemausa {
namespace io {

class iocp_server : public server {
public:
    iocp_server();
    ~iocp_server();
    virtual bool do_net_events();
    int do_iocp_net_events();
private:
    iocp iocp_;
    io_event io_event_ = {};    
};

} // namespace io 
} // namespace nemausa

#endif // _WIN32
#endif // IOCP_SERVER