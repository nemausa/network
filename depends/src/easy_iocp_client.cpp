#ifdef _WIN32

#include "depends/easy_iocp_client.hpp" 
#include "depends/cell_log.hpp"

void easy_iocp_client::on_init_socket() {
    iocp_.create();
    iocp_.reg(pclient_->sockfd(), pclient_);
}

void easy_iocp_client::close() {
    iocp_.destory();
    easy_tcp_client::close();
}

bool easy_iocp_client::on_run(int microseconds) {
    if (is_run()) {
        if (pclient_->need_write()) {
            auto p_iodata = pclient_->make_send_iodata();
            if (p_iodata) {
                if (!iocp_.post_send(p_iodata)) {
                    close();
                    return false;
                }
            }
            p_iodata = pclient_->make_recv_iodata();
            if (p_iodata) {
                if (!iocp_.post_recv(p_iodata)) {
                    close();
                    return false;
                }
            }
        } else {
            auto p_iodata = pclient_->make_recv_iodata();
            if (p_iodata) {
                if (!iocp_.post_recv(p_iodata)) {
                    close();
                    return false;
                }
            }
        }

        while (true) {
            int ret = do_iocp_net_events(microseconds);
            if (ret < 0) {
                return false;
            } else if (ret ==0) {
                do_msg();
                return true;
            }               
        }
        return true;
    }
    return false;
}

int easy_iocp_client::do_iocp_net_events(int microseconds) {
    int ret = iocp_.wait(ioevent_, microseconds);
    if (ret <0) {
        LOG_INFO("easy_iocp_client.do_iocp_net_events.wait clientid<%d> sockfd<%d>", pclient_->id_, (int)pclient_->sockfd());
        return ret;
    } else if (ret == 0) {
        return ret;
    }

    if (io_type_e::RECV == ioevent_.p_io_data->io_type) {
        if (ioevent_.bytes_trans <= 0) {
            LOG_INFO("easy_ioco_client.do_iocp_net_events sockfd%d RECV bytes_trans=%d", pclient_->sockfd(), ioevent_.bytes_trans);
            close();
            return -1;
        }
    } else if (io_type_e::SEND == ioevent_.p_io_data->io_type) {
        if (ioevent_.bytes_trans <= 0) {
            LOG_INFO("easy_ioco_client.do_iocp_net_events sockfd%d RECV bytes_trans=%d", pclient_->sockfd(), ioevent_.bytes_trans);
            close();
            return -1;
        }
        cell_client *pclient = (cell_client*)ioevent_.data.ptr;
        if (pclient) {
            pclient->send_to_iocp(ioevent_.bytes_trans);
        }
    } else {
        LOG_INFO("undefine io type.");
    }
    return ret;
}

#endif