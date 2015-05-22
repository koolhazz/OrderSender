#include "log.h"
#include "conf.h"
#include "IniFile.h"
#include "RedisHelper.h"
#include "record.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static int
__init_conf(conf_t* conf, const char* file)
{
	if (conf) {
		read_profile_string("redis",
							"host",
							conf->redis_host,
							sizeof conf->redis_host,
							"",
							file);
		read_profile_string("redis",
							"key",
							conf->redis_key,
							sizeof conf->redis_key,
							"",
							file);
		conf->redis_port = read_profile_int("redis", "port", 0, file);
	}

	return 0;
}

conf_t 			*g_conf;
redis_helper_t 	*g_redis;
bool 			is_daemon = true;

int
main(int argc, char** argv)
{
	init_log("", "log/");
	
	if (daemon(1, 0) == -1) {
		log_error("daemon failed.");
		return -1;	
	}

	NEW_CONF(g_conf);
	__init_conf(g_conf, "etc/server.ini");
	PRT_CONF(g_conf);

	g_redis = new redis_helper_t(g_conf->redis_host, g_conf->redis_port);
	if (g_redis == NULL) {
		log_error("redis creat failed.");
		return -1;
	}
	g_redis->Connect();
	if (!g_redis->IsActived()) {
		log_error("redis not actived.");
		return -1;
	}

	while (1) {
		order_log_recording(g_redis);
		usleep(5 * 1000);
	}

	return 0;
}
