/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "string.h"
#include "app_tws_ibrt_trace.h"
#include "app_tws_ctrl_thread.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_ibrt_customif_cmd.h"
#include "app_tws_if.h"
#include "app_dip.h"
#include "led.h"
#include "apps.h"



#ifdef BISTO_ENABLED
#include "gsound_custom.h"
#endif
#ifdef __DUAL_MIC_RECORDING__
#include "ai_control.h"
#include "app_recording_handle.h"
#endif
#if defined(IBRT)
/*
* custom cmd handler add here, this is just a example
*/

#define app_ibrt_custom_cmd_rsp_timeout_handler_null   (0)
#define app_ibrt_custom_cmd_rsp_handler_null           (0)
#define app_ibrt_custom_cmd_rx_handler_null            (0)

#ifdef BISTO_ENABLED
static void app_ibrt_bisto_dip_sync(uint8_t *p_buff, uint16_t length);
static void app_ibrt_bisto_dip_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif
#ifdef __DUAL_MIC_RECORDING__
static void app_ibrt_customif_audio_send(uint8_t* p_buff, uint16_t length);
static void app_ibrt_customif_audio_send_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t *ptrParam, uint16_t paramLen);
static void app_ibrt_customif_audio_send_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length);
#endif

#ifdef CUSTOM_BITRATE
static void app_ibrt_user_a2dp_info_sync(uint8_t *p_buff, uint16_t length);
static void app_ibrt_user_a2dp_info_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif
void app_ibrt_customif_test1_cmd_send(uint8_t *p_buff, uint16_t length);
static void app_ibrt_customif_test1_cmd_send_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

static void app_ibrt_customif_test2_cmd_send(uint8_t *p_buff, uint16_t length);
static void app_ibrt_customif_test2_cmd_send_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
static void app_ibrt_customif_test2_cmd_send_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
static void app_ibrt_customif_test2_cmd_send_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

static const app_tws_cmd_instance_t g_ibrt_custom_cmd_handler_table[]=
{
#ifdef GFPS_ENABLED
    {
        APP_TWS_CMD_SHARE_FASTPAIR_INFO,                        "SHARE_FASTPAIR_INFO",
        app_ibrt_share_fastpair_info,
        app_ibrt_shared_fastpair_info_received_handler,         0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null
    },
    {
        APP_TWS_CMD_GFPS_RING_STOP_SYNC,                        "TWS_CMD_GFPS_STOP_RING_STATE",
        app_ibrt_customif_gfps_stop_ring_sync_send,
        app_ibrt_customif_gfps_stop_ring_sync_recv,             0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null, NULL
    },
#endif
#ifdef BISTO_ENABLED
    {
        APP_TWS_CMD_BISTO_DIP_SYNC,                             "BISTO_DIP_SYNC",
        app_ibrt_bisto_dip_sync,
        app_ibrt_bisto_dip_sync_handler,                        0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null
    },
#endif
#ifdef __DUAL_MIC_RECORDING__
    {
        APP_IBRT_CUSTOM_CMD_DMA_AUDIO,                          "TWS_CMD_DMA_AUDIO",
        app_ibrt_customif_audio_send,
        app_ibrt_customif_audio_send_handler,                   0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null,
        app_ibrt_customif_audio_send_done,
    },
#endif
#ifdef CUSTOM_BITRATE
    {
        APP_TWS_CMD_A2DP_CONFIG_SYNC,                           "USER_A2DP_INFO_SHARE",
        app_ibrt_user_a2dp_info_sync,
        app_ibrt_user_a2dp_info_sync_handler,                   0,
        app_ibrt_cmd_rsp_timeout_handler_null,                  app_ibrt_cmd_rsp_handler_null
    },
#endif
    {
        APP_IBRT_CUSTOM_CMD_TEST1,                              "TWS_CMD_TEST1",
        app_ibrt_customif_test1_cmd_send,
        app_ibrt_customif_test1_cmd_send_handler,               0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null
    },
    {
        APP_IBRT_CUSTOM_CMD_TEST2,                              "TWS_CMD_TEST2",
        app_ibrt_customif_test2_cmd_send,
        app_ibrt_customif_test2_cmd_send_handler,               RSP_TIMEOUT_DEFAULT,
        app_ibrt_customif_test2_cmd_send_rsp_timeout_handler,   app_ibrt_customif_test2_cmd_send_rsp_handler
    },
};

