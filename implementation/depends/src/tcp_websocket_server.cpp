/**
* @file tcp_websocket_server.cpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-10-26-10-52
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_TCP_WEBSOCKET_SERVER_HPP_
#define NEMAUSA_IO_TCP_WEBSOCKET_SERVER_HPP_

#include "tcp_mgr.hpp"
#include "web_socket_clients.hpp"

namespace nemausa {
namespace io {

class tcp_websocket_server : public tcp_mgr 
{
public:
    virtual client * make_client(SOCKET csock);
    virtual void on_net_msg(server *pserver, client *pclient, data_header *header);
    virtual void on_net_msgws(server *pserver, web_socket_clients *pwd_client);
};

} // namespace nemausa
} // namespace io

#endif // NEMAUSA_IO_TCP_WEBSOCKET_SERVER_HPP_