#ifndef _doyou_io_WebSocketClientS_HPP_
#define _doyou_io_WebSocketClientS_HPP_

#include"HttpClientS.hpp"
#include"sha1.hpp"
#include"base64.hpp"

namespace doyou {
	namespace io {

		enum WebSocketOpcode {
			opcode_CONTINUATION = 0x0,
			opcode_TEXT = 0x1,
			opcode_BINARY = 0x2,
			opcode_CLOSE = 0x8,
			opcode_PING = 0x9,
			opcode_PONG = 0xA,
		};

		//WebSocket数据帧头部协议格式
		struct WebSocketHeader {
			//Extended payload length 16或64位
			uint64_t len;
			//操作代码，Opcode 的值决定了应该如何解析后续的 数据载荷（data payload）
			//如果操作代码是不认识的，那么接收端应该 断开连接（fail the connection）
			WebSocketOpcode opcode;
			//所有从客户端传送到服务端的数据帧，数据载荷都进行了掩码操作，
			//Mask 为 1，且携带了 4 字节的 Masking-key。
			//如果 Mask 为 0，则没有 Masking-key
			//备注：载荷数据的长度len，不包括 mask key 的长度。
			uint8_t masking_key[4];
			//如果值为true，表示这是 消息（message）的最后一个分片（fragment）
			//如果是 0，表示不是 消息（message）的最后一个 分片（fragment）
			bool fin;
			//表示是否要对数据（消息体）进行掩码操作
			//从客户端向服务端发送数据时，需要对数据进行掩码操作；
			//从服务端向客户端发送数据时，不需要对数据进行掩码操作。
			//如果服务端接收到的数据没有进行过掩码操作，服务端需要断开连接。
			bool mask;
			//数据载荷的长度,如果值为x
			//x 为 0~126：数据的长度为 x 字节。
			//x 为 126：后续 2 个字节代表一个 16 位的无符号整数，该无符号整数的值为数据的长度。
			//x 为 127：后续 8 个字节代表一个 64 位的无符号整数（最高位为 0），该无符号整数的值为数据的长度。
			//此外，如果 payload length 占用了多个字节的话，payload length 的二进制表达采用 网络序（big endian，重要的位在前）。
			uint8_t len0;
			//WebSocket数据帧头部字节长度
			uint8_t header_size;
		};

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
