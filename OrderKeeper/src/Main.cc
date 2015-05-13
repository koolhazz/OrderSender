#include "SyncFunc.h"
#include "MysqlHelper.h"
#include "RedisHelper.h"
#include "Log.h"
#include "ConfigDef.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>

using namespace Helper;

bool is_daemon = false;

static const char* const __CONF_FILE = "../etc/server.ini";

mysql_conf_t mysql_conf;
redis_conf_t redis_conf;

static 
int __init_conf();

static
void __printf_conf();

static 
void __chg_work_dir(const char* path);

int
main(int argc, char** argv)
{
	char LogName[100];
	
	__chg_work_dir(argv[0]);

	if (daemon(1, 0) == 0) {
		is_daemon = true;
	}

	snprintf(LogName, sizeof LogName, "Log_%d", getpid());
	init_log(LogName, "../log");
	
	__init_conf();
	__printf_conf();

	CMysqlHelper* mysql = new CMysqlHelper(mysql_conf.host, 
										   mysql_conf.port, 
		                                   mysql_conf.user, 
		                                   mysql_conf.passwd);
	mysql->Connect();
	mysql->UseDB(mysql_conf.db);
	if (!mysql->IsConnected()) {
		log_error("mysql connect failed.");
		return -1;
	}

	CRedisHelper* redis = new CRedisHelper(redis_conf.host, redis_conf.port, redis_conf.timeout);
	if (!redis->IsActived()) {
		log_error("redis connect failed.");
		return -1;
	}

	while (1) {
		sync_order_data(mysql, redis);
		usleep(1000000);
	}

	return 0;
}

int
__init_conf()
{
	memset(&mysql_conf, 0, sizeof mysql_conf);
	memset(&redis_conf, 0, sizeof redis_conf);

	load_mysql_conf(__CONF_FILE, &mysql_conf);
	load_redis_conf(__CONF_FILE, &redis_conf);

	return 0;
}

void
__printf_conf()
{
	log_debug("-------------------------------\n");
	log_debug("mysql->host: %s\n", mysql_conf.host);
	log_debug("mysql->port: %d\n", mysql_conf.port);
	log_debug("mysql->db: %s\n", mysql_conf.db);
	log_debug("mysql->user: %s\n", mysql_conf.user);
	log_debug("mysql->passwd: %s\n", mysql_conf.passwd);
	log_debug("-------------------------------\n");
	log_debug("redis->host: %s\n", redis_conf.host);
	log_debug("redis->port: %d\n", redis_conf.port);
	log_debug("redis->timeout: %d\n", redis_conf.timeout);
	log_debug("-------------------------------\n");
}

void __chg_work_dir(const char * path)
{
	char* rpath = realpath(path, NULL);

	char* p = dirname(rpath);

	chdir(p);

	free(rpath);
}
