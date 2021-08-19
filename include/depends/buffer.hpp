/**
* @file buffer.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-05-21-05
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef BUFFER
#define BUFFER

#include "iocp.hpp"
#include "task.hpp"

namespace nemausa {
namespace io {

class buffer {
public:
    buffer(int size = 8192);
    ~buffer();
    char *data();
    int length();
    int size();
    bool can_write(int size);
    bool push(const char *data, int length);
    void pop(int length);
    int send_to_socket(SOCKET sockfd);
    int recv_from_socket(SOCKET sockfd);
    bool has_msg();
    bool need_write();
#ifdef _WIN32
    io_data_base *make_recv_iodata(SOCKET sockfd);
    io_data_base *make_send_iodata(SOCKET sockfd);
    bool read_for_iocp(int nrecv);
    bool write_to_iocp(int nsend);
#endif
private:
    char *data_ = nullptr;
    int last_ = 0;
    int size_ = 0;
    int full_count_ = 0;
#ifdef _WIN32
    io_data_base iodata_ = {};
#endif
};

} // namespace io 
} // namespace nemausa

#endif // BUFFER