/*********************************************************************************************
* Filename	: tvh_service_main_socket.c
* Author   	: LinShunda
* Description:   
* Version:   
* Function List:	
* 1. --------- 
* History: 	
* <author>  	  <time>   			<version >   <desc>	
* LinShunda    	2013-02-06 	 		1.0    	 	build this moudle  	
* Copyright	: TCL Electronics(Shen Zhen) Co.,Ltd.
********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h> 		

#include "tvh_service.h"

#include "fpi_notify_event.h"

#include "fpi_common.h"
#include "tvh_common.h"
#include "tvh_service_init.h"
#include "uti_config.h"
//#include "tvh_service_debug_func_set.h"
#include "os.h"
#include "fpp_system.h"
#include "fpi_system.h"
#include "tvh_graphics_draw.h"
#include "app_common.h"
#include "app_user_setting.h"
#include "session.h"

#include "proxy.h"

#define GET_INIT_TIME 

SET_MODEL_ID(SITA_MODULE_COMMON);

void tvh_service_debug_func_set_init(void);

uint8_t tvh_get_config_path_table(st_config_path_ctrl **table ) ;

void set_config_path_table(st_config_path_ctrl *table, uint8_t u8_size ) ;


void app_msg_docking_notify(uint32_t un32_event_id, uint32_t un32_p1,uint32_t un32_p2 );

static void set_panel_update_config(void)
{
    
#if 1
     //panel
    char isUpdatePanel[128] = "";
     char pPanelName[128]=" ";

	 if(fpi_true == sys_cfg_get_section(e_system_ini, SYS_PANEL_ID_SECTION))//Amy add: panel id section check
	 {
	 
	 	fpi_info("Via panel_id ini file get m_pPanelName ======= \n");

		 if(sys_cfg_get_ini(e_panel_id_ini) != NULL)
		 {
			sys_cfg_get_string(e_panel_id_ini, PANEL_ID_PANEL_PATH_SECTION, PANEL_ID_PANEL_PARAM,pPanelName,128);
			
			if(0 == strcmp(pPanelName," ") || (pPanelName == NULL))
			{
				 fpi_err("error load panel fail!!!!\r\n");
				 return;
			}
	 	 }
	 }
	 else
	 {
     if(sys_cfg_get_ini(e_model_ini) != NULL)
     {
        sys_cfg_get_string(e_model_ini, MODEL_PANEL_SECTION,MODEL_PANEL_KEY_NAME,pPanelName,128);
        
        if(0 == strcmp(pPanelName," ") || (pPanelName == NULL))
        {
             fpi_info("error load panel fail!!!!\r\n");
             return;
        }
     }
	 }
	 

     fpi_info("load_ini_file, m_pPanelName = %s \n", pPanelName);
     if(FPI_ERROR_SUCCESS == fpi_system_get_env(PanelUpdateRequest, isUpdatePanel))
    {
        fpi_info(" isUpdatePanel = %s",isUpdatePanel);
        if(0 == strcmp(isUpdatePanel,"1"))
        {
            fpi_system_set_panel_file_name(pPanelName);
            fpi_system_set_env(PanelUpdateRequest,"0");
        }
    }
    else
    {
        fpi_info("set_panel_update_config get PanelUpdateRequest error!!!!");
    }
#endif
}
/******************************************************************************
*Function: main
*Description: 服务主函数 。
*Input:
*Output:
*Return:
*Others:
******************************************************************************/
int main(int argc, char *argv[] )
{
     fpi_system_config st_fpi_config;//need get system config
     uint8_t u8_no[5] = {0};
	 
    POWER_ON_TIMER_PRINTF(0, "tvh main start");
    
    if( argc >1 )
    {        
        if( argc >2 )
        {
            u8_no[0] = atoi(argv[2]) ;
        }
        set_log_level(argv[1],u8_no) ;

    }
	u8_no[0] = 1;
//	set_log_level("rmlog",u8_no);
//	set_log_level("tofile",u8_no);
    	   
	//clog_execcmdline(argc,argv);   
	tvh_service_init();
	session_pool_init();
    uti_config_init();
    {
        st_config_path_ctrl *table ;
        uint8_t un8_size ;
        un8_size = tvh_get_config_path_table(&table) ;
     //   fpi_printf("*load ini*table=%d\n",(int)table);
        set_config_path_table(table,un8_size);
    }
	load_ini_file(0xffff, fpi_false);    
	proxy_init(); 
	//char* pfile = uti_config_get_path_clog_file();
	//int rsize = uti_config_get_path_clog_rotate_file_size();
	//int rcount = uti_config_get_path_clog_rotate_file_count();
	//fpi_info("main---- pfile=%s,rsize=%d,rcount=%d\n",pfile,rsize,rcount);
	//if (pfile != NULL && rsize > 0 && rcount > 0){
	//	
		//clog_startlogsave(pfile,rsize,rcount);		
	//}
	
    tvh_database_init();
    POWER_ON_TIMER_PRINTF(1000, "config_db_init");
	fpi_init(&st_fpi_config);
    
    POWER_ON_TIMER_PRINTF(2000, "fpp driver init");

	app_user_setting_init();
	POWER_ON_TIMER_PRINTF(2001, "app_user_setting_init");
    tvh_model_init(argc, argv);
    set_panel_update_config();

	app_init(argc,argv);
	
    POWER_ON_TIMER_PRINTF(3000, "after module init");
#ifdef DEBUG_TVH_SERVICE
        tvh_service_debug_func_set_init(); 
#endif    
    POWER_ON_TIMER_PRINTF(3001, "debug init");

	//断掉消息通路
 	//fpi_notify_init( tvh_service_send_system_message ) ;  
	//tvh_notifys_init( tvh_service_send_system_messages ) ;
 	//tvh_notifyx_init( tvh_service_send_system_messagex ) ;

	//使用新的消息通路
 	fpi_notify_init( app_msg_docking_notify ) ;  
    tvh_notifys_init( NULL ) ;
    tvh_notifyx_init( NULL ) ;
	proxy_add();
	
	fpp_system_clear_bootimg();
	app_common_set_state(EN_COMMON_STATE_INIT_FINISH, true);
	
    fpi_info("sitatvservice end\n");
    while (1) 
    {
        OS_THREAD_INSERT_INFO(COURSE_MAIN_THREAD_ID);
        //if server client still not connect success ,then sleep
        if(fpi_false == tvh_service_get_comunicate_sucess_status())
        {   
            os_msleep(20);
            continue;
        }
        
        /* process event */
		//tvh_service_process_event(); //用非阻塞socket,要不会影响routine
		os_msleep(20);
    }

    tvh_service_exit();
    tvh_model_exit();
    fpi_exit();

    
    return 0;
}


