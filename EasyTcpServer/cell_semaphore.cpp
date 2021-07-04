#include "cell_semaphore.hpp"

cell_semaphore::cell_semaphore() {
    wait_ = 0;
    wakeup_ = 0; 
}

void cell_semaphore::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (--wait_ < 0) {
        cv_.wait(lock, []()->bool {

        });
        --wakeup_;
    }
}

void cell_semaphore::wake_up() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (++wait_ <= 0) {
        ++wakeup_;
        cv_.notify_one();
    }
}