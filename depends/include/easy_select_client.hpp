/**
* @file easy_select_client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-01-19-53
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef EASY_SELECT_CLIENT
#define EASY_SELECT_CLIENT

#include "cell_fdset.hpp"
#include "easy_tcp_client.hpp"

class easy_select_client : public easy_tcp_client {
public:
    bool on_run(int microseconds = 1);
protected:
    cell_fdset fd_read_;
    cell_fdset fd_write_;
};

#endif // EASY_SELECT_CLIENT