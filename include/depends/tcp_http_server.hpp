/**
* @file tcp_http_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-26-15-30
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_TCP_HTTP_SERVER_HPP_
#define NEMAUSA_IO_TCP_HTTP_SERVER_HPP_

#include "depends/tcp_mgr.hpp"
#include "depends/http_clients.hpp"

namespace nemausa {
namespace io {

class tcp_http_server : public tcp_mgr  {
    virtual client * make_client(SOCKET csock);
    virtual void on_msg(server *pserver, client *pclient, data_header *header);
    virtual void on_msg_http(server *pserver, http_clients *pclient);
};

} // namespace nemausa
} // namespace io

#endif // NEMAUSA_IO_TCP_HTTP_SERVER_HPP_