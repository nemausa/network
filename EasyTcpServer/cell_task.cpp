#include "cell_task.hpp"

cell_task::cell_task() {
}

cell_task::~cell_task() {
    
}

void cell_task::do_task() {
    
}


void cell_task_server::add_task(cell_task *task) {
    std::lock_guard<std::mutex> lock(mutex_);
    task_buff_.push_back(task);
    task_buff_.emplace_back(task);
}

void cell_task_server::start() {
    is_run_ = true;
    std::thread t(std::mem_fn(&cell_task_server::on_run), this);
    t.detach();
}  

void cell_task_server::close() {
    printf("cell_task_server%d close begin\n", service_id_);
    if (is_run_) {
        is_run_ = false;

    }
    printf("cell_task_server%d close end\n", service_id_);
}

void cell_task_server::on_run() {
    while (is_run_) {
        if (!task_buff_.empty()) {
            std::lock_guard<std::mutex> lock(mutex_);
            task_list_.insert(task_list_.end(), task_buff_.begin(),
                task_buff_.end());
            task_buff_.clear();
        }

        if (task_list_.empty()) {
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);
            continue;
        }

        for (auto task : task_list_) {
            task->do_task();
            delete task;
        }
        task_list_.clear();
    }

}

