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
#include <time.h>
#include <sys/time.h>

using namespace Json;
using namespace std;
using namespace Helper;

#define __WORKER_SLEEP_TIME 10 * 1000 // 10ms
#define NOW()				time(NULL)

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
__order2json(CMysqlHelper* mysql, string& json, const order_data_t& order)
{
	CMysqlResult 	*result;
	Value			value;
	struct timeval	tv;
	char			sql[128];
	FastWriter		writer;

	snprintf(sql, sizeof sql, "select * from %s where pid = %lu", order.table.c_str(), order.id);
	log_debug("ORDER: %s", sql);
	if (mysql->IsConnected()) {
		result = mysql->ExecuteQuery(sql);
		if (result && result->HasNext()) {
			value["id"] 			= (UInt64)result->GetLong(0);
			value["pid"] 			= (UInt64)result->GetLong(1);
			value["mid"] 			= (UInt)result->GetInt(2);
			value["sitmid"] 		= result->GetString(3);
			value["buyer"] 			= result->GetString(4);
			value["sid"] 			= result->GetInt(5);
			value["appid"] 			= result->GetInt(6);
			value["pmode"] 			= result->GetInt(7);
			value["pamount"]		= result->GetFloat(8);
			value["pcoins"]			= (UInt)result->GetInt(9);
			value["pchips"]			= (UInt64)result->GetLong(10);
			value["pcard"]			= (UInt)result->GetInt(11);
			value["pnum"]			= result->GetInt(12);
			value["payconfid"]		= (UInt)result->GetInt(13);
			value["pcoinsnow"]		= (UInt)result->GetInt(14);
			value["pdealno"]		= result->GetString(15);
			value["pbankno"]		= result->GetString(16);
			value["desc"]			= result->GetString(17);
			value["pstarttime"]		= (UInt)result->GetInt(18);
			value["pendtime"]		= (UInt)result->GetInt(19);
			value["pstatus"]		= result->GetInt(20);
			value["pamount_rate"] 	= result->GetFloat(21);
			value["pamount_unit"] 	= result->GetString(22);
			value["pamount_usd"]	= result->GetFloat(23);
			value["ext_1"]			= result->GetInt(24);
			value["ext_2"]			= result->GetInt(25);
			value["ext_3"]			= result->GetInt(26);
			value["ext_4"]			= result->GetString(27); 
			value["ext_5"]			= result->GetString(28);
			value["ext_6"]			= result->GetString(29);
			value["ext_7"]			= result->GetString(30);
			value["ext_8"]			= result->GetString(31);
			value["ext_9"]			= result->GetString(32);
			value["ext_10"]  		= result->GetString(33);

			gettimeofday(&tv, NULL);
			value["lts_at"] 		= (UInt64)tv.tv_sec;
			value["m_at"]			= (UInt64)(tv.tv_sec * 1000 + tv.tv_usec);

			json = writer.write(value);
			delete result;
		}

	}

	return 0;
}

static inline int
__sync_order(const order_data_t& order, CRedisHelper* redis, const string& sql)
{
	Value 		value;
	FastWriter 	writer;
	string		json;

	value["id"] = (UInt64)order.id;
	value["table"] = order.table;
	value["sql"] = sql;
	value["stime"] = (UInt64)NOW();
	value["type"] = 1;

	json = writer.write(value);
	log_debug("SYNC: %s", json.c_str());

	if (redis->IsActived()) {
		redis->Enqueue("ORDER_NEW_Q", json);
	}

	return 0;
}

static inline int
order2db(const order_data_t& order, CMysqlHelper* mysql, CRedisHelper* redis)
{
	ostringstream 	os;
	unsigned long 	err;
	string 			sql;
	char			buff[1024];
	string			line;

	if (mysql->IsConnected()) {
		os << "update " << order.table << " set pstatus = " << order.status << ", `desc` = concat(`desc`,'|48|" 
			<< order.status << "'), pendtime = " << order.time << ", ext_1 = " << order.time 
				<< " where pid = " << order.id;
		sql = os.str();

		log_debug("SQL: %s", sql.c_str());

		err = mysql->ExecuteNonQuery(sql);

		__sync_order(order, redis, sql);

		__order2json(mysql, line, order);

		snprintf(buff, sizeof buff, "E303EAA51D09FB12CC9F2679812B9858|%lu|update_order_info\t%s",
				 NOW(), line.c_str());
		if (redis && redis->IsActived()) {
			redis->Enqueue("ORDER_UPDATE_LINE_Q", buff);
			log_debug("LINE: %s", buff);
		}

		return err;
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
			<< ", " << 7 << ", " << order.status << ", " << order.time << ", " << order.time << ")";
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
			usleep(__WORKER_SLEEP_TIME);
			return 0;
		}

		log_debug("JSON: %s", data.c_str());

		if (json2order(data, order) == -1) {
			log_error("json2order failed.");

			return -1;
		}

		order2db(order, mysql, redis);

		if (order.status != 2) {
			order2except(order, mysql);
		}
	}

	return 0;
}


