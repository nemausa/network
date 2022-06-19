#include "depends/fdset.hpp"

namespace nemausa {
namespace io {

cell_fdset::cell_fdset() {
}

cell_fdset::~cell_fdset() {
    if (pfdset_) {
        delete [] pfdset_;
        pfdset_ = nullptr;
    }
}

void cell_fdset::create(int max_fd) {
    int socket_num = AMX_FD;
#ifdef _WIN32   
    if (socket_num < 64)
        socket_num = 64;
    fdsize_ = sizeof(u_int) + (sizeof(SOCKET)*socket_num);
#else
    if (socket_num < 65535)
        socket_num = 65535;
    fdsize_ = socket_num / (8 * sizeof(char));
#endif
    pfdset_ = (fd_set*)new char[fdsize_];
    memset(pfdset_, 0, fdsize_);
    max_sockfd_ = socket_num;
}

void cell_fdset::add(SOCKET s) {
#ifdef _WIN32
    FD_SET(s, pfdset_);
#else
    if (s < max_sockfd_) {
        FD_SET(s, pfdset_);
    } else {
        //SPDLOG_LOGGER_INFO(spdlog::get(MULTI_SINKS), 
                // "add sock<%d> max_fd<%d>", (int)s, AMX_FD);
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

} // namespace io 
} // namespace nemausa
