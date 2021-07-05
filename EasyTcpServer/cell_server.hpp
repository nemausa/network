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
#include "cell_semaphore.hpp"
#include "cell_thread.hpp"
#include "cell_client.hpp"
#include "cell_task.hpp"
#include "cell_buffer.hpp"

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
    cell_server(int id, observer *ob = nullptr);
    virtual ~cell_server();
    virtual void on_msg(cell_client *client, data_header *header);
    void close();
    bool on_run(cell_thread *pthread);
    void read_data(fd_set &fd_read);
    void write_data(fd_set &fd_write);
    int recv_data(cell_client *client);
    void add_client(cell_client* client);
    void clear_client();
    void start();
    size_t count();
    void check_time();
    void on_leave(cell_client *pclient);
private:
    std::map<SOCKET, cell_client*> clients_;
    std::vector<cell_client*> clients_buff_;
    std::mutex mutex_;
    cell_task_server task_server_;
    fd_set fd_back_;
    SOCKET max_socket_;
    observer *observer_;
    time_t old_clock_ = timestamp::now_milliseconds();
    time_t now_clock_;
    cell_thread thread_;
    cell_semaphore sem_;
    int id_;
    bool client_change_;
};

#endif // CELL_SERVER