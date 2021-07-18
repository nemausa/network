/**
* @file cell_buffer.hpp
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
#ifndef CELL_BUFFER
#define CELL_BUFFER

class cell_buffer {
public:
    cell_buffer(int size = 8192);
    ~cell_buffer();
    char *data();
    bool push(const char *data, int length);
    void pop(int length);
    int send_to_socket(SOCKET sockfd);
    int recv_from_socket(SOCKET sockfd);
    bool has_msg();
    bool need_write();
private:
    char *data_;
    int last_;
    int size_;
    int full_count_;
};

#endif // CELL_BUFFER