int app_ibrt_customif_cmd_table_get(void **cmd_tbl, uint16_t *cmd_size)
{
    *cmd_tbl = (void *)&g_ibrt_custom_cmd_handler_table;
    *cmd_size = ARRAY_SIZE(g_ibrt_custom_cmd_handler_table);
    return 0;
}


#ifdef BISTO_ENABLED
static void app_ibrt_bisto_dip_sync(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_BISTO_DIP_SYNC, p_buff, length);
}

static void app_ibrt_bisto_dip_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    gsound_mobile_type_get_callback(*(MOBILE_CONN_TYPE_E *)p_buff);
}
#endif
#ifdef __DUAL_MIC_RECORDING__
static void app_ibrt_customif_audio_send(uint8_t* p_buff, uint16_t length) {
    TRACE(1, "%s", __func__);
    app_recording_send_data_to_master();
}

static void app_ibrt_customif_audio_send_handler(uint16_t rsp_seq, uint8_t* p_buff, uint16_t length) {
    ai_function_handle(CALLBACK_STORE_SLAVE_DATA, (void*)p_buff, length);
    //TRACE(1, "%s", __func__);
}

static void app_ibrt_customif_audio_send_done(uint16_t cmdcode, uint16_t rsp_seq, uint8_t* ptrParam,
        uint16_t paramLen) {
    TRACE(1, "%s", __func__);
    app_recording_audio_send_done();
}
#endif
void app_ibrt_customif_cmd_test(ibrt_custom_cmd_test_t *cmd_test)
{
    tws_ctrl_send_cmd(APP_IBRT_CUSTOM_CMD_TEST1, (uint8_t*)cmd_test, sizeof(ibrt_custom_cmd_test_t));
    tws_ctrl_send_cmd(APP_IBRT_CUSTOM_CMD_TEST2, (uint8_t*)cmd_test, sizeof(ibrt_custom_cmd_test_t));
}

void app_ibrt_customif_test1_cmd_send(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_IBRT_CUSTOM_CMD_TEST1, p_buff, length);
    TRACE(1,"%s", __func__);
}
extern "C" void earphone_inout_res(uint8_t leftinoutflg,uint8_t rightinoutflg);
extern "C" bool earin_left_flg;
extern "C" bool earin_right_flg;
extern "C" uint8_t Left_batteryvolume;
extern "C" uint8_t Right_batteryvolume;
extern int app_shutdown(void);
static void app_ibrt_customif_test1_cmd_send_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);

    switch(p_buff[0])
	{
        case LEDONOFF_SYNC:
	   	aw22127_led2_onoff(p_buff[1],0);
	   	break;
		
	   case VOLUME_SYNC:
	   	aw22127_volume_set(p_buff[1],0);
		break;
		 
	   case BTCONNECTSTATIC_SYNC:
	   	aw22127_btconnected_sync();
		break;
		  
	   case BTDISCONNECTSTATIC_SYNC:
	   	aw22127_btdisconnected_sync();
		break;
		   
	   case RHYTHMTYPE_SYNC:
	   	aw22127_rhythm_colors(p_buff[1],0);
		break;

	   case THEME_SYNC:
	   	aw22127_theme_colors(p_buff[1],0);
	    break;

		case EARINOUT_SYNC:
        if(app_tws_is_left_side())
        {
          //earin_left_flg = true;
		  earin_right_flg = p_buff[2];
        }
        else if(app_tws_is_right_side())
        {
          //earin_right_flg = true;
		  earin_left_flg = p_buff[1];
        }		        
		earphone_inout_res(earin_left_flg,earin_right_flg);
		break;

		case LED2AUTO_SYNC:
			aw22127_led2_colors(p_buff[1],p_buff[2],p_buff[3],p_buff[4],p_buff[5],p_buff[6],p_buff[7],p_buff[8],p_buff[9],p_buff[10],p_buff[11],p_buff[12],p_buff[13],p_buff[14],p_buff[15],p_buff[16],p_buff[17],p_buff[18],p_buff[19],p_buff[20],false);
			break;

		case POWEROFF_SYNC:
			app_shutdown();
			break;


		case TWSBATTERY_SYNC:
			if(app_tws_is_left_side())
			{
			  Right_batteryvolume = p_buff[1];
			}
			else if(app_tws_is_right_side())
			{
			  Left_batteryvolume = p_buff[1];
			}				
			break;

	  default:break;	
	}

	
}

