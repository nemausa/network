#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<iostream>
#include<winsock2.h>
#include<stdio.h>

#pragma comment(lib,"ws2_32.lib")

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
    
    // recv
    char buf[256] = {};
    int len = recv(sock, buf, 256, 0);
    if (len > 0) {
        printf("receive: %s\n", buf);
    } else {
        printf("error recv\n");
    }

    // close socket
    closesocket(sock);
    
    WSACleanup();
    getchar();

    return 0;
}