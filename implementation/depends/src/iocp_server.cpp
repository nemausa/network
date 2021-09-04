#include "depends/iocp_server.hpp"

namespace nemausa {
namespace io {

#ifdef _WIN32

iocp_server::iocp_server() {
    iocp_.create();
}

iocp_server::~iocp_server() noexcept {
    close();
}

bool iocp_server::do_net_events() {
    client *pclient = nullptr;
    for (auto iter = clients_.begin(); iter != clients_.end(); iter++) {
        pclient = iter->second;
        if (pclient->need_write()) {
            auto p_io_data = pclient->make_send_iodata();
            if (p_io_data) {
                if (!iocp_.post_send(p_io_data)) {
                    pclient->post_send_complete();
                    pclient->on_close();
                    continue;
                }
            }
            p_io_data = pclient->make_recv_iodata();
            if (p_io_data) {
                if (!iocp_.post_recv(p_io_data)) {
                    pclient->post_recv_complete();
                    pclient->on_close();
                }
            }
        } else {
            auto p_io_data = pclient->make_recv_iodata();
            if (p_io_data) {
                if (!iocp_.post_recv(p_io_data)) {
                    pclient->post_recv_complete();
                    pclient->on_close();
                }
            }
        }
    }
    while (true) {
        int ret = do_iocp_net_events();
        if (ret <0) {
            return false;
        } else if (ret == 0) {
            return true;
        }
    }
    return true;
}

// 每次只处理一件网络事件
// ret = -1 iocp error
// ret = 0  nothing
// ret = 1  iocp events

int iocp_server::do_iocp_net_events() {
    int ret = iocp_.wait(io_event_, 1);
    if (ret < 0) {
        SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), 
                "iocp_server{}.wait", id_);
        return ret;
    } else if (ret == 0) {
        return ret;
    }

    if (io_type_e::RECV == io_event_.p_io_data->io_type) {
        client *pclient = (client*)io_event_.data.ptr;
        if (pclient) {
            if (io_event_.bytes_trans <= 0) {
                // SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), 
                //         "rm_client sockfd={}, RECV byte_trans = {}", 
                //         io_event_.p_io_data->sockfd, io_event_.bytes_trans);
                pclient->post_recv_complete();
                pclient->on_close();
                return ret;
            }
            pclient->recv_for_iocp(io_event_.bytes_trans);
            on_recv(pclient);
        }
    } else if (io_type_e::SEND == io_event_.p_io_data->io_type) {
        client *pclient = (client*)io_event_.data.ptr;
        if (pclient) {
            if (io_event_.bytes_trans <= 0) {
                // SPDLOG_LOGGER_ERROR(spdlog::get(FILE_SINK), 
                //         "rm_client sockfd={}, RECV byte_trans = {}", 
                //         io_event_.p_io_data->sockfd, io_event_.bytes_trans);
                pclient->post_send_complete();
                pclient->on_close();
                return ret;
            }
            pclient->send_to_iocp(io_event_.bytes_trans);
        } 
    } else {
        SPDLOG_LOGGER_WARN(spdlog::get(MULTI_SINKS), "undefine io type");
    }
    return ret;
}

#endif

} // namespace io 
} // namespace nemausa