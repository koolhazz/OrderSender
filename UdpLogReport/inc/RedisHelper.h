
#ifndef __REDIS_HELPER_H_
#define __REDIS_HELPER_H_

#include "Log.h"
#include "hiredis/hiredis.h"

#include <string>

using namespace std;

namespace Helper
{

class CRedisHelper
{
public:
	CRedisHelper(const string& host, unsigned short port, unsigned short second = 5)
		:reply(0), host_(host), port_(port)
	{
		timeout.tv_sec = second;
		timeout.tv_usec = 0;
		
		context = redisConnectWithTimeout(host_.c_str(), port_, timeout);
	}

	~CRedisHelper() 
	{
		if(reply) freeReply();
		if(context) redisFree(context); 
	}
	
public:

	int Connect(const char* host, const unsigned short& port, const unsigned short second = 5);
	int Connect();

	int Set(const string& key, const string& value);
	string& Get(const string& key, string& value);

	int Enqueue(const string& queue, const string& value);
	string& Dequeue(const string& queue, string& value);

	int Push(const string& stack, const string& value);
	string& Pop(const string& stack, string& value);

	bool IsActived() { return ping(); }
	
private:
	redisContext* context;
	redisReply* reply;
	struct timeval timeout;

	string host_;
	unsigned short port_;

	void freeReply() 
	{ 
		if(reply) 
		{
			freeReplyObject(reply);
			reply = NULL;
		}
	}

	CRedisHelper(const CRedisHelper& rhs) {}  //����copy����

	CRedisHelper& operator =(const CRedisHelper& rsh) { return *this; } // ���ܸ�ֵ

	bool ping();
};
}
#endif
