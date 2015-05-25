#include "RedisHelper.h"
#include "Log.h"
#include "ConfigDef.h"
#include "UDPClass.h"
#include "json/json.h"

#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>

using namespace std;
using namespace Json;
using namespace Helper;

static const char* const CONFIG_FILE = "../etc/server.ini";
static const unsigned int SLEEP_TIME = 1000 * 1000; // second

bool is_daemon = false;

static ERedisConf redis_conf;
static EInterfaceConf interface_conf;
static EQueueConf php_queue_conf;
static EQueueConf server_queue_conf;
static EUDPConf UDP_conf;

static
void
print_conf();

static
void 
CDWorkDir(const char * path);

int
main(int argc, char** argv)
{
    CDWorkDir(argv[0]);
	 
#ifdef _DAEMON_
    if (daemon(1, 0) == 0)
    {
        is_daemon = true;
    }
#endif

    char log_name[30] = {0};
	
    snprintf(log_name, 30, "Log_%d", getpid());

    init_log(log_name, "../log");
	 
    log_debug("server running...\n");
	 	 
    GetRedisConf(&redis_conf, CONFIG_FILE);
    //GetInterfaceConf(&interface_conf, CONFIG_FILE);
    GetPhpQueueConf(&php_queue_conf, CONFIG_FILE);
    GetServerQueueConf(&server_queue_conf, CONFIG_FILE);
    GetUDPConf(&UDP_conf,CONFIG_FILE);

    print_conf();
	
    Helper::CRedisHelper redis(redis_conf.host, redis_conf.port);

	CUDP UDPDataSend;
	if(UDPDataSend.UDPCreateSocket()==0)
	{
		UDPDataSend.UDPConnect(UDP_conf.host,UDP_conf.port);
	}

	string json; 
	
		
	while (1) {
		if (!redis.IsActived()) {
			log_debug("redis reconnect.\n");
			redis.Connect();
		}
		
		json = redis.Dequeue(server_queue_conf.name, json);
		
		if (json.empty()) {
			usleep(SLEEP_TIME);
			continue;
	    } else {
			log_debug("JSON: %s\n", json.c_str());
		}
		
		if(UDPDataSend.UDPSend((void *)(json.c_str()), json.length()) < 0 ) {
			if(UDPDataSend.UDPCreateSocket() == 0) {
				UDPDataSend.UDPConnect(UDP_conf.host,UDP_conf.port);
			}

			UDPDataSend.UDPSend((void *)(json.c_str()), json.length());
		}

		json.clear();
		
	}

	UDPDataSend.CloseFd();

    return 0;
}

void print_conf()
{
	 log_debug("----------------------------------------\n");
	 log_debug("redis->host: %s\n", redis_conf.host);
	 log_debug("redis->port: %d\n", redis_conf.port);
	 log_debug("----------------------------------------\n");
	 log_debug("interface->url: %s\n", interface_conf.url);
	 log_debug("----------------------------------------\n");
	 log_debug("php queue->name: %s\n", php_queue_conf.name);
	 log_debug("----------------------------------------\n");
	 log_debug("udp->host: %s\n", UDP_conf.host);
	 log_debug("udp->port: %d\n", UDP_conf.port);
	 log_debug("----------------------------------------\n");
	 log_debug("server queue->name: %s\n", server_queue_conf.name);
	 log_debug("----------------------------------------\n");
}

void CDWorkDir(const char * path)
{
	char* rpath = realpath(path, NULL);

	char* p = dirname(rpath);

	int i = chdir(p);
	
	(void)i;

	free(rpath);
}
