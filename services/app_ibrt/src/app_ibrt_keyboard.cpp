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
#include "cmsis_os.h"
#include <string.h>
#include "app_ibrt_keyboard.h"
#include "app_tws_ibrt_trace.h"
#include "app_ibrt_if.h"
#include "app_ibrt_ui_test.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_tws_ctrl_thread.h"
#include "app_trace_rx.h"
#include "btapp.h"
#include "apps.h"
#include "app_tws_if.h"
#include "led.h"
#include "app_anc.h"

#if defined(IBRT)
#ifdef TILE_DATAPATH
extern "C" void app_tile_key_handler(APP_KEY_STATUS *status, void *param);
#endif
extern struct BT_DEVICE_T  app_bt_device;
extern void app_bt_volumedown();
extern void app_bt_volumeup();
extern void app_otaMode_enter(APP_KEY_STATUS *status, void *param);

#if defined(__BT_DEBUG_TPORTS__) && !defined(TPORTS_KEY_COEXIST)
extern void app_ibrt_simulate_charger_plug_in_test(void);
extern void app_ibrt_simulate_charger_plug_out_test(void);
#endif


//extern "C" bool boxin_flg;
#ifdef IBRT_SEARCH_UI
void app_ibrt_search_ui_handle_key(APP_KEY_STATUS *status, void *param)
{
  //  ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);


    if (APP_KEY_CODE_GOOGLE != status->code)
    {
        switch(status->event)
        {


#ifdef SPT513
           case APP_KEY_EVENT_EARIN:
		   	bt_key_handle_earin();
		   	break;

			case APP_KEY_EVENT_EAROUT:
			bt_key_handle_earout();
			break;
#endif


            case APP_KEY_EVENT_CLICK:
                app_tws_if_handle_click();
                break;

            case APP_KEY_EVENT_DOUBLECLICK:
				bt_key_handle_forward();
                break;
				

            case APP_KEY_EVENT_LONGPRESS_LEFT:
			if(read_led_on_status())
			{
               aw22127_led2_onoff(0,1);
			}
			else
			{
               aw22127_led2_onoff(1,1);
			}
			break;


			case APP_KEY_EVENT_LONGPRESS_RIGHT:
				//app_anc_loop_switch_on_off();
				app_anc_loop_switch();
			break;

				
            case APP_KEY_EVENT_LONGPRESS:

                break;
				

            case APP_KEY_EVENT_TRIPLECLICK:
            #ifdef TILE_DATAPATH
                app_tile_key_handler(status,NULL);
            #else
                app_otaMode_enter(NULL,NULL);
            #endif
                break;
            case HAL_KEY_EVENT_LONGLONGPRESS:
                TRACE(0,"long long press");
                app_shutdown();
                break;

            case APP_KEY_EVENT_ULTRACLICK:
                TRACE(0,"ultra kill");
                break;

            case APP_KEY_EVENT_RAMPAGECLICK:
                TRACE(0,"rampage kill!you are crazy!");
                break;

            case APP_KEY_EVENT_UP:
                break;
        }
    }

#ifdef TILE_DATAPATH
    if(APP_KEY_CODE_TILE == status->code)
        app_tile_key_handler(status,NULL);
#endif
}
#endif

void app_ibrt_normal_ui_handle_key(APP_KEY_STATUS *status, void *param)
{
    if (APP_KEY_CODE_GOOGLE != status->code)
    {
        switch(status->event)
        {
            case APP_KEY_EVENT_CLICK:
                TRACE(0,"first blood.");
                app_tws_if_handle_click();
                break;

            case APP_KEY_EVENT_DOUBLECLICK:
                TRACE(0,"double kill");
                app_ibrt_if_enter_freeman_pairing();
                break;

            case APP_KEY_EVENT_LONGPRESS:
                app_ibrt_if_enter_pairing_after_tws_connected();
                break;

            case APP_KEY_EVENT_TRIPLECLICK:
            #ifdef TILE_DATAPATH
                app_tile_key_handler(status,NULL);
            #endif
                break;

            case HAL_KEY_EVENT_LONGLONGPRESS:
                TRACE(0,"long long press");
                break;

            case APP_KEY_EVENT_ULTRACLICK:
                TRACE(0,"ultra kill");
                app_shutdown();
                break;

            case APP_KEY_EVENT_RAMPAGECLICK:
                TRACE(0,"rampage kill!you are crazy!");
#ifndef HAL_TRACE_RX_ENABLE
                app_trace_rx_open();
#endif
                break;

            case APP_KEY_EVENT_UP:
                break;
        }
    }

#ifdef TILE_DATAPATH
    if(APP_KEY_CODE_TILE == status->code)
        app_tile_key_handler(status,NULL);
#endif
}

