/**
* @file websocket_clienc.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-10-27-09-14
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_WEBSOCKET_CLIENC_HPP_
#define NEMAUSA_IO_WEBSOCKET_CLIENC_HPP_

#include "http_clientc.hpp"
#include "sha1.hpp"
#include "base64.hpp"
#include "websocket.hpp"

namespace nemausa {
namespace io {
class websocket_clientc : public http_clientc
{
private:
    websocket_header wsh_ = {};
    int32_t mask_key_ = rand();
public:
    websocket_clientc(SOCKET sockfd = INVALID_SOCKET,
            int send_size = SEND_BUFF_SIZE,
            int recv_size = RECV_BUFF_SIZE);
    virtual bool has_msg();
    virtual void pop_msg();
    virtual bool has_msgws();
    char * fetch_data();
    void do_mask(int len);
    int write_header(websocket_code_e opcode, 
            uint64_t len, 
            bool mask, 
            int32_t mask_key);
    int write_text(const char * pdata, int len);
    int ping();
    int pong();
    websocket_header & websocket_header();
};

} // namespace nemausa
} // namespace io

#endif // NEMAUSA_IO_WEBSOCKET_CLIENC_HPP_