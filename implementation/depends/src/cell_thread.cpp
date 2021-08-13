#include "depends/cell_thread.hpp"

namespace nemausa {
namespace io {

cell_thread::cell_thread() {

}

void cell_thread::start(event_call on_create,
        event_call on_run,
        event_call on_destory) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!is_run_) {
        is_run_ = true;
        if (on_create)
            on_create_ = on_create;
        if (on_run)
            on_run_ = on_run;
        if (on_destory)
            on_destory_ = on_destory;
        std::thread t(std::mem_fn(&cell_thread::on_work), this);
        t.detach();
    }
}

void cell_thread::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_run_) {
        is_run_ = false;
        sem_.wait();
    }
}

void cell_thread::exit() {
    if (is_run_) {
        std::lock_guard<std::mutex> lock(mutex_);
        is_run_ = false;
    }
}

bool cell_thread::is_run() {
    return is_run_;
}

void cell_thread::on_work() {
    if (on_create_)
        on_create_(this);
    if (on_run_)
        on_run_(this);
    if (on_destory_)
        on_destory_(this);

    sem_.wake_up();
    is_run_ = false;
}

void cell_thread::sleep(time_t _t) {
    std::chrono::milliseconds t(_t);
    std::this_thread::sleep_for(t);
}

} // namespace io 
} // namespace nemausa
