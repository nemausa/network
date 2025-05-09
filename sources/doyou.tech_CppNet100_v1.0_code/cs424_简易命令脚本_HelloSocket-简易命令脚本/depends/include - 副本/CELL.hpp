#ifndef _CELL_HPP_
#define _CELL_HPP_

//SOCKET
#ifdef _WIN32
	#define FD_SETSIZE      10000
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include<windows.h>
	#include<WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")
	
	typedef fd_set cell_fd_set;
	#define CELL_FD_ZERO(set) FD_ZERO(&set)
	#define CELL_FD_SET(fd,set) FD_SET(fd,set)
#else
	#include<unistd.h> //uni std
	#include<arpa/inet.h>
	#include<string.h>
	#include<signal.h>
	#include<sys/socket.h>
    //#include<fcntl.h>
    //#include<stdlib.h>

    #define CELL_MAX_FD 10240//FD_SETSIZE

	#define CELL_FD_SET(fd,set) \
	if(fd < CELL_MAX_FD)\
	{\
		FD_SET(fd, set);\
	}\
	else\
	{\
		CELLLog::Error("CELL_FD_SET fd<%d> CELL_MAX_FD<%d>  \n",fd,CELL_MAX_FD);\
	}

#if __linux__
    struct cell_fd_set {
        unsigned long fds_bits[CELL_MAX_FD / (8 * sizeof(long))];
    };
	#define CELL_FD_ZERO(set) memset(&set,0,sizeof(set))
	//typedef fd_set cell_fd_set;
	//#define CELL_FD_ZERO(set) FD_ZERO(&set)
#else
    typedef fd_set cell_fd_set;
    #define CELL_FD_ZERO(set) FD_ZERO(&set)
#endif

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif
//
#include"MessageHeader.hpp"
#include"CELLTimestamp.hpp"
#include"CELLTask.hpp"
#include"CELLLog.hpp"

//
#include<stdio.h>

//缓冲区最小单元大小
#ifndef RECV_BUFF_SZIE
#define RECV_BUFF_SZIE 8192
#define SEND_BUFF_SZIE 16384
#endif // !RECV_BUFF_SZIE

#endif // !_CELL_HPP_
