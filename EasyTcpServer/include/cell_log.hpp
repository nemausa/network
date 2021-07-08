/**
* @file cell_log.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-08-21-35
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_LOG
#define CELL_LOG

#include "cell.hpp"
#include "cell_task.hpp"
#include <ctime>
using namespace std::chrono;

class cell_log {
public:
    cell_log() {
        task_server_.start();
    }

    ~cell_log() {
        task_server_.close();
        if (logfile_) {
            info("cell_log fclose(log_file_)\n");
            fclose(logfile_);
            logfile_ = nullptr;
        }
    }

    static cell_log &initstance() {
        static cell_log log;
        return log;
    }

    void set_path(const char *path, const char *mode) {
        if (logfile_) {
            info("cell::set_path log_file_ != nullptr\n");
            fclose(logfile_);
            logfile_ = nullptr;
        }

        logfile_ = fopen(path, mode);
        if (logfile_) {
            info("cell_log::set_path success,<%s,%s>\n", path, mode);
        } else {
            info("cell_log::set_path failed,<%s,%s>\n", path, mode);
        }
    }

    static void info(const char *pstr) {
        cell_log *plog = &initstance();
        plog->task_server_.add_task([=]() {
            if (plog->logfile_) {
                auto t = system_clock::now();
                auto tnow = system_clock::to_time_t(t);
                std::tm *now = std::gmtime(&tnow);
                fprintf(plog->logfile_, "%s", "info ");
                fprintf(plog->logfile_, "[%4d-%02d-%02d %02d:%02d:%02d]", 
                    now->tm_year, 
                    now->tm_mon + 1,
                    now->tm_mday,
                    now->tm_hour,
                    now->tm_min,
                    now->tm_sec);
                fprintf(plog->logfile_, "%s", pstr);
                fflush(plog->logfile_);
            }
            printf("%s", pstr);
        });
    }

    template<typename ...Args>
    static void info(const char *pformat, Args ... args) {
        cell_log *plog = &initstance();
        plog->task_server_.add_task([=]() {
            if (plog->logfile_) {
               auto t = system_clock::now();
                auto tnow = system_clock::to_time_t(t);
                std::tm *now = std::gmtime(&tnow);
                fprintf(plog->logfile_, "%s", "info ");
                fprintf(plog->logfile_, "[%4d-%02d-%02d %02d:%02d:%02d]", 
                    now->tm_year +1900, 
                    now->tm_mon + 1,
                    now->tm_mday,
                    now->tm_hour,
                    now->tm_min,
                    now->tm_sec);
                fprintf(plog->logfile_, pformat, args...);
                fflush(plog->logfile_);
            }
            printf(pformat, args...);
        });
    }
private:
    FILE *logfile_;
    cell_task_server task_server_;
};

#endif // CELL_LOG