#include <thread>
#include <cstring>

#include "depends/cell_log.hpp"
#include "depends/cell_config.hpp"
#include "depends/easy_server_mgr.hpp"

#include "utils/conf.hpp"
//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/basic_file_sink.h"
//#include "spdlog/sinks/daily_file_sink.h"
#ifndef VERSION
const char *VERSION_INFO = "version: 0.0.1";
#endif


class MyServer : public easy_server_mgr {
public:
    MyServer() {
        send_back_ = config::instance().has_key("sendback");
        send_full_ = config::instance().has_key("sendfull");
        check_msg_id_ = config::instance().has_key("check_msg_id");
    }

    virtual void on_join(cell_client *pclient) {
        easy_server_mgr::on_join(pclient);
    }

    virtual void on_leave(cell_client *pclient) {
        easy_server_mgr::on_leave(pclient);
    }

    virtual void on_msg(cell_server *server, cell_client *pclient, data_header *header) {
        easy_tcp_server::on_msg(server, pclient, header);
        switch(header->cmd) {
        case CMD_LOGIN: {
            pclient->reset_heart_time();
            login *lg = (login*)header;
            if (check_msg_id_) {
                if (lg->msg_id != pclient->recv_id) {
                    LOG_ERROR("on_msg socket<%d> msg_id<%d> recv_msg_id<%d> %d",
                            pclient->sockfd(),lg->msg_id, pclient->recv_id,
                            lg->msg_id - pclient->recv_id);
                }
                ++pclient->recv_id;
            }
            if (send_back_) {
                login_result ret;
                ret.msg_id = pclient->send_id;
                if (SOCKET_ERROR == pclient->send_data(&ret)) {
                    if (send_full_) {
                        LOG_WARN("socket<%d> send full", pclient->sockfd());
                    }
                } else {
                    ++pclient->send_id;
                }
            }
        }
        break;
        case CMD_C2S_HEART: {
            pclient->reset_heart_time();
            s2c_heart ret;
            pclient->send_data(&ret);
        }
        break;
        default:
            LOG_INFO("recv socket<%d> undefine msgtype, datalen: %d",
                    pclient->sockfd(), header->length);
        break;
        }
    }
private:
    bool send_back_;
    bool send_full_;
    bool check_msg_id_;
};

int main(int argc, char *args[]) {

    config::instance().load("server.conf");
    const char *ip = config::instance().get_string("ip");
    cell_log::instance().set_path("server_log.txt", "w", false);

    int port = config::instance().get_int_default("port", 4567);
    int thread_num = config::instance().get_int_default("thread_num", 1);

    if (config::instance().has_key("-p")) {
        LOG_INFO("has key -p");
    }
    MyServer server;
    server.init_socket();
    server.bind(ip, port);
    server.listen(64);
    server.start(thread_num);


    while(true) {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) {
            LOG_INFO("exit thread\n");
            server.close();
            break;
        } else {
            LOG_INFO("undefined commad\n");
        }
    }

    LOG_INFO("exit\n");
    getchar();
    return 0;
}