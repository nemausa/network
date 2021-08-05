/**
* @file cell_fdset.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-09-21-49
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_FDSET
#define CELL_FDSET

#include "cell.hpp"

#define CELL_AMX_FD 102400 

class cell_fdset {
public:
    cell_fdset();
    ~cell_fdset();
    void add(SOCKET s);
    void del(SOCKET s);
    void zero();
    bool has(SOCKET s);
    fd_set *fdset();
    void copy(cell_fdset &set);
private:
    fd_set *pfdset_;
    size_t fdsize_;
};

#endif // CELL_FDSET