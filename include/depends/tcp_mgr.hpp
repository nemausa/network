/**
* @file tcp_mgr.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-03-23-23
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_SERVER_MGR
#define EASY_SERVER_MGR

#include "tcp_epoll_server.hpp"
#include "tcp_iocp_server.hpp"
#include "tcp_select_server.hpp"

#include "tcp_epoll_client.hpp"
#include "tcp_iocp_client.hpp"
#include "tcp_select_client.hpp"


namespace nemausa {
namespace io {

#ifdef _WIN32
    // typedef tcp_select_server tcp_mgr;
    typedef tcp_iocp_server tcp_mgr;
    typedef tcp_iocp_client tcp_client_mgr;
#elif __linux__
    typedef tcp_epoll_server tcp_mgr; 
    typedef tcp_epoll_client tcp_client_mgr;
#else
    typedef tcp_select_server tcp_mgr;
    typedef tcp_select_client tcp_client_mgr;
#endif

} // namespace io 
} // namespace nemausa

#endif // EASY_SERVER_MGR