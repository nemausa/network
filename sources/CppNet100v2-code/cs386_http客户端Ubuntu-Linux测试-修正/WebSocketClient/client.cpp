#include"Config.hpp"
#include"TcpWebSocketClient.hpp"

using namespace doyou::io;

int main(int argc, char *args[])
{
#if _WIN32 && _CONSOLE
	system("chcp 65001");
#endif

	//设置运行日志名称
	Log::Instance().setLogPath("clientLog", "w", false);
	Config::Instance().Init(argc, args);

	TcpWebSocketClient wsClient;
	wsClient.connect("ws://192.168.110.129:4567/");

	while (true)
	{
		wsClient.OnRun(1);
		//httpClient1.OnRun(1);
	}
	wsClient.Close();
	return 0;
}