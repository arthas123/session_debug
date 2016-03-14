/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: sbinder_signal.sidl
 */

#ifndef __sbinder_session_h__
#define __sbinder_session_h__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BINDER_SERVICE)

#include "tos_session.h"
#include "fpi_debug_helper.h"

///< call this api to only add this service
int sbinder_signal_session_add();
int sbinder_session_service_add();
#elif defined(BINDER_CLIENT)
 #error This file is only used on server
#else
 #error Must define BINDER_SERVICE or BINDER_CLIENT
#endif


#ifdef __cplusplus
}
#endif

#endif