#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<iostream>
#include<Winsock2.h>
#include<stdio.h>
#include<algorithm>

#include <vector>
// #pragma comment(lib,"ws2_32.lib")

enum cmd_type_e{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_ERROR
};

struct data_header {
    short length;
    short cmd;
};

struct login : public data_header {
    login() {
        length = sizeof(login);
        cmd = CMD_LOGIN;
    }
    char user_name[32];
    char password[32];
};

struct login_result : public data_header {
    login_result() {
        length = sizeof(login_result);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
};

struct logout : public data_header {
    logout() {
        length = sizeof(logout);
        cmd = CMD_LOGOUT;
    }
    char user_name[32];
};

struct logout_result : public data_header {
    logout_result() {
        length = sizeof(logout_result);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
};

std::vector<SOCKET> clients;

int processor(SOCKET sock) {
    char sz_recv[4096] = {};
    int len = recv(sock, sz_recv, sizeof(data_header), 0);
    data_header *header = (data_header*)sz_recv;
    if (len < 0) {
        printf("client close\n");
        return -1;
    }

    int header_len = sizeof(data_header);
    switch (header->cmd) {
    case CMD_LOGIN: {
        recv(sock, sz_recv + header_len, header->length - header_len, 0);
        login *l = (login*)sz_recv;
        printf("receive cmd: CMD_LOGIN,data length=%d, username=%s, password=%s\n", l->length, l->user_name, l->password);
        login_result ret;
        send(sock, (char*)&ret, sizeof(login_result), 0);
    }
    break;
    case CMD_LOGOUT: {
        recv(sock, sz_recv + header_len, header->length - header_len, 0);
        logout *l = (logout*)sz_recv;
        printf("receive cmd: CMD_LOGOUT,data length=%d, username=%s \n", l->length, l->user_name);
        logout_result ret;
        send(sock, (char*)&ret, sizeof(ret), 0);
    }
    break;
    default: {
        data_header header = {0, CMD_ERROR};
        send(sock, (char*)&header, sizeof(header), 0);
    }
    break;
    }


}

int main() {
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);

    // 1 create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 2 bind port
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(4567);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (SOCKET_ERROR == bind(sock, (sockaddr*)&sin, sizeof(sin))){
        printf("error bind\n");
    } else {
        printf("success bind\n");
    }

    // 3 listen port
    if (SOCKET_ERROR == listen(sock, 5)) {
        printf("error listen\n");
    } else {
        printf("success listen\n");
    }

    while (true) {
        fd_set fd_read;
        fd_set fd_write;
        fd_set fd_exp;

        FD_ZERO(&fd_read);
        FD_ZERO(&fd_write);
        FD_ZERO(&fd_exp);

        FD_SET(sock, &fd_read);
        FD_SET(sock, &fd_write);
        FD_SET(sock, &fd_exp);

        for (int n= (int)clients.size()-1; n >= 0; n--) {
            FD_SET(clients[n], &fd_read);
        }

        timeval t = {0, 0};
        int ret  = select(sock + 1, &fd_read, &fd_write, &fd_exp, &t);
        if (ret < 0) {
            printf("select close\n");
            break;
        }
        if (FD_ISSET(sock, &fd_read)) {
            FD_CLR(sock, &fd_read);
            
            // accept client
            sockaddr_in client_add = {};
            int len = sizeof(sockaddr_in);
            SOCKET c_sock = INVALID_SOCKET;
            c_sock = accept(sock, (sockaddr*)&client_add, &len);
            if (INVALID_SOCKET == c_sock) {
                printf("error, invalid client\n");
            } 
            clients.emplace_back(c_sock);
            printf("new client socket=%d, ip = %s \n",(int)c_sock, inet_ntoa(client_add.sin_addr));
        }

        for (size_t n = 0; n < fd_read.fd_count; n++) {
            if (-1 == processor(fd_read.fd_array[n])) {
                auto iter = std::find(clients.begin(), clients.end(), (SOCKET)(fd_read.fd_array[n]));
                if (iter != clients.end()) {
                    clients.erase(iter);
                }
            }
        }
    }

    for (size_t n = clients.size() -1; n >= 0; n--) {
        closesocket(sock);
    }

    closesocket(sock);    
    WSACleanup();
    getchar();
    return 0;
}