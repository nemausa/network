#ifndef _CELL_LocalData_H_
#define _CELL_LocalData_H_

#include<string>
#include<fstream>

#include"CJsonObject.hpp"
#include"Log.hpp"

namespace doyou {
	namespace io {
		//读写本地数据
		class LocalData
		{
		public:
			static bool json_read(std::string file_name, neb::CJsonObject& json)
			{
				//1. 读取文件中的json str
				std::string json_str;
				if (!read(file_name, json_str))
				{
					CELLLog_Warring("LocalData::json_read::!read file >> %s", file_name.c_str());
					return false;
				}
				if (json_str.empty())
				{
					CELLLog_Warring("LocalData::json_read::json_str.empty >> %s", file_name.c_str());
					return false;
				}
				//2. 把json str解析为 json object
				if (!json.Parse(json_str))
				{
					CELLLog_Error("LocalData::json_read::json.Parse <%s> error : %s", file_name.c_str(), json.GetErrMsg().c_str());
					return false;
				}

				return true;
			}

			static bool json_write(const std::string& file_name, neb::CJsonObject& json)
			{
				return write(file_name, json.ToString());
			}

			static bool read(std::string file_name, std::string& str)
			{
				std::ifstream inFile;
				//打开文件
				inFile.open(file_name);
				//判断文件是否打开
				if (!inFile.is_open())
				{
					CELLLog_Warring("LocalData::read::!inFile.is_open >> %s", file_name.c_str());
					return false;
				}
				//读取数据为字符串
				std::string data((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

				str = data;
				return true;
			}

			static bool write(const std::string& file_name, const std::string& str)
			{
				std::ofstream outFile;
				//打开文件
				outFile.open(file_name);
				//判断文件是否打开
				if(!outFile.is_open())
				{
					CELLLog_Warring("LocalData::write::!outFile.is_open >> %s", file_name.c_str());
					return false;
				}
				//写入
				outFile << str;
				//关闭文件
				outFile.close();

				return true;
			}

		};
	}
}
#endif // !_CELL_LocalData_H_
