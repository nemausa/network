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
#include "cell_timestamp.hpp"
#include "cell_fdset.hpp"
#include "cell_semaphore.hpp"
#include "cell_thread.hpp"
#include "cell_client.hpp"
#include "cell_task.hpp"
#include "cell_buffer.hpp"
#include "net_event.hpp"


class cell_server : public subject {
public:
    cell_server();
    virtual ~cell_server();
    void set_id(int id);
    void set_event(net_event *event);
    virtual void on_msg(cell_client *client, data_header *header);
    void close();
    void on_run(cell_thread *pthread);
    virtual bool do_net_events() = 0;
    void check_time();
    virtual void on_leave(cell_client *pclient);
    virtual void on_join(cell_client *pclient);
    void on_recv(cell_client *pclient);
    void do_msg();
    int recv_data(cell_client *client);
    void add_client(cell_client* client);
    void start();
    size_t client_count();
    void clear_client();
protected:
    std::map<SOCKET, cell_client*> clients_;
    int id_;
    bool client_change_;
private:
    std::vector<cell_client*> clients_buff_;
    std::mutex mutex_;
    net_event *p_net_event_ = nullptr;
    cell_task_server task_server_;
    time_t old_clock_ = cell_timestamp::now_milliseconds();
    cell_thread thread_;
};

#endif // CELL_SERVER