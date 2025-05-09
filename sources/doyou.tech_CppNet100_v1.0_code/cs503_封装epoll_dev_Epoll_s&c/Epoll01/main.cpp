#include"CELLEpoll.hpp"

#include<stdio.h>
#include<vector>
#include<thread>
#include<algorithm>

std::vector<SOCKET> g_clients;

bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("退出cmdThread线程\n");
			break;
		}
		else {
			printf("不支持的命令。\n");
		}
	}
}

//缓冲区
char g_szBUff[4096] = {};
int g_nLen = 0;
int readData(SOCKET cSock)
{
	g_nLen = (int)recv(cSock, g_szBUff, 4096, 0);
	return g_nLen;
}

int WriteData(SOCKET cSock)
{
	if(g_nLen > 0)
	{
		int nLen = (int)send(cSock, g_szBUff, g_nLen, 0);
		g_nLen = 0;
		return nLen;
	}
	return 1;
}

int clientLeave(SOCKET cSock)
{
	close(cSock);
	printf("客户端<socket=%d>已退出\n", cSock);
	auto itr = std::find(g_clients.begin(), g_clients.end(), cSock);
	g_clients.erase(itr);
}

int main()
{
	//启动cmd线程
	std::thread t1(cmdThread);
	t1.detach();
	/////////////////////////////////
	//-- 用Socket API建立简易TCP服务端
	// 1 建立一个socket 套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 2.1 bind 绑定用于接受客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);//host to net unsigned short
	_sin.sin_addr.s_addr = INADDR_ANY;
	// 2.2
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("错误,绑定网络端口失败...\n");
	}
	else {
		printf("绑定网络端口成功...\n");
	}
	
	// 3 listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock, 64))
	{
		printf("错误,监听网络端口失败...\n");        
	}
	else {
		printf("监听网络端口成功...\n");
	}
	//--------
	const int maxClient = 60000;
	//
	CELLEpoll ep;
	ep.create(maxClient);
	ep.ctl(EPOLL_CTL_ADD, _sock, EPOLLIN);
	//
	int msgCount = 0;
	int cCount = 0;
	//
	while (g_bRun)
	{

		int n = ep.wait(1);
		auto events = ep.events();
		for(int i = 0; i < n; i++)
		{
			//当服务端socket发生事件时，表示有新客户端连接
			if(events[i].data.fd == _sock)
			{
				if(events[i].events & EPOLLIN)
				{
					// 4 accept 等待接受客户端连接
					sockaddr_in clientAddr = {};
					int nAddrLen = sizeof(sockaddr_in);
					SOCKET _cSock = INVALID_SOCKET;
					_cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
					cCount++;
					if (INVALID_SOCKET == _cSock)
					{
						printf("错误,接受到无效客户端SOCKET %d 错误代码是%d，错误信息是’%s’...\n",cCount, errno, strerror(errno));
					}
					else
					{
						g_clients.push_back(_cSock);
						ep.ctl(EPOLL_CTL_ADD, _cSock, EPOLLIN);
						printf("新客户端加入：socket = %d,IP = %s  %d\n", (int)_cSock, inet_ntoa(clientAddr.sin_addr),cCount);
					}
					continue;
				}
				// printf("other %d...\n",cCount);
			}
			//当前socket有数据可读，也有可能时发生错误
			if(events[i].events & EPOLLIN)
			{
				printf("EPOLLIN|%d\n",++msgCount);
				auto cSock = events[i].data.fd;
				int ret = readData(cSock);
				if(ret <= 0)
				{
					clientLeave(cSock);
				}else{
					printf("收到客户端数据：id = %d, socket = %d, len = %d\n",msgCount, cSock, ret);
				}
				ep.ctl(EPOLL_CTL_MOD, cSock, EPOLLOUT);
			}
			//当前socket是否可写数据
			if(events[i].events & EPOLLOUT)
			{
				printf("EPOLLOUT|%d\n",msgCount);
				auto cSock = events[i].data.fd;
				int ret = WriteData(cSock);
				if(ret <= 0)
				{
					clientLeave(cSock);
				}
				if(msgCount < 10)
					ep.ctl(EPOLL_CTL_MOD, cSock, EPOLLIN);
				else
					ep.ctl(EPOLL_CTL_DEL, cSock, 0);
			}
			/*
			if(events[i].events & EPOLLERR)
			{
				auto cSock = events[i].data.fd;
				printf("EPOLLERR：id = %d, socket = %d\n",msgCount, cSock);
			}
			if(events[i].events & EPOLLHUP)
			{
				auto cSock = events[i].data.fd;
				printf("EPOLLHUP：id = %d, socket = %d\n",msgCount, cSock);
			}
			*/
		}
	}
	
	for(auto client: g_clients)
	{
		close(client);
	}
	ep.destory();
	close(_sock);
	printf("已退出。\n");
	return 0; 
}


/*
cell_epoll_ctl(epfd, EPOLL_CTL_ADD, cSock, EPOLLOUT|EPOLLIN);
c++ 函数传参 按位与 按位或
C/C++函数参数使用位或运算
https://www.zhihu.com/question/23814540/answer/25745880
https://blog.csdn.net/sinat_29003361/article/details/52713749
*/