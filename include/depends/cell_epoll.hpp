/**
* @file cell_epoll.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-23-19-53
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_EPOLL
#define CELL_EPOLL

#if __linux__

#include "cell.hpp"
#include "cell_client.hpp"
#include <sys/epoll.h>
#include "cell_network.hpp"
#define EPOLL_ERROR             (-1)

class cell_epoll {
public:
    ~cell_epoll();
    int create(int max_events);
    void destory();
    // 向epoll对象注册需要管理、监听的socket文件描述符
    int ctl(int op, SOCKET sockfd, uint32_t events); 
    int ctl(int op, cell_client *pclient, uint32_t events); 
    int wait(int timeout); 
    epoll_event *events(); 
private:
    epoll_event *pevents_ = nullptr;
    int max_events_ = 1;
    int epfd_ = -1;
};


#endif

#endif // CELL_EPOLL