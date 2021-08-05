#include "depends/cell_task.hpp"

void cell_task_server::add_task(cell_task task) {
    std::lock_guard<std::mutex> lock(mutex_);
    task_buff_.push_back(task);
}

void cell_task_server::start() {
    thread_.start(nullptr, [this](cell_thread *pthread) {
        on_run(pthread);
    });
}  

void cell_task_server::close() {
    thread_.close();
}

void cell_task_server::on_run(cell_thread *pthread) {
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

