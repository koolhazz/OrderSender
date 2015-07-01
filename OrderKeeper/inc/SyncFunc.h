#ifndef __SYNC_FUNC_H_
#define __SYNC_FUNC_H_

#include "ConfigDef.h"


namespace Helper 
{
class CRedisHelper;
class CMysqlHelper;
}

using namespace Helper;

extern int
sync_order_data(CMysqlHelper* mysql, CRedisHelper* redis);

#endif
