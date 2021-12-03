#include "websocket_clients.hpp"

namespace nemausa {
namespace io {

websocket_clients::websocket_clients(SOCKET sockfd,
        int send_size, int recv_size):
        http_clients(sockfd, send_size, recv_size)
{

}

bool websocket_clients::hand_shake()
{
    
}
} // namespace nemausa
} // namespace io