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

#include <map>
#include <vector>

#include "cell.hpp"
#include "timestamp.hpp"
#include "cell_fdset.hpp"
#include "cell_semaphore.hpp"
#include "cell_thread.hpp"
#include "cell_client.hpp"
#include "cell_task.hpp"
#include "cell_buffer.hpp"



class cell_server : public subject {
public:
    cell_server();
    virtual ~cell_server();
    virtual void on_msg(cell_client *client, data_header *header);
    void close();
    void on_run(cell_thread *pthread);
    bool do_select();
    void do_msg();
    void read_data();
    void write_data();
    int recv_data(cell_client *client);
    void add_client(cell_client* client);
    void clear_client();
    void start();
    size_t count();
    void check_time();
    void on_leave(cell_client *pclient);
    void set_id(int id);
protected:
    std::map<SOCKET, cell_client*> clients_;
    int id_;
    bool client_change_;
private:
    std::vector<cell_client*> clients_buff_;
    std::mutex mutex_;
    cell_task_server task_server_;
    observer *observer_;
    time_t old_clock_ = timestamp::now_milliseconds();
    time_t now_clock_;
    cell_thread thread_;
    cell_semaphore sem_;
};

#endif // CELL_SERVER