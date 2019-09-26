#include "tos_input_source.h"
#include "fpi_cec.h"
#include "app_user_setting.h"
#include "app_input_source.h"
#include "app_channel.h"
#include "app_channel_dtv_scan.h"
#include "app_channel_atv_scan.h"
#include "tos_dtv_scan.h"
#include "tos_androidHandler.h"
#include "tos_handleHelper.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "tos_tcli.h"
#include "app_msg_manager.h"
#include "sys_config.h"
#include "tos_picture_setting.h"
#include "tos_notify.h"
#include "tos_session.h"
#include "session.h"

//extern session_info session_info_index[SESSION_NUM];
//test notes 4321 4567 567
SET_MODEL_ID(SITA_MODULE_LINEIN);

//#define DBG_SRC fpi_trace
//#define DBG_SRC_ERROR fpi_err
#define DBG_SRC(fmt, arg...)        fpi_info((char *)fmt, ##arg)// fpi_trace((char *)fmt, ##arg)
#define DBG_SRC_ERROR(fmt, arg...)         fpi_err((char *)fmt, ##arg)

//static session_info *session_pool_addr;
static uint32_t ui_input_soure_mapping[][2]=
{
    {E_INPUT_ATV, FPI_LINEIN_CATV},
    {E_INPUT_DTV, FPI_LINEIN_CDTV},
    {E_INPUT_AV_I, FPI_LINEIN_CVBS1},   //{E_INPUT_AV_I, FPI_LINEIN_CVBS1},    //mark 20130806 set cvbs1 to scart1
    {E_INPUT_AV_II, FPI_LINEIN_CVBS2},
    {E_INPUT_YPBPR, FPI_LINEIN_COMPONENT1},
    {E_INPUT_PC, FPI_LINEIN_PC1},
    {E_INPUT_HDMI_I, FPI_LINEIN_HDMI1}, 
    {E_INPUT_HDMI_II, FPI_LINEIN_HDMI2}, 
    {E_INPUT_HDMI_III, FPI_LINEIN_HDMI3},
    {E_INPUT_KTV,FPI_LINEIN_KTV},
    {E_INPUT_STORAGE,FPI_LINEIN_STORAGE},
    {E_INPUT_SCART, FPI_LINEIN_SCART1},
    {E_INPUT_HDMI_IV, FPI_LINEIN_HDMI4}, 
    {E_INPUT_NUM,FPI_LINEIN_INVALID}
};


fpi_error tos_source_set_input_async(EN_API_SOURCE_INPUT input,EN_ACT_CTRL act) {
	return app_source_set_input_async(input,act);
}


