#include "log.h"
#include "def.h"
#include "RedisHelper.h"
#include "sender.h"
#include "timer.h"
#include "conf.h"
#include "IniFile.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef class CRedisHelper redis_helper_t; 

#define UNUSED(x) (void)x
#define PCONF(c) do {											\
	if (c) {													\
		log_debug("redis_host: %s", c->redis_ip);				\
		log_debug("redis_port: %d", c->redis_port);				\
		log_debug("redis_key: %s", c->redis_key);				\
		log_debug("redis_log_key: %s", c->redis_log_key);		\
		log_debug("redis_update_key: %s", c->redis_update_key);	\
		log_debug("redis_except_key: %s", c->redis_except_key);	\
	}															\
} while (0)

bool 				is_daemon = true;
redis_helper_t 		*redis;
conf_t				*conf;
char 				*log_buff;


static void 
__load_conf(const char* file, conf_t* conf)
{
	if (conf) {
		read_profile_string("redis", "ip", conf->redis_ip, sizeof conf->redis_ip, "", file);
		conf->redis_port = read_profile_int("redis", "port", 0, file);
		read_profile_string("redis", 
							"key", 
							conf->redis_key, 
							sizeof conf->redis_key, 
							"", 
							file);
		read_profile_string("redis", 
							"log_key", 
							conf->redis_log_key, 
							sizeof conf->redis_log_key, 
							"", 
							file);
		read_profile_string("redis",
							"update_key",
							conf->redis_update_key,
							sizeof conf->redis_update_key,
							"",
							file);
		read_profile_string("redis",
							"except_key",
							conf->redis_except_key,
							sizeof conf->redis_except_key,
							"",
							file);
	}
}

int
main(int argc, char* argv[])
{
	sender_t 		*sender;
	char			log_name[256];
	
	if (daemon(1, 0)) {
		log_error("daemon failed.");
		return -1;
	}

	memset(log_name, 0, sizeof log_name);
	snprintf(log_name, sizeof log_name, "Log_%d", getpid());
	init_log(log_name, "log/");

	NEW_CONF(conf);
	__load_conf("etc/server.ini", conf);
	PCONF(conf);
	
	redis = new redis_helper_t(conf->redis_ip, conf->redis_port, 5);
	if (redis == NULL) {
		log_error("redis create failed.");
		return -1;
	}
	redis->Connect();
	if (!redis->IsActived()) {
		log_error("reids not actived.");
		return -1;
	}
		
	sender = new sender_t(redis);
	if (sender == NULL) {
		log_error("sender create failed.");
		return -1;
	}

	timer_init();

	NEW_LB(log_buff);
	
	while (1) {
		sender->run(); // 队列中获取待发货的订单信息
		timer_run();   // 执行定时器
	}

	// sender->run();
	// timer_run();

	// delete sender;
	// sender = NULL;
	// redis = NULL;
	// DEL_LB(log_buff);
	// timer_list_del();
	// DEL_CONF(conf);
	
	return 0;	
}



