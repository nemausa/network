#ifndef _doyou_io_INetClientS_HPP_
#define _doyou_io_INetClientS_HPP_

#include"WebSocketClientS.hpp"
#include"CJsonObject.hpp"
#include"INetStateCode.hpp"

namespace doyou {
	namespace io {

		//客户端数据类型
		class INetClientS :public WebSocketClientS
		{
		private:
			std::string _link_name;
			std::string _link_type = "client";
			bool _is_ss_link = false;
			bool _is_cc_link = false;
			//
			std::string _token;
			int64_t _userId = 0;
		public:
			INetClientS(SOCKET sockfd = INVALID_SOCKET, int sendSize = SEND_BUFF_SZIE, int recvSize = RECV_BUFF_SZIE) :
				WebSocketClientS(sockfd, sendSize, recvSize)
			{

			}

			const std::string& link_name()
			{
				return _link_name;
			}

			void link_name(const std::string& str)
			{
				_link_name = str;
			}

			const std::string& link_type()
			{
				return _link_type;
			}

			void link_type(const std::string& str)
			{
				_link_type = str;
			}

			bool is_ss_link()
			{
				return _is_ss_link;
			}

			void is_ss_link(bool b)
			{
				_is_ss_link = b;
			}

			bool is_cc_link()
			{
				return _is_cc_link;
			}

			void is_cc_link(bool b)
			{
				_is_cc_link = b;
			}

			const std::string& token()
			{
				return _token;
			}

			void token(const std::string& str)
			{
				_token = str;
			}

			int clientId()
			{
				return (int)this->sockfd();
			}

			int64_t userId()
			{
				return _userId;
			}

			void userId(int64_t n)
			{
				_userId = n;
			}

			bool is_login()
			{
				return _userId != 0;
			}

			bool transfer(neb::CJsonObject& msg)
			{
				std::string retStr = msg.ToString();
				if (SOCKET_ERROR == this->writeText(retStr.c_str(), retStr.length()))
				{
					CELLLog_Error("INetClientS::transfer::writeText SOCKET_ERROR.");
					return false;
				}
				return true;
			}

			template<typename vT>
			void response(neb::CJsonObject& msg, const vT& data, int state = state_code_ok)
			{
				int msgId = 0;
				if (!msg.Get("msgId", msgId))
				{
					CELLLog_Error("not found key<msgId>.");
					return;
				}

				neb::CJsonObject ret;
				ret.Add("state", state);
				ret.Add("msgId", msgId);
				ret.Add("type", msg_type_resp);
				ret.Add("time", Time::system_clock_now());
				ret.Add("data", data);

				transfer(ret);
			}

			template<typename vT>
			void resp_error(neb::CJsonObject& msg, const vT& data, int state = state_code_error)
			{
				response(msg, data, state);
			}
		};
	}
}
#endif // !_doyou_io_INetClientS_HPP_
