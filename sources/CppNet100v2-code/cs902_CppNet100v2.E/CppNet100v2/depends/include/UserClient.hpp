#ifndef _doyou_io_UserClient_HPP_
#define _doyou_io_UserClient_HPP_

#include"INetClient.hpp"
#include"TaskTimer.hpp"

namespace doyou {
	namespace io {
		class UserClient
		{
		public:
			static std::string s_csGateUrl;
			static std::vector<std::string> s_text_arr;
			static int s_nSendSleep;
		private:
			TaskTimer _taskTimer;
			INetClient _csGate;
			int64_t _userId = 0;
			std::string _token;
			bool _is_change_gate = false;

			int _test_id = 0;
			std::string _username;
			std::string _password;
			std::string _nickname;

			//每个会话组多少人
			int _group_user_max = 3;
			//会话组的id和key
			int _group_id = 0;
			int _group_key = 0;
			int _nText = 0;
		public:
			void Init(int test_id)
			{
				_test_id = test_id;
				_username = "testaa"+std::to_string(test_id);
				_password = "testmm" + std::to_string(test_id);
				_nickname = "nname" + std::to_string(test_id);

				//int s_size = Config::Instance().getInt("nSendBuffSize", SEND_BUFF_SZIE);
				//int r_size = Config::Instance().getInt("nRecvBuffSize", RECV_BUFF_SZIE);

				_csGate.connect("csGate", s_csGateUrl.c_str(), SEND_BUFF_SZIE, RECV_BUFF_SZIE);

				_csGate.reg_msg_call("onopen", std::bind(&UserClient::onopen_csGate, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("onclose", std::bind(&UserClient::onclose_csGate, this, std::placeholders::_1, std::placeholders::_2));

				_csGate.reg_msg_call("sc_msg_logout", std::bind(&UserClient::sc_msg_logout, this, std::placeholders::_1, std::placeholders::_2));

				_csGate.reg_msg_call("sc_msg_group_join", std::bind(&UserClient::sc_msg_group_join, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("sc_msg_group_exit", std::bind(&UserClient::sc_msg_group_exit, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("sc_msg_group_say", std::bind(&UserClient::sc_msg_group_say, this, std::placeholders::_1, std::placeholders::_2));
			}

			void Run()
			{
				_csGate.run(0);
				_taskTimer.OnRun();
			}

			void Close()
			{
				_csGate.close();
			}

		private:
			void onopen_csGate(INetClient* client, neb::CJsonObject& msg)
			{
				reg_client();
			}

			void onclose_csGate(INetClient* client, neb::CJsonObject& msg)
			{

			}

			void reg_client()
			{
				neb::CJsonObject json;
				json.Add("type", "Client");
				json.Add("cckey", "ccmm00@123456");
				if (!_token.empty())
					json.Add("token", _token);

				_csGate.request("cs_reg_client", json, [this](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { CELLLog_Error("cs_reg_client error <state=%d> msg: %s", state, msg("data").c_str()); return; }

					//CELLLog_Info(msg("data").c_str());

					if (!_is_change_gate)
					{
						_is_change_gate = true;
						login();
					}
					else {
						login_by_token();
					}
				});
			}

			void reg_user()
			{
				neb::CJsonObject json;
				json.Add("username", _username);
				json.Add("password", _password);
				json.Add("nickname", _nickname);
				json.Add("sex", _test_id%2);

				_csGate.request("cs_msg_register", json, [this](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { CELLLog_Error("cs_msg_register error <state=%d> msg: %s", state, msg("data").c_str()); return; }


					if (!msg["data"].Get("userId", _userId))
					{
						CELLLog_Error("not found key <userId>");
						return;
					}

					//CELLLog_Info("reg_user: userId=%lld", _userId);
					login();
				});
			}

			void login()
			{
				neb::CJsonObject json;
				json.Add("username", _username);
				json.Add("password", _password);

				_csGate.request("cs_msg_login", json, [this](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { 
						CELLLog_Error("cs_msg_login error <state=%d> msg: %s", state, msg("data").c_str()); 
						reg_user();
						return; 
					}

					
					if (!msg["data"].Get("userId", _userId))
					{
						CELLLog_Error("not found key <userId>");
						return;
					}

					if (!msg["data"].Get("token", _token))
					{
						CELLLog_Error("not found key <token>");
						return;
					}

					//CELLLog_Info("login: userId=%lld, token=%s", _userId, _token.c_str());
					change_run_gate();
				});
			}

			void login_by_token()
			{
				neb::CJsonObject json;
				json.Add("token", _token);

				_csGate.request("cs_msg_login_by_token", json, [this](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { CELLLog_Error("cs_msg_login_by_token error <state=%d> msg: %s", state, msg("data").c_str()); return; }


					if (!msg["data"].Get("userId", _userId))
					{
						CELLLog_Error("not found key <userId>");
						return;
					}

					if (!msg["data"].Get("token", _token))
					{
						CELLLog_Error("not found key <token>");
						return;
					}

					//CELLLog_Info("login_by_token: userId=%lld, token=%s", _userId, _token.c_str());

					test_group();
				});
			}

			void sc_msg_logout(INetClient* client, neb::CJsonObject& msg)
			{
				CELLLog_Info("logout: %s", msg("data").c_str());
				_userId = 0;
				_token.clear();
			}

			void change_run_gate()
			{
				neb::CJsonObject json;
				//json.Add("token", _token);
				//1.获取新业务网关的地址（LinkServer/LoginGate/RunGate）
				_csGate.request("cs_get_run_gate", json,[this](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { CELLLog_Error("cs_get_run_gate error <state=%d> msg: %s", state, msg("data").c_str()); return; }

					std::string gate_url = msg("data");
					//CELLLog_Info("run_gate_url: %s", gate_url.c_str());

					//2.关闭原网络连接
					_csGate.to_close();

					//3.连接新业务网关LinkServer
					_csGate.connect("csGate", gate_url.c_str(), SEND_BUFF_SZIE, RECV_BUFF_SZIE);
				});
			}

			void test_group()
			{
				//每个会话组多少人
				_group_user_max = 10;
				//会话组的id和key
				_group_id = 20000 + (_test_id / _group_user_max);
				_group_key = 10000 + (_test_id / _group_user_max);
				//会话组的创建者
				if (_test_id%_group_user_max == 0)
				{
					_taskTimer.add_task_1s(0, 100, [this]() {
						group_create();
					});
				}
				else {//会话组的参与者
					_taskTimer.add_task_1s(0, 2000, [this]() {
						group_join();
					});
				}

				_nText = rand() % s_text_arr.size();

				

				int nd = (_test_id % _group_user_max);
				nd = 4000 + (nd * 100);

				_taskTimer.add_task_1s(0, nd, [this]() {
					_taskTimer.add_task(0, s_nSendSleep, [this]() {
						group_say();
					});
				});
			}

			void group_create()
			{
				neb::CJsonObject json;
				json.Add("group_id", _group_id);
				json.Add("group_key", _group_key);

				//CELLLog_Info("group_create1: test_id=%d, userId=%lld, group_id=%d, group_key=%d", _test_id, _userId, _group_id, _group_key);
				_csGate.request("cs_msg_group_create", json, [this](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { CELLLog_Error("cs_msg_group_create error <state=%d> msg: %s", state, msg("data").c_str()); return; }
					
					int g_id = 0;
					if (!msg["data"].Get("group_id", g_id))
					{
						CELLLog_Error("not found key <group_id>");
						return;
					}

					int g_key = 0;
					if (!msg["data"].Get("group_key", g_key))
					{
						CELLLog_Error("not found key <group_key>");
						return;
					}

					//CELLLog_Info("group_create2: test_id=%d, userId=%lld, group_id=%d, group_key=%d", _test_id, _userId, g_id, g_key);
				});
			}

			void group_join()
			{
				neb::CJsonObject json;
				json.Add("group_id", _group_id);
				json.Add("group_key", _group_key);

				//CELLLog_Info("group_join1: test_id=%d, userId=%lld, group_id=%d, group_key=%d", _test_id, _userId, _group_id, _group_key);
				_csGate.request("cs_msg_group_join", json, [this](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { CELLLog_Error("cs_msg_group_join error <state=%d> msg: %s", state, msg("data").c_str()); return; }

					int g_id = 0;
					if (!msg["data"].Get("group_id", g_id))
					{
						CELLLog_Error("not found key <group_id>");
						return;
					}

					int g_key = 0;
					if (!msg["data"].Get("group_key", g_key))
					{
						CELLLog_Error("not found key <group_key>");
						return;
					}

					//CELLLog_Info("group_join2: test_id=%d, userId=%lld, group_id=%d, group_key=%d", _test_id, _userId, g_id, g_key);
				});
			}

			void group_say()
			{
				neb::CJsonObject json;
				json.Add("group_id", _group_id);
				json.Add("group_key", _group_key);

				
				if (++_nText >= s_text_arr.size())
					_nText = 0;
				_nText = 23;
				json.Add("say", s_text_arr.at(_nText));

				_csGate.request("cs_msg_group_say", json, [this](INetClient* client, neb::CJsonObject& msg) {
					int state = state_code_ok;
					if (!msg.Get("state", state)) { CELLLog_Error("not found key <state>"); return; }
					if (state != state_code_ok) { CELLLog_Error("cs_msg_group_say error <state=%d> msg: %s", state, msg("data").c_str()); return; }

				});
			}

			void group_exit()
			{

			}

			void sc_msg_group_join(INetClient* client, neb::CJsonObject& msg)
			{
				int g_id = 0;
				if (!msg["data"].Get("group_id", g_id))
				{
					CELLLog_Error("not found key <group_id>");
					return;
				}

				int clientId = 0;
				if (!msg["data"].Get("clientId", clientId))
				{
					CELLLog_Error("not found key <clientId>");
					return;
				}

				//CELLLog_Info("test_id=%d, userId=%lld >> sc_msg_group_join: group_id=%d, clientId=%d", _test_id, _userId, g_id, clientId);
			}

			void sc_msg_group_exit(INetClient* client, neb::CJsonObject& msg)
			{
				int g_id = 0;
				if (!msg["data"].Get("group_id", g_id))
				{
					CELLLog_Error("not found key <group_id>");
					return;
				}

				int clientId = 0;
				if (!msg["data"].Get("clientId", clientId))
				{
					CELLLog_Error("not found key <clientId>");
					return;
				}

				//CELLLog_Info("test_id=%d, userId=%lld >> sc_msg_group_exit: group_id=%d, clientId=%d", _test_id, _userId, g_id, clientId);
			}

			void sc_msg_group_say(INetClient* client, neb::CJsonObject& msg)
			{
				int g_id = 0;
				if (!msg["data"].Get("group_id", g_id))
				{
					CELLLog_Error("not found key <group_id>");
					return;
				}

				int clientId = 0;
				if (!msg["data"].Get("clientId", clientId))
				{
					CELLLog_Error("not found key <clientId>");
					return;
				}

				std::string say;
				if (!msg["data"].Get("say", say))
				{
					CELLLog_Error("not found key <say>");
					return;
				}

				//CELLLog_Info("test_id=%d, userId=%lld >> sc_msg_group_say: group_id=%d, clientId=%d, say=%s", _test_id, _userId, g_id, clientId, say.c_str());
			}
		};
	}
}
#endif // !_doyou_io_UserClient_HPP_
