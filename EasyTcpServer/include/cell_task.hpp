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
#include "cell_thread.hpp"

class cell_task_server {
typedef std::function<void()> cell_task;
public:
    void add_task(cell_task task);
    void start();
    void close();
protected:
    void on_run(cell_thread *pthread);

private:
    std::list<cell_task> task_list_;
    std::list<cell_task> task_buff_;
    std::mutex mutex_;
    cell_thread thread_;
public:
    int service_id_;
};

#endif // CELL_TASK