int app_ibrt_if_keyboard_notify(APP_KEY_STATUS *status, void *param)
{
    if (app_tws_ibrt_tws_link_connected())
    {
        tws_ctrl_send_cmd(APP_TWS_CMD_KEYBOARD_REQUEST, (uint8_t *)status, sizeof(APP_KEY_STATUS));
    }
    return 0;
}

void app_ibrt_send_keyboard_request(uint8_t *p_buff, uint16_t length)
{
    if (app_tws_ibrt_tws_link_connected())
    {
        app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_KEYBOARD_REQUEST, p_buff, length);
    }
}

void app_ibrt_keyboard_request_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    APP_KEY_STATUS *key_status=(APP_KEY_STATUS *)p_buff;
    if(key_status == NULL)
        return ;

    if (app_tws_ibrt_tws_link_connected())
    {
#ifdef IBRT_SEARCH_UI

        if(key_status->code==APP_KEY_CODE_FN1)
        {
            app_ibrt_search_ui_handle_key(key_status, NULL);
        }
        else
        {
            app_ibrt_search_ui_gpio_key_handle(key_status,NULL);
        }

#else
        app_ibrt_normal_ui_handle_key(key_status, NULL);
#endif

        app_ibrt_ui_test_voice_assistant_key(key_status, NULL);
    }
}

void app_ibrt_if_start_user_action(uint8_t *p_buff, uint16_t length)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    TRACE(3,"%s role %d code %d",__func__, p_ibrt_ctrl->current_role, p_buff[0]);

    if (IBRT_SLAVE == p_ibrt_ctrl->current_role)
    {
        app_ibrt_ui_send_user_action(p_buff, length);
    }
    else
    {
        app_ibrt_ui_perform_user_action(p_buff, length);
    }
}

extern void app_bt_volumeup();
extern void app_bt_volumedown();

void app_ibrt_ui_perform_user_action(uint8_t *p_buff, uint16_t length)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    if (IBRT_MASTER != p_ibrt_ctrl->current_role)
    {
        TRACE(2,"%s not ibrt master, skip cmd %02x\n", __func__, p_buff[0]);
        return;
    }

    switch (p_buff[0])
    {
        case IBRT_ACTION_PLAY:
            a2dp_handleKey(AVRCP_KEY_PLAY);
            break;
        case IBRT_ACTION_PAUSE:
            a2dp_handleKey(AVRCP_KEY_PAUSE);
            break;
        case IBRT_ACTION_FORWARD:
            a2dp_handleKey(AVRCP_KEY_FORWARD);
            break;
        case IBRT_ACTION_BACKWARD:
            a2dp_handleKey(AVRCP_KEY_BACKWARD);
            break;
        case IBRT_ACTION_AVRCP_VOLUP:
            a2dp_handleKey(AVRCP_KEY_VOLUME_UP);
            break;
        case IBRT_ACTION_AVRCP_VOLDN:
            a2dp_handleKey(AVRCP_KEY_VOLUME_DOWN);
            break;
        case IBRT_ACTION_HFSCO_CREATE:
            if (!btif_hf_audio_link_is_up())
            {
                btif_pts_hf_create_audio_link();
            }
            else
            {
                TRACE(1,"%s cannot create audio link\n", __func__);
            }
            break;
        case IBRT_ACTION_HFSCO_DISC:
            if (btif_hf_audio_link_is_up())
            {
                btif_pts_hf_disc_audio_link();
            }
            else
            {
                TRACE(1,"%s cannot disc audio link\n", __func__);
            }
            break;
        case IBRT_ACTION_REDIAL:
            if (btif_hf_service_link_is_up())
            {
                btif_pts_hf_redial_call();
            }
            else
            {
                TRACE(1,"%s cannot redial call\n", __func__);
            }
            break;
        case IBRT_ACTION_ANSWER:
            if (btif_hf_service_link_is_up())
            {
                btif_pts_hf_answer_call();
            }
            else
            {
                TRACE(1,"%s cannot answer call\n", __func__);
            }
            break;
        case IBRT_ACTION_HANGUP:
            if (btif_hf_service_link_is_up())
            {
                btif_pts_hf_hangup_call();
            }
            else
            {
                TRACE(1,"%s cannot hangup call\n", __func__);
            }
            break;
        case IBRT_ACTION_LOCAL_VOLUP:
            app_bt_volumeup();
            app_ibrt_sync_volume_info();
            break;
        case IBRT_ACTION_LOCAL_VOLDN:
            app_bt_volumedown();
            app_ibrt_sync_volume_info();
            break;
        default:
            TRACE(2,"%s unknown user action %d\n", __func__, p_buff[0]);
            break;
    }
}

#endif

