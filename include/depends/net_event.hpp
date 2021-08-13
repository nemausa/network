/**
* @file net_event.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-01-20-54
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef NET_EVENT
#define NET_EVENT

namespace nemausa {
namespace io {

class client;
class server;
class data_header;

class net_event {
public:
    virtual void on_join(client *pclient) = 0;
    virtual void on_leave(client *pclient) = 0;
    virtual void on_msg(server *pserver, client *pclient, data_header *header) = 0;
    virtual void on_recv(client *pclient) = 0;
};

} // namespace io 
} // namespace nemausa

#endif // NET_EVENT