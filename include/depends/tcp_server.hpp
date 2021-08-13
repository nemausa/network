/**
* @file tcp_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-20-18-43
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_TCP_SERVER
#define EASY_TCP_SERVER

#include <vector>

#include "server.hpp"
#include "timestamp.hpp"
#include "cell_thread.hpp"
#include "net_event.hpp"

namespace nemausa {
namespace io {

class tcp_server : public net_event {
public:
    tcp_server();
    ~tcp_server();
    SOCKET init_socket();
    int bind(const char* ip, unsigned short port);
    int listen(int n);
    SOCKET accept();
    void add_client_to_server(client *client);
    void close();
    bool is_run();
    template<class T>
    void start(int server_count) {
        for (int n = 0; n < server_count; n++) {
            auto server = new T();
            server->set_id(n);
            servers_.push_back(server);
            server->set_event(this);
            server->start();
        }
        thread_.start(nullptr,
            [this](cell_thread* pthread) {
                on_run(pthread);
            });
    }
    virtual void on_join(client *pclient);
    virtual void on_leave(client *pclient);
    virtual void on_msg(server *pserver, client *pclient, data_header *header);
    virtual void on_recv(client *pclient);
protected:
    virtual void on_run(cell_thread *pthread) = 0;
    void time4msg();
    int sockfd();
private:
    cell_thread thread_;
    std::vector<server*> servers_;
    SOCKET sockfd_;
    timestamp time_;
protected:
    int send_buffer_size_;
    int recv_buffer_size_;
    int max_client_;
    std::atomic_int recv_count_;
    std::atomic_int message_count_;
    std::atomic_int client_count_;
};

} // namespace io 
} // namespace nemausa

#endif // EASY_TCP_SERVER