static fpi_error tos_source_set_input_v1(EN_API_SOURCE_INPUT input,EN_ACT_CTRL act)
{
    tos_picture_video_mute(0x12345678, fpi_false); // 2015-11-27 临时措施，还要去扑掉
    DBG_SRC("tos_source_set_input src =%d\n",input);
    fpi_error ret = FPI_ERROR_FAIL;
    int i;
    fpi_linein_t linein = FPI_LINEIN_INVALID;
    fpi_cec_manual_switch_para manPara = {0};

#if 0     //出错，只在欧洲有用到，app_user_setting_get_last_service_type获得都是0，暂时屏蔽
    if(E_INPUT_DTV == input)//海外区信源dtv和atv均会传下来DTV。需要根据最后一次service type来判定信源
    {
        DBG_SRC("lwf tv input ,judge by last service type\n");
        uint8_t u8ServiceType = 0;
        app_user_setting_get_last_service_type(&u8ServiceType);
        DBG_SRC("lwf tv last service type==%d\n",u8ServiceType);
        if(EN_SERVICETYPE_ATV == u8ServiceType)
        {
            linein = FPI_LINEIN_CATV;
        }
        else
        {
            linein = FPI_LINEIN_CDTV;
        }
    }
    else
#endif        
    {
        DBG_SRC("lwf not tv input =0x%x\n",linein);
        for(i = 0; i < E_INPUT_NUM; ++i)
        {
            if(input == (EN_API_SOURCE_INPUT)ui_input_soure_mapping[i][0])
            {
               linein = (fpi_linein_t)ui_input_soure_mapping[i][1];
               break;
            }
        }
    }
    fpi_set_target_linein(linein);
    if( act == EN_ACT_EXEC_SAVE)
	{
        fpi_trace("EN_ACT_EXEC_SAVE\n");
        DBG_SRC("going to change linein =%d,0x%x\n",linein,linein);
        if(linein != FPI_LINEIN_INVALID)
        {
         //   uint32_t lastline = app_get_db_current_linein();
        //    DBG_SRC_ERROR("-->before change, line ==%d\n",lastline);
            if(app_set_input_source(linein))
            {
                manPara.bManual = fpi_true;
                fpi_cec_is_manual_switch_linein(manPara);
                                
                ret = FPI_ERROR_SUCCESS;
                
          /*    if(linein != lastline )
                {
                    DBG_SRC_ERROR("-->set last line in\n");
                    app_save_db_lastest_linein(lastline);
                }*/
            }
            else
            {
                DBG_SRC_ERROR("set src fail\n");
                ret = FPI_ERROR_FAIL;

            }
        }
#if 0 //不应该耦合在这里，有 tos 接口可以直接调用 。
        if((input==E_INPUT_ATV) ||
            (input==E_INPUT_DTV)) 
        {
            DBG_SRC_ERROR("  tv source to play===%d\n",__LINE__);
            uint16_t curr_ch_index =0;
            
            if(app_channel_list_get_current_by_linein(&curr_ch_index,linein)==FPI_ERROR_SUCCESS)
            {
                if(app_channel_list_start_play_by_index(curr_ch_index)!=FPI_ERROR_SUCCESS)
                {
                    DBG_SRC_ERROR("[%s] app_channel_list_start_play_by_index failure curr index=%d\n", __FUNCTION__, curr_ch_index);   
                }
            }
            else
            {
                DBG_SRC_ERROR("[%s] app_channel_list_get_current_by_linein failure curr index=%d\n", __FUNCTION__, curr_ch_index);
            }
        }
 #endif       
        
        return ret;
	}

    if( act == EN_ACT_EXEC )
    {
        fpi_info("%s EN_ACT_EXEC! s_current_linein is not equal to the value of db \n",__FUNCTION__);
		app_set_pre_input_source(linein);
        fpi_linein_changeto(linein) ;
		app_notify_event(APP_NOTIFY_LINEIN_CHANGE_FINISH, 0, 0);
    }

     if( act == EN_ACT_SAVE )
    {
        fpi_trace("EN_ACT_EXEC\n");
        app_save_db_current_linein(linein);
    }

     if( act == 3 )
    {
        fpi_warn("set input demo\n");
        ret = app_set_input_source_v2( linein ) ;
        return ret;

    }
return FPI_ERROR_SUCCESS;
}

static fpi_error tos_source_set_input_v2(EN_API_SOURCE_INPUT input,EN_ACT_CTRL act)
{
	fpi_info("tos_source_set_input v2 src =%d\n",input);
    
    int i;
    fpi_linein_t linein = FPI_LINEIN_INVALID;

	{       
        for(i = 0; i < E_INPUT_NUM; ++i)
        {
            if(input == (EN_API_SOURCE_INPUT)ui_input_soure_mapping[i][0])
            {
               linein = (fpi_linein_t)ui_input_soure_mapping[i][1];
               break;
            }
        }
    }
	fpi_set_target_linein(linein);
	switch(act)
	{
		case EN_ACT_EXEC_SAVE:
			{
				fpi_trace("EN_ACT_EXEC_SAVE\n");
				return app_set_input_source_v2( linein );
			}break;
		case EN_ACT_EXEC :
			{
				fpi_info("%s EN_ACT_EXEC! s_current_linein is not equal to the value of db \n",__FUNCTION__);
				fpi_linein_t current_linein ;
    			fpi_linein_get_current(&current_linein);
				fpi_error ret = app_set_input_source_v2(linein);
				app_save_db_lastest_linein(current_linein);
        		app_save_db_current_linein(current_linein);
				return ret;
			}break;
		case EN_ACT_SAVE : 
			{
				fpi_trace("EN_ACT_SAVE\n");
				return app_save_current_linein(linein);
			}break;
		default:
			{
				fpi_err_code(FPI_ERROR_LINEIN_INVALID_PARAMETER,"the action is no acting =%d\n",act );
			}break;
	}

	return FPI_ERROR_LINEIN_INVALID_PARAMETER;
}

