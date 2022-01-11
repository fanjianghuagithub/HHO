/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifdef AUDIO_OUTPUT_DC_AUTO_CALIB
#include <stdlib.h>
#include <string.h>
#include <cmsis.h>
#ifdef RTOS
#include <cmsis_os.h>
#endif
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_codec.h"
#include "audioflinger.h"
#include "pmu.h"
#ifdef CODEC_DAC_DC_NV_DATA
#include "nvrecord.h"
#include "nvrecord_env.h"
#include "nvrecord_dev.h"
#endif
#ifdef CODEC_DAC_DC_DYN_BUF
#include "heap_api.h"
#endif

//#define CODEC_DAC_DC_MUTE_TEST

#define DAC_DC_SET_VALID_CH(c)    (DAC_DC_VALID_MARK|((c)&0xF))
#define DAC_DC_GET_VALID_CH(v)    ((v)&0xF)
#define DAC_DC_GET_VALID_MARK(v)  ((v)&(~(0xF)))

#define DC_OUT_VALUE      0x2000
#define DC_CALIB_BUF_SIZE 0x1000

bool static af_open_for_dac_dc_calib = false;

static void codec_dac_dc_dump_param(char *s, void *cfg);

static uint8_t *codec_dac_dc_calib_malloc(uint32_t len)
{
#ifndef CODEC_DAC_DC_DYN_BUF
    static uint8_t play_dac_buff[DC_CALIB_BUF_SIZE];
    ASSERT(len <= DC_CALIB_BUF_SIZE, "%s: len=%d out of range %d", __func__, len, DC_CALIB_BUF_SIZE);
    return (uint8_t *)play_dac_buff;
#else
    uint8_t *play_dac_buff = NULL;

    ASSERT(len <= DC_CALIB_BUF_SIZE, "%s: len=%d out of range %d", __func__, len, DC_CALIB_BUF_SIZE);
    syspool_init_specific_size(DC_CALIB_BUF_SIZE);
    syspool_get_buff(&play_dac_buff, len);
    ASSERT(play_dac_buff != NULL, "%s: alloc buff failed", __func__);
    return play_dac_buff;
#endif
}

static void codec_dac_dc_calib_free(uint8_t *buf)
{
    //TODO: free memory
}

