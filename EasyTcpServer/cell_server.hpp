/**
* @file cell_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-20-17-54
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_SERVER
#define CELL_SERVER
#include "cell.hpp"

class sendmsg_to_client : public cell_task {
public:
    sendmsg_to_client(client_socket *client, data_header *data) {
        client_ = client;
        data_ = data;
    }

    void do_task() {
        client_->send_data(data_);
        delete data_;
    }
private:
    client_socket *client_;
    data_header *data_;
};


class cell_server : public subject {
public:
    cell_server(SOCKET sockfd = INVALID_SOCKET, observer *ob = nullptr);
    virtual ~cell_server();
    virtual void on_msg(SOCKET c_sock, data_header *header);
    void close();
    bool on_run();
    bool is_run();
    int recv_data(client_socket *client);
    void add_client(client_socket* client);
    void start();
    size_t count();
private:
    SOCKET sockfd_;
    SOCKET max_socket_;
    std::mutex mutex_;
    std::thread thread_;
    observer *observer_;
    std::map<SOCKET, client_socket*> clients_;
    std::vector<client_socket*> clients_buff_;
    fd_set fd_back_;
    bool client_change_;
};

#endif // CELL_SERVER