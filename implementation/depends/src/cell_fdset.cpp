
#include "depends/cell_fdset.hpp"


cell_fdset::cell_fdset() {
    int socket_num = CELL_AMX_FD;
#ifdef _WIN32   
    fdsize_ = sizeof(u_int) + (sizeof(SOCKET)*socket_num);
#else
    fdsize_ = socket_num / (8 * sizeof(char));
#endif
    pfdset_ = (fd_set*)new char[fdsize_];
    memset(pfdset_, 0, fdsize_);
}

cell_fdset::~cell_fdset() {
    if (pfdset_) {
        delete [] pfdset_;
        pfdset_ = nullptr;
    }
}

void cell_fdset::add(SOCKET s) {
#ifdef _WIN32
    FD_SET(s, pfdset_);
#else
    if (s < CELL_AMX_FD) {
        FD_SET(s, pfdset_);
    } else {
        LOG_INFO("add sock<%d> cell_max_fd<%d>", (int)s, CELL_AMX_FD);
    }
#endif
}

void cell_fdset::del(SOCKET s) {
    FD_CLR(s, pfdset_);
}

void cell_fdset::zero() {
#ifdef _WIN32
    FD_ZERO(pfdset_);
#else
    memset(pfdset_, 0, fdsize_);
#endif
}

bool cell_fdset::has(SOCKET s) {
    return FD_ISSET(s, pfdset_);
}

fd_set *cell_fdset::fdset() {
    return pfdset_;
}

void cell_fdset::copy(cell_fdset &set) {
    memcpy(pfdset_, set.fdset(), set.fdsize_);
}