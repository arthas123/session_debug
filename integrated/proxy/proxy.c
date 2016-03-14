#include <proxy.h>
#include "servicemanager/service_manager.h"
#include "sbinder/sbinderServ.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <proxy.h>
#include <string.h>
#include "clog.h"

#if defined(BINDER_SERVICE)
#include "utils/property/remote_property_module.h"
#include "utils/tcli/remote_tos_tcli.h"

#include "sbinder_input_source.h"
#include "sbinder_bluetooth.h"
#include "sbinder_ktv_setting.h"
#include "sbinder_function_safety.h"
#include "sbinder_cec_mhl.h"
#include "app_common.h"
#include "sbinder_function_power.h"
#include "sbinder_booking.h"
#include "sbinder_satellite.h"
#include "sbinder_demod.h"
#include "sbinder_picture_quality.h"
#include "sbinder_teletext.h"
#include "sbinder_subtitle.h"
#include "sbinder_atv_scan.h"
#include "sbinder_picture_setting.h"
#include "sbinder_sound_setting.h"
#include "sbinder_network_setting.h"
#include "sbinder_errorcode.h"

#include "sbinder_dtv_scan.h"
#include "sbinder_3d.h"
#include "sbinder_epg.h"
#include "sbinder_dm_channel.h"
#include "sbinder_dm_system.h"
#include "sbinder_channel_play.h"
#include "sbinder_function_setting.h"
#include "sbinder_picture_zoom.h"
#include "sbinder_message.h"
#include "sbinder_signal.h"
#include "sbinder_usb_monitor.h"
#include "sbinder_media_scan.h"
#include "sbinder_factory.h"
#include "sbinder_hotel.h"
#include "sbinder_ci.h"
#include "sbinder_mheg.h"
#include "sbinder_hbbtv.h"
#include "sbinder_cc.h"
#include "sbinder_panel.h"
#include "sbinder_pvr.h"
#include "sbinder_snapshot_record.h"
#include "sbinder_tifhal_helper.h"
#include "remote_tifhal_helper.h"
#include "remote_itv_audio_module.h"
#include "remote_itv_platform_module.h"
#include "remote_itv_smc_module.h"
#include "remote_itv_tuner_module.h"
#include "remote_itv_video_module.h"
#include "remote_itv_demux_module.h"
#include "remote_itv_desc_module.h"
//#include "sbinder_power_suspend.h"
#include "sbinder_ts_player.h"
#include "sbinder_audio.h"
#include "sbinder_zoom.h"
#include "sbinder_picture.h"
#include "sbinder_session.h"
#include "sbinder_decoder.h"

static void run_my_cmd(int i,char* s) {
    tos_tcli_printf("i=%d\n",i);
    tos_tcli_printf("s=%s\n",s);
}
TOS_TCLI_COMMAND(mycmd,"my command","long help\n of my command\n","is",run_my_cmd);

static int s_myvalue = 0;
TOS_TCLI_INTERGER(s_myvalue,"short help of s_currentValue","long help\nof s_currentValue");



int serviceManagerLoop(void)
{
    sbinder_serv();
    return 0;
}

void binder_server_start(){
	pthread_t id;
	pthread_create(&id,NULL,(void *)serviceManagerLoop,NULL);
}

