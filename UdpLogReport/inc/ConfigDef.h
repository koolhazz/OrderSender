#ifndef __CONFIG_DEF_H_
#define __CONFIG_DEF_H_

typedef struct 
{
	 char		host[15];
	 unsigned short port;
}ERedisConf, *pERedisConf;

typedef struct 
{
	 char		host[20];
	 unsigned short port;
}EUDPConf, *pEUDPConf;


typedef struct
{
	 char 	url[200];
	 char	md5[50];
}EInterfaceConf, *pEInterfaceConf;

typedef struct
{
	 char	name[50];
}EQueueConf, *pEQueueConf;

extern
int
GetRedisConf(pERedisConf conf, const char* file);

extern
int
GetInterfaceConf(pEInterfaceConf conf, const char* file);

extern
int
GetPhpQueueConf(pEQueueConf conf, const char* file);

extern
int
GetServerQueueConf(pEQueueConf conf, const char* file);


extern
int
GetUDPConf(pEUDPConf conf, const char* file);

#endif
