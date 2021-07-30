/**
* @file easy_iocp_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-30-13-32
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_IOCP_SERVER
#define EASY_IOCP_SERVER

#ifdef _WIN32

#include "easy_tcp_server.hpp"
#include "cell_iocp.hpp"
#include "cell_iocp_server.hpp"

class easy_iocp_server : public easy_tcp_server {
public:
    void start(int n);
protected:
    void on_run(cell_thread *pthread);
    SOCKET iocp_accept(SOCKET sock);
};
#endif

#endif // EASY_IOCP_SERVER