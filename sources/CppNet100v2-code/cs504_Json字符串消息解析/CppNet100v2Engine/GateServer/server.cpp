#include"Log.hpp"
#include"Config.hpp"
#include"TcpWebSocketServer.hpp"

#include "CJsonObject.hpp"

using namespace doyou::io;

class MyServer:public TcpWebSocketServer
{
public:
	virtual void OnNetMsgWS(Server* pServer, WebSocketClientS* pWSClient)
	{
		WebSocketHeader& wsh = pWSClient->WebsocketHeader();
		if (wsh.opcode == opcode_PONG)
		{
			CELLLog_Info("websocket server say: PONG");
			return;
		}
		auto dataStr = pWSClient->fetch_data();
		CELLLog_Info("websocket client say: %s", dataStr);

		neb::CJsonObject json;
		if (!json.Parse(dataStr))
		{
			CELLLog_Error("json.Parse error : %s", json.GetErrMsg().c_str());
		}

		int msgId = 0;
		if (!json.Get("msgId", msgId))
		{
			CELLLog_Error("not found key<%s>.", "msgId");
		}

		time_t time = 0;
		if (!json.Get("time", time))
		{
			CELLLog_Error("not found key<%s>.", "time");
		}

		std::string data;
		if (!json.Get("data", data))
		{
			CELLLog_Error("not found key<%s>.", "data");
		}

		return;
	}
private:

};

int main(int argc, char* args[])
{
	//设置运行日志名称
	Log::Instance().setLogPath("serverLog", "w", false);
	Config::Instance().Init(argc, args);

	const char* strIP = Config::Instance().getStr("strIP", "any");
	uint16_t nPort = Config::Instance().getInt("nPort", 4567);
	int nThread = Config::Instance().getInt("nThread", 1);

	if (strcmp(strIP, "any") == 0)
	{
		strIP = nullptr;
	}

	MyServer server;
	if (Config::Instance().hasKey("-ipv6"))
	{
		CELLLog_Info("-ipv6");
		server.InitSocket(AF_INET6);
	}
	else {
		CELLLog_Info("-ipv4");
		server.InitSocket();
	}

	server.Bind(strIP, nPort);
	server.Listen(SOMAXCONN);
	server.Start(nThread);

	//在主线程中等待用户输入命令
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			server.Close();
			break;
		}
		else {
			CELLLog_Info("undefine cmd");
		}
	}

	CELLLog_Info("exit.");

	return 0;
}
