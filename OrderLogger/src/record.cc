#include "record.h"
#include "log.h"
#include "RedisHelper.h"
#include "conf.h"
#include "json/json.h"
#include <string>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

using std::string;
using namespace Json;

#define __FMT_BUFF_SZ 32 * 1024 // 1M
#define NEW_FB(fb) fb = (char*)calloc(1, __FMT_BUFF_SZ)
#define DEL_FB(fb) do {	\
	free(fb);			\
	fb = NULL;			\
} while (0)

typedef class CRedisHelper redis_helper_t;
extern conf_t *g_conf;
static char* fmt_buff;

static const char* __LOG_FORMAT = "记录时间:%s 类型:发货结果 进程PID:%d 服务器IP:0.0.0.0 客户端IP:0.0.0.0 脚本入口:/data/public/wwwroot/paycenter/webapp/pay.boyaa.com/web/facebook/inc/local_currency_payment.php,line:155 日志调用:0  用户IP:0 用户ID:0 订单号:0 商品ID:0 商品名称:游戏币 支付渠道:0 支付PMODE:0 应用名称:0 应用ID:0 支付金额:0  日志内容:发货地址:%s 返回结果:%ld    业务cgi响应码:%ld 环境参数:";

static string __attribute__((always_inline))
__date()
{
	time_t 		t;
	struct tm 	tm;
	char 		temp[20];

	time(&t);
	localtime_r(&t, &tm);

	snprintf(temp, sizeof temp, "%4d-%02d-%02d %02d:%02d:%02d", 
			 tm.tm_year + 1900,
			 tm.tm_mon + 1,
			 tm.tm_mday,
			 tm.tm_hour,
			 tm.tm_min,
			 tm.tm_sec);

	return temp;
}


int
order_log_recording(redis_helper_t* redis)
{
	string 	log;
	Value	value;
	Reader 	reader;

	if (redis && redis->IsActived()) {
		redis->Dequeue(g_conf->redis_key, log);
		if (!log.empty() && reader.parse(log, value)) {
			log_debug(__LOG_FORMAT, __date().c_str(),
					  getpid(),
					  value["url"].asCString(),
					  value["errorcode"].asInt64(),
					  value["httpcode"].asInt64());
					  
			if (fmt_buff == NULL) NEW_FB(fmt_buff);
			snprintf(fmt_buff, 
					 __FMT_BUFF_SZ, 
					 __LOG_FORMAT, 
					 __date().c_str(),
					 getpid(),
				     value["url"].asCString(),
				     value["errorcode"].asInt64(),
				     value["httpcode"].asInt64());
			redis->Enqueue(g_conf->redis_udp_key, fmt_buff);
		}
	}		

	return 0;
}

