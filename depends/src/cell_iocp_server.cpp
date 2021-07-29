#ifdef _WIN32

#include "cell_iocp_server.hpp"

cell_iocp_server::cell_iocp_server() {
    iocp_.create();
}

cell_iocp_server::~cell_iocp_server() noexcept {
    close();
}

bool cell_iocp_server::do_net_events() {
    cell_client *pclient = nullptr;
    for (auto iter = clients_.begin(); iter != clients_.end();) {
        pclient = iter->second;
        if (pclient->need_write()) {
            auto p_io_data = pclient->make_send_io_data();
        }
    }
}


#endif