/**
* @file server.hpp
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
#ifndef SERVER
#define SERVER

#include <map>
#include <vector>

#include "cell.hpp"
#include "timestamp.hpp"
#include "fdset.hpp"
#include "semaphore.hpp"
#include "cell_thread.hpp"
#include "client.hpp"
#include "task.hpp"
#include "buffer.hpp"
#include "net_event.hpp"


class server {
public:
    server();
    virtual ~server();
    void set_id(int id);
    void set_event(net_event *event);
    virtual void on_msg(client *client, data_header *header);
    void close();
    void on_run(cell_thread *pthread);
    virtual bool do_net_events() = 0;
    void check_time();
    virtual void on_leave(client *pclient);
    virtual void on_join(client *pclient);
    void on_recv(client *pclient);
    void do_msg();
    int recv_data(client *client);
    void add_client(client* client);
    void start();
    size_t client_count();
    void clear_client();
protected:
    std::map<SOCKET, client*> clients_;
    int id_ = -1;
    bool client_change_ = true;
private:
    std::vector<client*> clients_buff_;
    std::mutex mutex_;
    net_event *p_net_event_ = nullptr;
    task_server task_server_;
    time_t old_clock_ = timestamp::now_milliseconds();
    cell_thread thread_;
};

#endif // SERVER