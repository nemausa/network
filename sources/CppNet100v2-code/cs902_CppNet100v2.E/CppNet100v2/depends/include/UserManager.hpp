#ifndef _doyou_io_UserManager_HPP_
#define _doyou_io_UserManager_HPP_

#include<cstdint>
#include<string>
#include<map>

namespace doyou {
	namespace io {
		class UserManager
		{
		private:
			//user基础数据
			class User
			{
			public:
				std::string token;
				int64_t userId = 0;
				int clientId = 0;
			public:
				void offline()
				{
					clientId = 0;
				}

				bool is_online()
				{
					return clientId != 0;
				}
			};
			typedef User* UserPtr;
			//通过token索引查询user
			std::map<std::string, UserPtr>_token2user;
			//通过token索引查询user
			std::map<int64_t, UserPtr>_userId2user;
			//通过token索引查询user
			std::map<int, UserPtr>_clientId2user;
		public:
			
			bool add(const std::string& token, int64_t userId, int clientId)
			{
				if (get_by_token(token) || get_by_userId(userId) || get_by_userId(clientId))
				{
					return false;
				}

				UserPtr user = new User();
				user->token = token;
				user->userId = userId;
				user->clientId = clientId;

				_token2user[token] = user;
				_userId2user[userId] = user;
				_clientId2user[clientId] = user;
				return true;
			}

			void remove(UserPtr user)
			{
				if (!user)
					return;

				_userId2user.erase(user->userId);
				_token2user.erase(user->token);
				_clientId2user.erase(user->clientId);

				delete user;
			}

			UserPtr get_by_token(const std::string& token)
			{
				auto itr = _token2user.find(token);
				if (itr == _token2user.end())
					return nullptr;

				return itr->second;
			}

			UserPtr get_by_userId(int64_t userId)
			{
				auto itr = _userId2user.find(userId);
				if (itr == _userId2user.end())
					return nullptr;

				return itr->second;
			}

			UserPtr get_by_clientId(int clientId)
			{
				auto itr = _clientId2user.find(clientId);
				if (itr == _clientId2user.end())
					return nullptr;

				return itr->second;
			}

		};
	}
}
#endif // !_doyou_io_UserManager_HPP_
