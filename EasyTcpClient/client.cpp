#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <thread>

// #pragma comment(lib,"ws2_32.lib")

enum cmd_type_e{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_JOIN,
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

struct new_join : public data_header {
    new_join() {
        length = sizeof(new_join);
        cmd = CMD_NEW_JOIN;
        sock = 0;
    }
    int sock;
};

int processor(SOCKET sock) {
    char sz_recv[4096] = {};
    int header_len  = sizeof(data_header);
    int len = recv(sock, sz_recv, header_len, 0);
    data_header *header = (data_header*)sz_recv;
    if (len < 0) {
        printf("task end\n");
        return -1;
    }
    switch (header->cmd) {
    case CMD_LOGIN_RESULT: {
        recv(sock, sz_recv + header_len, header->length - header_len, 0);
        login_result *result = (login_result*)sz_recv;
        printf("receiver cmd: CMD_LOGIN_RESULT, data length=%d\n", sock, result->length);
    }
    break;
    case CMD_LOGOUT_RESULT: {
        recv(sock, sz_recv + header_len, header->length - header_len, 0);
        logout_result *result = (logout_result*)sz_recv;
        printf("receiver cmd: CMD_LOGIN_RESULT, data length=%d\n", sock, result->length);
    }
    break;
    case CMD_NEW_JOIN: {
        recv(sock, sz_recv + header_len, header->length - header_len, 0);
        new_join *result = (new_join*)sz_recv;
        printf("receiver cmd: CMD_LOGIN_RESULT, data length=%d\n", sock, result->length);
    }
    break;
    }
}

bool is_run = true;
void cmd_thread(SOCKET sock) {
    while(true) {
        char buf[256] = {};
        scanf("%s", buf);
        if (0 == strcmp(buf, "exit")) {
            is_run = false;
            printf("exit thread\n");
            break;
        } else if (0 == strcmp(buf, "login")) {
            login l;
            strcpy(l.user_name, "Nemausa");
            strcpy(l.password, "Nemausa");
            send(sock, (const char*)&l, sizeof(login), 0);
        } else if (0 == strcmp(buf, "logout")) {
            logout l;
            strcpy(l.user_name, "Nemausa");
            send(sock, (const char*)&l, sizeof(logout), 0);
        } else {
            printf("undefined commad\n");
        }
        
    }
}


int main() {
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);

    // create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock) {
        printf("error socket\n");
    } else {
        printf("succes socket\n");
    }

    // connect
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(4567); 
    sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if (SOCKET_ERROR ==connect(sock,(sockaddr*)&sin, sizeof(sockaddr_in))) {
        printf("error connect\n");
    } else {
        printf("success connect\n");
    }

    std::thread t(cmd_thread, sock);
    t.detach();

    while (is_run) {
        fd_set fd_read;
        FD_ZERO(&fd_read);
        FD_SET(sock, &fd_read);
        timeval t = {0, 0};
        int ret = select(sock, &fd_read, 0, 0, &t);
        if (ret < 0) {
            printf("task end\n");
            break;
        }

        if (FD_ISSET(sock, &fd_read)) {
            FD_CLR(sock, &fd_read);
            if (-1 == processor(sock)) {
                printf("slecet end\n");
                break;
            }
        }
    }
    
    // close socket
    closesocket(sock);
    
    WSACleanup();
    getchar();

    return 0;
}