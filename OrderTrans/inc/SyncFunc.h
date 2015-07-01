#ifndef __SYNC_FUNC_H_
#define __SYNC_FUNC_H_

class CRedisHelper;

int
order_trans(CRedisHelper* redis_list, unsigned int redis_nums, CRedisHelper* producer);

#endif

