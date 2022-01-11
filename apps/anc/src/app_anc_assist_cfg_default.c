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
#include "anc_assist.h"


const static int16_t custom_leak_detect_pcm_data[] = {
   #include "res/ld/tone_peco.h"
};

AncAssistConfig anc_assist_cfg = {
    .bypass = 0,
    .debug_en = 0,

    .ff_howling_en  = 0,
    .fb_howling_en  = 0,
    .noise_en   = 0,
    .wind_en    = 0,
	.pilot_en   = 0,
	.pnc_en     = 0,
	.noise_classify_en  = 0,
    .wsd_en     = 0,
    .extern_kws_en     = 0,

    .ff_howling_cfg = {
        .ind0 = 32,         // 62.5Hz per 62.5*32=2k
        .ind1 = 120,        // 62.5*120=7.5k
        .time_thd = 500,    // ff recover time 500*7.5ms=3.75s
        .power_thd = 1e4f,
    },

    .fb_howling_cfg = {
        .ind0 = 32,         // 62.5Hz per 62.5*32=2k
        .ind1 = 120,        // 62.5*120=7.5k
        .time_thd = 500,    // ff recover time 500*7.5ms=3.75s
        .power_thd = 1e5f,
    },

    .noise_cfg = {
        .power_thd0 = 15,
        .power_thd1 = 30,
        .snr_thd = 100,
        .silence_gain = 0.5,
    },

    .wind_cfg = {
        .scale_size = 8,           // freq range,8/scale=1k
        .wind_to_small_targettime = 250, // time=250*7.5ms=1.875s
        .to_none_targettime = 500, // time=500*7.5ms=3.75s
        .power_thd = 0.0005,
        .no_thd = 0.8,
        .small_thd = 0.6,
        .strong_thd = 0.3,
        .gain_none = 1.0,
        .gain_small_to_none = 0.75,
        .gain_small = 0.5,
        .gain_strong_to_small = 0.25,
        .gain_strong = 0.0,
    },

    .pilot_cfg = {
        .dump_en = 0,
	    .delay = 310,
	    .cal_period = 25,
        .gain_smooth_ms = 300,

        .adaptive_anc_en = 0,
	    .thd_on = {5.6400,  1.5648,  0.6956,  0.4672,  0.4089,  0.2694, 0.1853,0.1373,0.0629,0.0325,0.0043},
	    .thd_off = {5.6400,  1.5648,  0.6956,  0.4672,  0.4089,  0.2694, 0.1853,0.1373,0.0629,0.0325,0.0043},

        .wd_en =0,
        .inear_thd = 1, //0.1,
        .outear_thd = 0.5, // 0.02,

        .custom_leak_detect_en = 0,
        .custom_leak_detect_playback_loop_num = 2,
        .custom_pcm_data = custom_leak_detect_pcm_data,
        .custom_pcm_data_len = sizeof(custom_leak_detect_pcm_data) / sizeof(int16_t),
        .gain_local_pilot_signal = 0.0,
    },

    .pnc_cfg = {
        .pnc_lower_bound = 22,
        .pnc_upper_bound = 25,
	    .out_lower_bound = 34,
	    .out_upper_bound = 38,
        .cal_period = 25,
        .out_thd = 8.0,
    },

    .noise_classify_cfg = {
        .slope_thd1 = 3.5,
        .slope_thd2 = 1.5,
        .cache_num = 300,
        .ps_index1 = 8,
        .ps_index2 = 16,
    },
};