#include "IniFile.h"
#include "ConfigDef.h"
#include "Log.h"

#include <stdio.h>
#include <string.h>
#include <string>

using std::string;

int
load_conf(const char* file, conf_t* c)
{
	char 			node[32];
	redis_conf_t 	*rc;
	
	c->redis_nums = read_profile_int("server", "redis_nums", 0, file);
	c->redis_list = (redis_conf_t*)malloc(sizeof(*c->redis_list) * c->redis_nums);
	rc = c->redis_list;
	
	for (int i = 0; i < c->redis_nums; i++) {
		snprintf(node, sizeof node, "redis%d", i);
		read_profile_string(node, "host", (rc + i)->host, sizeof (rc + i)->host, "", file);
		read_profile_string(node, "key", (rc + i)->key, sizeof (rc + i)->key, "", file);
		(rc + i)->port = read_profile_int(node, "port", 0, file);
		(rc + i)->timeout = read_profile_int(node, "timeout", 0, file);
	}

	read_profile_string("producer", "host", c->producer.host, sizeof c->producer.host, "", file);
	read_profile_string("producer", "key", c->producer.key, sizeof c->producer.key, "", file);
	c->producer.port = read_profile_int("producer", "port", 0, file);
	c->producer.timeout = read_profile_int("producer", "timeout", 0, file);
	
	return 0;
}

void
print_conf(conf_t* c)
{
	log_debug("redis_nums: %d", c->redis_nums);
	for (int i = 0; i < c->redis_nums; i++) {
		log_debug("redis[%d]:host %s", i, (c->redis_list + i)->host);
		log_debug("redis[%d]:port %d", i, (c->redis_list + i)->port); //c->redis_list[i].port
		log_debug("redis[%d]:key %s", i, (c->redis_list + i)->key);
		log_debug("redis[%d]:timeout %d", i, (c->redis_list + i)->timeout);
	}

	log_debug("producer:host %s", c->producer.host);
	log_debug("producer:port %d", c->producer.port); 
	log_debug("producer:key %s", c->producer.key);
	log_debug("producer:timeout %d", c->producer.timeout);
}

