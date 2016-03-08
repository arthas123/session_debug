#include <stdlib.h>
#include <stdio.h>
#include "session.h"
#include "tos_session.h"

#define SESSION_NUM 10

static session_info session_pool[SESSION_NUM];

fpi_error get_session_pool_addr(session_info **session_pool_ad)
{
	*session_pool_ad = session_pool;
	return FPI_ERROR_SUCCESS;
}

fpi_error session_pool_init()
{
	int i = 0;

	for(i = 0; i < SESSION_NUM; i++){
		session_pool[i].session_status = SESSION_DESTROYED;
		session_pool[i].session_id = 1;
		session_pool[i].source_type = SESSION_TYPE_INVALID;
	}

	return FPI_ERROR_SUCCESS;
}