static void app_ibrt_customif_test2_cmd_send(uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
}

static void app_ibrt_customif_test2_cmd_send_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    tws_ctrl_send_rsp(APP_IBRT_CUSTOM_CMD_TEST2, rsp_seq, NULL, 0);
    TRACE(1,"%s", __func__);

}

static void app_ibrt_customif_test2_cmd_send_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);

}

static void app_ibrt_customif_test2_cmd_send_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
}

#ifdef CUSTOM_BITRATE
//#include "product_config.h"
#include "nvrecord_extension.h"
extern void a2dp_avdtpcodec_aac_user_configure(uint32_t bitrate,uint8_t user_configure);
extern void a2dp_avdtpcodec_sbc_user_configure(uint32_t bitpool,uint8_t user_configure);
extern void app_audio_dynamic_update_dest_packet_mtu(uint8_t codec_index, uint8_t packet_mtu, uint8_t user_configure);

void app_ibrt_user_a2dp_info_sync_tws_share_cmd_send(uint8_t *p_buff, uint16_t length)
{
    tws_ctrl_send_cmd(APP_TWS_CMD_A2DP_CONFIG_SYNC, p_buff, length);
}

static void app_ibrt_user_a2dp_info_sync(uint8_t *p_buff, uint16_t length)
{
    if((app_tws_ibrt_mobile_link_connected())){
        app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_A2DP_CONFIG_SYNC, p_buff, length);
    }
}

static void app_ibrt_user_a2dp_info_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    ibrt_custome_codec_t *a2dp_user_config_ptr=(ibrt_custome_codec_t *)p_buff;
    TRACE(4,"%s %d %d %d",__func__,a2dp_user_config_ptr->aac_bitrate,a2dp_user_config_ptr->sbc_bitpool,a2dp_user_config_ptr->audio_latentcy);
    if((app_tws_ibrt_slave_ibrt_link_connected())){
        a2dp_avdtpcodec_sbc_user_configure(a2dp_user_config_ptr->sbc_bitpool, true);
        a2dp_avdtpcodec_aac_user_configure(a2dp_user_config_ptr->aac_bitrate, true); 
        app_audio_dynamic_update_dest_packet_mtu(0, (a2dp_user_config_ptr->audio_latentcy-USER_CONFIG_AUDIO_LATENCY_ERROR)/3, true);//sbc
        app_audio_dynamic_update_dest_packet_mtu(1, (a2dp_user_config_ptr->audio_latentcy-USER_CONFIG_AUDIO_LATENCY_ERROR)/23, true);//aac
        uint32_t lock = nv_record_pre_write_operation();
        nv_record_get_extension_entry_ptr()->a2dp_user_info.aac_bitrate = a2dp_user_config_ptr->aac_bitrate;
        nv_record_get_extension_entry_ptr()->a2dp_user_info.sbc_bitpool = a2dp_user_config_ptr->sbc_bitpool;
        nv_record_get_extension_entry_ptr()->a2dp_user_info.audio_latentcy = a2dp_user_config_ptr->audio_latentcy;
        nv_record_post_write_operation(lock);
        nv_record_flash_flush();
    }
}
#endif
#endif
