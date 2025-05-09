#ifndef _doyou_io_LinkServer_HPP_
#define _doyou_io_LinkServer_HPP_

#include"INetServer.hpp"
#include"INetClient.hpp"

namespace doyou {
	namespace io {
		class LinkServer
		{
		private:
			INetServer _netserver;
			INetClient _ss_gate;
			std::string _myurl;
		public:
			void Init()
			{
				auto ssGateUrl = Config::Instance().getStr("ssGateUrl", "ws://127.0.0.1:4567");
				_ss_gate.connect("ssGate", ssGateUrl);
				_ss_gate.on_other_push = std::bind(&LinkServer::on_other_push, this, std::placeholders::_1, std::placeholders::_2);

				_ss_gate.reg_msg_call("onopen", std::bind(&LinkServer::onopen_csGate, this, std::placeholders::_1, std::placeholders::_2));
				_ss_gate.reg_msg_call("ss_msg_user_logout", std::bind(&LinkServer::ss_msg_user_logout, this, std::placeholders::_1, std::placeholders::_2));
				_ss_gate.reg_msg_call("ss_msg_user_login", std::bind(&LinkServer::ss_msg_user_login, this, std::placeholders::_1, std::placeholders::_2));
				_ss_gate.reg_msg_call("cs_get_run_gate", std::bind(&LinkServer::cs_get_run_gate, this, std::placeholders::_1, std::placeholders::_2));

				_myurl = Config::Instance().getStr("myurl", "ws://127.0.0.1:5000");
				const char* strIP = Config::Instance().getStr("strIP", "any");
				uint16_t nPort = Config::Instance().getInt("nPort", 5000);
				_netserver.Init(strIP, nPort);

				_netserver.on_other_msg = std::bind(&LinkServer::on_other_msg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
				_netserver.on_client_leave = std::bind(&LinkServer::on_client_leave, this, std::placeholders::_1);
				_netserver.on_net_run = std::bind(&LinkServer::on_net_run, this, std::placeholders::_1);

				_netserver.reg_msg_call("cs_msg_heart", std::bind(&LinkServer::cs_msg_heart, this,std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				_netserver.reg_msg_call("cs_reg_client", std::bind(&LinkServer::cs_reg_client, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				
				
				
			}

			void Close()
			{
				_netserver.Close();
			}

		private:
			void onopen_csGate(INetClient* client, neb::CJsonObject& msg)
			{
				neb::CJsonObject json;
				json.Add("type", "LinkServer");
				json.Add("name", "LinkServer001");
				json.Add("sskey", "ssmm00@123456");
				json.AddEmptySubArray("apis");
				json["apis"].Add("ss_msg_user_logout");
				json["apis"].Add("ss_msg_user_login");
				json["apis"].Add("cs_get_run_gate");

				client->request("ss_reg_server", json, [](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { CELLLog_Error("ss_reg_server error <state=%d> msg: %s", state, msg("data").c_str()); return; }

					int clientId = 0;
					if (!msg["data"].Get("clientId", clientId))
					{
						CELLLog_Error("ss_reg_server: not found key<clientId>.");
						return;
					}

					client->clientId(clientId);
					CELLLog_Info("ss_reg_server: clientId=%d", clientId);
				});
			}

			void on_other_push(INetClient* client, neb::CJsonObject& msg)
			{
				int clientId = 0;
				if (!msg.Get("clientId", clientId))
				{
					CELLLog_Error("LinkServer::on_other_push::not found key<clientId>.");
					return;
				}

				clientId = ClientId::get_client_id(clientId);
				auto clientS = dynamic_cast<INetClientS*>(_netserver.find_client(clientId));
				if (!clientS)
				{
					CELLLog_Error("LinkServer::on_other_push::find_client(%d) miss.", clientId);
					return;
				}

				clientS->transfer(msg);
			}

			void cs_get_run_gate(INetClient* client, neb::CJsonObject& msg)
			{
				client->response(msg, _myurl);
			}

			void cs_msg_heart(Server* server, INetClientS* client, neb::CJsonObject& msg)
			{
				if (client->is_cc_link() || client->is_ss_link())
				{
					//CELLLog_Info("LinkServer::cs_msg_heart");

					//neb::CJsonObject ret;
					//ret.Add("data", "wo ye bu ji dao.");
					//client->response(msg, ret);

					//client->respone(msg, "wo ye bu ji dao.");
				}
			}

			void cs_reg_client(Server* server, INetClientS* client, neb::CJsonObject& msg)
			{
				auto cckey = msg["data"]("cckey");
				auto cckey_local = Config::Instance().getStr("cckey", "ccmm00@123456");
				if (cckey != cckey_local)
				{
					client->resp_error(msg, "cckey error.");
					return;
				}
				auto type = msg["data"]("type");

				client->link_type(type);
				client->link_name(type);
				client->is_cc_link(true);

				client->response(msg, "cs_reg_client ok!");
			}

			void ss_msg_user_login(INetClient* client, neb::CJsonObject& msg)
			{
				int clientId = 0;
				if (!msg["data"].Get("clientId", clientId))
				{
					CELLLog_Error("not found key<clientId>.");
					return;
				}

				int64_t userId = 0;
				if (!msg["data"].Get("userId", userId))
				{
					CELLLog_Error("not found key<userId>.");
					return;
				}

				std::string token;
				if (!msg["data"].Get("token", token))
				{
					CELLLog_Error("not found key<token>.");
					return;
				}

				clientId = ClientId::get_client_id(clientId);
				auto clientS = dynamic_cast<INetClientS*>(_netserver.find_client(clientId));
				if (!clientS)
				{
					CELLLog_Error("LinkServer::ss_msg_user_login::server->find_client(%d) miss.", clientId);
					return;
				}

				clientS->userId(userId);
				clientS->token(token);
				CELLLog_Info("LinkServer::ss_msg_user_login::clientId<%d>userId<%lld>.", clientId, userId);
			}

			void ss_msg_user_logout(INetClient* client, neb::CJsonObject& msg)
			{
				int clientId = 0;
				if (!msg["data"].Get("clientId", clientId))
				{
					CELLLog_Error("not found key<clientId>.");
					return;
				}

				int64_t userId = 0;
				if (!msg["data"].Get("userId", userId))
				{
					CELLLog_Error("not found key<userId>.");
					return;
				}

				std::string token;
				if (!msg["data"].Get("token", token))
				{
					CELLLog_Error("not found key<token>.");
					return;
				}

				clientId = ClientId::get_client_id(clientId);
				auto clientS = dynamic_cast<INetClientS*>(_netserver.find_client(clientId));
				if (!clientS)
				{
					CELLLog_Error("LinkServer::ss_msg_user_logout::server->find_client(%d) miss.", clientId);
					return;
				}

				if (userId != clientS->userId())
				{
					CELLLog_Error("LinkServer::ss_msg_user_logout::userId<%lld> != clientS->userId<%lld>.", userId, clientS->userId());
					return;
				}

				clientS->userId(0);
				clientS->token("");
				CELLLog_Info("LinkServer::ss_msg_user_logout::clientId<%d>userId<%lld>.", clientId, userId);
			}

			void on_other_msg(Server* server, INetClientS* client, std::string& cmd, neb::CJsonObject& msg)
			{
				int clientId = 0;
				if (!msg.Get("clientId", clientId))
				{
					CELLLog_Error("LinkServer::on_other_msg::not found key<clientId>.");
					return;
				}

				int msgId = 0;
				if (!msg.Get("msgId", msgId))
				{
					CELLLog_Error("LinkServer::on_other_msg::not found key<msgId>.");
					return;
				}

				//表示从客户端发来的请求
				msg.Add("is_cc", true, true);

				_ss_gate.request(msg, [this, clientId, msgId](INetClient* client, neb::CJsonObject& msg) {
					auto clientS = dynamic_cast<INetClientS*>(_netserver.find_client(clientId));
					if (!clientS)
					{
						CELLLog_Error("LinkServer::on_other_msg::_netserver.find_client(%d) miss.", clientId);
						return;
					}

					msg.Replace("msgId", msgId);
					msg.Delete("clientId");

					clientS->transfer(msg);
				});
			}

			void on_client_leave(INetClientS* client)
			{
				if (client->is_login())
				{
					int clientId = ClientId::set_link_id(_ss_gate.clientId(), client->clientId());
					neb::CJsonObject msg;
					msg.Add("clientId", clientId);
					msg.Add("userId", client->userId());
					_ss_gate.broadcast("ss_msg_user_exit", msg);
				}

				{
					int clientId = ClientId::set_link_id(_ss_gate.clientId(), client->clientId());
					neb::CJsonObject msg;
					msg.Add("clientId", clientId);
					_ss_gate.broadcast("ss_msg_client_exit", msg);
				}
			}

			void on_net_run(Server* pServer)
			{
				_ss_gate.run(0);
			}
		};
	}
}
#endif // !_doyou_io_LinkServer_HPP_
