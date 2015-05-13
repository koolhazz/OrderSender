#ifndef __CONF_H_
#define __CONF_H_

#include <stdlib.h>

typedef struct conf_s conf_t;
struct conf_s {
	char 			redis_ip[16];
	char 			redis_key[64];
	char			redis_log_key[64];
	char 			redis_except_key[64];
	char			redis_update_key[64];
	unsigned short 	redis_port;
};
#define NEW_CONF(c) do { 					\
	c = (conf_t*)malloc(sizeof(conf_t));	\
	if (c) {								\
		memset(c, 0, sizeof *c);			\
	}										\
} while(0)
#define DEL_CONF(c) free(c)	

#endif
