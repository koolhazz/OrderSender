#include "SyncFunc.h"
#include "RedisHelper.h"
#include "MysqlHelper.h"
#include "json/json.h"
#include "Log.h"
#include "ConfigDef.h"

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdint.h>

using namespace Json;
using namespace std;
using namespace Helper;

extern redis_conf_t redis_conf;

typedef struct order_data_s order_data_t;
struct order_data_s {
	uint64_t 	id;
	uint64_t 	errorcode;
	uint64_t 	httpcode;
	int64_t		time;
	int	 		status;
	string 		url;
	string 		table;
	string 		errormsg;
};

static inline int
json2order(const string& json, order_data_t& order)
{
	Value 	value;
	Reader 	reader;

	if (reader.parse(json, value)) {
		order.id 		= value["id"].asUInt64();
		order.url 		= value["url"].asString();
		order.status 	= value["status"].asInt();
		order.httpcode 	= value["httpcode"].asInt64();
		order.errorcode = value["errorcode"].asUInt64();
		order.errormsg 	= value["errormsg"].asString();
		order.table 	= value["table"].asString();
		order.time 		= value["time"].asInt64();
	} else {
		return -1;
	}

	return 0;
}

static inline int
order2db(const order_data_t& order, CMysqlHelper* mysql)
{
	ostringstream 	os;

	if (mysql->IsConnected()) {
		os << "update " << order.table << " set pstatus = " << order.status << ", `desc` = '48|" 
			<< order.status << "', pendtime = " << order.time << " where pid = " << order.id << endl;
		
		log_debug("SQL: %s", os.str().c_str());

		return mysql->ExecuteNonQuery(os.str());
	}

	return 0;
}

static inline int
order2except(const order_data_t& order, CMysqlHelper* mysql)
{
	ostringstream os;

	if (mysql->IsConnected()) {
		os << "insert into paycenter_order_badcgi_log(pid, sid, appid, httpcode, frequency, status, createstamp, overstamp) values(" 
			<< order.id << ", " << 0 << ", " << 0 << ", " << order.httpcode 
			<< ", " << 7 << ", " << order.status << ", " << order.time << ", " << order.time << ")" << endl;
		log_debug("SQL: %s", os.str().c_str());

		return mysql->ExecuteNonQuery(os.str()); 
	}

	return 0;
}

int 
sync_order_data(CMysqlHelper* mysql, CRedisHelper* redis)
{
	string 			data;
	Value			json;
	Reader			reader;
	order_data_t 	order;

	if (redis->IsActived()) {
		redis->Dequeue(redis_conf.key, data);

		if (data.empty()) {
			return 0;
		}

		log_debug("JSON: %s", data.c_str());

		if (json2order(data, order) == -1) {
			log_error("json2order failed.");

			return -1;
		}

		order2db(order, mysql);

		if (order.status != 2) {
			order2except(order, mysql);
		}
	}

	return 0;
}


