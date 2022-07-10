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
                    //  SPDLOG_LOGGER_ERROR(spdlog::get(MULTI_SINKS), 
                    //         "on_msg socket<{}> msg_id<{}> recv_msg_id<{}> {}",
                    //         pclient->sockfd(),lg->msg_id, pclient->recv_id,
                    //         lg->msg_id - pclient->recv_id);
                }
                ++pclient->recv_id;
            }
            if (send_back_) {
                login_result ret;
                ret.msg_id = pclient->send_id;
                if (SOCKET_ERROR == pclient->send_data(&ret)) {
                    if (send_full_) {
                        //  SPDLOG_LOGGER_WARN(spdlog::get(MULTI_SINKS), 
                        //  "socket<{}> send full", pclient->sockfd());
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
            //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                    // "recv socket<{}> undefine msgtype, datalen: {}",
                    // pclient->sockfd(), header->length);
        break;
        }
    }
private:
    bool send_back_;
    bool send_full_;
    bool check_msg_id_;
};

int main(int argc, char *args[]) {

    // 初始化参数
    FLAGS_logtostderr = false;  //TRUE:标准输出,FALSE:文件输出
    FLAGS_alsologtostderr = true;  //除了日志文件之外是否需要标准输出
    FLAGS_colorlogtostderr = true;  //标准输出带颜色
    FLAGS_logbufsecs = 0;   //设置可以缓冲日志的最大秒数，0指实时输出
    FLAGS_max_log_size = 10;  //日志文件大小(单位：MB)
    FLAGS_stop_logging_if_full_disk = true; //磁盘满时是否记录到磁盘
    google::InitGoogleLogging("tcp_server");
    google::SetLogDestination(google::GLOG_INFO,"./log/");
    LOG(INFO) << "this is log";
    LOG(WARNING) << "this is warnning";
    LOG(ERROR) << "this is error";

    config::instance().load("server.conf");
    const char *ip = config::instance().get_string("ip");


    int port = config::instance().get_int_default("port", 4567);
    int thread_num = config::instance().get_int_default("thread_num", 1);

    if (config::instance().has_key("-p")) {
        LOG(INFO) << "has key -p";
    }
    MyServer server;
    if (config::instance().has_key("ipv6")) {
        LOG(INFO) << "ipv6";
        server.init_socket(AF_INET6);
    } else {
        LOG(INFO) << "ipv4";
        server.init_socket(AF_INET);
    }
    server.bind(ip, port);
    server.listen(SOMAXCONN);
    server.start(thread_num);


    while(true) {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) {
            LOG(INFO) << "exit thread";
            server.close();
            break;
        } else {
            LOG(INFO) << "undefined commad";
        }
    }

    LOG(INFO) << "exit";
    google::ShutdownGoogleLogging();
    getchar();
    return 0;
}