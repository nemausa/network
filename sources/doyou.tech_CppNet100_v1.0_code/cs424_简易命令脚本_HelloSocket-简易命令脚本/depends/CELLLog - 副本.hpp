#ifndef _CELL_LOG_HPP_
#define _CELL_LOG_HPP_

//#include"CELL.hpp"
#include"CELLTask.hpp"
#include<ctime>
class CELLLog
{
	//Info
	//Debug
	//Warring
	//Error
private:
	CELLLog()
	{
		_taskServer.Start();
	}

	~CELLLog()
	{
		_taskServer.Close();
		if (_logFile)
		{
			Info("CELLLog fclose(_logFile)\n");
			fclose(_logFile);
			_logFile = nullptr;
		}
	}
public:
	static CELLLog& Instance()
	{
		static  CELLLog sLog;
		return sLog;
	}

	void setLogPath(const char* logPath, const char* mode)
	{
		if (_logFile)
		{
			Info("CELLLog::setLogPath _logFile != nullptr\n");
			fclose(_logFile);
			_logFile = nullptr;
		}
			
		_logFile = fopen(logPath, mode);
		if (_logFile)
		{
			Info("CELLLog::setLogPath success,<%s,%s>\n", logPath, mode);
		}
		else {
			Info("CELLLog::setLogPath failed,<%s,%s>\n", logPath, mode);
		}
	}

	static void Error(const char* pStr)
	{
		Error("%s", pStr);
	}

	template<typename ...Args>
	static void Error(const char* pformat, Args ... args)
	{
		Echo("Error ", pformat, args...);
	}

	static void Warring(const char* pStr)
	{
		Warring("%s", pStr);
	}

	template<typename ...Args>
	static void Warring(const char* pformat, Args ... args)
	{
		Echo("Warring ", pformat, args...);
	}

	static void Debug(const char* pStr)
	{
		Debug("%s", pStr);
	}

	template<typename ...Args>
	static void Debug(const char* pformat, Args ... args)
	{
		Echo("Debug ", pformat, args...);
	}

	static void Info(const char* pStr)
	{
		Info("%s", pStr);
	}

	template<typename ...Args>
	static void Info(const char* pformat, Args ... args)
	{
		Echo("Info ",pformat, args...);
	}

	template<typename ...Args>
	static void Echo(const char* type, const char* pformat, Args ... args)
	{
		CELLLog* pLog = &Instance();
		pLog->_taskServer.addTask([=]() {
			if (pLog->_logFile)
			{
				auto t = system_clock::now();
				auto tNow = system_clock::to_time_t(t);
				//fprintf(pLog->_logFile, "%s", ctime(&tNow));
				std::tm* now = std::gmtime(&tNow);
				fprintf(pLog->_logFile, "%s", type);
				fprintf(pLog->_logFile, "[%d-%d-%d %d:%d:%d]", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
				fprintf(pLog->_logFile, pformat, args...);
				fflush(pLog->_logFile);
			}
			printf(pformat, args...);
		});
	}
private:
	FILE* _logFile = nullptr;
	CELLTaskServer _taskServer;
};

#endif // !_CELL_LOG_HPP_
