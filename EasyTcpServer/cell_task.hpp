/**
* @file cell_task.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-25-20-22
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_TASK
#define CELL_TASK

#include <thread>
#include <mutex>
#include <list>
#include <functional>

class cell_task {
public:
    cell_task();

    virtual ~cell_task();
    virtual void do_task();
};

class cell_task_server {

public:
    void add_task(cell_task *task);
    void start();
    void close();
protected:
    void on_run();

private:
    std::list<cell_task*> task_list_;
    std::list<cell_task*> task_buff_;
    std::mutex mutex_;
    bool is_run_;
public:
    int service_id_;
};

#endif // CELL_TASK