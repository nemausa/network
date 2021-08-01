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

#include <ctime>

#include "cell.hpp"
#include "cell_task.hpp"

using namespace std::chrono;

#define LOG_INFO(...)       cell_log::info(__VA_ARGS__)
#define LOG_WARN(...)       cell_log::warn(__VA_ARGS__)
#define LOG_ERROR(...)      cell_log::error(__VA_ARGS__)
#define LOG_DEBUG(...)      cell_log::debug(__VA_ARGS__)
#define LOG_PERROR(...)     cell_log::perror(__VA_ARGS__)
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

    static cell_log &instance() {
        static cell_log log;
        return log;
    }

    void set_path(const char *path, const char *mode, bool has_date) {
        if (logfile_) {
            info("cell::set_path log_file_ != nullptr\n");
            fclose(logfile_);
            logfile_ = nullptr;
        }
        static char log_path[256] = {};
        if (has_date) {
            auto t = system_clock::now();
            auto t_now = system_clock::to_time_t(t);
            std::tm *now = std::localtime(&t_now);
            sprintf(log_path, "%s[%4d-%02d-%02d-%02d-%02d-%02d].txt", 
                    path,
                    now->tm_year + 1990,
                    now->tm_mon + 1,
                    now->tm_mday,
                    now->tm_hour,
                    now->tm_min,
                    now->tm_sec);
        } else {
            sprintf(log_path, "%s.txt", path);
        }

        logfile_ = fopen(path, mode);
        if (logfile_) {
            info("cell_log::set_path success,<%s,%s>\n", path, mode);
        } else {
            info("cell_log::set_path failed,<%s,%s>\n", path, mode);
        }
    }

    static void perror(const char *pstr) {
        perror("%s", pstr);
    }

    template<typename ...Args>
    static void perror(const char *pformat, Args... args) {
#ifdef _WIN32
        auto err_code = GetLastError();
        instance().task_server_.add_task([=]() {
            char text[256] = {};
            FormatMessageA(
                    FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    err_code,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPSTR)&text,
                    256,
                    NULL
            );
            echo_real(true,  "perror ", pformat, args...);
            echo_real(false, "perror ", "errno=%d, errmsg=%s", err_code, text);
        });
#else
        auto err_code = errno;
        instance().task_server_.add_task([=]() {
            echo_real(true,  "perror ", pformat, args...);
            echo_real(false, "perror ", "errno=%d, errmsg=%s", err_code, strerror(err_code));
        });
        
#endif        
    }


    static void error(const char *pstr) {
        error("%s", pstr);
    }

    template<typename ...Args>
    static void error(const char *pformat, Args ... args) {
        echo("error ", pformat, args...);
    }

    static void warn(const char *pstr) {
        warn("%s", pstr);
    }

    template<typename ...Args>
    static void warn(const char *pformat, Args ... args) {
        echo("warn  ", pformat, args...);
    }

    static void debug(const char *pstr) {
        debug("%s", pstr);
    }

    template<typename ...Args>
    static void debug(const char *pformat, Args ... args) {
        echo("debug ", pformat, args...);
    } 

    static void info(const char *pstr) {
        info("%s", pstr);
    }

    template<typename ...Args>
    static void info(const char *pformat, Args ... args) {
        echo("info  ", pformat, args...);
    }

    template<typename ...Args>
    static void echo(const char *type, const char *pformat, Args ... args) {
        cell_log *plog = &instance();
        plog->task_server_.add_task([=]() {
            echo_real(true, type, pformat, args...);
        });
    }

    template<typename ...Args>
    static void echo_real(bool br, const char *type, const char *pformat, Args ... args) {
        cell_log *plog = &instance();
        if (plog->logfile_) {
                auto t = system_clock::now();
                auto tnow = system_clock::to_time_t(t);
                std::tm *now = std::gmtime(&tnow);
                if (type) {
                    fprintf(plog->logfile_, "%s", type);
                }
                fprintf(plog->logfile_, "[%4d-%02d-%02d %02d:%02d:%02d]", 
                    now->tm_year, 
                    now->tm_mon + 1,
                    now->tm_mday,
                    now->tm_hour,
                    now->tm_min,
                    now->tm_sec);
                fprintf(plog->logfile_, pformat, args...);
                if (br) {
                    fprintf(plog->logfile_, "%s", "\n");
                }
                fflush(plog->logfile_);
        }
        if (type) {
            printf("%s", type);
        }
        printf(pformat, args...);
        if (br) {
            printf("%s", "\n");
        }
    }
private:
    FILE *logfile_;
    cell_task_server task_server_;
};

#endif // CELL_LOG