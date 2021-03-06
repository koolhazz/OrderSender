#ifndef __DEF_H_
#define __DEF_H_

#include "log.h"
#include "json/value.h"
#include "json/writer.h"
#include "json/reader.h"

#include <time.h>
#include <sys/time.h>
#include <string>
using std::string;
using namespace Json;

/* 业务接口响应码 */
enum send_rsp_code_t {
	RSP_SUCCESS 			= 1,	
	RSP_AGAIN				= -1,
	RSP_ERR_PARAM			= -2,
	RSP_NO_USER				= -3,	
	RSP_NO_INTERFACE		= -4,
	RSP_INVAILD_ORDER		= -5,
	RSP_INVAILD_ORDER_ID 	= -6,
	RSP_DUP_ORDER			= -7,
	RSP_SEND_FAILED			= -8,
	RSP_KEEP_1				= -9,
	RSP_502					= -10,
	RSP_KEEP_2				= -11,
	RSP_TIMEOUT				= -12,
	RSP_FIREWALL			= -13
};

/* 订单状态 */
enum order_status_t {
	ORDER_FINISH 	= 2,
	ORDER_ERR_CGI 	= 54,
	ORDER_ERR_ORDER = 53 
};

/* 重试时间间隔 */
enum order_timer_t {
	tv_30_s = 30,
	tv_60_s = 60,
	tv_5_m	= 300,
	tv_10_m = 600,
	tv_30_m	= 1800,
	tv_1_h 	= 3600	
};

#if 1
const int retry_timer_value[6] = { 
	tv_30_s, 
	tv_60_s, 
	tv_5_m,
	tv_10_m,
	tv_30_m,
	tv_1_h
};
#else
const int retry_timer_value[6] = { 
	5, 
	5, 
	5,
	5,
	5,
	5
};
#endif
#define RTV_LEN() (int)((sizeof retry_timer_value) / sizeof(int))
#define RTV_VALUE(idx) retry_timer_value[idx]

typedef struct order_data_s order_data_t;
struct order_data_s {
	unsigned long 	id;
	string 			url;
	string			table;
	int				times;
};
#define NEW_OD(j, o) do { 					\
	o = new order_data_t;					\
	if (o) {								\
		o->id = j["id"].asUInt64();			\
		o->url = j["url"].asString();		\
		o->table = j["table"].asString();	\
		o->times = 0;						\
	}										\
} while(0)
#define DEL_OD(o) delete o
#define PRT_OD(o) do {							\
	log_debug("id: %lu", o->id);				\
	log_debug("url: %s", o->url.c_str());		\
	log_debug("table: %s", o->table.c_str());	\
	log_debug("times: %d", o->times);			\
} while (0)

#define __LOG_BUFF_SZ 1 * ( 1 << 10)
#define NEW_LB(lb) lb = (char*)malloc(__LOG_BUFF_SZ)
#define DEL_LB(lb) free(lb)


#define NOW() time(NULL)
#endif
