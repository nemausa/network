#include"Log.hpp"
#include"Config.hpp"
#include"LoginServer.hpp"
#include"DBUser.hpp"

using namespace doyou::io;

int main(int argc, char* args[])
{
#if _WIN32 && _CONSOLE
	system("chcp 65001");
#endif // _WIN32 && _CONSOLE

	//设置运行日志名称
	Log::Instance().setLogPath("LoginServerLog", "w", false);
	Config::Instance().Init(argc, args);
	////////////////////
	DBUser db;
	db.init();
	int count = 100000;
	int nMax = 10;
	char username[32] = {};
	char nickname[32] = {};

	Timestamp timestamp00;
	//事务开始
	db.execDML("begin;");
	////////
	CELLLog_Info("db.add_user");
	for (int n = 0; n < nMax; n++)
	{
		Timestamp timestamp;
		for (int i = 0; i < count; i++)
		{
			sprintf(username, "test%05d", db.makeId());
			sprintf(nickname, "abc%05d", db.makeId());
			if (db.add_user(username, "mm123456", nickname, i % 2) > 0)
			{
				//CELLLog_Info(username);
			}
		}
		auto t = timestamp.getElapsedSecond();
		CELLLog_Info("%d   count=%d,  time=%f,      %f,      %f", n, count, t, t / count, count / t);
	}
	CELLLog_Info("db.add_user %f", timestamp00.getElapsedSecond());
	CELLLog_Info("#####################");
	CELLLog_Info("db.hasByKV");
	for (int n = 0; n < nMax; n++)
	{
		Timestamp timestamp;
		for (int i = 0; i < count; i++)
		{
			sprintf(username, "test%05d", 100000+i+1+(n*count));
			if (db.hasByKV("user_info", "username", username))
			{
				//CELLLog_Info(username);
			}
		}
		auto t = timestamp.getElapsedSecond();
		CELLLog_Info("%d   count=%d,  time=%f,      %f,      %f", n, count, t, t / count, count / t);
	}
	CELLLog_Info("db.hasByKV %f", timestamp00.getElapsedSecond());
	CELLLog_Info("#####################");
	CELLLog_Info("db.findByKV");
	for (int n = 0; n < nMax; n++)
	{
		Timestamp timestamp;
		for (int i = 0; i < count; i++)
		{
			sprintf(username, "test%05d", 100000 + i + 1 + (n*count));
			neb::CJsonObject ret;
			if (db.findByKV("password,nickname","user_info", "username", username, ret))
			{
				//CELLLog_Info(username);
			}
		}
		auto t = timestamp.getElapsedSecond();
		CELLLog_Info("%d   count=%d,  time=%f,      %f,      %f", n, count, t, t / count, count / t);
	}
	CELLLog_Info("db.findByKV %f", timestamp00.getElapsedSecond());
	CELLLog_Info("#####################");
	CELLLog_Info("db.updateByKV");
	int mm = Time::system_clock_now() % 100000;
	for (int n = 0; n < nMax; n++)
	{
		Timestamp timestamp;
		for (int i = 0; i < count; i++)
		{
			sprintf(username, "test%05d", 100000 + i + 1 + (n*count));
			sprintf(nickname, "abc%05d", mm);
			if (db.updateByKV("user_info", "username", username, "password", nickname))
			{
				//CELLLog_Info("%s >> %s", username, nickname);
			}
		}
		auto t = timestamp.getElapsedSecond();
		CELLLog_Info("%d   count=%d,  time=%f,      %f,      %f", n, count, t, t / count, count / t);
	}
	CELLLog_Info("db.updateByKV %f", timestamp00.getElapsedSecond());
	CELLLog_Info("#####################");
	CELLLog_Info("db.deleteByKV");
	for (int n = 0; n < nMax; n++)
	{
		Timestamp timestamp;
		for (int i = 0; i < count; i++)
		{
			sprintf(username, "test%05d", 100000 + i + 1 + (n*count));
			if (db.deleteByKV("user_info", "username", username))
			{
				//CELLLog_Info(username);
			}
		}
		auto t = timestamp.getElapsedSecond();
		CELLLog_Info("%d   count=%d,  time=%f,      %f,      %f", n, count, t, t / count, count / t);
	}
	CELLLog_Info("db.deleteByKV %f", timestamp00.getElapsedSecond());
	//提交事务
	db.execDML("commit;");

	system("pause");
	return 0;
	////////////////////
	db.deleteByKV2("user_info", "state", 0, "sex", 0);
	if (db.hasByKV("user_info", "username", "user001"))
	{
		CELLLog_Info("has_username user001 true");
	}
	else {
		CELLLog_Info("has_username user001 false");
	}

	if (db.hasByKV("user_info", "sex", 1))
	{
		CELLLog_Info("has sex 1 true");
	}
	else {
		CELLLog_Info("has sex 1 false");
	}

	if (db.has_username("user001"))
	{
		CELLLog_Info("has_username true");
	}
	else {
		CELLLog_Info("has_username false");
	}

	//db.add_user("user003", "mm123456", "qwer", 0);
	//db.add_user("user004", "mm123456", "hjkl", 1);

	//db.updateByKV("user_info", "username", "user001", "password", "159263");
	//db.updateByKV("user_info", "username", "user002", "sex", 1);
	//db.updateByKV("user_info", "username", "user003", "create_date", Time::system_clock_now());

	db.updateByKV2("user_info", "state", 0, "sex", 0, "password", "123123");

	neb::CJsonObject ret;
	//auto b = db.findByKV("user_info", "sex",1, ret);
	auto b = db.findByKV2("user_info", "state", 0, "sex", 0, ret);
	if (b && ret.IsArray())
	{
		int length = ret.GetArraySize();
		for (size_t i = 0; i < length; i++)
		{
			std::string id = ret[i]("id");
			int64 userId = 0;
			ret[i].Get("userId", userId);
			std::string username =  ret[i]("username");
			std::string password = ret[i]("password");
			std::string nickname = ret[i]("nickname");
			std::string sex = ret[i]("sex");
			std::string state = ret[i]("state");
			std::string create_date = ret[i]("create_date");
			CELLLog_Info("findByKV2: userId=%d username=%s password=%s nickname=%s", userId, username.c_str(), password.c_str(), nickname.c_str());
		}
	}

	db.close();
	return 0;
	///////////////////
	LoginServer server;
	server.Init();
	while (true)
	{
		server.Run();
	}
	server.Close();

	////在主线程中等待用户输入命令
	//while (true)
	//{
	//	char cmdBuf[256] = {};
	//	scanf("%s", cmdBuf);
	//	if (0 == strcmp(cmdBuf, "exit"))
	//	{
	//		server.Close();
	//		break;
	//	}
	//	else {
	//		CELLLog_Info("undefine cmd");
	//	}
	//}

	CELLLog_Info("exit.");

	return 0;
}
