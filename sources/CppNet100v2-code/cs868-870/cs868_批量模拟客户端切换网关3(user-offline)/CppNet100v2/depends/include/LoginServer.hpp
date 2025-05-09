#ifndef _doyou_io_LoginServer_HPP_
#define _doyou_io_LoginServer_HPP_

#include<regex>

#include"INetClient.hpp"
#include"DBUser.hpp"
#include"UserManager.hpp"

namespace doyou {
	namespace io {
		class LoginServer
		{
		private:
			INetClient _csGate;
			DBUser _dbuser;
			UserManager _userManager;
		public:
			void Init()
			{
				_dbuser.init();

				auto csGateUrl = Config::Instance().getStr("csGateUrl", "ws://127.0.0.1:4567");
				_csGate.connect("csGate", csGateUrl);

				_csGate.reg_msg_call("onopen", std::bind(&LoginServer::onopen_csGate, this, std::placeholders::_1, std::placeholders::_2));

				_csGate.reg_msg_call("ss_msg_client_exit", std::bind(&LoginServer::ss_msg_client_exit, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("ss_msg_user_exit", std::bind(&LoginServer::ss_msg_user_exit, this, std::placeholders::_1, std::placeholders::_2));

				_csGate.reg_msg_call("cs_msg_login", std::bind(&LoginServer::cs_msg_login, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("cs_msg_register", std::bind(&LoginServer::cs_msg_register, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("cs_msg_change_pw", std::bind(&LoginServer::cs_msg_change_pw, this, std::placeholders::_1, std::placeholders::_2));
				_csGate.reg_msg_call("cs_msg_login_by_token", std::bind(&LoginServer::cs_msg_login_by_token, this, std::placeholders::_1, std::placeholders::_2));

				//_csGate.reg_msg_call("ss_msg_get_user_by_token", std::bind(&LoginServer::ss_msg_get_user_by_token, this, std::placeholders::_1, std::placeholders::_2));
			}

			void Run()
			{
				_csGate.run(1);
				_dbuser.run();
			}

			void Close()
			{
				_csGate.close();
			}

		private:
			const std::string make_token(int64 userId, int clientId)
			{
				std::stringstream ss;
				ss << Time::system_clock_now() <<'@'<< userId << '@' << clientId;
				gloox::SHA sha1;
				sha1.feed(ss.str());
				return sha1.hex();
			}

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
				json["apis"].Add("cs_msg_login_by_token");
				json["apis"].Add("ss_msg_client_exit");
				json["apis"].Add("ss_msg_user_exit");

				client->request("ss_reg_server", json, [](INetClient* client, neb::CJsonObject& msg) {
					CELLLog_Info(msg("data").c_str());
				});
			}

			void ss_msg_client_exit(INetClient* client, neb::CJsonObject& msg)
			{
				int clientId = 0;
				if (!msg["data"].Get("clientId", clientId))
				{
					client->resp_error(msg, "not found key<clientId>.");
					return;
				}
				CELLLog_Info("ss_msg_client_exit: clientId<%d>.", clientId);

				//�ж��Ƿ��¼��
				//������֤
				auto user = _userManager.get_by_clientId(clientId);
				if (user)
				{
					//����userΪ����״̬
					user->offline();
					//�Ƴ��û���¼
					//_userManager.remove(user);
				}
			}

			void ss_msg_user_exit(INetClient* client, neb::CJsonObject& msg)
			{
				int clientId = 0;
				if (!msg["data"].Get("clientId", clientId))
				{
					client->resp_error(msg, "not found key<clientId>.");
					return;
				}
				int64_t userId = 0;
				if (!msg["data"].Get("userId", userId))
				{
					client->resp_error(msg, "not found key<userId>.");
					return;
				}
				CELLLog_Info("ss_msg_user_exit: clientId<%d> userId<%lld>.", clientId, userId);
			}

			void cs_msg_login(INetClient* client, neb::CJsonObject& msg)
			{
				int clientId = 0;
				if (!msg.Get("clientId", clientId))
				{
					CELLLog_Error("not found key<clientId>.");
					return;
				}

				//��ǰ�����ֶλ�ȡ����֤
				std::string username;
				std::string password;
				{
					if (!msg["data"].Get("username", username))
					{
						client->resp_error(msg, "not found key <username>.");
						return;
					}

					if (username.empty())
					{
						client->resp_error(msg, "<username> can not be empty!");
						return;
					}
					//������ʽ
					std::regex reg1("^[0-9a-zA-Z]{6,16}$");
					if (!regex_match(username, reg1))
					{
						client->resp_error(msg, "<username> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("password", password))
					{
						client->resp_error(msg, "not found key<password>.");
						return;
					}

					if (password.empty())
					{
						client->resp_error(msg, "<password> can not be empty!");
						return;
					}

					//������ʽ
					if (!regex_match(password, reg1))
					{
						client->resp_error(msg, "<password> format is incorrect!");
						return;
					}
				}
				//
				CELLLog_Info("LoginServer::cs_msg_login: username=%s password=%s", username.c_str(), password.c_str());

				neb::CJsonObject users;
				_dbuser.findByKV("password,userId","user_info","username", username.c_str(), users);
				if (users.GetArraySize() < 1)
				{
					client->resp_error(msg, "<username> does not exist!");
					return;
				}

				//�Ƚϵ�ǰ�����Ƿ���ȷ
				auto password_now = users[0]("password");
				if (password_now != password)
				{
					client->resp_error(msg, "<password> is wrong!");
					return;
				}

				int64 userId = 0;
				if (!users[0].Get("userId", userId))
				{
					CELLLog_Error("unknow error.");
					return;
				}
				//�ж��Ƿ��¼��
				//������֤
				auto user = _userManager.get_by_userId(userId);
				if(user)
				{
					if (user->is_online())
					{
						//֪ͨ��ǰ�ѵ�¼�û������������ط���¼
						client->push(user->clientId, "sc_msg_logout", "Someone is trying to login this account!");

						//֪ͨ�����û��ǳ�
						neb::CJsonObject ret;
						ret.Add("userId", user->userId);
						ret.Add("token", user->token);
						ret.Add("clientId", user->clientId);
						int linkId = ClientId::get_link_id(user->clientId);
						client->push(linkId, "ss_msg_user_logout", ret);
					}

					//���ѵ�¼�û��Ƴ�
					_userManager.remove(user);
				}
				//ǩ������ ���ɵ�¼����
				auto token = make_token(userId, clientId);
				//��¼���� �����û�����
				if (!_userManager.add(token, userId, clientId))
				{
					client->resp_error(msg, "userManager add failed!");
					return;
				}
				//֪ͨ�����û���¼
				neb::CJsonObject ret;
				ret.Add("userId", userId);
				ret.Add("token", token);
				ret.Add("clientId", clientId);
				int linkId = ClientId::get_link_id(clientId);
				client->push(linkId, "ss_msg_user_login", ret);
				//���ص�¼���
				neb::CJsonObject json;
				json.Add("userId", userId);
				json.Add("token", token);
				client->response(msg, json);
			}

			void cs_msg_register(INetClient* client, neb::CJsonObject& msg)
			{
				//��ǰ�����ֶλ�ȡ����֤
				std::string username;
				std::string password;
				std::string nickname;
				int sex = -1;
				{
					if (!msg["data"].Get("username", username))
					{
						client->resp_error(msg, "not found key <username>.");
						return;
					}

					if (username.empty())
					{
						client->resp_error(msg, "<username> can not be empty!");
						return;
					}
					//������ʽ
					std::regex reg1("^[0-9a-zA-Z]{6,16}$");
					if (!regex_match(username, reg1))
					{
						client->resp_error(msg, "<username> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("password", password))
					{
						client->resp_error(msg, "not found key<password>.");
						return;
					}

					if (password.empty())
					{
						client->resp_error(msg, "<password> can not be empty!");
						return;
					}

					//������ʽ
					if (!regex_match(password, reg1))
					{
						client->resp_error(msg, "<password> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("nickname", nickname))
					{
						client->resp_error(msg, "not found key<nickname>.");
						return;
					}

					if (nickname.empty())
					{
						client->resp_error(msg, "<nickname> can not be empty!");
						return;
					}

					if (nickname.length() <3 || nickname.length() > 16)
					{
						client->resp_error(msg, "<nickname> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("sex", sex))
					{
						client->resp_error(msg, "not found key<sex>.");
						return;
					}

					if (sex !=0 && sex != 1)
					{
						client->resp_error(msg, "<sex> is only 0 or 1!");
						return;
					}
				}
				//
				CELLLog_Info("LoginServer::cs_msg_register: username=%s password=%s", username.c_str(), password.c_str());
				//�ж��û����Ƿ��Ѵ���
				if (_dbuser.has_username(username))
				{
					client->resp_error(msg, "username already exists");
					return;
				}
				//�ж��ǳ��Ƿ��Ѵ���
				if (_dbuser.has_nickname(nickname))
				{
					client->resp_error(msg, "nickname already exists");
					return;
				}
				//�����û�����
				auto userId = _dbuser.add_user(username, password, nickname, sex);
				if (userId > 0)
				{
					neb::CJsonObject ret;
					ret.Add("userId", userId);
					client->response(msg, ret);
				}
				else {
					client->resp_error(msg, "unkown error.");
				}
			}

			void cs_msg_change_pw(INetClient* client, neb::CJsonObject& msg)
			{
				//��ǰ�����ֶλ�ȡ����֤
				std::string username;
				std::string password_old;
				std::string password_new;
				{
					if (!msg["data"].Get("username", username))
					{
						client->resp_error(msg, "not found key <username>.");
						return;
					}

					if (username.empty())
					{
						client->resp_error(msg, "<username> can not be empty!");
						return;
					}
					//������ʽ
					std::regex reg1("^[0-9a-zA-Z]{6,16}$");
					if (!regex_match(username, reg1))
					{
						client->resp_error(msg, "<username> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("password_old", password_old))
					{
						client->resp_error(msg, "not found key<password_old>.");
						return;
					}

					if (password_old.empty())
					{
						client->resp_error(msg, "<password_old> can not be empty!");
						return;
					}

					//������ʽ
					if (!regex_match(password_old, reg1))
					{
						client->resp_error(msg, "<password_old> format is incorrect!");
						return;
					}

					if (!msg["data"].Get("password_new", password_new))
					{
						client->resp_error(msg, "not found key<password_new>.");
						return;
					}

					if (password_new.empty())
					{
						client->resp_error(msg, "<password_new> can not be empty!");
						return;
					}

					//������ʽ
					if (!regex_match(password_new, reg1))
					{
						client->resp_error(msg, "<password_new> format is incorrect!");
						return;
					}
				}
				//
				CELLLog_Info("LoginServer::cs_msg_change_pw: username=%s password_old=%s password_new=%s", username.c_str(), password_old.c_str(), password_new.c_str());
				
				//��ȡ�û�����
				neb::CJsonObject users;
				_dbuser.findByKV("user_info", "username", username.c_str(), users);
				if (users.GetArraySize() < 1)
				{
					client->resp_error(msg, "<username> is incorrect!");
					return;
				}
				//�Ƚϵ�ǰ�����Ƿ���ȷ
				auto password_now = users[0]("password");
				if (password_now != password_old)
				{
					client->resp_error(msg, "<password> is incorrect!");
					return;
				}
				//�����û�����
				int changes = _dbuser.updateByKV("user_info", "username", username.c_str(), "password",password_new.c_str());
				if (changes == 1)
				{
					client->response(msg, "change password success.");
				}
				else {
					client->resp_error(msg, "unkown error.");
				}
			}

			void ss_msg_get_user_by_token(INetClient* client, neb::CJsonObject& msg)
			{
				//��ǰ�����ֶλ�ȡ����֤
				std::string token;
				{
					if (!msg["data"].Get("token", token))
					{
						client->resp_error(msg, "not found key <token>.");
						return;
					}

					if (token.empty())
					{
						client->resp_error(msg, "<token> can not be empty!");
						return;
					}
					//������ʽ
					std::regex reg1("^[0-9a-zA-Z]{6,128}$");
					if (!regex_match(token, reg1))
					{
						client->resp_error(msg, "<token> format is incorrect!");
						return;
					}
				}
				//
				CELLLog_Info("LoginServer::ss_msg_get_user_by_token: token=%s", token.c_str());
				//������֤
				auto user = _userManager.get_by_token(token);
				if (!user)
				{
					client->resp_error(msg, "Invalid token!");
					return;
				}
				
				neb::CJsonObject json;
				json.Add("token", user->token);
				json.Add("userId", user->userId);
				json.Add("clientId", user->clientId);
				//���ؽ��
				client->response(msg, json);
			}

			void cs_msg_login_by_token(INetClient* client, neb::CJsonObject& msg)
			{
				int clientId = 0;
				if (!msg.Get("clientId", clientId))
				{
					CELLLog_Error("not found key<clientId>.");
					return;
				}

				//��ǰ�����ֶλ�ȡ����֤
				std::string token;
				{
					if (!msg["data"].Get("token", token))
					{
						client->resp_error(msg, "not found key <token>.");
						return;
					}

					if (token.empty())
					{
						client->resp_error(msg, "<token> can not be empty!");
						return;
					}
					//������ʽ
					std::regex reg1("^[0-9a-zA-Z]{6,128}$");
					if (!regex_match(token, reg1))
					{
						client->resp_error(msg, "<token> format is incorrect!");
						return;
					}
				}
				//
				CELLLog_Info("LoginServer::cs_msg_login_by_token: token=%s", token.c_str());
				//�ж��Ƿ��¼��
				//������֤
				auto user = _userManager.get_by_token(token);
				if (!user)
				{
					client->resp_error(msg, "Invalid token!");
					return;
				}

				int64_t userId = user->userId;
				{
					if (user->is_online()) {
						//֪ͨ��ǰ�ѵ�¼�û������������ط���¼
						client->push(user->clientId, "sc_msg_logout", "Someone is trying to login this account!");

						//֪ͨ�����û��ǳ�
						neb::CJsonObject ret;
						ret.Add("userId", user->userId);
						ret.Add("token", user->token);
						ret.Add("clientId", user->clientId);
						int linkId = ClientId::get_link_id(user->clientId);
						client->push(linkId, "ss_msg_user_logout", ret);
					}

					//���ѵ�¼�û��Ƴ�
					_userManager.remove(user);
				}
				//ǩ������ ���ɵ�¼����
				token = make_token(userId, clientId);
				//��¼���� �����û�����
				if (!_userManager.add(token, userId, clientId))
				{
					client->resp_error(msg, "userManager add failed!");
					return;
				}
				//֪ͨ�����û���¼
				neb::CJsonObject ret;
				ret.Add("userId", userId);
				ret.Add("token", token);
				ret.Add("clientId", clientId);
				int linkId = ClientId::get_link_id(clientId);
				client->push(linkId, "ss_msg_user_login", ret);
				//���ص�¼���
				neb::CJsonObject json;
				json.Add("userId", userId);
				json.Add("token", token);
				client->response(msg, json);
			}
		};
	}
}
#endif // !_doyou_io_LoginServer_HPP_
