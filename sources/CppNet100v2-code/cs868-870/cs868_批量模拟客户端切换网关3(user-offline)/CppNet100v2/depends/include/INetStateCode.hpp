#ifndef _doyou_io_INetStateCode_hpp_
#define _doyou_io_INetStateCode_hpp_
namespace doyou {
	namespace io {

#define msg_type_req 1
#define msg_type_resp 2
#define msg_type_push 3
#define msg_type_broadcast 4
#define msg_type_push_s 5

#define state_code_ok 0
#define state_code_error 1
#define state_code_timeout 2
#define state_code_undefine_cmd 3
#define state_code_server_busy 4
#define state_code_server_off 5

		class ClientId
		{
			static const int _link_id_mask = 100000;
		public:
			static int set_link_id(int link_id, int client_id)
			{
				return ((link_id * _link_id_mask) + client_id);
			}

			static int get_link_id(int id)
			{
				if (id >= _link_id_mask)
				{
					return id / _link_id_mask;
				}
				return id;
			}

			static int get_client_id(int id)
			{
				if (id >= _link_id_mask)
				{
					return id % _link_id_mask;
				}
				return id;
			}
		};
	}
}
#endif // !_doyou_io_INetStateCode_hpp_