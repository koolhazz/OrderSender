#include "timer_handler.h"
#include "timer.h"
#include "def.h"
#include "conf.h"
#include "log.h"
#include "json/value.h"
#include "json/writer.h"
#include "json/reader.h"
#include "RedisHelper.h"
#include <curl/curl.h>

using namespace Json;

typedef class CRedisHelper 	redis_helper_t;
extern redis_helper_t 		*redis;
extern conf_t 				*conf;
extern char					*log_buff;

#ifndef __SEND_LOG
#define __SEND_LOG(o, hc, rc) do {											\
	if (redis && redis->IsActived()) {										\
		snprintf(log_buff, __LOG_BUFF_SZ, 									\
				 "{\"url\":\"%s\", \"errorcode\":%ld, \"httpcode\":%ld}",		\
				 o->url.c_str(), 											\
				 rc, 														\
				 hc);														\
		redis->Enqueue(conf->redis_log_key, log_buff);						\
	}																		\
} while (0)																	
#endif

#ifndef __REDO_CHECK
#define __REDO_CHECK(o, errno, v, stat) do {									\
	if (o->times > 6) {															\
		v["id"] = (UInt64)o->id;												\
		v["url"] = o->url;														\
		v["table"] = o->table;													\
		v["status"] = stat;														\
		v["errcode"] = (Int64)errno;											\
		v["errmsg"] = "";														\
		v["time"] = (Int64)NOW();												\
		v["retry"] = o->times;													\
		if (redis && redis->IsActived()) {										\
			redis->Enqueue(conf->redis_except_key, writer.write(v).c_str());	\
			redis->Enqueue(conf->redis_update_key, writer.write(v).c_str());	\
			DEL_OD(o);															\
		}																		\
	} else {																	\
		timer_start(RTV_VALUE(o->times), 										\
			timeout_request, o, sizeof *o, false);								\
	}																			\
} while (0)
#endif

size_t
header_data(char *buffer, 
			size_t size, 
			size_t nitems, 
			void* userdata)
{
	return size * nitems;
}

size_t
write_data(char* buffer, 
		   size_t size, 
		   size_t nitems, 
		   void* userdata)
{
	*((long*)userdata) = atol(buffer);

	return size * nitems;
}


void
timeout_request(void* arg)
{
	order_data_t 	*o;
	long 			rsp_code;
	long			http_code;
	CURLcode		curl_code;
	CURL			*curl;
	Value			value;
	FastWriter		writer;

	__BEGIN__(__func__);
		
	o = (order_data_t*)arg;
	o->times++;
	PRT_OD(o);

	curl = curl_easy_init();
	
	curl_easy_setopt(curl, CURLOPT_URL, o->url.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_data);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rsp_code);

	curl_code = curl_easy_perform(curl);
	if (curl_code == CURLE_OK) {
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		if (http_code == 200) {
			switch (rsp_code) {
				case -1:
				case 1:
				case 7:
					/* 更新队列处理 */
					__SEND_LOG(o, http_code, rsp_code);
					if (redis && redis->IsActived()) {
						value["id"] = (UInt64)o->id;
						value["url"] = o->url;
						value["table"] = o->table;
						value["status"] = 2;
						value["errcode"] = (Int64)rsp_code;
						value["errmsg"] = "";
						value["httpcode"] = (Int64)http_code;
						value["time"] = (Int64)NOW();

						redis->Enqueue("ORDER_UPDATE_Q", writer.write(value).c_str());
					}
					break;
				default :
					__SEND_LOG(o, http_code, rsp_code);
					__REDO_CHECK(o, rsp_code, value, 53);
					break;
			}
		} else { //http_code != 200
			__SEND_LOG(o, http_code, rsp_code);
			__REDO_CHECK(o, http_code, value, 54);
		}
	}
	
	curl_easy_cleanup(curl);

	__END__(__func__, 0);

	return;
}



