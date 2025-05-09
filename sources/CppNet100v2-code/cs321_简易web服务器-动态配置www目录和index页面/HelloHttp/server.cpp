#include"Log.hpp"
#include"Config.hpp"
#include"TcpHttpServer.hpp"

using namespace doyou::io;

class MyServer:public TcpHttpServer
{
public:
	virtual void OnNetMsg(Server* pServer, Client* pClient, netmsg_DataHeader* header)
	{
		HttpClient* pHttpClient = dynamic_cast<HttpClient*>(pClient);
		if (!pHttpClient)
			return;

		if(!pHttpClient->getRequestInfo())
			return;

		if (pHttpClient->url_compre("/add"))
		{
			int a = pHttpClient->args_getInt("a", 0);
			int b = pHttpClient->args_getInt("b", 0);
			int c = a + b;

			char respBodyBuff[32] = {};
			sprintf(respBodyBuff, "a+b=%d", c);

			pHttpClient->resp200OK(respBodyBuff, strlen(respBodyBuff));
		}
		else if (pHttpClient->url_compre("/sub"))
		{
			int a = pHttpClient->args_getInt("a", 0);
			int b = pHttpClient->args_getInt("b", 0);
			int c = a - b;

			char respBodyBuff[32] = {};
			sprintf(respBodyBuff, "a-b=%d", c);

			pHttpClient->resp200OK(respBodyBuff, strlen(respBodyBuff));
		}
		else {
			if (!respFile(pHttpClient))
			{
				pHttpClient->resp404NotFound();
			}
		}
	}

	bool respFile(HttpClient* pHttpClient)
	{
		std::string filePath;

		if (pHttpClient->url_compre("/"))
		{
			filePath = _wwwRoot + pHttpClient->url() + _indexPage;
		}
		else {
			filePath = _wwwRoot + pHttpClient->url();
		}

		FILE * file = fopen(filePath.c_str(), "rb");
		if (!file)
			return false;

		//��ȡ�ļ���С
		fseek(file, 0, SEEK_END);
		auto bytesize = ftell(file);
		rewind(file);

		//���ͻ������Ƿ���д����ô������
		if (!pHttpClient->canWrite(bytesize))
		{
			CELLLog_Warring("!pHttpClient->canWrite(bytesize), url=%s", filePath.c_str());
			fclose(file);
			return false;
		}

		//��ȡ
		char* buff = new char[bytesize];
		auto readsize = fread(buff, 1, bytesize, file);
		if (readsize != bytesize)
		{
			CELLLog_Warring("readsize != bytesize, url=%s", filePath.c_str());
			//�ͷ��ڴ�
			delete[] buff;
			//�ر��ļ�
			fclose(file);
			return false;
		}

		pHttpClient->resp200OK(buff, readsize);

		//�ͷ��ڴ�
		delete[] buff;
		//�ر��ļ�
		fclose(file);

		return true;
	}

	void wwwRoot(const char* www)
	{
		_wwwRoot = www;
	}

	void indexPage(const char* index)
	{
		_indexPage = index;
	}
private:
	std::string _wwwRoot;
	std::string _indexPage;
};

int main(int argc, char* args[])
{
	//����������־����
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

	const char* wwwroot = Config::Instance().getStr("wwwroot", "");
	const char* indexpage = Config::Instance().getStr("indexpage", "");
	server.wwwRoot(wwwroot);
	server.indexPage(indexpage);

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

//char htmlStr[] = 
//"\
//<!DOCTYPE html>\
//<html>\
//<head>\
//<title>HelloWeb</title>\
//</head>\
//<body>\
//    <button>GET</button>\
//	<button>POST</button>\
//</body>\
//</html>\
//";