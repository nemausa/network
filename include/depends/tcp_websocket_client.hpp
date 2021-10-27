/**
* @file tcp_websocket_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-10-26-14-53
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NEMAUSA_IO_TCP_WEBSOCKET_CLIENT_HPP_
#define NEMAUSA_IO_TCP_WEBSOCKET_CLIENT_HPP_
#include <queue>

#include "tcp_mgr.hpp"
#include "websocket_clientc.hpp"

#include "sha1.hpp"
#include "base64.hpp"

namespace nemausa {
namespace io {

class tcp_websocket_client : public tcp_client_mgr
{

};

} // namespace nemausa
} // namespace io

#endif // NEMAUSA_IO_TCP_WEBSOCKET_CLIENT_HPP_