fpi_error tos_source_set_input(EN_API_SOURCE_INPUT input,EN_ACT_CTRL act)
{
	fpi_error ret = FPI_ERROR_FAIL;
	struct timespec times;
    struct timespec times2;
	int ms;
	
		    // stop scan if is scaning
    if(app_channel_dtv_is_scan() || app_channel_atv_is_scan())
    { // stop scan
    	
        fpi_error tmp = tos_tv_scan_stop(E_FPI_DTV_STREAM_PORT_0);
	 fpi_info("function = %s, line = %d,tos_tv_scan_stop = %d\n",__FUNCTION__,__LINE__,tmp);
    }
	
	clock_gettime(CLOCK_MONOTONIC, &times);
	if( sys_cfg_get_model_version(SITA_MODULE_LINEIN) == 2 )
	{
		ret = tos_source_set_input_v2(input,act);
	}
	else
	{
		ret = tos_source_set_input_v1(input,act);
	}
	clock_gettime(CLOCK_MONOTONIC, &times2);
	ms=(times2.tv_sec - times.tv_sec)*1000+((times2.tv_nsec- times.tv_nsec)/(1000*1000) );
	fpi_info("tos_source_set_input_time input=0x%x,act=%d, =%d ms\n",input,act, ms );
	
	return ret;
}



fpi_error tos_source_get_input(EN_SOURCE_SAVE_TYPE type,EN_API_SOURCE_INPUT *value)
{
    if( type == EN_SOURCE_CURRENT )
   {
        fpi_trace(" enter %s line =%d\n",__FUNCTION__,__LINE__);
        if(NULL == value)
        {
            DBG_SRC_ERROR("ops point null ,function = %s,line = %d\n",__FUNCTION__,__LINE__);
            return FPI_ERROR_FAIL;

        }
        int i;
        fpi_linein_t linein = app_get_input_sources();
        fpi_debug("fpi linein =%d \n",linein);

        for(i = 0; i < E_INPUT_NUM; ++i)
        {
            if(linein == (fpi_linein_t)ui_input_soure_mapping[i][1])
            {
                
                *value = (EN_API_SOURCE_INPUT)ui_input_soure_mapping[i][0];
                fpi_debug(" current source = %d \n",*value);
                break;
            }
        }
        
    }

   else if( type == EN_SOURCE_LASTEST )
    {
        fpi_trace(" enter %s,line = %d\n",__FUNCTION__,__LINE__);
        int i = 0;
        fpi_linein_t linein = FPI_LINEIN_INVALID;
        linein = app_get_lastest_input_source();
        for(i = 0; i < E_INPUT_NUM; ++i)
        {
            if(linein == (fpi_linein_t)ui_input_soure_mapping[i][1])
            {
                
                *value = (EN_API_SOURCE_INPUT)ui_input_soure_mapping[i][0];
                DBG_SRC(" last saved source = %d \n",*value );
                break;
            }
        }
        DBG_SRC("return success\n");
        return FPI_ERROR_SUCCESS;
    }

	else
	{
		int i;
		fpi_linein_t linein = fpi_get_target_linein();
		for(i = 0; i < E_INPUT_NUM; ++i)
        {
            if(linein == (fpi_linein_t)ui_input_soure_mapping[i][1])
            {
                
                *value = (EN_API_SOURCE_INPUT)ui_input_soure_mapping[i][0];
                DBG_SRC(" last saved source = %d \n",*value );
                break;
            }
        } 
	}
return FPI_ERROR_SUCCESS;
}



