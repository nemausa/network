/**
* @file cell_semaphore.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-04-16-53
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_SEMAPHORE
#define CELL_SEMAPHORE

#include <condition_variable>
#include <thread>
#include <chrono>

class cell_semaphore {
public:
    cell_semaphore();
    void wait();
    void wake_up();
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int wait_;
    int wakeup_;
};

#endif // CELL_SEMAPHORE