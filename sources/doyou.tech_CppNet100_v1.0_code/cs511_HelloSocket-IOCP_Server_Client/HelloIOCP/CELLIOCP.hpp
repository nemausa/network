#ifndef _CELL_IOCP_HPP_
#define _CELL_IOCP_HPP_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<mswsock.h>
#include<stdio.h>

enum IO_TYPE
{
	ACCEPT = 10,
	RECV,
	SEND
};

//���ݻ������ռ��С
#define IO_DATA_BUFF_SIZE 1024
struct IO_DATA_BASE
{
	//�ص���
	OVERLAPPED overlapped;
	//
	SOCKET sockfd;
	//���ݻ�����
	char buffer[IO_DATA_BUFF_SIZE];
	//ʵ�ʻ��������ݳ���
	int length;
	//��������
	IO_TYPE iotype;
};

struct IO_EVENT
{
	IO_DATA_BASE* pIoData;
	SOCKET sockfd = INVALID_SOCKET;
	DWORD bytesTrans = 0;
};

class CELLIOCP
{
public:
	~CELLIOCP()
	{
		destory();
	}

	//������ɶ˿�IOCP
	bool create()
	{
		_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (NULL == _completionPort)
		{
			printf("CreateIoCompletionPort failed with error %d\n", GetLastError());
			return false;
		}
		return true;
	}

	void destory()
	{
		if (_completionPort)
		{
			CloseHandle(_completionPort);
			_completionPort = NULL;
		}
	}

	//����IOCP��ServerSocket
	bool reg(SOCKET sockfd)
	{
		//��ɼ�
		auto ret = CreateIoCompletionPort((HANDLE)sockfd, _completionPort, (ULONG_PTR)sockfd, 0);
		if (!ret)
		{
			printf("����IOCP��ServerSocketʧ��,������=%d\n", GetLastError());
			return false;
		}
		return true;
	}

	//��IOCPͶ�ݽ������ӵ�����
	void postAccept(IO_DATA_BASE* pIO_DATA)
	{
		if (!_AcceptEx)
			printf("error, postAccept _AcceptEx is null\n");

		pIO_DATA->iotype = IO_TYPE::ACCEPT;
		pIO_DATA->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (FALSE == _AcceptEx(_sockServer
			, pIO_DATA->sockfd
			, pIO_DATA->buffer
			, 0
			, sizeof(sockaddr_in) + 16
			, sizeof(sockaddr_in) + 16
			, NULL
			, &pIO_DATA->overlapped
		))
		{
			int err = WSAGetLastError();
			if (ERROR_IO_PENDING != err)
			{
				printf("AcceptEx failed with error %d\n", err);
				return;
			}
		}
	}

	//��IOCPͶ�ݽ������ݵ�����
	void postRecv(IO_DATA_BASE* pIO_DATA)
	{
		pIO_DATA->iotype = IO_TYPE::RECV;
		WSABUF wsBuff = {};
		wsBuff.buf = pIO_DATA->buffer;
		wsBuff.len = IO_DATA_BUFF_SIZE;
		DWORD flags = 0;
		ZeroMemory(&pIO_DATA->overlapped, sizeof(OVERLAPPED));

		if (SOCKET_ERROR == WSARecv(pIO_DATA->sockfd, &wsBuff, 1, NULL, &flags, &pIO_DATA->overlapped, NULL))
		{
			int err = WSAGetLastError();
			if (ERROR_IO_PENDING != err)
			{
				printf("WSARecv failed with error %d\n", err);
				return;
			}
		}
	}
	//��IOCPͶ�ݷ������ݵ�����
	void postSend(IO_DATA_BASE* pIO_DATA)
	{
		pIO_DATA->iotype = IO_TYPE::SEND;
		WSABUF wsBuff = {};
		wsBuff.buf = pIO_DATA->buffer;
		wsBuff.len = pIO_DATA->length;
		DWORD flags = 0;
		ZeroMemory(&pIO_DATA->overlapped, sizeof(OVERLAPPED));

		if (SOCKET_ERROR == WSASend(pIO_DATA->sockfd, &wsBuff, 1, NULL, flags, &pIO_DATA->overlapped, NULL))
		{
			int err = WSAGetLastError();
			if (ERROR_IO_PENDING != err)
			{
				printf("WSASend failed with error %d\n", err);
				return;
			}
		}
	}

	int wait(IO_EVENT& ioEvent,int timeout)
	{
		ioEvent.bytesTrans = 0;
		ioEvent.pIoData = NULL;
		ioEvent.sockfd = INVALID_SOCKET;
		if (FALSE == GetQueuedCompletionStatus(_completionPort
			, &ioEvent.bytesTrans
			, (PULONG_PTR)&ioEvent.sockfd
			, (LPOVERLAPPED*)&ioEvent.pIoData
			, timeout))
		{
			int err = GetLastError();
			if (WAIT_TIMEOUT == err)
			{
				return 0;
			}
			if (ERROR_NETNAME_DELETED == err)
			{
				return 1;
			}
			printf("GetQueuedCompletionStatus failed with error %d\n", err);
			return -1;
		}
		return 1;
	}

	bool loadAcceptEx(SOCKET ListenSocket)
	{
		if (INVALID_SOCKET != _sockServer)
		{
			printf("loadAcceptEx _sockServer != INVALID_SOCKET\n");
			return false;
		}
		if (_AcceptEx)
		{
			printf("loadAcceptEx _AcceptEx != NULL\n");
			return false;
		}
		_sockServer = ListenSocket;
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		DWORD dwBytes = 0;
		int iResult = WSAIoctl(ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidAcceptEx, sizeof(GuidAcceptEx),
			&_AcceptEx, sizeof(_AcceptEx),
			&dwBytes, NULL, NULL);

		if (iResult == SOCKET_ERROR) {
			printf("WSAIoctl failed with error: %u\n", WSAGetLastError());
			return false;
		}
		return true;
	}
private:
	//��AcceptEx���������ڴ��У�����Ч�ʸ���
	LPFN_ACCEPTEX _AcceptEx = NULL;
	HANDLE _completionPort = NULL;
	SOCKET _sockServer = INVALID_SOCKET;
};

#endif //_WIN32

#endif // !_CELL_IOCP_HPP_