#include "EasyTcpClient.hpp"
#include<thread>

void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			printf("退出cmdThread线程\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lydmm");
			client->SendData(&login);

		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "lyd");
			client->SendData(&logout);
		}
		else {
			printf("不支持的命令。\n");
		}
	}
}

int main()
{

	EasyTcpClient client1;
	client1.Connect("192.168.74.1", 4567);

	EasyTcpClient client2;
	client2.Connect("192.168.74.136", 4567);

	EasyTcpClient client3;
	client3.Connect("192.168.74.134", 4567);
/*
	//启动UI线程
	std::thread t1(cmdThread, &client1);
	t1.detach();

	std::thread t2(cmdThread, &client2);
	t2.detach();

	std::thread t3(cmdThread, &client3);
	t3.detach();
*/
	Login login;
	strcpy(login.userName, "lyd");
	strcpy(login.PassWord, "lydmm");
	while (client1.isRun() || client2.isRun() || client3.isRun())
	{
		client1.OnRun();
		client2.OnRun();
		client3.OnRun();

		client1.SendData(&login);
		client2.SendData(&login);
		client3.SendData(&login);
		//printf("空闲时间处理其它业务..\n");
		//Sleep(1000);
	}
	client1.Close();
	client2.Close();
	client3.Close();

	printf("已退出。\n");
	getchar();
	return 0;
}