static int codec_dac_dc_do_calib(uint32_t *status)
{
    int ret = 0;
    uint32_t i;
    uint32_t dc_l = 0, dc_r = 0;
    uint16_t ana_dc_l = 0, ana_dc_r = 0, max_l, max_r;
    enum HAL_SYSFREQ_USER_T user = HAL_SYSFREQ_USER_APP_7;
    uint32_t num;
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg, *calib_cfg;
    bool calib_ch_l, calib_ch_r;
    struct AF_CODEC_CALIB_CFG_T acfg, *afcfg;

    afcfg = &acfg;
    memset((void*)afcfg, 0, sizeof(acfg));

    hal_sysfreq_req(user, HAL_CMU_FREQ_104M);
    hal_sysfreq_print();
   // TRACE(1, "cpu clock: %u", hal_sys_timer_calc_cpu_freq(5, 0));

    if (af_open_for_dac_dc_calib) {
        af_open();
    }

    hal_codec_dac_dre_init_calib_cfg();
    hal_codec_set_dac_calib_status(true);
    calib_cfg = hal_codec_dac_dre_get_calib_cfg(&num);
    if (calib_cfg == NULL || num == 0) {
        TRACE(1, "%s: invalid calib_cfg=%x or num=%d", __func__, (int)calib_cfg, num);
        ret = -1;
        goto _exit0;
    }
    afcfg->len = DC_CALIB_BUF_SIZE;
    afcfg->buf = codec_dac_dc_calib_malloc(afcfg->len);

    calib_ch_l = true;
    calib_ch_r = false;

    // select DAC channel & open stream
    af_codec_calib_dac_chan_enable(calib_ch_l, calib_ch_r);
    af_codec_calib_dac_dc(CODEC_CALIB_CMD_OPEN, afcfg);

    // calib analog DAC DC
    max_l = max_r = 0;
#ifdef AUDIO_OUTPUT_SET_LARGE_ANA_DC
    for (i = 0, cfg = calib_cfg; i < num; i++, cfg++) {
        uint8_t ana_gain = cfg->ana_gain;
        uint8_t ini_gain = cfg->ini_ana_gain;
        uint8_t gain_offs = cfg->gain_offset;

        af_codec_calib_param_setup(DAC_PARAM_ANA_GAIN, ana_gain, ini_gain, gain_offs);
        ret = af_codec_calib_dac_dc(CODEC_CALIB_CMD_SET_LARGE_ANA_DC, afcfg);
        if (ret) {
            TRACE(1, "%s: error1 %d", __func__, ret);
            ret = -2;
            goto _exit1;
        }
        ana_dc_l = afcfg->ana_dc_l;
        ana_dc_r = afcfg->ana_dc_r;
        if(ana_dc_l > max_l) {
            max_l = ana_dc_l;
        }
        if(ana_dc_r > max_r) {
            max_r = ana_dc_r;
        }
    }
#else
    max_l = 0;
    max_r = 0;
#endif
    for (i = 0, cfg = calib_cfg; i < num; i++, cfg++) {
        cfg->ana_dc_l = max_l;
        cfg->ana_dc_r = max_r;
        TRACE(1, "AUTO CALIB ANA DAC DC: ana_gain=%d, L=0x%04X R=0x%04X, ret=%d",
            cfg->ana_gain, cfg->ana_dc_l, cfg->ana_dc_r, ret);
    }
    // calib digital DAC DC
    ana_dc_l = max_l;
    ana_dc_r = max_r;
    for (i = 0, cfg = calib_cfg; i < num; i++, cfg++) {
        uint8_t ana_gain = cfg->ana_gain;
        uint8_t ini_gain = cfg->ini_ana_gain;
        uint8_t gain_offs = cfg->gain_offset;

        af_codec_calib_param_setup(DAC_PARAM_ANA_GAIN, ana_gain, ini_gain, gain_offs);
        af_codec_calib_param_setup(DAC_PARAM_ANA_DC, ana_dc_l, ana_dc_r, 0);
        ret = af_codec_calib_dac_dc(CODEC_CALIB_CMD_DIG_DC, afcfg);
        if (ret) {
            TRACE(1, "%s: error2 %d", __func__, ret);
            ret = -3;
            goto _exit1;
        }
        dc_l = afcfg->dig_dc_l;
        dc_r = afcfg->dig_dc_r;
        cfg->dc_l = dc_l;
        cfg->dc_r = dc_r;
        cfg->valid = DAC_DC_SET_VALID_CH(1<<0);
        TRACE(1, "AUTO CALIB DAC DC: ana_gain=%d, L=0x%08X R=0x%08X, ret=%d",
            cfg->ana_gain, dc_l, dc_r, ret);
    }
    // calib digital DAC gain
    int32_t first_dc_l, first_dc_r;
    for (i = 0, cfg = calib_cfg; i < num; i++, cfg++) {
        uint8_t ana_gain = cfg->ana_gain;
        uint8_t ini_gain = cfg->ini_ana_gain;
        uint8_t gain_offs = cfg->gain_offset;
        uint32_t dig_dc_l = cfg->dc_l;
        uint32_t dig_dc_r = cfg->dc_r;
        float gain_comp_l, gain_comp_r;
        int out_l, out_r;

        af_codec_calib_param_setup(DAC_PARAM_ANA_GAIN, ana_gain, ini_gain, gain_offs);
        af_codec_calib_param_setup(DAC_PARAM_ANA_DC, ana_dc_l, ana_dc_r, 0);
        af_codec_calib_param_setup(DAC_PARAM_DIG_DC, dig_dc_l, dig_dc_r, 0);
        af_codec_calib_param_setup(DAC_PARAM_OUT_DC, DC_OUT_VALUE, 0, 0);
        ret = af_codec_calib_dac_dc(CODEC_CALIB_CMD_DIG_GAIN, afcfg);
        if (ret) {
            TRACE(1, "%s: error3 %d", __func__, ret);
            ret = -4;
            goto _exit1;
        }
        dc_l = afcfg->out_dc_l;
        dc_r = afcfg->out_dc_r;
        out_l = (int)dc_l;
        out_r = (int)dc_r;
        if (i == 0) {
            first_dc_l = out_l;
            first_dc_r = out_r;
            gain_comp_l = 1;
            gain_comp_r = 1;
        } else {
            gain_comp_l = (float)first_dc_l/(float)out_l;
            gain_comp_r = (float)first_dc_r/(float)out_r;
        }
        cfg->gain_l = gain_comp_l;
        cfg->gain_r = gain_comp_l;
        cfg->valid |= DAC_DC_SET_VALID_CH(1<<2);
        TRACE(1, "AUTO CALIB DAC GAIN: ana_gain=%d, L*1000=%d R*1000=%d",
            cfg->ana_gain, (int)(gain_comp_l*1000), (int)(gain_comp_r*1000));
    }
#if !defined(CODEC_DAC_DC_NV_DATA)
    for (i = 0; i < num; i++) {
        codec_dac_dc_dump_param("DRE_CALIB_PARAM", (void *)&calib_cfg[i]);
    }
#endif

_exit1:
    // close stream
    af_codec_calib_dac_dc(CODEC_CALIB_CMD_CLOSE, afcfg);
    codec_dac_dc_calib_free(afcfg->buf);
    hal_codec_set_dac_calib_status(false);
_exit0:
    if (af_open_for_dac_dc_calib) {
        af_close();
    }

    hal_sysfreq_req(user, HAL_CMU_FREQ_32K);
    return ret;
}