void proxy_init(  ) {
    CLOGI("binder_servier_init\n");

	ServiceManager_start();
/*
    sbinder_message_service_add();
    sbinder_signal_service_add();
	remote_property_module_service_add();
	sbinder_input_source_service_add();
	sbinder_bluetooth_service_add();
	sbinder_ktv_setting_service_add();
	sbinder_function_safety_service_add();
    sbinder_cec_mhl_service_add();
	sbinder_function_power_service_add();
    sbinder_booking_service_add();
	sbinder_satellite_service_add();
	sbinder_demod_service_add();
	sbinder_picture_quality_service_add();
    sbinder_subtitle_service_add();
    sbinder_ci_service_add();
    sbinder_mheg_service_add();
    sbinder_hbbtv_service_add();
    sbinder_cc_service_add();
    sbinder_teletext_service_add();
    sbinder_atv_scan_service_add();
	sbinder_picture_setting_service_add();
	sbinder_sound_setting_service_add();
      sbinder_network_setting_service_add();  //wusongjiang
    sbinder_dtv_scan_service_add();
	sbinder_dm_channel_service_add();
	sbinder_dm_system_service_add();
    sbinder_channel_play_service_add();

	sbinder_panel_service_add();
	sbinder_errorcode_service_add();

    sbinder_3d_service_add();
	sbinder_function_setting_service_add();
    sbinder_picture_zoom_service_add();
	remote_tos_tcli_service_add();
	sbinder_epg_service_add();
	sbinder_factory_service_add();
	sbinder_hotel_service_add();
    sbinder_usb_monitor_service_add();
    sbinder_media_scan_service_add();
	sbinder_pvr_service_add();
	sbinder_snapshot_record_service_add();
	sbinder_power_suspend_service_add();
	sbinder_tifhal_helper_service_add();
	remote_tifhal_helper_service_add();
	remote_itv_audio_module_service_add();
	remote_itv_smc_module_service_add();
	remote_itv_platform_module_service_add();
	remote_itv_video_module_service_add();
	remote_itv_tuner_module_service_add();
	remote_itv_demux_module_service_add();
	remote_itv_desc_module_service_add();
	remote_tos_tcli_service_add();	
	*/
	binder_server_start();
	
}


void proxy_add(  ) {
    CLOGI("binder_servier_add\n");

    sbinder_message_service_add();
    sbinder_signal_service_add();
	remote_property_module_service_add();
	sbinder_input_source_service_add();
	CLOGI("[shijian debug]sbinder_session_service_add...........................");
	sbinder_session_service_add();
	sbinder_bluetooth_service_add();
	sbinder_ktv_setting_service_add();
	sbinder_function_safety_service_add();
    sbinder_cec_mhl_service_add();
	sbinder_function_power_service_add();
    sbinder_booking_service_add();
	sbinder_satellite_service_add();
	sbinder_demod_service_add();
	sbinder_picture_quality_service_add();
    sbinder_subtitle_service_add();
    sbinder_ci_service_add();
    sbinder_mheg_service_add();
    sbinder_hbbtv_service_add();
    sbinder_cc_service_add();
    sbinder_teletext_service_add();
    sbinder_atv_scan_service_add();
	sbinder_picture_setting_service_add();
	sbinder_sound_setting_service_add();
      sbinder_network_setting_service_add();  //wusongjiang
    sbinder_dtv_scan_service_add();
	sbinder_dm_channel_service_add();
	sbinder_dm_system_service_add();
    sbinder_channel_play_service_add();

	sbinder_panel_service_add();
	sbinder_errorcode_service_add();

    sbinder_3d_service_add();
	sbinder_function_setting_service_add();
    sbinder_picture_zoom_service_add();
	remote_tos_tcli_service_add();
	sbinder_epg_service_add();
	sbinder_factory_service_add();
	sbinder_hotel_service_add();
    sbinder_usb_monitor_service_add();
    sbinder_media_scan_service_add();
	sbinder_pvr_service_add();
	sbinder_snapshot_record_service_add();
	//sbinder_power_suspend_service_add();
	sbinder_tifhal_helper_service_add();
	remote_tifhal_helper_service_add();
	remote_itv_audio_module_service_add();
	remote_itv_smc_module_service_add();
	remote_itv_platform_module_service_add();
	remote_itv_video_module_service_add();
	remote_itv_tuner_module_service_add();
	remote_itv_demux_module_service_add();
	remote_itv_desc_module_service_add();
	remote_tos_tcli_service_add();	
	sbinder_ts_player_service_add();
	sbinder_audio_service_add();
	sbinder_zoom_service_add();
	sbinder_picture_service_add();
	sbinder_decoder_service_add();
		
}


#elif defined(BINDER_CLIENT)

extern int _tos_tcli_init();

static int s_inited = 0;

void proxy_init() {
    CLOGI("binder_client_proxy_init %d\n",s_inited);

    if(s_inited == 0) {
        s_inited = 1;   
    	sbinder_start();
    	_tos_tcli_init();
    }
}


#else
 #error Must define BINDER_SERVICE or BINDER_CLIENT
#endif


