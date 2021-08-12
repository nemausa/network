#include "depends/task.hpp"

void task_server::add_task(task task) {
    std::lock_guard<std::mutex> lock(mutex_);
    task_buff_.push_back(task);
}

void task_server::start() {
    thread_.start(nullptr, [this](cell_thread *pthread) {
        on_run(pthread);
    });
}  

void task_server::close() {
    thread_.close();
}

void task_server::on_run(cell_thread *pthread) {
    while (pthread->is_run()) {
        if (!task_buff_.empty()) {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto ptask : task_buff_) {
                task_list_.push_back(ptask);
            }
            task_buff_.clear();
        }

        if (task_list_.empty()) {
            cell_thread::sleep(1);
            continue;
        }

        for (auto task : task_list_) {
            task(); 
        }
        task_list_.clear();
    }
    for (auto task : task_buff_) {
        task();
    }
}

