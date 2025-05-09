#ifndef _doyou_io_memberManager_HPP_
#define _doyou_io_memberManager_HPP_

#include"CJsonObject.hpp"
#include"INetClientS.hpp"
#include<vector>
#include<algorithm>

namespace doyou {
	namespace io {

		//小组
		class Group
		{
		public:
			bool add(int64_t client)
			{
				auto itr = std::find(_member.begin(), _member.end(), client);
				if (itr == _member.end())
				{
					_member.push_back(client);
					return true;
				}
				return false;
			}

			bool del(int64_t client)
			{
				auto itr = std::find(_member.begin(), _member.end(), client);
				if (itr != _member.end())
				{
					_member.erase(itr);
					return true;
				}
				return false;
			}

			bool has(int64_t client)
			{
				auto itr = std::find(_member.begin(), _member.end(), client);
				return (itr != _member.end());
			}

			int key()
			{
				return _key;
			}

			void key(int key)
			{
				_key = key;
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
			int _key = 0;
			int _id = 0;
		};

		//会话小组管理
		class GroupManager
		{
		private:
			std::map<int, Group> _map_member;
			int _index_id = 10000;
		public:
			bool create(int& group_id, int group_key, int64_t client)
			{
				//如果group_id值为0
				//由GroupManager分配一个group_id
				if (group_id <= 0)
				{
					group_id = ++_index_id;
				}

				//先查找group_id是否已存在
				auto itr = _map_member.find(group_id);
				if (itr != _map_member.end())
				{//join可能失败
					return join(group_id, group_key, client);
				}
				else {
					Group a;
					a.add(client);
					a.key(group_key);
					a.id(group_id);
					_map_member[group_id] = a;
				}

				return true;
			}

			bool join(int group_id, int group_key, int64_t client)
			{
				auto itr = _map_member.find(group_id);

				if (itr == _map_member.end())
					return false;

				if(itr->second.key() != group_key)
					return false;

				itr->second.add(client);
				return true;
			}

			bool find(int64_t client, std::vector<int>& group_list)
			{
				
				for (auto& itr: _map_member)
				{
					if (itr.second.has(client))
					{
						group_list.push_back(itr.second.id());
					}
				}
				return !group_list.empty();
			}

			bool del(int group_id, int64_t client)
			{
				auto itr = _map_member.find(group_id);
				if (itr == _map_member.end())
					return false;

				auto ret = itr->second.del(client);

				if (itr->second.empty())
					_map_member.erase(itr);
				
				return ret;
			}

			Group* get(int group_id)
			{
				auto itr = _map_member.find(group_id);
				if (itr == _map_member.end())
					return nullptr;

				return &itr->second;
			}

			bool has(int group_id, int64_t client)
			{
				auto itr = _map_member.find(group_id);
				if (itr == _map_member.end())
					return false;

				return itr->second.has(client);
			}
		};
	}
}
#endif // !_doyou_io_memberManager_HPP_
