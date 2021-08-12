/**
* @file network.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-09-15-16
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NETWORK
#define NETWORK

#include "cell.hpp"

class network {
private:
    network();
    ~network();
public:
    static void init();
    static int make_nonblock(SOCKET fd);
    static int make_reuseaddr(SOCKET fd);
    static int make_nodelay(SOCKET fd);
    static int destory_socket(SOCKET sockfd);
};

#endif // NETWORK