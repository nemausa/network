/**
* @file cell_iocp_server.hpp
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
#ifndef CELL_IOCP_SERVER
#define CELL_IOCP_SERVER

#if _WIN32

#include "cell_server.hpp"
#include "cell_iocp.hpp"

class cell_iocp_server : public cell_server {
public:
    cell_iocp_server();
    ~cell_iocp_server();
    bool do_net_events();
    int do_iocp_net_events();
    void rm_client(cell_client *pclient);
    void rm_client(io_event &_io_event);
private:
    cell_iocp iocp_;
    io_event io_event_ = {};    
};

#endif

#endif // CELL_IOCP_SERVER