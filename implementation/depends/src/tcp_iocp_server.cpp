#include "depends/tcp_iocp_server.hpp" 
#include "depends/network.hpp"

namespace nemausa {
namespace io {

#ifdef _WIN32

void tcp_iocp_server::start(int n) {
    tcp_server::start<iocp_server>(n);
}

void tcp_iocp_server::on_run(cell_thread *pthread) {
    iocp iocp;
    iocp.create();
    iocp.reg(sockfd());
    iocp.load_accept(sockfd());

    const int len = 1024;
    char buf[len] = {};

    io_data_base iodata = {};
    iodata.wsabuff.buf = buf;
    iodata.wsabuff.len = len;

    iocp.post_accept(&iodata);
    io_event ioevent = {};
    while (pthread->is_run()) {
        time4msg();
        int ret = iocp.wait(ioevent, 1);
        if (ret < 0) {
            SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), 
                    "tcp_iocp_server.on_run wait exit");
            pthread->exit();
            break;
        }
        if (ret == 0) {
            continue;
        }

        if (io_type_e::ACCEPT == ioevent.p_io_data->io_type) {
            iocp_accept(ioevent.p_io_data->sockfd);
            iocp.post_accept(&iodata);
        }
    }
}

SOCKET tcp_iocp_server::iocp_accept(SOCKET sock) {
   if (INVALID_SOCKET == sock) {
       SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "accept INVALID_SOCKET");
   } else {
       if (client_count_ < max_client_) {
           client *pclient = new client(sock, send_buffer_size_, recv_buffer_size_);
           if (!pclient) {
                SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), "new client error");           
           }
           add_client_to_server(pclient);
       } else {
           network::destory_socket(sock);
           SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "accept to maxclient");           
       }
   }
   return sock;
}

#endif

} // namespace io 
} // namespace nemausa