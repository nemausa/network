#include"CELLIOCP.hpp"

#define nClient 3

//-- ��Socket API��������TCP�����
//-- IOCP Server��������
int main()
{
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//------------//
	// 1 ����һ��socket
	// ��ʹ��socket���������׽���ʱ����Ĭ������WSA_FLAG_OVERLAPPED��־
	//////
	// ע������Ҳ������ WSASocket��������socket
	// ���һ��������Ҫ����Ϊ�ص���־��WSA_FLAG_OVERLAPPED��
	// SOCKET sockServer = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	//////
	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 2.1 ���ö���IP��˿���Ϣ 
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);//host to net unsigned short
	_sin.sin_addr.s_addr = INADDR_ANY;
	// 2.2 ��sockaddr��ServerSocket
	if (SOCKET_ERROR == bind(sockServer, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("����,������˿�ʧ��...\n");
	}
	else {
		printf("������˿ڳɹ�...\n");
	}

	// 3 ����ServerSocket
	if (SOCKET_ERROR == listen(sockServer, 64))
	{
		printf("����,��������˿�ʧ��...\n");
	}
	else {
		printf("��������˿ڳɹ�...\n");
	}
	//-------IOCP Begin-------//
	//4 ������ɶ˿�IOCP
	CELLIOCP iocp;
	iocp.create();
	
	//5 ����IOCP��ServerSocket
	//��ɼ�
	iocp.reg(sockServer);

	//6 ��IOCPͶ�ݽ������ӵ�����
	iocp.loadAcceptEx(sockServer);


	IO_DATA_BASE ioData[nClient] = {};
	for (int n = 0; n < nClient; n++)
	{
		iocp.postAccept(&ioData[n]);
	}
		
	IO_EVENT ioEvent = {};
	int msgCount = 0;
	while (true)
	{
		int ret = iocp.wait(ioEvent, 1);
		if (ret < 0)
		{
			break;
		}
		if (ret == 0)
		{
			continue;
		}
		//7.1 �������� ���
		if (IO_TYPE::ACCEPT == ioEvent.pIoData->iotype)
		{
			printf("�¿ͻ��˼��� sockfd=%d\n", ioEvent.pIoData->sockfd);
			//7.2 ����IOCP��ClientSocket
			if (!iocp.reg(ioEvent.pIoData->sockfd))
			{
				//printf("����IOCP��ClientSocket=%dʧ��\n", ioEvent.pIoData->sockfd);
				closesocket(ioEvent.pIoData->sockfd);
				iocp.postAccept(ioEvent.pIoData);
				continue;
			}
			//7.3 ��IOCPͶ�ݽ�����������
			iocp.postRecv(ioEvent.pIoData);
		}
		//8.1 �������� ��� Completion
		else if (IO_TYPE::RECV == ioEvent.pIoData->iotype)
		{
			if (ioEvent.bytesTrans <= 0)
			{//�ͻ��˶Ͽ�����
				printf("�ر� sockfd=%d, RECV bytesTrans=%d\n", ioEvent.pIoData->sockfd, ioEvent.bytesTrans);
				closesocket(ioEvent.pIoData->sockfd);
				iocp.postAccept(ioEvent.pIoData);
				continue;
			}
			//printf("�յ�����: sockfd=%d, bytesTrans=%d msgCount=%d\n", ioEvent.pIoData->sockfd, ioEvent.bytesTrans, ++msgCount);
			ioEvent.pIoData->length = ioEvent.bytesTrans;
			//8.2 ��IOCPͶ�ݷ�����������
			iocp.postSend(ioEvent.pIoData);
		}
		//9.1 �������� ��� Completion
		else if (IO_TYPE::SEND == ioEvent.pIoData->iotype)
		{
			if (ioEvent.bytesTrans <= 0)
			{//�ͻ��˶Ͽ�����
				printf("�ر� sockfd=%d, SEND bytesTrans=%d\n", ioEvent.pIoData->sockfd, ioEvent.bytesTrans);
				closesocket(ioEvent.pIoData->sockfd);
				iocp.postAccept(ioEvent.pIoData);
				continue;
			}
			//printf("��������: sockfd=%d, bytesTrans=%d msgCount=%d\n", ioEvent.pIoData->sockfd, ioEvent.bytesTrans, msgCount);
			//9.2 ��IOCPͶ�ݽ�����������
			iocp.postRecv(ioEvent.pIoData);
		}
		else {
			printf("δ������Ϊ sockfd=%d", ioEvent.sockfd);
		}
	}

	//------------//
	//10.1 �ر�ClientSocket
	for (int n = 0; n < nClient; n++)
	{
		closesocket(ioData[n].sockfd);
	}
	//10.2 �ر�ServerSocket
	closesocket(sockServer);
	//10.3 �ر���ɶ˿�
	iocp.destory();
	//���Windows socket����
	WSACleanup();
	return 0;
}