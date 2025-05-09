#include"Config.hpp"
#include"TcpHttpClient.hpp"

using namespace doyou::io;

class MyHttpClient : public TcpHttpClient
{
private:
	typedef std::function<void(HttpClientC*)> EventCall;
public:
	MyHttpClient()
	{
		NetWork::Init();
	}
public:
	virtual void OnNetMsg(netmsg_DataHeader* header)
	{
		HttpClientC* pHttpClient = dynamic_cast<HttpClientC*>(_pClient);
		if (!pHttpClient)
			return;
		
		if (!pHttpClient->getResponseInfo())
			return;

		if (_onRespCall)
		{
			_onRespCall(pHttpClient);
		}
	}

	virtual void OnDisconnect() {
		if (_onRespCall)
		{
			_onRespCall(nullptr);
		}
	};

	void get(const char* httpurl, EventCall onRespCall)
	{
		_onRespCall = onRespCall;

		deatch_http_url(httpurl);
		if (0 == hostname2ip(_host.c_str(), _port.c_str()))
		{
			url2get(_host.c_str(), _path.c_str(), _args.c_str());
		}
	}

	int hostname2ip(const char* hostname, const char* port)
	{
		if (!hostname)
		{
			Log::Warring("hostname2ip(hostname is null ptr).");
			return -1;
		}

		if (!port)
		{
			Log::Warring("hostname2ip(port is null ptr).");
			return -1;
		}

		unsigned short port_ = 80;
		if (port, strlen(port) > 0)
			port_ = atoi(port);

		//�������Ͷ˿ںŲ��䣬�Ͳ��������ӷ����
		if (isRun() && _host0 == hostname && port_ == _port0)
			return 0;

		addrinfo hints = {};
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_IP;
		hints.ai_flags = AI_ALL;
		addrinfo* pAddrList = nullptr;
		int ret = getaddrinfo(hostname, nullptr, &hints, &pAddrList);
		if (0 != ret)
		{
			Log::PError("%s getaddrinfo", hostname);
			freeaddrinfo(pAddrList);
			return ret;
		}

		char ipStr[256] = {};
		for (auto pAddr = pAddrList; pAddr != nullptr; pAddr = pAddr->ai_next)
		{
			ret = getnameinfo(pAddr->ai_addr, pAddr->ai_addrlen, ipStr, 255, nullptr, 0, NI_NUMERICHOST);
			if (0 != ret)
			{
				Log::PError("%s getnameinfo", hostname);
				continue;
			}
			else {
				if(pAddr->ai_family == AF_INET6)
					Log::Info("%s ipv6: %s", hostname, ipStr);
				else if (pAddr->ai_family == AF_INET)
					Log::Info("%s ipv4: %s", hostname, ipStr);
				else {
					Log::Info("%s addr: %s", hostname, ipStr);
					continue;
				}
				
				if (connet2ip(pAddr->ai_family, ipStr, port_))
				{
					_host0 = _host;
					_port0 = port_;
					break;
				}
			}
		}

		freeaddrinfo(pAddrList);
		return ret;
	}
private:
	void url2get(const char* host, const char* path, const char* args)
	{
		std::string msg = "GET ";
		
		if(path && strlen(path) > 0)
			msg += path;
		else
			msg += "/";

		if (args && strlen(args) > 0)
		{
			msg += "?";
			msg += args;
		}

		msg += " HTTP/1.1\r\n";

		msg += "Host: ";
		msg += host;
		msg += "\r\n";

		msg += "Connection: keep-alive\r\n";
		msg += "Accept: */*\r\n";

		msg += "Origin: ";
		msg += host;
		msg += "\r\n";

		msg += "\r\n";

		SendData(msg.c_str(), msg.length());
	}

	bool connet2ip(int af, const char* ip, unsigned short port)
	{
		if (!ip)
			return false;

		if (INVALID_SOCKET == InitSocket(af, 10240, 102400))
			return false;

		if (SOCKET_ERROR == Connect(ip, port))
			return false;

		Log::Info("connet2ip(%s,%d)", ip, port);
	}

	void deatch_http_url(std::string httpurl)
	{
		_httpType.clear();
		_host.clear();
		_port.clear();
		_path.clear();
		_args.clear();

		auto pos1 = httpurl.find("://");
		if (pos1 != std::string::npos)
		{
			_httpType = httpurl.substr(0, pos1);
			pos1 += 3;
		}
		else {
			pos1 = 0;
		}
		auto pos2 = httpurl.find('/', pos1);
		if (pos2 != std::string::npos)
		{
			_host = httpurl.substr(pos1, pos2 - pos1);
			_path = httpurl.substr(pos2);

			pos1 = _path.find('?');
			if (pos1 != std::string::npos)
			{
				_args = _path.substr(pos1 + 1);
				_path = _path.substr(0, pos1);
			}
		}
		else {
			_host = httpurl.substr(pos1);
		}

		pos1 = _host.find(':');
		if (pos1 != std::string::npos)
		{
			_port = _host.substr(pos1 + 1);
			_host = _host.substr(0, pos1);
		}
	}
private:
	std::string _httpType;
	std::string _host;
	std::string _port;
	std::string _path;
	std::string _args;
	//��һ�����������/������
	std::string _host0;
	//��һ������Ķ˿ں�
	unsigned short _port0;

	EventCall _onRespCall = nullptr;

public:
	void test()
	{
		static int i = 0;
		this->get("http://192.168.1.117:4567/sub?c=5&n=2", [this](HttpClientC* pHttpClient) {
			if (pHttpClient)
			{
				CELLLog_Info("recv server msg. len=%s %d", pHttpClient->header_getStr("Content-Length", "?"), ++i);
				//CELLLog_Info("%s", pHttpClient->content());
			}
			else
			{
				CELLLog_Info("server disconnect. %d", ++i);
			}
			test();
		});
		//if (i%100 > 50)
		//{
		//	this->get("https://www.baidu.com", [this](HttpClientC* pHttpClient) {
		//		CELLLog_Info("recv server msg. %d", ++i);
		//		test();
		//	});
		//}
		//else {
		//	this->get("https://www.163.com", [this](HttpClientC* pHttpClient) {
		//		CELLLog_Info("recv server msg. %d", ++i);
		//		test();
		//	});
		//}

	}
};

int main(int argc, char *args[])
{
#if _WIN32 && _CONSOLE
	system("chcp 65001");
#endif
	
	//����������־����
	Log::Instance().setLogPath("clientLog", "w", false);
	Config::Instance().Init(argc, args);

	MyHttpClient httpClient;

	////1
	//httpClient.get("https://www.baidu.com", [&httpClient](HttpClientC* pHttpClient) {
	//	CELLLog_Info("recv server msg.1\n");
	//	//2
	//	httpClient.get("https://www.baidu.com", [&httpClient](HttpClientC* pHttpClient) {
	//		CELLLog_Info("recv server msg.2\n");
	//		//3
	//		httpClient.get("https://www.baidu.com", [](HttpClientC* pHttpClient) {
	//			CELLLog_Info("recv server msg.3\n");

	//		});
	//	});
	//});

	httpClient.test();

	//auto respStr = pHttpClient->content();
	//CELLLog_Info("%s\n", respStr);

	//httpClient.get("ipv4.dosfu.com");
	//httpClient.get("ipv6.dosfu.com");
	//httpClient.get("dosfu.com");
	//httpClient.get("www.dosfu.com/add.php?a=1&b=2");
	while (true)
	{
		httpClient.OnRun();
	}
	httpClient.Close();
	return 0;
}