static void codec_dac_dc_dump_param(char *s, void *cfg)
{
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *c = (struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *)cfg;

    TRACE(1, "--------DUMP DAC DC param: %s, cfg=%x--------", s, (uint32_t)cfg);
    TRACE(1, "valid:        0x%x", c->valid);
    TRACE(1, "dc_l:         0x%x(%d)", c->dc_l, c->dc_l);
    TRACE(1, "dc_r:         0x%x(%d)", c->dc_r, c->dc_r);
    TRACE(1, "ana_dc_l:     0x%x", c->ana_dc_l);
    TRACE(1, "ana_dc_r:     0x%x", c->ana_dc_r);
    TRACE(1, "gain_l*1000:  %d", (int)(c->gain_l*1000));
    TRACE(1, "gain_r*1000:  %d", (int)(c->gain_r*1000));
    TRACE(1, "ana_gain:     0x%x", c->ana_gain);
    TRACE(1, "ini_ana_gain: 0x%x", c->ini_ana_gain);
    TRACE(1, "gain_offset:  0x%x", c->gain_offset);
    TRACE(1, "step_mode:    0x%x", c->step_mode);
    TRACE(1, "top_gain:     0x%x", c->top_gain);
    TRACE(1, "--------DUMP END--------");
}

static bool codec_dac_dc_load_calib_value(void)
{
#ifdef CODEC_DAC_DC_NV_DATA
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T tmp[4] = {0};
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg;
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_NV_T *nv_cfg;
    uint32_t num, i, valid = 0;
    bool success = false;

    TRACE(1, "%s: start", __func__);
    cfg = hal_codec_dac_dre_get_calib_cfg(&num);
    nv_cfg = nv_record_get_extension_entry_ptr()->dac_dre_calib_cfg_nv;
    TRACE(1, "cfg=%x, nv_cfg=%x, num=%d", (int)cfg, (int)nv_cfg, num);

    for (i = 0; i < num; i++) {
        int error = 0;
        codec_dac_dc_dump_param("NV", (void *)&nv_cfg[i]);
        error = hal_codec_dac_dre_check_calib_cfg((struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *)&nv_cfg[i]);
        if (!error) {
            TRACE(1, "nv_cfg[%d](%x) -> tmp[%d](%x)", i, (int)(&nv_cfg[i]), i, (int)(&tmp[i]));
            memcpy((void *)&tmp[i], (void *)&nv_cfg[i], sizeof(struct HAL_CODEC_DAC_DRE_CALIB_CFG_T));
            valid++;
        }
    }
    if (valid == num) {
        for (i = 0; i < num; i++) {
            TRACE(1, "tmp[%d](%x) -> cfg[%d](%x)", i, (int)(&tmp[i]), i, (int)(&cfg[i]));
            memcpy((void *)&cfg[i], (void *)&tmp[i], sizeof(struct HAL_CODEC_DAC_DRE_CALIB_CFG_T));
            codec_dac_dc_dump_param("NV->DC", (void *)&cfg[i]);
        }
        success = true;
    }
    TRACE(1, "%s: success=%d, valid=%d, num=%d", __func__, success, valid, num);
    return success;
#else
    return false;
#endif
}

static bool codec_dac_dc_save_calib_value(void)
{
#ifdef CODEC_DAC_DC_NV_DATA
    uint32_t lock;
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_NV_T *nv_cfg;
    struct HAL_CODEC_DAC_DRE_CALIB_CFG_T *cfg;
    uint32_t num, i;

    cfg = hal_codec_dac_dre_get_calib_cfg(&num);
    nv_cfg = nv_record_get_extension_entry_ptr()->dac_dre_calib_cfg_nv;
    TRACE(1, "%s: cfg=%x, nv_cfg=%x, num=%d", __func__, (int)cfg, (int)nv_cfg, num);

    lock = nv_record_pre_write_operation();
    for (i = 0; i < num; i++) {
        int error = hal_codec_dac_dre_check_calib_cfg(&cfg[i]);
        ASSERT(!error, "%s: invalid param %x", __func__, error);

        TRACE(1, "nv_cfg[%d]=%x, cfg[%d]=%x", i, (int)(&nv_cfg[i]), i, (int)(&cfg[i]));
        memcpy((void *)&nv_cfg[i], (void *)&cfg[i], sizeof(struct HAL_CODEC_DAC_DRE_CALIB_CFG_T));
        codec_dac_dc_dump_param("DC->NV", (void *)&cfg[i]);
    }
    nv_record_post_write_operation(lock);
    nv_record_update_runtime_userdata();
    nv_record_flash_flush();
    osDelay(50);
#endif
    TRACE(1, "%s: done", __func__);
    return true;
}

