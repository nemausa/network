#include"EasyTcpServer.hpp"
#include"CELLMsgStream.hpp"

class MyServer : public EasyTcpServer
{
public:

	//cellServer 4 多个线程触发 不安全
	//如果只开启1个cellServer就是安全的
	virtual void OnNetJoin(CELLClient* pClient)
	{
		EasyTcpServer::OnNetJoin(pClient);
	}
	//cellServer 4 多个线程触发 不安全
	//如果只开启1个cellServer就是安全的
	virtual void OnNetLeave(CELLClient* pClient)
	{
		EasyTcpServer::OnNetLeave(pClient);
	}
	//cellServer 4 多个线程触发 不安全
	//如果只开启1个cellServer就是安全的
	virtual void OnNetMsg(CELLServer* pServer, CELLClient* pClient, netmsg_DataHeader* header)
	{
		EasyTcpServer::OnNetMsg(pServer, pClient, header);
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			pClient->resetDTHeart();
			//send recv 
			netmsg_Login* login = (netmsg_Login*)header;
			//CELLLog_Info("recv <Socket=%d> msgType：CMD_LOGIN, dataLen：%d,userName=%s PassWord=%s\n", cSock, login->dataLength, login->userName, login->PassWord);
			//忽略判断用户密码是否正确的过程
			netmsg_LoginR ret;
			if(SOCKET_ERROR == pClient->SendData(&ret))
			{
				//发送缓冲区满了，消息没发出去
				CELLLog_Info("<Socket=%d> Send Full\n", pClient->sockfd());
			}
			//netmsg_LoginR* ret = new netmsg_LoginR();
			//pServer->addSendTask(pClient, ret);
		}//接收 消息---处理 发送   生产者 数据缓冲区  消费者 
		break;
		case CMD_LOGOUT:
		{
			pClient->resetDTHeart();
			CELLReadStream r(header);
			//读取消息长度
			r.ReadInt16();
			//读取消息命令
			r.getNetCmd();
			auto n1 = r.ReadInt8();
			auto n2 = r.ReadInt16();
			auto n3 = r.ReadInt32();
			auto n4 = r.ReadFloat();
			auto n5 = r.ReadDouble();
			uint32_t n = 0;
			r.onlyRead(n);
			char name[32] = {};
			auto n6 = r.ReadArray(name, 32);
			char pw[32] = {};
			auto n7 = r.ReadArray(pw, 32);
			int ata[10] = {};
			auto n8 = r.ReadArray(ata, 10);
			///
			CELLWriteStream s(128);
			s.setNetCmd(CMD_LOGOUT_RESULT);
			s.WriteInt8(n1);
			s.WriteInt16(n2);
			s.WriteInt32(n3);
			s.WriteFloat(n4);
			s.WriteDouble(n5);
			s.WriteArray(name, n6);
			s.WriteArray(pw, n7);
			s.WriteArray(ata, n8);
			s.finsh();
			pClient->SendData(s.data(), s.length());
		}
		break;
		case CMD_C2S_HEART:
		{
			pClient->resetDTHeart();
			netmsg_s2c_Heart ret;
			pClient->SendData(&ret);
		}
		default:
		{
			CELLLog_Info("recv <socket=%d> undefine msgType,dataLen：%d\n", pClient->sockfd(), header->dataLength);
		}
		break;
		}
	}
private:

};

const char* argToStr(int argc, char* args[], int index, const char* def, const char* argName)
{
	if (index >= argc)
	{
		CELLLog_Error("argToStr, index=%d|argc=%d|argName=%s\n", index, argc, argName);
	}else {
		def = args[index];
	}
	CELLLog_Info("%s=%s\n",argName, def);
	return def;
}

int argToInt(int argc, char* args[], int index, int def, const char* argName)
{
	if (index >= argc)
	{
		CELLLog_Error("argToStr, index=%d|argc=%d|argName=%s\n", index, argc, argName);
	}
	else {
		def = atoi(args[index]);
	}
	CELLLog_Info("%s=%d\n", argName, def);
	return def;
}

int main(int argc,char* args[])
{

	const char* strIP = argToStr(argc, args, 1,"any", "strIP");
	uint16_t nPort = argToInt(argc, args, 2, 4567, "nPort");
	int nThread = argToInt(argc, args, 3, 1, "nThread");
	int nClient = argToInt(argc, args, 4, 1, "nClient");

	if (strcmp(strIP, "any") == 0)
	{
		strIP = nullptr;
	}

	CELLLog::Instance().setLogPath("serverLog","w");
	MyServer server;
	server.InitSocket();
	server.Bind(strIP, nPort);
	server.Listen(64);
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
			CELLLog_Info("undefine cmd\n");
		}
	}

	CELLLog_Info("exit.\n");
//#ifdef _WIN32
//	while (true)
//		Sleep(10);
//#endif
	return 0;
}
