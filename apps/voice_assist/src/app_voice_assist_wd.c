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
#include "hal_trace.h"
#include "anc_assist.h"
#include "app_anc_assist.h"
#include "app_voice_assist_wd.h"

static int32_t _voice_assist_wd_callback(void *buf, uint32_t len, void *other);

int32_t app_voice_assist_wd_init(void)
{
    app_anc_assist_register(ANC_ASSIST_USER_WD, _voice_assist_wd_callback);

    return 0;
}

int32_t app_voice_assist_wd_open(void)
{
    app_anc_assist_open(ANC_ASSIST_USER_WD);

    return 0;
}

int32_t app_voice_assist_wd_close(void)
{
    app_anc_assist_close(ANC_ASSIST_USER_WD);

    return 0;
}

static int32_t _voice_assist_wd_callback(void *buf, uint32_t len, void *other)
{
    // TRACE(0, "[%s] len = %d", __func__, len);

    uint32_t *res = (uint32_t *)buf;

    anc_assist_algo_status_t wd_changed = res[0];
    wd_status_t wd_status = res[1];

    if (wd_changed == ANC_ASSIST_ALGO_STATUS_CHANGED) {
        if (wd_status == WD_STATUS_IN_EAR) {
            TRACE(0, "[%s] Change to WD_STATUS_IN_EAR", __func__);
        } else {
            TRACE(0, "[%s] Change to WD_STATUS_OUT_EAR", __func__);
        }
    }

    return 0;
}

