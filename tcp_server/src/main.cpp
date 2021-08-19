#include <thread>
#include <cstring>

#include "depends/tcp_mgr.hpp"
#include "utils/conf.hpp"

#ifndef VERSION
const char *VERSION_INFO = "version: 0.0.1";
#endif

using namespace nemausa::io;
class MyServer : public tcp_mgr {
public:
    MyServer() {
        send_back_ = config::instance().has_key("sendback");
        send_full_ = config::instance().has_key("sendfull");
        check_msg_id_ = config::instance().has_key("check_msg_id");
    }

    virtual void on_join(client *pclient) {
        tcp_mgr::on_join(pclient);
    }

    virtual void on_leave(client *pclient) {
        tcp_mgr::on_leave(pclient);
    }

    virtual void on_msg(server *server, client *pclient, 
            data_header *header) {
        tcp_server::on_msg(server, pclient, header);
        switch(header->cmd) {
        case CMD_LOGIN: {
            pclient->reset_heart_time();
            login *lg = (login*)header;
            if (check_msg_id_) {
                if (lg->msg_id != pclient->recv_id) {
                     SPDLOG_LOGGER_ERROR(spdlog::get(MULTI_SINKS), 
                            "on_msg socket<{}> msg_id<{}> recv_msg_id<{}> {}",
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
                         SPDLOG_LOGGER_WARN(spdlog::get(MULTI_SINKS), 
                         "socket<{}> send full", pclient->sockfd());
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
            SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                    "recv socket<{}> undefine msgtype, datalen: {}",
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

    auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    auto console_logger = std::make_shared<spdlog::logger>(CONSOLE_SINK, console_sink); 
    spdlog::register_logger(console_logger);

    auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>
            ("logs/server.txt", 23, 59);
    auto file_logger = std::make_shared<spdlog::logger>(FILE_SINK, file_sink);
    spdlog::register_logger(file_logger);

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    auto combined_logger = 
            std::make_shared<spdlog::logger>(MULTI_SINKS, begin(sinks), end(sinks));
    //register it if you need to access it globally
    combined_logger->set_pattern(
            "[%Y-%m-%d %H:%M:%S.%e] [%-6t] [%^%-6l%$] [%-5n] [%!] [%#]  %v"); 
    spdlog::register_logger(combined_logger);
    spdlog::flush_every(std::chrono::seconds(5));

    int port = config::instance().get_int_default("port", 4567);
    int thread_num = config::instance().get_int_default("thread_num", 1);

    if (config::instance().has_key("-p")) {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "has key -p");
    }
    MyServer server;
    if (config::instance().has_key("ipv6")) {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "ipv6");
        server.init_socket(AF_INET6);
    } else {
        SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "ipv4");
        server.init_socket(AF_INET);
    }
    server.bind(ip, port);
    server.listen(SOMAXCONN);
    server.start(thread_num);


    while(true) {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) {
            SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "exit thread");
            server.close();
            break;
        } else {
            SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "undefined commad");
        }
    }

    SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "exit");
    getchar();
    return 0;
}