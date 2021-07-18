/**
* @file cell_network.hpp
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
#ifndef CELL_NETWORK
#define CELL_NETWORK

#include "cell.hpp"

class cell_network {
private:
    cell_network();
    ~cell_network();
public:
    static void init();
    static int make_reuseadd(SOCKET fd);
};

#endif // CELL_NETWORK