fpi_error tos_source_is_insert(EN_API_SOURCE_INPUT inSource, int *state)
{
    int i;
    fpi_linein_t linein = FPI_LINEIN_INVALID;
	fpi_bool bInset = 0;
    
    DBG_SRC("tos_source_is_insert start!!!\n");

    for(i = 0; i < E_INPUT_NUM; ++i)
    {
        if(inSource == (EN_API_SOURCE_INPUT)ui_input_soure_mapping[i][0])
        {
            linein = (fpi_linein_t)ui_input_soure_mapping[i][1];
            break;
        }
    }

    fpi_trace("tos_source_is_insert ,linein = %x\n", linein);
     if(app_input_source_src_intert_check(linein, &bInset) != FPI_ERROR_SUCCESS)
    {
        fpi_err("tos_source_is_insert fail!!!\n");
        return FPI_ERROR_FAIL;
    }
    else
    {
    	
		if(bInset == fpi_false)
		{
			 *state = 0;
		 }
		else
		 {
			 *state = 1;
		 }
        fpi_trace("tos_source_is_insert, isInsert = %d\n", *state);
        fpi_trace("tos_source_is_insertseccess!!!\n");
        return FPI_ERROR_SUCCESS;
    }

    return FPI_ERROR_SUCCESS;
}
fpi_error tos_source_set_bootinput(EN_TOS_SOURCE_BOOT_INPUT bootSource){

	char s_bootSource[5]="\0";
	sprintf(s_bootSource,"%d",bootSource);
	if(sita_property_set(BOOT_SOURCE_KEY,s_bootSource))
	{
		return FPI_ERROR_FAIL;
	}
	return FPI_ERROR_SUCCESS;
}



fpi_error tos_source_set_hdmi_edid_ver(EN_API_SOURCE_INPUT input, int32_t ver)
{
    fpi_linein_t linein;
    
    DBG_SRC(" tos_source_set_hdmi_edid_ver, input = %d, ver = %d \n",input, ver);
    switch(input)
    {
        case E_INPUT_HDMI_I:
        {
            linein = FPI_LINEIN_HDMI1;
        }
        break;
        case E_INPUT_HDMI_II:
        {
            linein = FPI_LINEIN_HDMI2;
        }
        break;
        case E_INPUT_HDMI_III:
        {
            linein = FPI_LINEIN_HDMI3;
        }
        break;
        case E_INPUT_HDMI_IV:
        {
            linein = FPI_LINEIN_HDMI4;
        }
        break;
        default:
        {
            DBG_SRC_ERROR(" tos_source_set_hdmi_edid_ver error! input = %d \n ", input);
            return FPI_ERROR_FAIL;
        }
        break;
    }
    
    return fpi_input_set_hdmi_edid_ver(linein, ver);
}

fpi_error tos_source_request(uint32_t session_id)
{
	session_info *session_pool_addr;
	fpi_info("Enter %s, line %d\n",__FUNCTION__, __LINE__);
	
	get_session_pool_addr(&session_pool_addr);
	
	if(session_pool_addr[session_id].session_status != SESSION_DESTROYED)
	{
		session_pool_addr[session_id].session_status = SESSION_REQUESTED;
		//app_notify_event(CMD_UI_NOTIFY_SESSION,session_id,1);    //第三个参数1 代表session已申请，控制权已被抢占
		return FPI_ERROR_SUCCESS;
	}

	fpi_info("Exit %s, line %d\n",__FUNCTION__, __LINE__);
	return FPI_ERROR_FAIL;
}

fpi_error tos_source_release(uint32_t session_id)
{
	//session_id = 0;
	//session_info *session_pool_addr;
	fpi_info("Enter %s, line %d, session_id = %d\n",__FUNCTION__, __LINE__,session_id);
	
	//get_session_pool_addr(&session_pool_addr);
	
	//session_pool_addr[session_id].session_status = SESSION_RELEASED;
	//app_notify_event(CMD_UI_NOTIFY_SESSION,session_id,0);    //第三个参数0表示session已释放

	fpi_info("Exit %s, line %d\n",__FUNCTION__, __LINE__);
	return FPI_ERROR_SUCCESS;
}

fpi_error tos_source_set_input_with_session(uint32_t session_id,EN_API_SOURCE_INPUT input,EN_ACT_CTRL act)
{
	
	fpi_error ret = FPI_ERROR_FAIL;
	fpi_info("Enter %s, line %d\n",__FUNCTION__, __LINE__);
	session_info *session_pool_addr;
	get_session_pool_addr(&session_pool_addr);
	
	if((session_pool_addr[session_id].session_status == SESSION_REQUESTED) && (session_pool_addr[session_id].source_type == SESSION_TYPE_INPUT_SOURCE))
	{
		ret = tos_source_set_input_v1(input,act);	
		/*
		if(FPI_ERROR_SUCCESS == ret)
		{
			app_notify_event(CMD_UI_NOTIFY_SESSION, session_id, 2);    //第三个参数2 表示这个session用完了，可以释放了
		}
		*/
	}

	fpi_info("Exit %s, line %d\n",__FUNCTION__, __LINE__);
	return ret;
}
