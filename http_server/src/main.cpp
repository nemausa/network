#include <thread>
#include <cstring>

#include "utils/conf.hpp"
#include "depends/tcp_http_server.hpp"


using namespace nemausa::io;
class MyServer : public tcp_http_server {
public:
    virtual void on_msg_http(server *pserver, http_clients *p_http_client) {
        if (p_http_client->url_compare("/add")) {
            int a = p_http_client->args_int("a", 0);
            int b = p_http_client->args_int("b", 0);
            int c = a + b;

            char sz_body[32] = {};
            sprintf(sz_body, "a+b=%d", c);

            p_http_client->resp_200_ok(sz_body, strlen(sz_body));
        } else if (p_http_client->url_compare("/sub")) {
            int a = p_http_client->args_int("a", 0);
            int b = p_http_client->args_int("b", 0);
            int c = a - b;

            char sz_body[32] = {};
            sprintf(sz_body, "a-b=%d", c);

            p_http_client->resp_200_ok(sz_body, strlen(sz_body));
        } else if (p_http_client->url_compare("/jsonTest")) {
            auto token = p_http_client->args_str("token", nullptr);
            if (token) {
                auto json = p_http_client->args_str("json", "no json data");
                p_http_client->resp_200_ok(json, strlen(json));
            } else {
                auto ret = "{\"status\":\"error\"}";
                p_http_client->resp_200_ok(ret, strlen(ret));
            }
        } else {
            if (!respfile(p_http_client))
                p_http_client->resp_400_not_found();
        } 
    }

    bool respfile(http_clients *p_http_client) {
        std::string file_path;

        if (p_http_client->url_compare("/")) {
            file_path = www_root_ + p_http_client->url() + index_page_; 
        } else {
            file_path = www_root_ +p_http_client->url();
        }

        FILE *file = fopen(file_path.c_str(), "rb");
        if (!file)
            return false;
        
        fseek(file, 0, SEEK_END);
        auto size = ftell(file);
        rewind(file);

        if (!p_http_client->can_write(size)) {
            // SPDLOG_LOGGER_WARN(spdlog::get(MULTI_SINKS), "url={}", file_path);
            fclose(file);
            return false;
        }

        char *buff = new char[size];
        auto read_size = fread(buff, 1, size, file);
        if (read_size != size) {
            // SPDLOG_LOGGER_WARN(spdlog::get(MULTI_SINKS), "url={}", file_path);
            delete[] buff;
            fclose(file);
            return false;
        }

        p_http_client->resp_200_ok(buff, read_size);
        delete[] buff;
        fclose(file);
        return true;
    }

    void www_root(const char *www) {
        www_root_ = www;
    }

    void index_page(const char *index) {
        index_page_ = index;
    }

private:
    std::string www_root_;
    std::string index_page_;
};

int main(int argc, char *args[]) {

    config::instance().load("server.conf");
    const char *ip = config::instance().get_string("ip");

    // auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    // auto console_logger = std::make_shared<spdlog::logger>(CONSOLE_SINK, console_sink); 
    // spdlog::register_logger(console_logger);

    // auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>
    //         ("logs/server.txt", 23, 59);
    // auto file_logger = std::make_shared<spdlog::logger>(FILE_SINK, file_sink);
    // spdlog::register_logger(file_logger);

    // std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    // auto combined_logger = 
    //         std::make_shared<spdlog::logger>(MULTI_SINKS, begin(sinks), end(sinks));
    // //register it if you need to access it globally
    // combined_logger->set_pattern(
    //         "[%Y-%m-%d %H:%M:%S.%e] [%-6t] [%^%-6l%$] [%-5n] [%!] [%#]  %v"); 
    // spdlog::register_logger(combined_logger);
    // spdlog::flush_every(std::chrono::seconds(5));

    int port = config::instance().get_int_default("port", 4567);
    int thread_num = config::instance().get_int_default("thread_num", 1);

    if (config::instance().has_key("-p")) {
        // //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "has key -p");
    }
    MyServer server;
    if (config::instance().has_key("ipv6")) {
        // //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "ipv6");
        server.init_socket(AF_INET6);
    } else {
        // //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "ipv4");
        server.init_socket(AF_INET);
    }

    const char *root = config::instance().get_string("wwwroot");
    const char *page = config::instance().get_string("page");
    server.www_root(root);
    server.index_page(page);

    server.bind(ip, port);
    server.listen(SOMAXCONN);
    server.start(thread_num);


    while(true) {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) {
            // //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "exit thread");
            server.close();
            break;
        } else {
            // //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "undefined commad");
        }
    }

    // //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), "exit");
    return 0;
}