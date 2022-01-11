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
#ifndef __APP_ANC_UTILS_H__
#define __APP_ANC_UTILS_H__

#include "plat_types.h"

typedef enum {
    ANC_GAIN_USER_APP_ANC = 0,
    ANC_GAIN_USER_ANC_ASSIST,
    ANC_GAIN_USER_TUNING,
    ANC_GAIN_USER_QTY
} anc_gain_user_t;

#ifdef __cplusplus
extern "C" {
#endif

int32_t app_anc_reset_gain(enum ANC_TYPE_T type, float gain_l, float gain_r);
int32_t app_anc_enable_gain(enum ANC_TYPE_T type);
int32_t app_anc_disable_gain(enum ANC_TYPE_T type);
int32_t app_anc_set_gain_f32(anc_gain_user_t user, enum ANC_TYPE_T type, float gain_l, float gain_r);

#ifdef __cplusplus
}
#endif

#endif
