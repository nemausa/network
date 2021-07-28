#ifdef __WIN32
#include "cell_iocp.hpp"

cell_iocp::~cell_iocp() {
    destory();
}

bool cell_iocp::create() {
    completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == completion_port_) {
        cell_log::info("iocp create failed, CreateIoCompletionPort");
        return false;
    }
    return true;
}

void cell_iocp::destory() {
    if (completion_port_) {
        CloseHandle(completion_port_);
        completion_port_ = NULL;
    }
}

bool cell_iocp::reg(SOCKET sockfd) {
    auto ret = CreateIoCompletionPort((HANDLE)sockfd, completion_port_, (ULONG_PTR)sockfd, 0);
    if (!ret) {
        cell_log::info("iocp reg sockfd failed, CreateIoCompletionPort");
        return false;
    }
    return true;
} 

bool cell_iocp::reg(SOCKET sockfd, void *ptr) {
    auto ret= CreateIoCompletionPort((HANDLE)sockfd, completion_port_, (ULONG_PTR)ptr, 0);
    if (!ret) {
        cell_log::info("iocp reg sockfd failed, CreateIoCompletionPort");
        return false;
    }
    return true;
}

bool cell_iocp::post_accept(io_data_base *p_io_data) {
   if (!acceptex_) {
       cell_log::info("error, post_accept acceptex_ is null");
       return false;
   } 

   p_io_data->io_type = io_type_e::ACCEPT;
   p_io_data->sockfd  = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (FALSE == acceptex_(sock_server_,
            p_io_data->sockfd,
            p_io_data->wsabuff.buf,
            0,
            sizeof(sockaddr_in) + 16,
            sizeof(sockaddr_in) + 16,
            NULL,
            &p_io_data->overlapped)) {
                int err = WSAGetLastError();
                if (ERROR_IO_PENDING != err) {
                    cell_log::info("acceptex failed with error %d", err);
                    return false;
                } 
            } 
    return true;
}

bool cell_iocp::post_recv(io_data_base *p_io_data) {
    p_io_data->io_type = io_type_e::RECV;
    DWORD flags = 0;
    ZeroMemory(&p_io_data->overlapped, sizeof(OVERLAPPED));

    if (SOCKET_ERROR == WSARecv(p_io_data->sockfd, &p_io_data->wsabuff, 1, NULL, &flags, &p_io_data->overlapped, NULL)) {
        int err = WSAGetLastError();
        if (ERROR_IO_PENDING != err) {
            if (WSAECONNRESET == err) {
                return false;
            }
            cell_log::info("WSARecv failed with error %d", err);
            return false;
        }
    }
    return true;
}

bool cell_iocp::post_send(io_data_base *p_io_data) {
    p_io_data->io_type = io_type_e::SEND;
    DWORD flags = 0;
    ZeroMemory(&p_io_data->overlapped, sizeof(OVERLAPPED));

    if (SOCKET_ERROR == WSASend(p_io_data->sockfd, &p_io_data->wsabuff, 1, NULL, flags, &p_io_data->overlapped, NULL)) {
        int err = WSAGetLastError();
        if (ERROR_IO_PENDING != err) {
            if (WSAECONNRESET == err) {
                return false;
            }
            cell_log::info("WSASend failed with error %d", err);
            return false;
        }
    }
    return true;
}

int cell_iocp::wait(io_event &io_event, int timeout) {
    io_event.bytes_trans = 0;
    io_event.p_io_data = NULL;
    io_event.data.ptr = NULL;
    if (FALSE == GetQueuedCompletionStatus(completion_port_,
            &io_event.bytes_trans,
            (PULONG_PTR)&io_event.data,
            (LPOVERLAPPED*)&io_event.p_io_data,
            timeout)) {
                int err = WSAGetLastError();
                if (WAIT_TIMEOUT == err) {
                    return 0;
                } 
                if (ERROR_NETNAME_DELETED == err) {
                    return 1;
                }
                if (ERROR_CONNECTION_ABORTED == err) {
                    return 1;
                }
                cell_log::info(" GetQueuedCompletionStatus");
                return -1;
            }
    return 1;
}

bool cell_iocp::load_accept(SOCKET listen_socket) {
    if (INVALID_SOCKET != sock_server_) {
        cell_log::info("load_accept sock_server_ != INVALID_SOCKET");
        return false;
    }
    if (acceptex_) {
        cell_log::info("load_accept acceptex_ != NULL");
        return false;
    }
    sock_server_ = listen_socket;
    GUID guid_acceptex = WSAID_ACCEPTEX;
    DWORD dwbytes = 0;
    int result = WSAIoctl(listen_socket, 
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guid_acceptex, 
        sizeof(guid_acceptex),
        &acceptex_,
        sizeof(acceptex_),
        &dwbytes,
        NULL,
        NULL);
    if (result == SOCKET_ERROR) {
        cell_log::info("WSAIoctl failed with error: %u", WSAGetLastError());
        return false;
    }    
    return true;
}

#endif 