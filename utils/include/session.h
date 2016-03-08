/**@defgroup 会话管理模块
@{

@brief 用于创建销毁会话,具体会话如何使用,管理由个模块自行决定,本模块仅作为工具模块协助产生唯一的会话ID

@author lihui02@tcl.com

@version 1.0.0 2015/12/16 初稿
*/

#ifndef __SESSION_H__
#define __SESSION_H__

#include "tos_session.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	int32_t session_id;
	EN_SESSION_STATUS session_status;
	EM_SESSION_TYPE source_type;
}session_info;

//static session_info session_info_index[SESSION_NUM];
fpi_error get_session_pool_addr(session_info **session_pool_ad);

fpi_error session_pool_init(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
