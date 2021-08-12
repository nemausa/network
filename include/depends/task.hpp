/**
* @file task.hpp
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
#ifndef TASK
#define TASK

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "cell_thread.hpp"

class task_server {
typedef std::function<void()> task;
public:
    void add_task(task task);
    void start();
    void close();
protected:
    void on_run(cell_thread *pthread);
private:
    std::list<task> task_list_;
    std::list<task> task_buff_;
    std::mutex mutex_;
    cell_thread thread_;
public:
    int service_id_ = -1;
};

#endif // TASK