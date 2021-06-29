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
#include <signal.h>
#include "cell_server.hpp"
#include "timestamp.hpp"


class easy_tcp_server : public subject {
private:
    SOCKET sockfd_;
    std::vector<cell_server*> cell_servers_;
    timestamp time_;
    observer* observer_;
public:
    easy_tcp_server();
    ~easy_tcp_server();
    SOCKET init_socket();
    int bind(const char* ip, unsigned short port);
    int listen(int n);
    SOCKET accept();
    void add_client_to_server(client_socket *client);
    void start(int cellserver_count);
    void close();
    bool is_run();
    bool on_run();
    void time4msg();

};

#endif // EASY_TCP_SERVER
