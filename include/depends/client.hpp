/**
* @file client.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-30-00-21
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CLIENT
#define CLIENT

#include "cell.hpp"
#include "buffer.hpp"
#include "network.hpp"

#define CLIENT_HEART_DEAD_TIME  120000
#define CLIENT_SEND_BUFF_TIME   200

namespace nemausa {
namespace io {

    enum client_state_e {
        create = 10,
        join,
        run,
        close
    };

class client {
public:
    client(SOCKET sockfd = INVALID_SOCKET, 
           int send_size = SEND_BUFF_SIZE, 
           int recv_size = RECV_BUFF_SIZE);
    virtual ~client();
    void destory();
    SOCKET sockfd();
    int recv_data();
    virtual bool has_msg();
    data_header *front_msg();
    virtual void pop_msg();
    bool need_write();
    int send_data_real();
    virtual void on_send_complete();
    int send_data(data_header *header);
    int send_data(const char *data, int length);
    void reset_heart_time();
    void reset_send_time();
    bool check_heart_time(time_t dt);
    bool check_send_time(time_t dt);
    void setip(char *ip);
    char* getip();
    client_state_e state();
    void state(client_state_e _state);
    void on_close();
    bool is_close();
#ifdef _WIN32
    io_data_base *make_recv_iodata();
    void recv_for_iocp(int nrecv);
    io_data_base *make_send_iodata();
    void send_to_iocp(int nsend);
    void post_recv_complete();
    void post_send_complete();
    bool is_post_action();
#endif
public:
    int id_ = -1;
    int service_id_ = -1;
    int recv_id = 1;
    int send_id = 1;
protected:
    buffer recv_buffer_;
    buffer send_buffer_;
    time_t heart_time_ = 0;
    time_t send_time_ = 0;
    SOCKET sockfd_ = INVALID_SOCKET;
    char ip_[INET6_ADDRSTRLEN] = {};
    bool is_post_recv_ = false;
    bool is_post_send_ = false;
    client_state_e state_ = client_state_e::create;
};

} // namespace io
} // namesapce nemausa

#endif // CLIENT
