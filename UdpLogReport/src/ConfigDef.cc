#include "ConfigDef.h"
#include "IniFile.h"

int GetRedisConf(pERedisConf conf, const char* file)
{
	 read_profile_string("redis", "host", conf->host, sizeof(conf->host), "", file);
	 conf->port =  read_profile_int("redis", "port", 6370, file);
	 
	 return 0;
}

int GetInterfaceConf(pEInterfaceConf conf, const char* file)
{
	 read_profile_string("interface", "url", conf->url, sizeof(conf->url), "", file);
	 read_profile_string("interface", "md5", conf->md5, sizeof(conf->md5), "", file);
	 return 0;
}

int GetPhpQueueConf(pEQueueConf conf, const char* file)
{
	 return read_profile_string("phpqueue", "key", conf->name, sizeof(conf->name), "", file);
}

int GetServerQueueConf(pEQueueConf conf, const char* file)
{
	 return read_profile_string("serverqueue", "key", conf->name, sizeof(conf->name), "", file);
}

int GetUDPConf(pEUDPConf conf, const char* file)
{
	 read_profile_string("udp", "host", conf->host, sizeof(conf->host), "", file);
	 conf->port =  read_profile_int("udp", "port", 6370, file);
	 return 0;
}

