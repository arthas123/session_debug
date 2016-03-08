#include <stdio.h>
#include "tos_session.h"
#include "tos_notify.h"
#include "session.h"
#include "fpi_debug_helper.h"

static uint32_t s_sessionpool = 1;

//static session_info *session_pool_addr;

fpi_error tos_session_create(EM_SESSION_TYPE source_type, uint32_t* session_id)
{
	int index = 0;
	session_info *session_pool_addr;
	
	if(get_session_pool_addr(&session_pool_addr)){
		
		while(session_pool_addr[index].session_status != SESSION_DESTROYED)
		{
			index++;
		}
	
		*session_id = index;
	
		session_pool_addr[index].session_id = s_sessionpool++;
		session_pool_addr[index].session_status = SESSION_CREATED;
		session_pool_addr[index].source_type = source_type;
	
		return FPI_ERROR_SUCCESS;
	}
	else{
		return FPI_ERROR_FAIL;
	}
	
}

fpi_error tos_session_destory(uint32_t session_id)
{
	
	session_info *session_pool_addr;
	get_session_pool_addr(&session_pool_addr);
	
	session_pool_addr[session_id].session_status= SESSION_DESTROYED;
	session_pool_addr[session_id].source_type = SESSION_TYPE_INVALID;

	
	return FPI_ERROR_SUCCESS;
}

