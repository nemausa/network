/**
* @file cell_select_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-01-14-41
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_SELECT_SERVER
#define CELL_SELECT_SERVER

#include "cell_server.hpp"
#include "cell_fdset.hpp"

class cell_select_server : public cell_server {
public:
    ~cell_select_server();
    bool do_net_events();
    void write_data();
    void read_data();
private:
    cell_fdset fd_read_;
    cell_fdset fd_write_;
    cell_fdset fd_read_bak_;
    SOCKET max_socket_;
};

#endif // CELL_SELECT_SERVER