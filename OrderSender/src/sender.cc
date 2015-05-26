#include "sender.h"
#include "RedisHelper.h"
#include "def.h"
#include "json/value.h"
#include "json/writer.h"
#include "json/reader.h"
#include "log.h"
#include "conf.h"
#include "timer_handler.h"
#include "timer.h"

#include <string>
#include <curl/curl.h>
#include <stdlib.h>
#include <stddef.h>

using std::string;
using namespace Json;


extern conf_t 				*conf;
typedef class CRedisHelper 	redis_helper_t;
extern redis_helper_t 		*redis;
extern char					*log_buff;

#ifndef __SEND_LOG
#define __SEND_LOG(o, hc, rc) do {											\
	if (redis && redis->IsActived()) {										\
		snprintf(log_buff, __LOG_BUFF_SZ, 											\
				 "{\"url\":\"%s\", \"errorcode\":%ld, \"httpcode\":%ld}", 		\
				 o->url.c_str(), 											\
				 rc, 														\
				 hc);														\
		redis->Enqueue(conf->redis_log_key, log_buff);								\
	}																		\
} while (0)																	
#endif

sender_t::sender_t(redis_helper_t* redis)
	:_redis(redis),
	_retry(0)
{}

sender_t::~sender_t()
{
	delete _redis;
}

int
sender_t::run()
{
	string 			value;
	Reader 			reader;
	Value 			json;
	FastWriter		writer;
	CURL			*curl = NULL;
	CURLcode 		res;
	long			http_code;
	long			rsp_code;
	order_data_t 	*o;

	//__BEGIN__(__func__);
	http_code = rsp_code = 0;

	if (_redis) {
		if (_redis->IsActived()) {
			_redis->Dequeue(conf->redis_key, value);

			if (!value.empty() && reader.parse(value, json)) {
				log_debug("JSON: %s", value.c_str());
				NEW_OD(json, o);
				
				curl = curl_easy_init();
				
				curl_easy_setopt(curl, CURLOPT_URL, o->url.c_str());
				curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
				curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_data);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				//curl_easy_setopt(curl, CURLOPT_HEADERDATA, &rsp_code);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rsp_code);

				res = curl_easy_perform(curl);
				if (res == CURLE_OK) {
					curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

					if (http_code == 200) {
						/* 判断返回值 */
						switch (rsp_code) {
							case RSP_SUCCESS:
							case RSP_AGAIN:
							case RSP_DUP_ORDER:
								/* 订单状态为 2 */
								__SEND_LOG(o, http_code, rsp_code);
								if (redis && redis->IsActived()) {
									json["id"] = (UInt64)o->id;
									json["url"] = o->url;
									json["table"] = o->table;
									json["status"] = ORDER_FINISH;
									json["errcode"] = (Int64)rsp_code;
									json["errmsg"] = "";
									json["httpcode"] = (Int64)http_code;
									json["time"] = (Int64)NOW();
									
									redis->Enqueue("ORDER_UPDATE_Q", writer.write(json).c_str());
								}
								DEL_OD(o); // 成功后删除
								
								break;
							default:
								/* 订单状态为 53 */
								__SEND_LOG(o, http_code, rsp_code);
								timer_start(RTV_VALUE(0), 
											timeout_request, o, sizeof *o, false);
								break;
						}
					} else {
						__SEND_LOG(o, http_code, rsp_code);
						timer_start(RTV_VALUE(0), 
							timeout_request, o, sizeof *o, false);
					}
				} else {
					__SEND_LOG(o, http_code, rsp_code);
					timer_start(RTV_VALUE(0), 
						timeout_request, o, sizeof *o, false);		
				}

				curl_easy_cleanup(curl);
			} else { // json is empty
				usleep(5 * 1000); // 5ms sleep
			}
		}
	}

	//__END__(__func__, 0);
	return 0;
}


