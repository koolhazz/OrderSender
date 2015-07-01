#include "SyncFunc.h"
#include "Log.h"
#include "RedisHelper.h"
#include "ConfigDef.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

conf_t 	g_conf;
bool 	is_daemon = true;
#define __CONF_FILE "etc/server.ini"

int
main(int argc, char** argv)
{
	char 			log_name[256];
	CRedisHelper 	*producer;
	CRedisHelper 	*consumers;

	if (daemon(1, 0)) {
		log_error("daemon failed.");
		return -1;
	}

	memset(log_name, 0, sizeof log_name);
	snprintf(log_name, sizeof log_name, "Log_%d", getpid());
	init_log(log_name, "log/");

	load_conf(__CONF_FILE, &g_conf);
	print_conf(&g_conf);

	producer = new CRedisHelper(g_conf.producer.host, g_conf.producer.port, g_conf.producer.timeout);
	if (producer == NULL) {
		log_error("producer create failed.");
		return -1;
	}
	if (!producer->IsActived()) {
		log_error("prodecer not actived.");
		return -1;
	}

	consumers = new CRedisHelper[g_conf.redis_nums];
	for (int i = 0; i < g_conf.redis_nums; i++) {
		(consumers + i)->Connect((g_conf.redis_list + i)->host, 
								 (g_conf.redis_list + i)->port, 
								 5);
		if (!(consumers + i)->IsActived()) {
			log_error("consumers %d not actived.", i);
			return -1;
		}
	}

	while (1) {
		order_trans(consumers, g_conf.redis_nums, producer);
	}

	return 0;	
}

