#ifndef __CONFIG_DEF_H_
#define __CONFIG_DEF_H_

typedef struct mysql_conf_s mysql_conf_t;
struct mysql_conf_s {
	char 			host[16];
	unsigned short 	port;
	char			db[50];
	char			user[50];
	char			passwd[50];
};


typedef struct redis_conf_s redis_conf_t;
struct redis_conf_s {
	char			host[16];
	char 			key[64];
	unsigned short 	port;
	unsigned short 	timeout;
};

int 
load_mysql_conf(const char* file, mysql_conf_t* c);

int
load_redis_conf(const char* file, redis_conf_t* c);
	
#endif

