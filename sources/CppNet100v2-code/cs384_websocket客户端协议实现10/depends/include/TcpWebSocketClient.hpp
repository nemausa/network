#ifndef _doyou_io_TcpWebSocketClient_HPP_
#define _doyou_io_TcpWebSocketClient_HPP_

#include"TcpClientMgr.hpp"
#include"WebSocketClientC.hpp"

#include"sha1.hpp"
#include"base64.hpp"

#include<queue>

namespace doyou {
	namespace io {
		class TcpWebSocketClient :public TcpClientMgr
		{
		public:
			TcpWebSocketClient()
			{
				NetWork::Init();
			}
		protected:
			virtual Client* makeClientObj(SOCKET cSock, int sendSize, int recvSize)
			{
				return new WebSocketClientC(cSock, sendSize, recvSize);
			}
		public:
			virtual void OnNetMsg(netmsg_DataHeader* header)
			{
				WebSocketClientC* pWSClient = dynamic_cast<WebSocketClientC*>(_pClient);
				if (!pWSClient)
					return;

				if (clientState_join == pWSClient->state())
				{
					if (!pWSClient->getResponseInfo())
						return;

					if (handshake())
					{
						CELLLog_Info("WebSocketClientC::handshake, Good.");
						pWSClient->state(clientState_run);

						std::string msg = "bu hao.";
						for (int i = 0; i < 10; i++)
							msg +="-hello web";
						msg += "===";
						pWSClient->writeText(msg.c_str(), msg.length());
					}
					else {
						CELLLog_Warring("WebSocketClientC::handshake, Bad.");
						pWSClient->onClose();
					}
				}
				else if (clientState_run == pWSClient->state()) {
					auto data = pWSClient->fetch_data();
					//CELLLog_Info("websocket server say: %s", data);

					WebSocketHeader& wsh = pWSClient->WebsocketHeader();
					pWSClient->writeText(data, wsh.len);
				}
			}

			bool handshake()
			{

				WebSocketClientC* pWSClient = dynamic_cast<WebSocketClientC*>(_pClient);
				if (!pWSClient)
					return false;

				auto strUpgrade = pWSClient->header_getStr("Upgrade", "");
				if (0 != strcmp(strUpgrade, "websocket"))
				{
					CELLLog_Error("WebSocketClientC::handshake, not found Upgrade:websocket");
					return false;
				}

				auto sKeyAccept = pWSClient->header_getStr("Sec-WebSocket-Accept", nullptr);
				if (!sKeyAccept)
				{
					CELLLog_Error("WebSocketClientC::handshake, not found Sec-WebSocket-Key");
					return false;
				}

				std::string cKeyAccept = _cKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

				unsigned char strSha1[20] = {};
				SHA1_String((const unsigned char*)cKeyAccept.c_str(), cKeyAccept.length(), strSha1);

				cKeyAccept = Base64Encode(strSha1, 20);

				if (cKeyAccept != sKeyAccept)
				{
					CELLLog_Error("WebSocketClientC::handshake, cKeyAccept != sKeyAccept!");
					return false;
				}
				return true;
			}

			void connect(const char* httpurl)
			{
				deatch_http_url(httpurl);
				if (0 == hostname2ip(_host.c_str(), _port.c_str()))
				{
					_pClient->state(clientState_join);

					_cKey = "1234567890123456";
					_cKey = Base64Encode((const unsigned char*)_cKey.c_str(), _cKey.length());

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
				if (isRun())
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
						if (pAddr->ai_family == AF_INET6)
							Log::Info("%s ipv6: %s", hostname, ipStr);
						else if (pAddr->ai_family == AF_INET)
							Log::Info("%s ipv4: %s", hostname, ipStr);
						else {
							Log::Info("%s addr: %s", hostname, ipStr);
							continue;
						}

						if (connet2ip(pAddr->ai_family, ipStr, port_))
						{
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

				if (path && strlen(path) > 0)
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

				//msg += "Connection: keep-alive\r\n";
				msg += "Accept: */*\r\n";

				msg += "Origin: ";
				msg += host;
				msg += "\r\n";
				////
				msg += "Connection: Upgrade\r\n";
				msg += "Upgrade: websocket\r\n";
				msg += "Sec-WebSocket-Version: 13\r\n";

				msg += "Sec-WebSocket-Key: ";
				msg += _cKey;
				msg += "\r\n";
				////

				msg += "\r\n";

				SendData(msg.c_str(), msg.length());
			}

			bool connet2ip(int af, const char* ip, unsigned short port)
			{
				if (!ip)
					return false;

				if (INVALID_SOCKET == InitSocket(af, 202400, 202400))
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
			////
			std::string _cKey;
		};
	}
}
#endif // !_doyou_io_TcpWebSocketClient_HPP_
