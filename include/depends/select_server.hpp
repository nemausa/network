/**
* @file select_server.hpp
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
#ifndef SELECT_SERVER
#define SELECT_SERVER

#include "server.hpp"
#include "fdset.hpp"

namespace nemausa {
namespace io {

class select_server : public server {
public:
    ~select_server();
    virtual bool do_net_events();
    void write_data();
    void read_data();
private:
    cell_fdset fd_read_;
    cell_fdset fd_write_;
    cell_fdset fd_read_bak_;
    SOCKET max_socket_;
};

} // namespace io 
} // namespace nemausa

#endif // SELECT_SERVER