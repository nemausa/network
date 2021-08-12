#include "depends/semaphore.hpp"

semaphore::semaphore() {
    wait_ = 0;
    wakeup_ = 0; 
}

void semaphore::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (--wait_ < 0) {
        cv_.wait(lock, [this]()->bool {
            return wakeup_ > 0;
        });
        --wakeup_;
    }
}

void semaphore::wake_up() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (++wait_ <= 0) {
        ++wakeup_;
        cv_.notify_one();
    }
}