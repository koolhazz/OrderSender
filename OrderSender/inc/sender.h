#ifndef __SENDER__H_
#define __SENDER__H_

typedef class CRedisHelper redis_helper_t;

class sender_t {
public:
	sender_t(redis_helper_t *redis);
	~sender_t();
public:
	int run();
private:
	redis_helper_t 	*_redis;
	int 			_retry;		
};

#endif
