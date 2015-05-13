#ifndef __CONF_H_
#define __CONF_H_

typedef struct conf_s conf_t;
struct conf_s {
	char 			redis_host[16];
	unsigned short 	redis_port;
	char			redis_key[64];
};
#define NEW_CONF(c) do {			\
	c = (conf_t*)malloc(sizeof *c);	\
	if (c) memset(c, 0, sizeof *c);	\
} while (0) 
#define PRT_CONF(c) do {							\
	log_info("redis->host: %s", c->redis_host);	\
	log_info("redis->port: %d", c->redis_port);	\
	log_info("redis->key: %s", c->redis_key);		\
} while (0)

#endif
