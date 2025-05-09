#include"Log.hpp"
#include"Config.hpp"
#include"GroupServer.hpp"
#include"DBUser.hpp"

using namespace doyou::io;

int main(int argc, char* args[])
{
#if _WIN32 && _CONSOLE
	system("chcp 65001");
#endif // _WIN32 && _CONSOLE

	//设置运行日志名称
	Log::Instance().setLogPath("GroupServerLog", "w", false);
	Config::Instance().Init(argc, args);

	GroupServer server;
	server.Init();
	while (true)
	{
		server.Run();
	}
	server.Close();

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
