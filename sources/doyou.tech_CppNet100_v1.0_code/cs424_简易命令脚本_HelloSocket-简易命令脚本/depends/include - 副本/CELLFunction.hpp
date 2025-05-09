#ifndef _CELL_FUNCTION_HPP_
#define _CELL_FUNCTION_HPP_

#include"CELLLog.hpp"

static int argsToInt(int argc, char *args[], int i, int def, const char * argName)
{
	if (i >= argc)
	{
		CELLLog::Info("argsToInt error: i<%d> >= argc<%d>,%s\n", i, argc, argName);
	}
	else {
		try
		{
			def = atoi(args[i]);
		}
		catch (const std::exception& e)
		{
			CELLLog::Info("argsToInt error: i<%d>,argc<%d>,cmd<%s>,%s\n", i, argc, args[i], argName);
		}
	}
	CELLLog::Info("%s = %d\n", argName, def);
	return def;
}

static const char * argsToStr(int argc, char *args[], int i, const char * def, const char * argName)
{
	if (i >= argc)
	{
		CELLLog::Info("argsToStr error: i<%d> >= argc<%d>,%s\n", i, argc, argName);
	}
	else {
		def = args[i];
	}
	CELLLog::Info("%s = %s\n", argName, def);
	return def;
}

#endif // !_CELL_FUNCTION_HPP_
