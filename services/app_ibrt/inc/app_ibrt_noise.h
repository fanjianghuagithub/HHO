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
#if defined(IBRT)
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void app_ibrt_get_peer_noise_info(uint8_t *p_buff, uint16_t length);
void app_ibrt_get_peer_noise_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void app_ibrt_get_peer_noise_info_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
bool app_ibrt_ui_tws_switch_according_noise_needed(void);
void app_ibrt_ui_check_noise_roleswitch_timer_cb(void const *current_evt);
void app_ibrt_ui_noise_reset(void);
void app_ibrt_ui_noise_process(float noise_power, float snr);

#ifdef __cplusplus
}
#endif

#endif