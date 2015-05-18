#ifndef __TIMER_HANDLER_H_
#define __TIMER_HANDLER_H_

#include <stddef.h>
#include <string.h>

extern void
timeout_request(void* arg);

extern size_t 
header_data(char *buffer, 
		  	size_t size, 
		  	size_t nitems, 
		  	void* userdata);

extern size_t
write_data(char* buffer, 
		   size_t size, 
		   size_t nitems, 
		   void* userdata);

#endif
