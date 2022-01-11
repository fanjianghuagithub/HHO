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
#ifndef __APP_IBRT_IF_CUSTOM_CMD__
#define __APP_IBRT_IF_CUSTOM_CMD__

#include "app_ibrt_custom_cmd.h"

typedef enum
{
    APP_IBRT_CUSTOM_CMD_TEST1 = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x01,
    APP_IBRT_CUSTOM_CMD_TEST2 = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x02,

    APP_TWS_CMD_SHARE_FASTPAIR_INFO = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x03,

#ifdef BISTO_ENABLED
    APP_TWS_CMD_BISTO_DIP_SYNC      = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x04,
#endif
#ifdef __DUAL_MIC_RECORDING__
    APP_IBRT_CUSTOM_CMD_DMA_AUDIO     = APP_IBRT_CMD_BASE | APP_IBRT_CUSTOM_CMD_PREFIX | 0x05,
#endif
#ifdef GFPS_ENABLED
    APP_TWS_CMD_GFPS_RING_STOP_SYNC = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x08,
#endif
#ifdef CUSTOM_BITRATE
    APP_TWS_CMD_A2DP_CONFIG_SYNC  = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x09,
#endif
    //new customer cmd add here
} app_ibrt_custom_cmd_code_e;

typedef struct
{
    uint8_t rsv;
    uint8_t buff[6];
} __attribute__((packed))ibrt_custom_cmd_test_t;

#ifdef CUSTOM_BITRATE
typedef struct
{
    uint32_t aac_bitrate;
    uint32_t sbc_bitpool;
    uint32_t audio_latentcy;
} ibrt_custome_codec_t; 
void app_ibrt_user_a2dp_info_sync_tws_share_cmd_send(uint8_t *p_buff, uint16_t length);

#define USER_CONFIG_DEFAULT_AUDIO_LATENCY (280)
#define USER_CONFIG_AUDIO_LATENCY_ERROR (20)

#define USER_CONFIG_MIN_AAC_BITRATE (197)
#define USER_CONFIG_MAX_AAC_BITRATE (256)
#define USER_CONFIG_DEFAULT_AAC_BITERATE USER_CONFIG_MAX_AAC_BITRATE

#define USER_CONFIG_MIN_SBC_BITPOOL (41)
#define USER_CONFIG_MAX_SBC_BITPOOL (53)
#define USER_CONFIG_DEFAULT_SBC_BITPOOL USER_CONFIG_MAX_SBC_BITPOOL
#endif

void app_ibrt_customif_cmd_test(ibrt_custom_cmd_test_t *cmd_test);
extern void app_ibrt_customif_test1_cmd_send(uint8_t *p_buff, uint16_t length);



#endif
