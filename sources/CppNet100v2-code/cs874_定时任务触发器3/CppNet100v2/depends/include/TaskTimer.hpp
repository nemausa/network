#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_

#include<mutex>
#include<list>
#include<functional>

#include"Timestamp.hpp"

namespace doyou {
	namespace io {
		//任务定时触发器
		class TaskTimer
		{
		private:
			//事件
			typedef std::function<void()> Event;
			//任务结构
			struct Task
			{
				//任务的id
				int id;
				//事件执行次数
				int run_count;
				//执行事件的间隔事件
				time_t ms;
				//创建任务的时间点
				time_t t1;
				//要执行的事件
				Event e;
			};
		private:
			//任务数据
			std::list<Task> _tasks;
			//改变数据缓冲区时需要加锁
			std::mutex _mutex;
		public:
			//run_count次定时触发任务事件
			void addTask(int id, int run_count, int ms, Event e)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				time_t t1 = Time::system_clock_now();
				Task task = { id, run_count , ms, t1, e };
				_tasks.push_back(task);
			}

			//无限次定时触发任务事件
			void addTask(int id, int ms, Event e)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				time_t t1 = Time::system_clock_now();
				Task task = { id, -1, ms, t1, e };
				_tasks.push_back(task);
			}

		protected:
			//工作函数
			void OnRun()
			{
				//如果没有任务
				if (_tasks.empty())
				{
					return;
				}
				//当前时间
				time_t d = Time::system_clock_now();
				//处理任务
				for (auto itr = _tasks.begin(); itr != _tasks.end(); ++itr)
				{
					//当前时间 - 起点时间 >= 间隔时间
					if (d >= itr->ms)
					{
						//更新计时起点时间
						itr->t1 = d; 
						//触发任务事件的回调方法
						itr->e();
						//如果剩余触发次数>0
						if (itr->run_count > 0)
						{	
							//触发次数-1
							--itr->run_count;
							//如果剩余次数为0
							if (itr->run_count == 0)
							{	
								//移除任务
								itr = _tasks.erase(itr);
							}
							//判断是否最后一个成员
							if (itr != _tasks.end())
							{
								break;
							}
						}
					}
				}
			}
		};
	}
}
#endif // !_CELL_TASK_H_
