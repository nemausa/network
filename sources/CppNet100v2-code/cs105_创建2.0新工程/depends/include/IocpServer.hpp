#ifndef _CELLIOCPServer_HPP_
#define _CELLIOCPServer_HPP_

#include"Server.hpp"
#include"Iocp.hpp"

namespace doyou {
	namespace io {
		//������Ϣ���մ��������
		class IocpServer :public Server
		{
		public:
			IocpServer()
			{
				_iocp.create();
			}

			~IocpServer() noexcept
			{
				Close();
			}

			bool DoNetEvents()
			{
				Client* pClient = nullptr;
				for (auto iter = _clients.begin(); iter != _clients.end(); )
				{
					pClient = iter->second;
					//��Ҫд���ݵĿͻ���,��postSend
					if (pClient->needWrite())
					{
						auto pIoData = pClient->makeSendIoData();
						if (pIoData)
						{
							if (!_iocp.postSend(pIoData))
							{
								OnClientLeave(pClient);
								iter = _clients.erase(iter);
								continue;
							}
						}
						//--
						pIoData = pClient->makeRecvIoData();
						if (pIoData)
						{
							if (!_iocp.postRecv(pIoData))
							{
								OnClientLeave(pClient);
								iter = _clients.erase(iter);
								continue;
							}
						}
					}
					else {
						auto pIoData = pClient->makeRecvIoData();
						if (pIoData)
						{
							if (!_iocp.postRecv(pIoData))
							{
								OnClientLeave(pClient);
								iter = _clients.erase(iter);
								continue;
							}
						}
					}
					iter++;
				}
				//---
				while (true)
				{
					int ret = DoIocpNetEvents();
					if (ret < 0)
					{
						return false;
					}
					else if (ret == 0)
					{
						return true;
					}
				}
				return true;
			}
			//ÿ��ֻ����һ�������¼�
			//ret = -1 iocp����
			//ret =  0 û���¼�
			//ret =  1 ���¼�����
			int DoIocpNetEvents()
			{
				int ret = _iocp.wait(_ioEvent, 1);
				if (ret < 0)
				{
					CELLLog_Error("IocpServer%d.OnRun.wait", _id);
					return ret;
				}
				else if (ret == 0)
				{
					return ret;
				}
				//---

				//�������� ��� Completion
				if (IO_TYPE::RECV == _ioEvent.pIoData->iotype)
				{
					if (_ioEvent.bytesTrans <= 0)
					{//�ͻ��˶Ͽ�����
					 //CELLLog_Info("rmClient sockfd=%d, IO_TYPE::RECV bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
						rmClient(_ioEvent);
						return ret;
					}
					//
					Client* pClient = (Client*)_ioEvent.data.ptr;
					if (pClient)
					{
						pClient->recv4iocp(_ioEvent.bytesTrans);
						OnNetRecv(pClient);
					}
					//
					//CELLLog_Info("IO_TYPE::RECV sockfd=%d, bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
				}
				//�������� ��� Completion
				else if (IO_TYPE::SEND == _ioEvent.pIoData->iotype)
				{
					if (_ioEvent.bytesTrans <= 0)
					{//�ͻ��˶Ͽ�����
					 //CELLLog_Info("rmClient sockfd=%d, IO_TYPE::SEND bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
						rmClient(_ioEvent);
						return ret;
					}
					//
					Client* pClient = (Client*)_ioEvent.data.ptr;
					if (pClient)
						pClient->send2iocp(_ioEvent.bytesTrans);
					//
					//CELLLog_Info("IO_TYPE::SEND sockfd=%d, bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
				}
				else {
					CELLLog_Warring("undefine io type.");
				}
				return ret;
			}

			void rmClient(Client* pClient)
			{
				auto iter = _clients.find(pClient->sockfd());
				if (iter != _clients.end())
					_clients.erase(iter);
				//
				OnClientLeave(pClient);
			}

			void rmClient(IO_EVENT& ioEvent)
			{
				Client* pClient = (Client*)_ioEvent.data.ptr;
				if (pClient)
					rmClient(pClient);
			}

			void OnClientJoin(Client* pClient)
			{
				_iocp.reg(pClient->sockfd(), pClient);
				auto pIoData = pClient->makeRecvIoData();
				if (pIoData)
					_iocp.postRecv(pIoData);
			}
		private:
			Iocp _iocp;
			IO_EVENT _ioEvent = {};
		};
	}
}
#endif // !_CELLIOCPServer_HPP_
