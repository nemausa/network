//#include"SelectServer.hpp"
//#include"EpollServer.hpp"
#include"TcpIocpServer.hpp"
#include"MsgStream.hpp"
#include"Config.hpp"

using namespace doyou::io;

class MyServer : public TcpIocpServer
{
public:
	MyServer()
	{
		_bSendBack = Config::Instance().hasKey("-sendback");
		_bSendFull = Config::Instance().hasKey("-sendfull");
		_bCheckMsgID = Config::Instance().hasKey("-checkMsgID");
	}
	//cellServer 4 ����̴߳��� ����ȫ
	//���ֻ����1��cellServer���ǰ�ȫ��
	virtual void OnNetJoin(Client* pClient)
	{
		TcpServer::OnNetJoin(pClient);
	}
	//cellServer 4 ����̴߳��� ����ȫ
	//���ֻ����1��cellServer���ǰ�ȫ��
	virtual void OnNetLeave(Client* pClient)
	{
		TcpServer::OnNetLeave(pClient);
	}
	//cellServer 4 ����̴߳��� ����ȫ
	//���ֻ����1��cellServer���ǰ�ȫ��
	virtual void OnNetMsg(Server* pServer, Client* pClient, netmsg_DataHeader* header)
	{
		TcpServer::OnNetMsg(pServer, pClient, header);
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			pClient->resetDTHeart();
			netmsg_Login* login = (netmsg_Login*)header;
			//�����ϢID
			if (_bCheckMsgID)
			{
				if (login->msgID != pClient->nRecvMsgID)
				{//��ǰ��ϢID�ͱ�������Ϣ������ƥ��
					CELLLog_Error("OnNetMsg socket<%d> msgID<%d> _nRecvMsgID<%d> %d", pClient->sockfd(), login->msgID, pClient->nRecvMsgID, login->msgID - pClient->nRecvMsgID);
				}
				++pClient->nRecvMsgID;
			}
			//��¼�߼�
			//......
			//��Ӧ��Ϣ
			if (_bSendBack)
			{
				netmsg_LoginR ret;
				ret.msgID = pClient->nSendMsgID;
				if (SOCKET_ERROR == pClient->SendData(&ret))
				{
					//���ͻ��������ˣ���Ϣû����ȥ,Ŀǰֱ��������
					//�ͻ�����Ϣ̫�࣬��Ҫ����Ӧ�Բ���
					//�������ӣ�ҵ��ͻ��˲�������ô����Ϣ
					//ģ�Ⲣ������ʱ�Ƿ���Ƶ�ʹ���
					if (_bSendFull)
					{
						CELLLog_Warring("<Socket=%d> Send Full", pClient->sockfd());
					}
				}
				else {
					++pClient->nSendMsgID;
				}
			}

			//CELLLog_Info("recv <Socket=%d> msgType��CMD_LOGIN, dataLen��%d,userName=%s PassWord=%s", cSock, login->dataLength, login->userName, login->PassWord);
		}//���� ��Ϣ---���� ����   ������ ���ݻ�����  ������ 
		break;
		case CMD_LOGOUT:
		{
			pClient->resetDTHeart();
			ReadByteStream r(header);
			//��ȡ��Ϣ����
			r.ReadInt16();
			//��ȡ��Ϣ����
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
			WriteByteStream s(128);
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
			CELLLog_Info("recv <socket=%d> undefine msgType,dataLen��%d", pClient->sockfd(), header->dataLength);
		}
		break;
		}
	}
private:
	//�Զ����־ �յ���Ϣ�󽫷���Ӧ����Ϣ
	bool _bSendBack;
	//�Զ����־ �Ƿ���ʾ�����ͻ�������д��
	bool _bSendFull;
	//�Ƿ�����յ�����ϢID�Ƿ�����
	bool _bCheckMsgID;
};

const char* argToStr(int argc, char* args[], int index, const char* def, const char* argName)
{
	if (index >= argc)
	{
		CELLLog_Error("argToStr, index=%d|argc=%d|argName=%s", index, argc, argName);
	}
	else {
		def = args[index];
	}
	CELLLog_Info("%s=%s", argName, def);
	return def;
}

int argToInt(int argc, char* args[], int index, int def, const char* argName)
{
	if (index >= argc)
	{
		CELLLog_Error("argToStr, index=%d|argc=%d|argName=%s", index, argc, argName);
	}
	else {
		def = atoi(args[index]);
	}
	CELLLog_Info("%s=%d", argName, def);
	return def;
}

int main(int argc, char* args[])
{
	//����������־����
	Log::Instance().setLogPath("serverLog", "w", false);
	Config::Instance().Init(argc, args);

	const char* strIP = Config::Instance().getStr("strIP", "any");
	uint16_t nPort = Config::Instance().getInt("nPort", 4567);
	int nThread = Config::Instance().getInt("nThread", 1);

	if (Config::Instance().hasKey("-p"))
	{
		CELLLog_Info("hasKey -p");
	}

	if (strcmp(strIP, "any") == 0)
	{
		strIP = nullptr;
	}

	MyServer server;
	server.InitSocket();
	server.Bind(strIP, nPort);
	server.Listen(SOMAXCONN);
	server.Start(nThread);

	//�����߳��еȴ��û���������
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
