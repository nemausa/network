/**
* @file tcp_select_server.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-01-20-02
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_SELECT_SERVER
#define EASY_SELECT_SERVER

#include "tcp_server.hpp"
#include "fdset.hpp"

class tcp_select_server : public tcp_server {
public:
    void start(int n);
protected:
    void on_run(cell_thread *pthread);
};

#endif // EASY_SELECT_SERVER