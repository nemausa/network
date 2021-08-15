#include "depends/cell.hpp"
#include "depends/iocp.hpp"
#include "depends/network.hpp"

#ifdef _WIN32
#include<windows.h>

#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<mswsock.h>
#include<stdio.h>

namespace nemausa {
namespace io {

iocp::~iocp() {
    destory();
}

bool iocp::create() {
    completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == completion_port_) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "iocp create failed, CreateIoCompletionPort");
        return false;
    }
    return true;
}

void iocp::destory() {
    if (completion_port_) {
        CloseHandle(completion_port_);
        completion_port_ = NULL;
    }
}

bool iocp::reg(SOCKET sockfd) {
    auto ret = CreateIoCompletionPort((HANDLE)sockfd, 
            completion_port_, 
            (ULONG_PTR)sockfd, 
            0);
    if (!ret) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "iocp reg sockfd failed, CreateIoCompletionPort");
        return false;
    }
    return true;
} 

bool iocp::reg(SOCKET sockfd, void *ptr) {
    auto ret= CreateIoCompletionPort((HANDLE)sockfd, 
            completion_port_, 
            (ULONG_PTR)ptr, 
            0);
    if (!ret) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "iocp reg sockfd failed, CreateIoCompletionPort");
        return false;
    }
    return true;
}

bool iocp::post_accept(io_data_base *p_io_data, int af) {
    if (!acceptex_) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "error, post_accept acceptex_ is null");
        return false;
    } 
    p_io_data->io_type = io_type_e::ACCEPT;
    SOCKET csock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == csock) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), "iocp create sockef failed");
    } else {
        network::make_reuseaddr(csock);
    }
    p_io_data->sockfd = csock;
    int addr_len = (AF_INET6 == af ? 
            sizeof(sockaddr_in6) : sizeof(sockaddr_in));
    if (FALSE == acceptex_(sock_server_,
            p_io_data->sockfd,
            p_io_data->wsabuff.buf,
            0,
            addr_len + 16,
            addr_len + 16,
            NULL,
            &p_io_data->overlapped)) {
        int err = WSAGetLastError();
        if (ERROR_IO_PENDING != err) {
            SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                    "acceptex failed with error {}", err);
            return false;
        } 
    } 
    return true;
}

bool iocp::post_recv(io_data_base *p_io_data) {
    p_io_data->io_type = io_type_e::RECV;
    DWORD flags = 0;
    ZeroMemory(&p_io_data->overlapped, sizeof(OVERLAPPED));

    if (SOCKET_ERROR == WSARecv(p_io_data->sockfd, 
            &p_io_data->wsabuff, 
            1, 
            NULL, 
            &flags, 
            &p_io_data->overlapped, 
            NULL)) {
        int err = WSAGetLastError();
        if (ERROR_IO_PENDING != err) {
            if (WSAECONNRESET == err) {
                return false;
            }
            SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                    "WSARecv failed with error {}", err);
            return false;
        }
    }
    return true;
}

bool iocp::post_send(io_data_base *p_io_data) {
    p_io_data->io_type = io_type_e::SEND;
    DWORD flags = 0;
    ZeroMemory(&p_io_data->overlapped, sizeof(OVERLAPPED));

    if (SOCKET_ERROR == WSASend(p_io_data->sockfd, 
            &p_io_data->wsabuff, 
            1, 
            NULL, 
            flags, 
            &p_io_data->overlapped, 
            NULL)) {
        int err = WSAGetLastError();
        if (ERROR_IO_PENDING != err) {
            if (WSAECONNRESET == err) {
                return false;
            }
            SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                    "WSASend failed with error {}", err);
            return false;
        }
    }
    return true;
}

int iocp::wait(io_event &io_event, int timeout) {
    io_event.bytes_trans = 0;
    io_event.p_io_data = NULL;
    io_event.data.ptr = NULL;
    if (FALSE == GetQueuedCompletionStatus(completion_port_,
            &io_event.bytes_trans,
            (PULONG_PTR)&io_event.data,
            (LPOVERLAPPED*)&io_event.p_io_data,
            timeout)) {
        int err =GetLastError();
        if (WAIT_TIMEOUT == err) {
            return 0;
        } 
        if (ERROR_NETNAME_DELETED == err) {
            return 1;
        }
        if (ERROR_CONNECTION_ABORTED == err) {
            return 1;
        }
        if (ERROR_SEM_TIMEOUT == err) {
            return 1;
        }      
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "GetQueuedCompletionStatus error code {}", err);
        return -1;
    }
    return 1;
}

bool iocp::load_accept(SOCKET listen_socket) {
    if (INVALID_SOCKET != sock_server_) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "load_accept sock_server_ != INVALID_SOCKET");
        return false;
    }
    if (acceptex_) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "load_accept acceptex_ != NULL");
        return false;
    }
    sock_server_ = listen_socket;
    GUID guid_acceptex = WSAID_ACCEPTEX;
    DWORD dwbytes = 0;
    int result = WSAIoctl(listen_socket, 
            SIO_GET_EXTENSION_FUNCTION_POINTER,
            &guid_acceptex, sizeof(guid_acceptex),
            &acceptex_, sizeof(acceptex_),
            &dwbytes,
            NULL,
            NULL);
    if (result == SOCKET_ERROR) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "WSAIoctl failed with error: {}", WSAGetLastError());
        return false;
    }    

    GUID guid_addrs = WSAID_GETACCEPTEXSOCKADDRS;
    dwbytes = 0;
    result = WSAIoctl(listen_socket, 
            SIO_GET_EXTENSION_FUNCTION_POINTER,
            &guid_addrs, sizeof(guid_addrs),
            &accept_addrs_, sizeof(accept_addrs_),
            &dwbytes,
            NULL,
            NULL);
    if (result == SOCKET_ERROR) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "WSAIoctl failed with error: {}", WSAGetLastError());
        return false;
    }    
    return true;
}

char *iocp::get_accept_addrs(io_data_base *p_io_data, int af) {
    if (AF_INET6 == af) {
        return get_accept_ipv6(p_io_data);
    } else {
        return get_accept_ipv4(p_io_data);
    }
}

char *iocp::get_accept_ipv4(io_data_base *p_io_data) {
    int local_len = 0;
    int remote_len = 0;
    sockaddr_in *local_addr = NULL;
    sockaddr_in *remote_addr = NULL;
    accept_addrs_(
            p_io_data->wsabuff.buf,
            0,
            sizeof(sockaddr_in) + 16,
            sizeof(sockaddr_in) + 16,
            (sockaddr**)&local_addr,
            &local_len,
            (sockaddr**)&remote_addr,
            &remote_len
    );
    char *ip = inet_ntoa(remote_addr->sin_addr);
    return ip;
}

char *iocp::get_accept_ipv6(io_data_base *p_io_data) {
    int local_len = 0;
    int remote_len = 0;
    sockaddr_in6 *local_addr = NULL;
    sockaddr_in6 *remote_addr = NULL;
    accept_addrs_(
            p_io_data->wsabuff.buf,
            0,
            sizeof(sockaddr_in6) + 16,
            sizeof(sockaddr_in6) + 16,
            (sockaddr**)&local_addr,
            &local_len,
            (sockaddr**)&remote_addr,
            &remote_len
    );
   static char ip[INET6_ADDRSTRLEN] = {};
   inet_ntop(AF_INET6, &remote_addr->sin6_addr, ip, INET6_ADDRSTRLEN - 1);
   return ip; 
}

} // namespace io 
} // namespace nemausa

#endif 