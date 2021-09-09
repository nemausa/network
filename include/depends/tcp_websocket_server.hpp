/**
* @file tcp_websocket_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-09-09-09-52
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_TCP_WEBSOCKET_SERVER_HPP_
#define NEMAUSA_IO_TCP_WEBSOCKET_SERVER_HPP_

#include "tcp_mgr.hpp"
// #include "websocket_clients.hpp"
namespace nemausa {
namespace io {

class tcp_websocket_server : public tcp_mgr {
public:
    virtual client * make_client(SOCKET csock, int send_size, int recv_size);
    virtual void on_msg(server *pserver, client *pclient, data_header *header);
    virtual void on_msgws(server *pserver);
}; 

} // namespace nemausa
} // namespace io



#endif // NEMAUSA_IO_TCP_WEBSOCKET_SERVER_HPP_