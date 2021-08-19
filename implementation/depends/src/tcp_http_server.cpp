#include "depends/tcp_http_server.hpp"

namespace nemausa {
namespace io {

client * tcp_http_server::make_client(SOCKET csock) {
    return new http_clients(csock, send_buffer_size_, recv_buffer_size_);
}

void tcp_http_server::on_msg(server *pserver, client *pclient, data_header *header) {
    tcp_server::on_msg(pserver, pclient, header);
    http_clients *p_http_client = dynamic_cast<http_clients*>(pclient);
    if (!p_http_client)
        return;
    
    if (!p_http_client->request_info())
        return;

    p_http_client->reset_heart_time();
    on_msg_http(pserver, p_http_client);
}

void tcp_http_server::on_msg_http(server *pserver, http_clients *p_http_client) {

}

} // namespace nemausa
} // namespace io