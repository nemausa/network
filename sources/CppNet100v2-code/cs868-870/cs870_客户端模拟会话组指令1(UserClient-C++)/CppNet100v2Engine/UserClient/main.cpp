#include"Log.hpp"
#include"Config.hpp"
#include"UserClient.hpp"


using namespace doyou::io;

int main(int argc, char* args[])
{
	//设置运行日志名称
	Log::Instance().setLogPath("UserClientLog", "w", false);
	Config::Instance().Init(argc, args);

	const int naxx = 30;
	UserClient clients[naxx];
	for (size_t i = 0; i < naxx; i++)
	{
		clients[i].Init(i);
	}
	
	while (true)
	{
		for (size_t i = 0; i < naxx; i++)
		{
			clients[i].Run();
		}
		
	}

	for (size_t i = 0; i < naxx; i++)
	{
		clients[i].Close();
	}

	////在主线程中等待用户输入命令
	//while (true)
	//{
	//	char cmdBuf[256] = {};
	//	scanf("%s", cmdBuf);
	//	if (0 == strcmp(cmdBuf, "exit"))
	//	{
	//		server.Close();
	//		break;
	//	}
	//	else {
	//		CELLLog_Info("undefine cmd");
	//	}
	//}

	CELLLog_Info("exit.");

	return 0;
}
