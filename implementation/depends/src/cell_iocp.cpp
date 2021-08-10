#ifdef _WIN32

#include "depends/cell.hpp"
#include "depends/cell_iocp.hpp"

#include<windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<mswsock.h>
#include<stdio.h>

cell_iocp::~cell_iocp() {
    destory();
}

bool cell_iocp::create() {
    completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == completion_port_) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "iocp create failed, CreateIoCompletionPort");
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

bool cell_iocp::reg(SOCKET sockfd, void *ptr) {
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

bool cell_iocp::post_accept(io_data_base *p_io_data) {
    if (!acceptex_) {
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "error, post_accept acceptex_ is null");
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
            SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                    "acceptex failed with error {}", err);
            return false;
        } 
    } 
    return true;
}

bool cell_iocp::post_recv(io_data_base *p_io_data) {
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

bool cell_iocp::post_send(io_data_base *p_io_data) {
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

int cell_iocp::wait(io_event &io_event, int timeout) {
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
        SPDLOG_LOGGER_ERROR(spdlog::get(LOG_NAME), 
                "GetQueuedCompletionStatus");
        return -1;
    }
    return 1;
}

bool cell_iocp::load_accept(SOCKET listen_socket) {
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
    return true;
}

#endif 