#include"Log.hpp"
#include"Config.hpp"
#include"UserClient.hpp"
#include"LocalData.hpp"

using namespace doyou::io;

std::string UserClient::s_csGateUrl;
std::vector<std::string> UserClient::s_text_arr;
int UserClient::s_nSendSleep = 1000;

int main(int argc, char* args[])
{
#if _WIN32 && _CONSOLE
	system("chcp 65001");
#endif // _WIN32 && _CONSOLE
	//设置运行日志名称
	Log::Instance().setLogPath("UserClientLog", "w", false);
	Config::Instance().Init(argc, args);
	///////////////////////
	{
		neb::CJsonObject text_config;
		if (LocalData::json_read("text_config.json", text_config))
		{
			auto text = text_config["text"];
			if (!text.IsArray())
			{
				CELLLog_Error("!text.IsArray");
				return 0;
			}

			int size = text.GetArraySize();
			for (size_t i = 0; i < size; i++)
			{
				UserClient::s_text_arr.push_back(text(i));
			}
		}
	}
	///////////////////////
	UserClient::s_csGateUrl = Config::Instance().getStr("csGateUrl", "ws://127.0.0.1:5000");
	UserClient::s_nSendSleep = Config::Instance().getInt("nSendSleep", 1000);
	int nClient = Config::Instance().getInt("nClient", 1000);
	int nTestId = Config::Instance().getInt("nTestId", 0);

	std::vector<UserClient*> clients;
	clients.resize(nClient);

	for (size_t i = 0; i < nClient; i++)
	{
		clients[i] = new UserClient();
	}

	for (size_t i = 0; i < nClient; i++)
	{
		clients[i]->Init(i+ nTestId);
	}
	
	while (true)
	{
		for (size_t i = 0; i < nClient; i++)
		{
			clients[i]->Run();
		}
		Thread::Sleep(20);
	}

	for (size_t i = 0; i < nClient; i++)
	{
		clients[i]->Close();
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
