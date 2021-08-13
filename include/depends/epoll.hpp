/**
* @file epoll.hpp
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
#ifndef EPOLL
#define EPOLL

#if __linux__

#include "cell.hpp"
#include "client.hpp"
#include <sys/epoll.h>
#include "network.hpp"
#define EPOLL_ERROR             (-1)

namespace nemausa {
namespace io {

class epoll {
public:
    ~epoll();
    int create(int max_events);
    void destory();
    // 向epoll对象注册需要管理、监听的socket文件描述符
    int ctl(int op, SOCKET sockfd, uint32_t events); 
    int ctl(int op, client *pclient, uint32_t events); 
    int wait(int timeout); 
    epoll_event *events(); 
private:
    epoll_event *pevents_ = nullptr;
    int max_events_ = 1;
    int epfd_ = -1;
};


} // namespace io
} // namesapce nemausa

#endif // __linux__
#endif // EPOLL
