#include "cell_iocp.hpp"

#define nclient 3

int main() {
    WORD ver = MAKEWORD(2, 2);
    WSAData dat;
    WSAStartup(ver,&dat);

	//------------//
	// 1 建立一个socket
	// 当使用socket函数创建套接字时，会默认设置WSA_FLAG_OVERLAPPED标志
	//////
	// 注意这里也可以用 WSASocket函数创建socket
	// 最后一个参数需要设置为重叠标志（WSA_FLAG_OVERLAPPED）
	// SOCKET sockServer = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	//////
    SOCKET sock_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 2.1 设置对外IP与端口信息
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(4567);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;

	// 2.1 设置对外IP与端口信息 
    if (SOCKET_ERROR == bind(sock_server, (sockaddr*)&sin, sizeof(sin))) {
        printf("error,bind\n");
    } else {
        printf("success bind\n");
    }

	// 3 监听ServerSocket
    if (SOCKET_ERROR == listen(sock_server, 64)) {
        printf("error,listen\n");
    } else {
        printf("success listen\n");
    }
  	//4 创建完成端口IOCP 
    cell_iocp iocp;
    iocp.create();

    io_event io_event = {};
	//5 关联IOCP与ServerSocket
	//完成键
    iocp.reg(sock_server);

	//6 向IOCP投递接受连接的任务
    iocp.load_accept(sock_server);

    io_data_base io_data[nclient] = {};
    for (int n = 0; n < nclient; n++) {
        iocp.post_accept(&io_data[n]);
    }


    int msg_count = 0;
    while (true) {
        int ret = iocp.wait(io_event, 1);
        if (ret < 0) {
            break;
        }
        if (ret == 0) {
            continue;
        }
		//7.1 接受连接 完成
        if (io_type_e::ACCEPT == io_event.p_io_data->io_type) {
            printf("new client socketfd=%d\n", io_event.p_io_data->sockfd);
            if (!iocp.reg(io_event.p_io_data->sockfd)) {
                printf("association failed\n");
                closesocket(io_event.p_io_data->sockfd);
                iocp.post_accept(io_event.p_io_data);
                continue;
            }
            iocp.post_accept(io_event.p_io_data);
        } else if (io_type_e::RECV == io_event.p_io_data->io_type) {
            if (io_event.bytes_trans <= 0) {
                printf("close socket=%d, recv bytestrans=%d\n", io_event.p_io_data->sockfd, io_event.bytes_trans);
                closesocket(io_event.p_io_data->sockfd);
                iocp.post_accept(io_event.p_io_data);
                continue;
            }
            iocp.post_send(io_event.p_io_data);
        } else if (io_type_e::SEND == io_event.p_io_data->io_type) {
            if (io_event.bytes_trans <= 0) {
                printf("close socket=%d, recv bytestrans=%d\n", io_event.p_io_data->sockfd, io_event.bytes_trans);
                closesocket(io_event.p_io_data->sockfd);
                iocp.post_accept(io_event.p_io_data);
                continue;
            }
            iocp.post_recv(io_event.p_io_data);
        } else {
            printf("undefine sockfd=%d\n", io_event.p_io_data->sockfd);
        }
    }

    for (int n = 0; n < nclient; n++) {
        closesocket(io_data[n].sockfd);
    }
    closesocket(sock_server);
    iocp.destory();
    WSACleanup();
    return 0;
}