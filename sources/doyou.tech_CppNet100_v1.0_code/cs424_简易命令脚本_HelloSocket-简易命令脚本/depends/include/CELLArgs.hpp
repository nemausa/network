#ifndef _CELL_ARGS_HPP_
#define _CELL_ARGS_HPP_

#include"CELLLog.hpp"
#include<map>

class CELLArgs
{
private:
	CELLArgs()
	{

	}

	~CELLArgs()
	{

	}
public:
	static CELLArgs& Instance()
	{
		static  CELLArgs obj;
		return obj;
	}

	void Init(int argc, char* args[])
	{
		_exePath = args[0];
		for (int n = 1; n < argc; n++)
		{
			madeCmd(args[n]);
		}
	}

	void madeCmd(char* cmd)
	{
		char* val = strchr(cmd, '=');
		if (val)
		{
			*val = '\0';
			val++;
			_kv[cmd] = val;
			CELLLog_Debug("madeCmd %s=%s", cmd, val);
		} else {
			_kv[cmd] = "";
			CELLLog_Debug("madeCmd %s", cmd);
		}
	}

	const char* getStr(const char* argName, const char* def)
	{
		auto itr = _kv.find(argName);
		if (_kv.find(argName) == _kv.end())
		{
			CELLLog_Error("CELLArgs not found <%s>", argName);
		}
		else {
			def = itr->second.c_str();
		}
		CELLLog_Info("%s=%s", argName, def);
		return def;
	}

	int getInt(const char* argName, int def)
	{
		auto itr = _kv.find(argName);
		if (_kv.find(argName) == _kv.end())
		{
			CELLLog_Error("CELLArgs not found <%s>", argName);
		}
		else {
			def = atoi(itr->second.c_str());
		}
		CELLLog_Info("%s=%d", argName, def);
		return def;
	}
private:
	std::string _exePath;
	std::map<std::string, std::string> _kv;
};

#endif // !_CELL_NET_WORK_HPP_
