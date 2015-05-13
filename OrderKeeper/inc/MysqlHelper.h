#ifndef __MYSQLHELPER_H__
#define __MYSQLHELPER_H__

#include <mysql.h>
#include <string>

namespace Helper
{

class CMysqlResult;

class CMysqlHelper
{
public:
	CMysqlHelper(const std::string& host, unsigned int port, const std::string& user, 
		const std::string& passwd);
	CMysqlHelper(const char* host, unsigned int port, const char* user, const char* passwd);	
	~CMysqlHelper();

	int Connect();
	int UseDB(const std::string& db);

	CMysqlResult* ExecuteQuery(const std::string& sql);
	unsigned long ExecuteNonQuery(const std::string& sql);

	bool IsConnected();

	unsigned int GetErrNo() { return mysql_errno(&mysql_);}
	const char* GetErrMsg() { return mysql_error(&mysql_);}

	//static CMysqlHelper* NewHelper();
	
private:
	std::string host_;
	unsigned int port_;
	std::string user_;
	std::string passwd_;

	MYSQL mysql_;
};

class CMysqlResult
{
public:
	CMysqlResult(MYSQL_RES* res);
	~CMysqlResult();

	bool HasNext();

	char* GetChar(int col);
	int	GetInt(int col);
	float GetFloat(int col);

private:
	MYSQL_RES* res_;
	MYSQL_ROW  row_;
};
}
#endif