#ifdef CODEC_DAC_DC_MUTE_TEST
static uint32_t play_data_handler(uint8_t *buf, uint32_t len)
{
    uint32_t curtime = TICKS_TO_MS(hal_sys_timer_get());

    memset((void*)buf, 0, len);

    (void)curtime;
    //TRACE(1, "[%8d] PLAY: buf=%x, len=%d", curtime, (int)buf, len);
    return 0;
}

int codec_dac_play_mute(bool on)
{
#define PLAY_RATE  (AUD_SAMPRATE_48000)
#define PLAY_BITS  (AUD_BITS_24)
#define PLAY_SIZE  (4)
#define PLAY_FRM   (8)
#define PLAY_CHNUM (AUD_CHANNEL_NUM_2)
#define PLAY_CHMAP (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
    int ret = 0;

    TRACE(1, "%s: on=%d", __func__, on);
    if (on) {
        struct AF_STREAM_CONFIG_T stream_cfg;
        uint32_t play_buf_size = (PLAY_RATE/1000)*PLAY_SIZE*PLAY_CHNUM*PLAY_FRM;
        uint8_t *play_buf = codec_dac_dc_calib_malloc(play_buf_size);

        hal_sysfreq_req(HAL_SYSFREQ_USER_APP_2, HAL_CMU_FREQ_52M);
        if (af_open_for_dac_dc_calib) {
            af_open();
        }
        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.sample_rate = PLAY_RATE;
        stream_cfg.bits        = PLAY_BITS;
        stream_cfg.channel_num = PLAY_CHNUM;
        stream_cfg.channel_map = PLAY_CHMAP;
        stream_cfg.device      = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.vol         = 15;
        stream_cfg.handler     = play_data_handler;
        stream_cfg.io_path     = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.data_ptr    = play_buf;
        stream_cfg.data_size   = play_buf_size;
        TRACE(1,"playback rate: %d, bits = %d, buf_size=%d",
            stream_cfg.sample_rate, stream_cfg.bits, play_buf_size);
        ret = af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);
        ASSERT(ret == 0, "af_stream_open playback failed: %d", ret);
        ret = af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        ASSERT(ret == 0, "af_stream_start playback failed: %d", ret);

        while (1) {
#ifndef RTOS
            extern void af_thread(void const *argument);
            af_thread(NULL);
#else
            osDelay(20);
#endif
        }

    } else {
        af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        if (af_open_for_dac_dc_calib) {
            af_close();
        }
        hal_sysfreq_req(HAL_SYSFREQ_USER_APP_2, HAL_CMU_FREQ_32K);
    }
    return ret;
}
#endif

int codec_dac_dc_auto_load(bool open_af, bool reboot, bool init_nv)
{
    static int done = 0;
    int r = 0;
    uint32_t time = hal_sys_timer_get();

    if (done) {
        TRACE(1, "%s: already done", __func__);
        return 0;
    }
    af_open_for_dac_dc_calib = open_af;
    TRACE(1, "%s: start: open_af=%d, reboot=%d", __func__, open_af, reboot);
#ifdef CODEC_DAC_DC_NV_DATA
    if (init_nv) {
        nv_record_init();
    }
#endif
    if (!codec_dac_dc_load_calib_value()) {
        r = codec_dac_dc_do_calib(NULL);
        if (!r) {
            codec_dac_dc_save_calib_value();
            codec_dac_dc_load_calib_value();
            done = 1;
            if (reboot) {
                hal_sys_timer_delay(MS_TO_TICKS(5));
                pmu_reboot();
            }
        }
    }
    time = hal_sys_timer_get() - time;
    TRACE(1, "%s: CALIB_COST_TIME=%d, r=%d", __func__, TICKS_TO_MS(time), r);

#ifdef CODEC_DAC_DC_MUTE_TEST
    codec_dac_play_mute(true);
#endif
    return r;
}

#endif /* AUDIO_OUTPUT_DC_AUTO_CALIB */
