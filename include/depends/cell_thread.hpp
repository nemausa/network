/**
* @file cell_thread.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-04-17-37
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef THREAD
#define THREAD

#include <functional>
#include "semaphore.hpp"

class cell_thread {
private:
    typedef std::function<void(cell_thread*)> event_call;
public:
    cell_thread();
    void start(event_call on_create = nullptr,
               event_call on_run = nullptr,
               event_call on_destory = nullptr);
    void close();
    void exit();
    bool is_run();
    static void sleep(time_t _t);
protected:
    void on_work();
private:
    event_call on_create_;
    event_call on_run_;
    event_call on_destory_;
    std::mutex mutex_;
    semaphore sem_;
    bool is_run_ = false;
};

#endif // THREAD