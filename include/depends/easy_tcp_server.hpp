/**
* @file easy_tcp_server.hpp
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

#include "cell_server.hpp"
#include "cell_timestamp.hpp"
#include "cell_thread.hpp"
#include "net_event.hpp"

class easy_tcp_server : public net_event {
public:
    easy_tcp_server();
    ~easy_tcp_server();
    SOCKET init_socket();
    int bind(const char* ip, unsigned short port);
    int listen(int n);
    SOCKET accept();
    void add_client_to_server(cell_client *client);
    void close();
    bool is_run();
    template<class T>
    void start(int cell_server_count) {
        for (int n = 0; n < cell_server_count; n++) {
            auto server = new T();
            server->set_id(n);
            cell_servers_.push_back(server);
            server->set_event(this);
            server->start();
        }
        thread_.start(nullptr,
            [this](cell_thread* pthread) {
                on_run(pthread);
            });
    }
    virtual void on_join(cell_client *pclient);
    virtual void on_leave(cell_client *pclient);
    virtual void on_msg(cell_server *pserver, cell_client *pclient, data_header *header);
    virtual void on_recv(cell_client *pclient);
protected:
    virtual void on_run(cell_thread *pthread) = 0;
    void time4msg();
    int sockfd();
private:
    cell_thread thread_;
    std::vector<cell_server*> cell_servers_;
    SOCKET sockfd_;
    cell_timestamp time_;
protected:
    int send_buffer_size_;
    int recv_buffer_size_;
    int max_client_;
    std::atomic_int recv_count_;
    std::atomic_int message_count_;
    std::atomic_int client_count_;
};

#endif // EASY_TCP_SERVER
