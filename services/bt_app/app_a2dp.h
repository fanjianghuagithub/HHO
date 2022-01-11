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
#ifndef __APP_A2DP_H__
#define __APP_A2DP_H__
#include "btapp.h"

#if defined(__cplusplus)
extern "C" {
#endif
#define MAX_A2DP_VOL   (127)
bool a2dp_is_music_ongoing(void);
void a2dp_volume_set(enum BT_DEVICE_ID_T id, uint8_t vol);
bool is_bd_addr_valid(bt_bdaddr_t * addr);
#if defined(A2DP_LDAC_ON)
void app_ibrt_restore_ldac_info(uint8_t sample_freq);
#endif
uint16_t a2dp_Get_curr_a2dp_conhdl(void);
#ifdef __cplusplus
}
#endif

#endif