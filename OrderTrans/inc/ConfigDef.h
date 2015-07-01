#ifndef __CONFIG_DEF_H_
#define __CONFIG_DEF_H_

typedef struct redis_conf_s redis_conf_t;
struct redis_conf_s {
	char			host[16];
	char 			key[64];
	unsigned short 	port;
	unsigned short 	timeout;
};

typedef struct conf_s conf_t;
struct conf_s {
	unsigned short 	redis_nums;
	redis_conf_t	*redis_list;
	redis_conf_t	producer;
};


int
load_conf(const char* file, conf_t* c);

void
print_conf(conf_t* c);
	
#endif

