#include"Log.hpp"
using namespace doyou::io;

int main()
{
	Log::Instance().setLogPath("HelloCpp.log","w",false);

	CELLLog_Info("a=%s","dasdas");
	{
		std::string log1 = "�ڶ����ַ���";
		char log2[] = "�������ַ���";
		const char* log3 = "���Ķ��ַ���";
		Log::Info("��һ���ַ���");
		Log::Info(log1.c_str());
		Log::Info(log2);
		Log::Info(log3);
	}

	return 0;
}