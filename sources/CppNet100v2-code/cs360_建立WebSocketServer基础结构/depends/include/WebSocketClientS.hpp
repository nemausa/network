#ifndef _doyou_io_WebSocketClientS_HPP_
#define _doyou_io_WebSocketClientS_HPP_

#include"HttpClientS.hpp"
#include"sha1.hpp"
#include"base64.hpp"

namespace doyou {
	namespace io {
		//客户端数据类型
		class WebSocketClientS :public HttpClientS
		{
		public:
			WebSocketClientS(SOCKET sockfd = INVALID_SOCKET, int sendSize = SEND_BUFF_SZIE, int recvSize = RECV_BUFF_SZIE) :
				HttpClientS(sockfd, sendSize, recvSize)
			{

			}

			bool handshake()
			{
				auto strUpgrade = this->header_getStr("Upgrade", "");
				if (0 != strcmp(strUpgrade, "websocket"))
				{
					CELLLog_Error("WebSocketClientS::handshake, not found Upgrade:websocket");
					return false;
				}

				auto cKey = this->header_getStr("Sec-WebSocket-Key", nullptr);
				if (!cKey)
				{
					CELLLog_Error("WebSocketClientS::handshake, not found Sec-WebSocket-Key");
					return false;
				}

				std::string sKey = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

				sKey = cKey + sKey;

				unsigned char strSha1[20] = {};
				SHA1_String((const unsigned char*)sKey.c_str(), sKey.length(), strSha1);

				std::string sKeyAccept = Base64Encode(strSha1, 20);


				char resp[256] = {};
				strcat(resp, "HTTP/1.1 101 Switching Protocols\r\n");
				strcat(resp, "Connection: Upgrade\r\n");
				strcat(resp, "Upgrade: websocket\r\n");
				strcat(resp, "Sec-WebSocket-Accept: ");
				strcat(resp, sKeyAccept.c_str());
				strcat(resp, "\r\n\r\n");

				this->SendData(resp, strlen(resp));

				return true;
			}
		};
	}
}
#endif // !_doyou_io_WebSocketClientS_HPP_
