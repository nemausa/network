#include <thread>
#include <cstring>
#include "cell_log.hpp"
#include "cell_config.hpp"
#include "easy_server_mgr.hpp"
#ifndef VERSION
const char *VERSION_INFO = "version: 0.0.1";
#endif


class MyServer : public easy_server_mgr {
public:
    MyServer() {
        send_back_ = cell_config::instance().has_key("sendback");
        send_full_ = cell_config::instance().has_key("sendfull");
        check_msg_id_ = cell_config::instance().has_key("check_msg_id");
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
    cell_log::instance().set_path("server_log.txt", "w", false);
    cell_config::instance().init(argc, args);

    const char *ip = cell_config::instance().get_string("ip", "any");
    uint16_t port = cell_config::instance().get_int("port", 4567);
    int thread_num = cell_config::instance().get_int("thread_num", 1);

    if (cell_config::instance().has_key("-p")) {
        LOG_INFO("has key -p");
    }
    if (strcmp(ip, "any") == 0) {
        ip = nullptr;
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