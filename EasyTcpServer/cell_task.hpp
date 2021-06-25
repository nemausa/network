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
private:
    std::list<cell_task*> task_list_;
    std::list<cell_task*> task_buff_;
    std::mutex mutex_;

public:
    void add_task(cell_task *task);
    void start();

protected:
    void on_run();

};

#endif // CELL_TASK