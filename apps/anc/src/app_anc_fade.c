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
#include "plat_types.h"
#include "hal_trace.h"
#include "hal_aud.h"
#include "hal_timer.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "anc_process.h"
#include "app_anc_utils.h"
#include "apps.h"
#ifdef IBRT
#include "app_tws_ibrt.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_tws_ctrl_thread.h"
#endif


typedef enum {
    APP_ANC_FADE_IDLE = 0,
    APP_ANC_FADE_IN,
    APP_ANC_FADE_OUT,
    APP_ANC_FADE_SWITCH_COEF,
} app_anc_fade_t;

#define ANC_FADE_STACK_SIZE (1 * 1024)

static osThreadId anc_fade_thread_tid;
static void anc_fade_thread(void const *argument);
osThreadDef(anc_fade_thread, osPriorityBelowNormal, 1, ANC_FADE_STACK_SIZE, "anc_fade");

static uint32_t g_anc_fade_types = 0;

extern void app_anc_update_coef(void);

static void anc_fadein_impl(uint32_t types)
{
    enum ANC_TYPE_T type = ANC_NOTYPE;
    bool fade_flag[ANC_TYPE_NUM];

    TRACE(0, "[%s] types: 0x%x", __func__, types);

    for (uint32_t index=0; index<ANC_TYPE_NUM; index++) {
        fade_flag[index] = false;
    }

    while (1) {
        bool fade_total_flag = true;
        for (uint32_t index=0; index<ANC_TYPE_NUM; index++) {
            type = types & (0x01 << index);
            if (fade_flag[index] || (type == 0)) {
                continue;
            }

            int32_t left_curr,  rigth_curr;
            int32_t left_tgt,   right_tgt;
            anc_get_gain(&left_curr, &rigth_curr, type);
            anc_get_cfg_gain(&left_tgt, &right_tgt, type);

            if (left_tgt > 0) {
                if (left_curr < left_tgt) {
                    left_curr++;
                }
            } else {
                if (left_curr > left_tgt) {
                    left_curr--;
                }
            }

            if (right_tgt > 0) {
                if (rigth_curr < right_tgt) {
                    rigth_curr++;
                }
            } else {
                if (rigth_curr > right_tgt) {
                    rigth_curr--;
                }
            }

            hal_sys_timer_delay_us(20);

            anc_set_gain(left_curr, rigth_curr, type);

            if ((left_curr == left_tgt) && (rigth_curr == right_tgt)) {
                TRACE(0, "[%s] %d fadein successfully(%d, %d)", __func__, index, left_curr, rigth_curr);
                fade_flag[index] = true;
            } else {
                fade_total_flag = false;
            }
        }

        if (fade_total_flag) {
            break;
        }
    }

    for (uint32_t index=0; index<ANC_TYPE_NUM; index++) {
        type = types & (0x01 << index);
        if (type) {
            app_anc_reset_gain(type, 1.0, 1.0);
        }
    }
}

static void anc_fadeout_impl(uint32_t types)
{
    enum ANC_TYPE_T type = ANC_NOTYPE;
    bool fade_flag[ANC_TYPE_NUM];

    TRACE(0, "[%s] types: 0x%x", __func__, types);

    for (uint32_t index=0; index<ANC_TYPE_NUM; index++) {
        fade_flag[index] = false;
    }

    while (1) {
        bool fade_total_flag = true;
        for (uint32_t index=0; index<ANC_TYPE_NUM; index++) {
            type = types & (0x01 << index);
            if (fade_flag[index] || (type == 0)) {
                continue;
            }

            int32_t left_curr,  rigth_curr;
            anc_get_gain(&left_curr, &rigth_curr, type);

            if (left_curr > 0) {
                left_curr--;
            } else if (left_curr < 0) {
                left_curr++;
            }

            if (rigth_curr > 0) {
                rigth_curr--;
            } else if (rigth_curr < 0) {
                rigth_curr++;
            }

            hal_sys_timer_delay_us(20);

            anc_set_gain(left_curr, rigth_curr, type);

            if ((left_curr == 0) && (rigth_curr == 0)) {
                TRACE(0, "[%s] %d fadeout successfully", __func__, index);
                fade_flag[index] = true;
            } else {
                fade_total_flag = false;
            }
        }

        if (fade_total_flag) {
            break;
        }
    }

    for (uint32_t index=0; index<ANC_TYPE_NUM; index++) {
        type = types & (0x01 << index);
        if (type) {
            app_anc_reset_gain(type, 0.0, 0.0);
        }
    }
}

static void anc_fade_thread(void const *argument)
{
    osEvent evt;
    evt.status = 0;
    uint32_t singles = 0;
	ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    while(1) {
        evt = osSignalWait(0,osWaitForever);
        
        singles = evt.value.signals;
        TRACE(0, "[%s] singles: %d", __func__, singles);

        if(evt.status == osEventSignal) {
            uint32_t anc_fade_types = g_anc_fade_types;
            switch (singles) {
                case APP_ANC_FADE_IN:
                    anc_fadein_impl(anc_fade_types);
                    break;
                case APP_ANC_FADE_OUT:
                    anc_fadeout_impl(anc_fade_types);
                    break;
                 case APP_ANC_FADE_SWITCH_COEF:
                    anc_fadeout_impl(anc_fade_types);
                    app_anc_update_coef();
                    anc_fadein_impl(anc_fade_types);
                    // TODO: Recommend to play "ANC SWITCH" prompt here...
                    
					if(IBRT_SLAVE != p_ibrt_ctrl->current_role) 
					{
						 // app_voice_report(APP_STATUS_INDICATION_GSOUND_MIC_OPEN,0);
					}


                    break;
                default:
                    break;
            }
        } else {
            TRACE(0,"anc fade thread evt error");
            continue;
        }
    }
}

int32_t app_anc_fade_init(void)
{
    TRACE(0, "[%s] ...", __func__);

    g_anc_fade_types = 0;
    anc_fade_thread_tid = osThreadCreate(osThread(anc_fade_thread),NULL);

    return 0;
}

int32_t app_anc_fade_deinit(void)
{
    TRACE(0, "[%s] ...", __func__);

    return 0;
}

int32_t app_anc_fadein(uint32_t types)
{
    TRACE(0, "[%s] types: 0x%0x, APP_ANC_FADE_IN ...", __func__, types);

    g_anc_fade_types = types;
    osSignalSet(anc_fade_thread_tid, APP_ANC_FADE_IN);

    return 0;
}

int32_t app_anc_fadeout(uint32_t types)
{
    TRACE(0, "[%s] types: 0x%0x, APP_ANC_FADE_OUT ...", __func__, types);

    g_anc_fade_types = types;
    osSignalSet(anc_fade_thread_tid, APP_ANC_FADE_OUT);

    return 0;
}

int32_t app_anc_fade_switch_coef(uint32_t types)
{
    TRACE(0, "[%s] types: 0x%0x, APP_ANC_FADE_SWITCH", __func__, types);

    g_anc_fade_types = types;
    osSignalSet(anc_fade_thread_tid, APP_ANC_FADE_SWITCH_COEF);

    return 0;
}
