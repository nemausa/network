#ifndef _doyou_io_LoginServer_HPP_
#define _doyou_io_LoginServer_HPP_

#include<regex>

#include"INetClient.hpp"
#include"DBUser.hpp"

namespace doyou {
	namespace io {
		class LoginServer
		{
		private:
			INetClient _csGate;
			DBUser _dbuser;
		public:
			void Init()
			{
				_dbuser.init();

				auto csGateUrl = Config::Instance().getStr("csGateUrl", "ws://127.0.0.1:4567");
				_csGate.connect("csGate", csGateUrl);

				_csGate.reg_msg_call("onopen", std::bind(&LoginServer::onopen_csGate, this, std::placeholders::_1, std::placeholders::_2));

				_csGate.reg_msg_call("cs_msg_login", std::bind(&LoginServer::cs_msg_login, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("cs_msg_register", std::bind(&LoginServer::cs_msg_register, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("cs_msg_change_pw", std::bind(&LoginServer::cs_msg_change_pw, this, std::placeholders::_1, std::placeholders::_2));
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
				int clientId = 0;
				if (!msg.Get("clientId", clientId))
				{
					CELLLog_Error("not found key<%s>.", "clientId");
					return;
				}

				int msgId = 0;
				if (!msg.Get("msgId", msgId))
				{
					CELLLog_Error("not found key<%s>.", "msgId");
					return;
				}

				client->response(clientId, msgId, "login successs.");
			}

			void cs_msg_register(INetClient* client, neb::CJsonObject& msg)
			{
				//通用基础字段获取与验证
				int clientId = 0;
				if (!msg.Get("clientId", clientId))
				{
					CELLLog_Error("not found key<%s>.", "clientId");
					return;
				}

				int msgId = 0;
				if (!msg.Get("msgId", msgId))
				{
					CELLLog_Error("not found key<%s>.", "msgId");
					return;
				}

				//当前请求字段获取与验证
				std::string username;
				std::string password;
				std::string nickname;
				int sex = -1;
				{
					if (!msg["data"].Get("username", username))
					{
						client->resp_error(clientId, msgId, "not found key <username>.");
						return;
					}

					if (username.empty())
					{
						client->resp_error(clientId, msgId, "<username> can not be empty!");
						return;
					}
					//正则表达式
					std::regex reg1("^[0-9a-zA-Z]{6,16}$");
					if (!regex_match(username, reg1))
					{
						client->resp_error(clientId, msgId, "<username> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("password", password))
					{
						client->resp_error(clientId, msgId, "not found key<password>.");
						return;
					}

					if (password.empty())
					{
						client->resp_error(clientId, msgId, "<password> can not be empty!");
						return;
					}

					//正则表达式
					if (!regex_match(password, reg1))
					{
						client->resp_error(clientId, msgId, "<password> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("nickname", nickname))
					{
						client->resp_error(clientId, msgId, "not found key<nickname>.");
						return;
					}

					if (nickname.empty())
					{
						client->resp_error(clientId, msgId, "<nickname> can not be empty!");
						return;
					}

					if (nickname.length() <3 || nickname.length() > 16)
					{
						client->resp_error(clientId, msgId, "<nickname> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("sex", sex))
					{
						client->resp_error(clientId, msgId, "not found key<sex>.");
						return;
					}

					if (sex !=0 && sex != 1)
					{
						client->resp_error(clientId, msgId, "<sex> is only 0 or 1!");
						return;
					}
				}
				//
				CELLLog_Info("LoginServer::cs_msg_register: msgId=%d username=%s password=%s",msgId , username.c_str(), password.c_str());
				//判断用户名是否已存在
				if (_dbuser.has_username(username))
				{
					client->resp_error(clientId, msgId, "username already exists");
					return;
				}
				//判断昵称是否已存在
				if (_dbuser.has_nickname(nickname))
				{
					client->resp_error(clientId, msgId, "nickname already exists");
					return;
				}
				//新增用户数据
				auto userId = _dbuser.add_user(username, password, nickname, sex);
				if (userId > 0)
				{
					neb::CJsonObject ret;
					ret.Add("userId", userId);
					client->response(clientId, msgId, ret);
				}
				else {
					client->resp_error(clientId, msgId, "unkown error.");
				}
			}

			void cs_msg_change_pw(INetClient* client, neb::CJsonObject& msg)
			{
				//通用基础字段获取与验证
				int clientId = 0;
				if (!msg.Get("clientId", clientId))
				{
					CELLLog_Error("not found key<%s>.", "clientId");
					return;
				}

				int msgId = 0;
				if (!msg.Get("msgId", msgId))
				{
					CELLLog_Error("not found key<%s>.", "msgId");
					return;
				}

				//当前请求字段获取与验证
				std::string username;
				std::string password_old;
				std::string password_new;
				{
					if (!msg["data"].Get("username", username))
					{
						client->resp_error(clientId, msgId, "not found key <username>.");
						return;
					}

					if (username.empty())
					{
						client->resp_error(clientId, msgId, "<username> can not be empty!");
						return;
					}
					//正则表达式
					std::regex reg1("^[0-9a-zA-Z]{6,16}$");
					if (!regex_match(username, reg1))
					{
						client->resp_error(clientId, msgId, "<username> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("password_old", password_old))
					{
						client->resp_error(clientId, msgId, "not found key<password_old>.");
						return;
					}

					if (password_old.empty())
					{
						client->resp_error(clientId, msgId, "<password_old> can not be empty!");
						return;
					}

					//正则表达式
					if (!regex_match(password_old, reg1))
					{
						client->resp_error(clientId, msgId, "<password_old> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("password_new", password_new))
					{
						client->resp_error(clientId, msgId, "not found key<password_new>.");
						return;
					}

					if (password_new.empty())
					{
						client->resp_error(clientId, msgId, "<password_new> can not be empty!");
						return;
					}

					//正则表达式
					if (!regex_match(password_new, reg1))
					{
						client->resp_error(clientId, msgId, "<password_new> format is incorrect!");
						return;
					}
				}
				//
				CELLLog_Info("LoginServer::cs_msg_change_pw: msgId=%d username=%s password_old=%s password_new=%s", msgId, username.c_str(), password_old.c_str(), password_new.c_str());
				
				//获取用户数据
				neb::CJsonObject users;
				_dbuser.findByKV("user_info", "username", username.c_str(), users);
				if (users.GetArraySize() < 1)
				{
					client->resp_error(clientId, msgId, "<username> is incorrect!");
					return;
				}
				//比较当前密码是否正确
				auto password_now = users[0]("password");
				if (password_now != password_old)
				{
					client->resp_error(clientId, msgId, "<password> is incorrect!");
					return;
				}
				//更新用户密码
				int changes = _dbuser.updateByKV("user_info", "username", username.c_str(), "password",password_new.c_str());
				if (changes == 1)
				{
					client->response(clientId, msgId, "change password success.");
				}
				else {
					client->resp_error(clientId, msgId, "unkown error.");
				}
			}

		};
	}
}
#endif // !_doyou_io_LoginServer_HPP_
