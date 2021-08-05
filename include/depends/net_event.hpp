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

class cell_client;
class cell_server;
class data_header;

class net_event {
public:
    virtual void on_join(cell_client *pclient) = 0;
    virtual void on_leave(cell_client *pclient) = 0;
    virtual void on_msg(cell_server *pserver, cell_client *pclient, data_header *header) = 0;
    virtual void on_recv(cell_client *pclient) = 0;
};

#endif // NET_EVENT