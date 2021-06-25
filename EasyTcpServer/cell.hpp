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
    #define FD_SETSIZE      2506
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <winsock2.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <string.h>
    #define SOCKET int
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR    (SOCKET)(-1)
#endif

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include "message_header.hpp"
#include "obersver.hpp"
#include "client_socket.hpp"
#include "cell_task.hpp"


#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240*5
#define SEND_BUFF_SIZE RECV_BUFF_SIZE
#endif


#endif // CELL