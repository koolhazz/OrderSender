#include "MysqlHelper.h"

//#include "ConfigDef.h"

#include <stdlib.h>
#include <mysql.h>


namespace Helper
{
CMysqlHelper::CMysqlHelper(const std::string& host, unsigned int port, const std::string& user, 
	const std::string& passwd)
	:host_(host),
	port_(port),
	user_(user),
	passwd_(passwd)
{
	mysql_init(&mysql_);
}

CMysqlHelper::CMysqlHelper(const char * host, unsigned int port, const char * user, const char * passwd)
	:host_(host),
	port_(port),
	user_(user),
	passwd_(passwd)
{
	mysql_init(&mysql_);
}
	

CMysqlHelper::~CMysqlHelper()
{
	mysql_close(&mysql_);
}

int
CMysqlHelper::Connect()
{
//	if (mysql_real_connect(&mysql_, host_.c_str(), user_.c_str(), passwd_.c_str(), 
//		NULL, port_, NULL, 0))

	if (mysql_real_connect(&mysql_, host_.c_str(), user_.c_str(), passwd_.c_str(), 
		NULL, port_, NULL, CLIENT_MULTI_RESULTS|CLIENT_MULTI_STATEMENTS))
	{
		return 0;//³É¹¦
	}
	else
	{
		return -1;//Ê§°Ü
	}
}

int 
CMysqlHelper::UseDB(const std::string& db)
{
	return mysql_select_db(&mysql_, db.c_str());
}

CMysqlResult*
CMysqlHelper::ExecuteQuery(const std::string& sql)
{
	int r = 0;

	if ( (r = mysql_query(&mysql_, sql.c_str())) != 0)
	{
		return NULL;
	}

	MYSQL_RES* res = mysql_store_result(&mysql_);

	if ( (r = mysql_next_result(&mysql_)) == 0)
	{
		MYSQL_RES* res2 = mysql_store_result(&mysql_);
		mysql_free_result(res2);
	}


	if (NULL == res)
		return NULL;

	return new CMysqlResult(res);
}

unsigned long
CMysqlHelper::ExecuteNonQuery(const std::string& sql)
{
	int res = mysql_query(&mysql_, sql.c_str());

	if (res)
	{
		MYSQL_RES* result = mysql_store_result(&mysql_);
		
		if ( result != NULL)
		{
			return mysql_affected_rows(&mysql_);
		}
		else
		{
			return 0;
		}
	}
	else
		return res;
}

bool
CMysqlHelper::IsConnected()
{
	int r = mysql_ping(&mysql_);

	if (0 == r)
		return true;
	else
		return false;
}

/*
CMysqlHelper*
CMysqlHelper::NewHelper()
{

	std::string host("172.30.8.210");
	std::string user("tbttdev");
	std::string passwd("123456");


	std::string host(g_Config->db->host);
	std::string user(g_Config->db->user);
	std::string passwd(g_Config->db->passwd);
	int port = g_Config->db->port;

	return new CMysqlHelper(host, port, user, passwd);
}
*/



/*CMysqlResult*/

CMysqlResult::CMysqlResult(MYSQL_RES* res)
	:res_(res)
{}

CMysqlResult::~CMysqlResult()
{
	mysql_free_result(res_);
}

bool
CMysqlResult::HasNext()
{
	if ( (row_ = mysql_fetch_row(res_)))
		return true;
	else
		return false;
}

char*
CMysqlResult::GetChar(int col)
{
	return row_[col];
}

int
CMysqlResult::GetInt(int col)
{
	return atoi(row_[col]);
}

float
CMysqlResult::GetFloat(int col)
{
	return atof(row_[col]);
}

}

