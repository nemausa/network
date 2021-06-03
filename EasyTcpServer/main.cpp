#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<iostream>
#include<Winsock2.h>
#include<stdio.h>

#pragma comment(lib,"ws2_32.lib")

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

    // 4 accept client
    sockaddr_in client_add = {};
    int len = sizeof(sockaddr_in);
    SOCKET c_sock = INVALID_SOCKET;
    char buff[] = "Hello, I'm Server.";

    while (true) {
        c_sock = accept(sock, (sockaddr*)&client_add, &len);
        if (INVALID_SOCKET == c_sock) {
            printf("error, invalid client\n");
        } 
        printf("new client ip = %s \n", inet_ntoa(client_add.sin_addr));
        
        // 5 send data to client
        send(c_sock, buff, strlen(buff) + 1, 0);
    }

    // 6 close socket
    closesocket(sock);
    
    WSACleanup();

    return 0;
}