#include "IniFile.h"
#include "ConfigDef.h"
#include "Log.h"

#include <stdio.h>
#include <string.h>

int
load_mysql_conf(const char * file, mysql_conf_t* c)
{
	read_profile_string("mysql", "host", c->host, sizeof(c->host), "", file);
	c->port = read_profile_int("mysql", "port", 0, file);
	read_profile_string("mysql", "db", c->db, sizeof(c->db), "", file);
	read_profile_string("mysql", "user", c->user, sizeof(c->user), "", file);
	read_profile_string("mysql", "passwd", c->passwd, sizeof(c->passwd), "", file);

	return 0;
}

int
load_redis_conf(const char * file, redis_conf_t* c)
{
	read_profile_string("redis", "host", c->host, sizeof c->host, "", file);
	read_profile_string("redis", "key", c->key, sizeof c->key, "", file);
	c->port = read_profile_int("redis", "port", 0, file);
	c->timeout = read_profile_int("redis", "timeout", 0, file);

	return 0;
}

