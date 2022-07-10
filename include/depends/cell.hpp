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
    #include <ws2ipdef.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
#else
#ifdef __APPLE__
    #define _DARWIN_UNLIMITED_SELECT
#endif
	#include <unistd.h> //uni std
	#include <arpa/inet.h>
	#include <string.h>
	#include <signal.h>
	#include <sys/socket.h>
    #include <netinet/tcp.h>
    #include <net/if.h>
    #include <netdb.h>
    #include <atomic>

    #include "glog/logging.h"
    #include <iostream>
    #include <iomanip>
    #define SOCKET int
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR            (-1)
#endif

#ifndef MULTI_SINKS
#define MULTI_SINKS     "multi_sinks"
#define CONSOLE_SINK    "console_sink"
#define FILE_SINK       "file_sink"
#endif

#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#define SEND_BUFF_SIZE 10240
#endif

#include "message_header.hpp"
#include "timestamp.hpp"
#include "task.hpp"


#endif // CELL