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

    iocp.post_accept(&iodata, af_);
    io_event ioevent = {};
    while (pthread->is_run()) {
        time4msg();
        int ret = iocp.wait(ioevent, 1);
        if (ret < 0) {
            //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                    // "tcp_iocp_server.on_run wait exit");
            pthread->exit();
            break;
        }
        if (ret == 0) {
            continue;
        }

        if (io_type_e::ACCEPT == ioevent.p_io_data->io_type) {
            char *ip = iocp.get_accept_addrs(ioevent.p_io_data, af_);
            iocp_accept(ioevent.p_io_data->sockfd, ip);
            iocp.post_accept(&iodata, af_);
        }
    }
}

SOCKET tcp_iocp_server::iocp_accept(SOCKET csock, char *ip) {
    if (INVALID_SOCKET == csock) {
        // SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), "accept invalid socket");
    } else {
        accept_client(csock, ip);
    }
    return csock;
}

#endif

} // namespace io 
} // namespace nemausa