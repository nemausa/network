#include <thread>
#include <cstring>

#include "depends/tcp_mgr.hpp"
#include "utils/conf.hpp"
#include "depends/tcp_http_server.hpp"
#include "depends/sha1.hpp"
#include "depends/base64.hpp"

using namespace nemausa::io;
class MyServer : public tcp_http_server 
{
public:
    virtual void on_msg(server *pserver, client *pclient, data_header *header)
    {
        tcp_server::on_msg(pserver, pclient, header);
        http_clients *http_client = dynamic_cast<http_clients*>(pclient);
        if (!http_client)
        {
            return;
        }

        if (!http_client->request_info())
        {
            return;
        }

        auto str_upgrade = http_client->header_str("Upgrade", "");
        if (0 != strcmp(str_upgrade, "websocket"))
        {
            LOG(ERROR) << "not found Upgrade:websocket";
            return;
        }

        auto c_key = http_client->header_str("Sec-WebSocket-Key", nullptr);
        if (!c_key)
        {
            LOG(ERROR) << "not found Sec-WebSocket-Key";
            return;
        }

		std::string s_key = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        s_key = c_key + s_key;

        unsigned char str_sha1[20] = {0};
        SHA1_String((const unsigned char*)s_key.c_str(), s_key.length(), str_sha1);

        std::string s_key_accept = Base64Encode(str_sha1, 20);

        char resp[256] = {0};
        strcat(resp, "HTTP/1.1 101 Switching Protocols\r\n");
		strcat(resp, "Connection: Upgrade\r\n");
		strcat(resp, "Upgrade: websocket\r\n");
		strcat(resp, "Sec-WebSocket-Accept: ");
		strcat(resp, s_key_accept.c_str());
		strcat(resp, "\r\n\r\n");

		http_client->send_data(resp, strlen(resp));
    }
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

    config::instance().load("websocket_server.conf");
    const char *ip = config::instance().get_string("ip");


    int port = config::instance().get_int_default("port", 4567);
    int thread_num = config::instance().get_int_default("thread_num", 1);

    if (config::instance().has_key("-p")) 
    {
        LOG(INFO) << "has key -p";
    }
    MyServer server;
    if (config::instance().has_key("ipv6")) 
    {
        LOG(INFO) << "ipv6";
        server.init_socket(AF_INET6);
    } 
    else 
    {
        LOG(INFO) << "ipv4";
        server.init_socket(AF_INET);
    }
    server.bind(ip, port);
    server.listen(SOMAXCONN);
    server.start(thread_num);


    while(true) 
    {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) 
        {
            LOG(INFO) << "exit thread";
            server.close();
            break;
        } 
        else 
        {
            LOG(INFO) << "undefined commad";
        }
    }

    LOG(INFO) << "exit";
    google::ShutdownGoogleLogging();
    return 0;
}