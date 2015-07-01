#include "SyncFunc.h"
#include "RedisHelper.h"
#include "json/json.h"
#include "Log.h"

using namespace Json;

int
order_trans(CRedisHelper* redis_list, unsigned int redis_nums, CRedisHelper* producer)
{
	string 			order;
	Reader			reader;
	Value			value;
	unsigned long 	redis_idx;

	if (producer->IsActived()) {
		producer->Dequeue("ORDER_NEW_Q", order);

		if (!order.empty() && reader.parse(order, value)) {
			redis_idx = value["id"].asUInt64() % redis_nums;
			log_debug("JSON: %s IDX: %lu", order.c_str(), redis_idx);
			if ((redis_list + redis_idx)->IsActived()) {
				(redis_list + redis_idx)->Enqueue("ORDER_NEW_Q", order);	
			}
		} else {
			usleep(50 * 1000);
		}
	}

	return 0;
}

