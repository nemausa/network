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
#include "timestamp.hpp"

class sendmsg_to_client : public cell_task {
public:
    sendmsg_to_client(cell_client *client, data_header *data) {
        client_ = client;
        data_ = data;
    }

    void do_task() {
        client_->send_data(data_);
        delete data_;
    }
private:
    cell_client *client_;
    data_header *data_;
};


class cell_server : public subject {
public:
    cell_server(SOCKET sockfd = INVALID_SOCKET, observer *ob = nullptr);
    virtual ~cell_server();
    virtual void on_msg(cell_client *client, data_header *header);
    void close();
    bool on_run();
    bool is_run();
    int recv_data(cell_client *client);
    void add_client(cell_client* client);
    void start();
    size_t count();
    void check_time();
private:
    SOCKET sockfd_;
    SOCKET max_socket_;
    std::mutex mutex_;
    std::thread thread_;
    observer *observer_;
    std::map<SOCKET, cell_client*> clients_;
    std::vector<cell_client*> clients_buff_;
    fd_set fd_back_;
    bool client_change_;
    time_t old_clock_ = timestamp::now_milliseconds();
    time_t now_clock_;
};

#endif // CELL_SERVER