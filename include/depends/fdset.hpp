/**
* @file fdset.hpp
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
#ifndef FDSET
#define FDSET

#include "cell.hpp"

#define AMX_FD 102400 

namespace nemausa {
namespace io {

class cell_fdset {
public:
    cell_fdset();
    ~cell_fdset();
    void create(int max_fd);
    void add(SOCKET s);
    void del(SOCKET s);
    void zero();
    bool has(SOCKET s);
    fd_set *fdset();
    void copy(cell_fdset &set);
private:
    fd_set *pfdset_;
    size_t fdsize_;
    int max_sockfd_ = 0;
};

} // namespace io 
} // namespace nemausa

#endif // FDSET