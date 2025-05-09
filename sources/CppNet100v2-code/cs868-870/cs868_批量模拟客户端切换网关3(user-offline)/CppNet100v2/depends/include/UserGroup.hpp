#ifndef _doyou_io_UserGroup_HPP_
#define _doyou_io_UserGroup_HPP_

#include"CJsonObject.hpp"
#include"INetClientS.hpp"
#include<vector>
#include<algorithm>

namespace doyou {
	namespace io {

		//小组
		class group_id_list
		{
		public:
			bool add(int64_t group_id)
			{
				auto itr = std::find(_member.begin(), _member.end(), group_id);
				if (itr == _member.end())
				{
					_member.push_back(group_id);
					return true;
				}
				return false;
			}

			bool del(int64_t group_id)
			{
				auto itr = std::find(_member.begin(), _member.end(), group_id);
				if (itr != _member.end())
				{
					_member.erase(itr);
					return true;
				}
				return false;
			}

			bool has(int64_t group_id)
			{
				auto itr = std::find(_member.begin(), _member.end(), group_id);
				return (itr != _member.end());
			}


			int id()
			{
				return _id;
			}

			void id(int id)
			{
				_id = id;
			}

			bool empty()
			{
				return _member.empty();
			}

			const std::vector<int64_t>& member()
			{
				return _member;
			}
		private:
			std::vector<int64_t> _member;
			int _id = 0;
		};

		//会话小组管理
		class UserGroup
		{
		private:
			std::map<int64_t, group_id_list> _map_member;
			int _index_id = 10000;
		public:
			bool add(int64_t user_id, int64_t group_id)
			{
				//先查找user_id是否已存在
				auto itr = _map_member.find(user_id);
				if (itr != _map_member.end())
				{
					return itr->second.add(group_id);
				}
				else {
					group_id_list a;
					a.add(group_id);
					a.id(user_id);
					_map_member[user_id] = a;
				}

				return true;
			}

			bool find(int64_t group_id, std::vector<int>& group_list)
			{
				
				for (auto& itr: _map_member)
				{
					if (itr.second.has(group_id))
					{
						group_list.push_back(itr.second.id());
					}
				}
				return !group_list.empty();
			}

			bool del(int64_t user_id, int64_t group_id)
			{
				auto itr = _map_member.find(user_id);
				if (itr == _map_member.end())
					return false;

				auto ret = itr->second.del(group_id);

				if (itr->second.empty())
					_map_member.erase(itr);
				
				return ret;
			}

			group_id_list* get(int64_t user_id)
			{
				auto itr = _map_member.find(user_id);
				if (itr == _map_member.end())
					return nullptr;

				return &itr->second;
			}

			bool has(int64_t user_id, int64_t group_id)
			{
				auto itr = _map_member.find(user_id);
				if (itr == _map_member.end())
					return false;

				return itr->second.has(group_id);
			}
		};
	}
}
#endif // !_doyou_io_UserGroup_HPP_
