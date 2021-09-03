#include "utils/conf.hpp"
#include "depends/tcp_http_client.hpp"

using namespace nemausa::io;

class my_client : public tcp_http_client {
public:
    void test() {
        static int i =0;
        if (++i > 100)
            return;
        char req_buff[256] = {};
        if (i % 100 > 66) {
            sprintf(req_buff, "http://127.0.0.1:4567/add?a=%d&b=1", i);
            this->get(req_buff, [this](http_clientc *p_http_client) {
                if (p_http_client) {
                    SPDLOG_LOGGER_INFO(spdlog::get(CONSOLE_SINK), 
                        "recv msg len={} {} | {}",
                        p_http_client->header_str("Content-Length", "?"),
                        i, p_http_client->content());
                } else {
                    SPDLOG_LOGGER_INFO(spdlog::get(CONSOLE_SINK),
                        "server disconnect {}", i); 
                }
                test();
            });   
        } else if (i % 100 > 33) {
            sprintf(req_buff, "http://127.0.0.1:4567/sub?a=%d&b=1", i);
            this->get(req_buff, [this](http_clientc *p_http_client) {
                if (p_http_client) {
                    SPDLOG_LOGGER_INFO(spdlog::get(CONSOLE_SINK), 
                        "recv msg len={} {} | {}",
                        p_http_client->header_str("Content-Length", "?"),
                        i, p_http_client->content());
                } else {
                    SPDLOG_LOGGER_INFO(spdlog::get(CONSOLE_SINK),
                        "server disconnect {}", i); 
                }
                test();
            });   
        } else {
            sprintf(req_buff, "token=abc123&json={\"a\":%d, \"b\":32}", i);
            this->post("http://127.0.0.1:4567/jsonTest", req_buff, 
                [this](http_clientc *p_http_client) {
                if (p_http_client) {
                    SPDLOG_LOGGER_INFO(spdlog::get(CONSOLE_SINK), 
                        "recv msg len={} {} | {}",
                        p_http_client->header_str("Content-Length", "?"),
                        i, p_http_client->content());
                } else {
                    SPDLOG_LOGGER_INFO(spdlog::get(CONSOLE_SINK),
                        "server disconnect {}", i); 
                }
                test();
            });   
        }
    }
};

int main() {
#if _WIN32 && _CONSOLE
    system("chcp 65001");
#endif
    config::instance().load("client.conf");
    auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    auto console_logger = std::make_shared<spdlog::logger>(CONSOLE_SINK, console_sink); 
    spdlog::register_logger(console_logger);

    auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>
            ("logs/client.txt", 23, 59);
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
    my_client http_client;
    http_client.test();
    while (true) {
        http_client.on_run(1);
    }
    http_client.close();
    return 0;
}