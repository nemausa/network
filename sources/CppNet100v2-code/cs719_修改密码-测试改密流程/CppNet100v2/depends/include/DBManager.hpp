#ifndef _doyou_io_DBManager_HPP_
#define _doyou_io_DBManager_HPP_

#include"Log.hpp"
#include"CppSQLite3.h"

namespace doyou {
	namespace io {
		class DBManager
		{
		protected:
			CppSQLite3DB _db;
			std::string _db_name;
		public:
			bool open(const char* db_name)
			{
				_db_name = db_name;
				try
				{
					_db.open(db_name);
					return true;
				}
				catch (CppSQLite3Exception& e)
				{
					CELLLog_Error("DBManager::open(%s) error: %s", db_name, e.errorMessage());
				}
				return false;
			}

			bool close()
			{
				try
				{
					_db.close();
					return true;
				}
				catch (CppSQLite3Exception& e)
				{
					CELLLog_Error("DBManager::close(%s) error: %s", _db_name.c_str(), e.errorMessage());
				}
				return false;
			}

			bool tableExists(const char* szTable)
			{
				try
				{
					return _db.tableExists(szTable);
				}
				catch (CppSQLite3Exception& e)
				{
					CELLLog_Error("DBManager::tableExists(%s.%s) error: %s", _db_name.c_str(), szTable, e.errorMessage());
				}
				return false;
			}

			bool query2json(CppSQLite3Query& query, neb::CJsonObject& json)
			{
				try {
					while (!query.eof())
					{
						neb::CJsonObject row;
						int num = query.numFields();
						for (int n = 0; n < num; n++)
						{
							auto k = query.fieldName(n);
							auto kType = query.fieldDataType(n);
							if (SQLITE_INTEGER == kType)
							{
								int64 v = query.getInt64Field(k, 0);
								row.Add(k, v);
							}
							else if (SQLITE_TEXT == kType)
							{
								auto v = query.getStringField(k);
								row.Add(k, v);
							}
							else if (SQLITE_FLOAT == kType)
							{
								auto v = query.getFloatField(k);
								row.Add(k, v);
							}
							else if (SQLITE_BLOB == kType)
							{
								//int nLen = 0;
								//auto v = query.getBlobField(k, nLen);
								//将数据拷贝到由我们控制的BLOB数据内存中管理
								//row.Add新的BLOB数据地址
								//row.Add(k, (uint64)v);
							}
							else if (SQLITE_NULL == kType)
							{
								//row.Add(k, "NULL");
							}
						}
						json.Add(row);
						query.nextRow();
					}
				}
				catch (CppSQLite3Exception& e)
				{
					CELLLog_Error("DBManager::query2json(%s) error: %s", _db_name.c_str(), e.errorMessage());
					return false;
				}
				return true;
			}

			bool execQuery(const char* sql, neb::CJsonObject& json)
			{
				CppSQLite3Query query;
				try
				{
					query = _db.execQuery(sql);
				}
				catch (CppSQLite3Exception& e)
				{
					CELLLog_Error("DBManager::execQuery(%s) sql:%s error: %s", _db_name.c_str(), sql, e.errorMessage());
					return false;
				}
				return query2json(query, json);
			}

			bool execQuery(const char* sql)
			{
				CppSQLite3Query query;
				try
				{
					query = _db.execQuery(sql);
					return !query.eof();
				}
				catch (CppSQLite3Exception& e)
				{
					CELLLog_Error("DBManager::execQuery(%s) sql:%s error: %s", _db_name.c_str(), sql, e.errorMessage());
				}
				return false;
			}

			int execDML(const char* sql)
			{
				try
				{
					return _db.execDML(sql);
				}
				catch (CppSQLite3Exception& e)
				{
					CELLLog_Error("DBManager::execDML(%s) sql:%s error: %s", _db_name.c_str(), sql, e.errorMessage());
				}
				return -1;
			}

			bool hasByKV(const char* table, const char* k, const char* v)
			{
				char sql_buff[1024] = {};
				auto sql = "SELECT 1 FROM %s WHERE %s='%s' LIMIT 1;";
				sprintf(sql_buff, sql, table, k, v);
				return execQuery(sql_buff);
			}

			bool findByKV(const char* table, const char* k, const char* v, neb::CJsonObject& json)
			{
				char sql_buff[1024] = {};
				auto sql = "SELECT * FROM %s WHERE %s='%s';";
				sprintf(sql_buff, sql, table, k, v);
				return execQuery(sql_buff, json);
			}

			bool findByKV(const char* table, const char* k, int v, neb::CJsonObject& json)
			{
				auto sql = "SELECT * FROM %s WHERE %s=%d;";
				auto sql_buff = Log::newFormatStr(sql, table, k, v);
				bool ret = execQuery(sql_buff, json);
				delete[] sql_buff;
				return ret;
			}

			bool findByKV(const char* table, const char* k, int64 v, neb::CJsonObject& json)
			{
				char sql_buff[1024] = {};
#ifdef _WIN32
				auto sql = "SELECT * FROM %s WHERE %s=%I64d;";
#else
				auto sql = "SELECT * FROM %s WHERE %s=%Ild;";
#endif
				sprintf(sql_buff, sql, table, k, v);
				return execQuery(sql_buff, json);
			}

			int updateByKV(const char* table, const char* k, const char* v, const char* uk, int64 uv)
			{
				char sql_buff[1024] = {};
#ifdef _WIN32
				auto sql = "UPDATE %s SET %s=%I64d WHERE %s='%s';";
#else
				auto sql = "UPDATE %s SET %s=%lld WHERE %s='%s';";
#endif
				sprintf(sql_buff, sql, table, uk, uv, k, v);
				return execDML(sql_buff);
			}

			int updateByKV(const char* table, const char* k, const char* v, const char* uk, int uv)
			{
				char sql_buff[1024] = {};
#ifdef _WIN32
				auto sql = "UPDATE %s SET %s=%d WHERE %s='%s';";
#else
				auto sql = "UPDATE %s SET %s=%d WHERE %s='%s';";
#endif
				sprintf(sql_buff, sql, table, uk, uv, k, v);
				return execDML(sql_buff);
			}

			int updateByKV(const char* table, const char* k, const char* v, const char* uk, const char* uv)
			{
				char sql_buff[1024] = {};
				auto sql = "UPDATE %s SET %s='%s' WHERE %s='%s';";
				sprintf(sql_buff, sql, table, uk, uv, k, v);
				return execDML(sql_buff);
			}
		};
	}
}
#endif // !_doyou_io_DBManager_HPP_
