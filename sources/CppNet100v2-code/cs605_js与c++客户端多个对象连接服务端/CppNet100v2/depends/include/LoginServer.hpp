#ifndef _doyou_io_LoginServer_HPP_
#define _doyou_io_LoginServer_HPP_

#include"INetClient.hpp"

namespace doyou {
	namespace io {
		class LoginServer
		{
		private:
			INetClient _csGate;
		public:
			void Init()
			{
				auto csGateUrl = Config::Instance().getStr("csGateUrl", "ws://127.0.0.1:4567");
				_csGate.connect("csGate", csGateUrl);

				_csGate.reg_msg_call("onopen", std::bind(&LoginServer::onopen_csGate, this, std::placeholders::_1, std::placeholders::_2));

				_csGate.reg_msg_call("cs_msg_login", std::bind(&LoginServer::cs_msg_login, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("cs_msg_register", std::bind(&LoginServer::cs_msg_register, this, std::placeholders::_1, std::placeholders::_2));
			}

			void Run()
			{
				_csGate.run(1);
			}

			void Close()
			{
				_csGate.close();
			}

		private:
			void onopen_csGate(INetClient* client, neb::CJsonObject& msg)
			{
				neb::CJsonObject json;
				json.Add("type", "LoginServer");
				json.Add("name", "LoginServer001");
				json.Add("sskey", "ssmm00@123456");
				json.AddEmptySubArray("apis");
				json["apis"].Add("cs_msg_login");
				json["apis"].Add("cs_msg_register");
				json["apis"].Add("cs_msg_change_pw");

				client->request("ss_reg_api", json, [](INetClient* client, neb::CJsonObject& msg) {
					CELLLog_Info(msg("data").c_str());
				});
			}

			void cs_msg_login(INetClient* client, neb::CJsonObject& msg)
			{
				CELLLog_Info("LoginServer::cs_msg_login");

				neb::CJsonObject ret;
				ret.Add("data", "login successs.");
				client->response(msg, ret);
			}

			void cs_msg_register(INetClient* client, neb::CJsonObject& msg)
			{
				int msgId = 0;
				if (!msg.Get("msgId", msgId))
				{
					CELLLog_Error("not found key<%s>.", "msgId");
					return;
				}

				std::string username;
				if (!msg["data"].Get("username", username))
				{
					CELLLog_Error("not found key<%s>.", "username");
					return;
				}

				std::string password = msg["data"]("password");

				CELLLog_Info(
					"LoginServer::cs_msg_register: msgId=%d username=%s password=%s",
					msgId , username.c_str(), password.c_str());

				neb::CJsonObject ret;
				ret.Add("data", "register successs.");
				client->response(msg, ret);
			}
			
		};
	}
}
#endif // !_doyou_io_LoginServer_HPP_
