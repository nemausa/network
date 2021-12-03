/**
* @file websocket_clients.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-10-27-14-49
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_WEBSOCKET_CLIENTS_HPP_
#define NEMAUSA_IO_WEBSOCKET_CLIENTS_HPP_

#include "http_clients.hpp"
#include "sha1.hpp"
#include "base64.hpp"
#include "websocket.hpp"

namespace nemausa {
namespace io {

class websocket_clients : public http_clients
{
private:
    websocket_header wsh_ = {};
public:
    websocket_clients(SOCKET sockfd = INVALID_SOCKET,
            int send_size = SEND_BUFF_SIZE,
            int recv_size = RECV_BUFF_SIZE);
    bool hand_shake();
    virtual bool has_msg();
    virtual void pop_msg();
    virtual bool has_msgws();
    char * fetch_data();
    int write_header(websocket_code_e opcode, uint64_t len);
    int write_text(const char *pdata, int len);
    int ping();
    int pong();
    websocket_header & websocket_header();
};

} // namespace nemausa
} // namespace io

#endif // NEMAUSA_IO_WEBSOCKET_CLIENTS_HPP_