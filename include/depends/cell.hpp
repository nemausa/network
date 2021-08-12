/**
* @file cell.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-25-20-49
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL
#define CELL

#ifdef _WIN32
    #define FD_SETSIZE      65535
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <windows.h>
    #include <winsock2.h>
    // #pragma comment(lib, "ws2_32.lib")
#else
#ifdef __APPLE__
    #define _DARWIN_UNLIMITED_SELECT
#endif
	#include <unistd.h> //uni std
	#include <arpa/inet.h>
	#include <string.h>
	#include <signal.h>
	#include <sys/socket.h>

    #define SOCKET int
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR            (-1)
    
#endif

#include "message_header.hpp"
#include "timestamp.hpp"
#include "task.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h" // for loading levels from the environment variable
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#ifndef LOG_NAME
#define LOG_NAME "name"
#endif
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#define SEND_BUFF_SIZE 10240
#endif


#endif // CELL