#ifndef __TGT_HARDWARE__
#define __TGT_HARDWARE__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_key.h"
#include "hal_aud.h"


/* single download */
#define CFG_HW_SINGLEL_NUM  (0)

#if (CFG_HW_SINGLEL_NUM >= 1)
extern const uint8_t cfg_hw_pin_level[CFG_HW_SINGLEL_NUM];
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_single[CFG_HW_SINGLEL_NUM];
#endif

//pwl
#define CFG_HW_PWL_NUM (0)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_pwl[CFG_HW_PWL_NUM];

//adckey define
#define CFG_HW_ADCKEY_NUMBER 0
#define CFG_HW_ADCKEY_BASE 0
#define CFG_HW_ADCKEY_ADC_MAXVOLT 1000
#define CFG_HW_ADCKEY_ADC_MINVOLT 0
#define CFG_HW_ADCKEY_ADC_KEYVOLT_BASE 130
extern const uint16_t CFG_HW_ADCKEY_MAP_TABLE[CFG_HW_ADCKEY_NUMBER];

#define BTA_AV_CO_SBC_MAX_BITPOOL  53

//gpiokey define
#define CFG_HW_GPIOKEY_NUM (2)
extern const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM];

// audio codec
#define CFG_HW_AUD_INPUT_PATH_NUM           2
extern const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM];

#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV (AUD_CHANNEL_MAP_CH0 | AUD_VMIC_MAP_VMIC1)
#define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)

//bt config
extern const char *BT_LOCAL_NAME;
extern const char *BLE_DEFAULT_NAME;
extern uint8_t ble_addr[6];
extern uint8_t bt_addr[7];

#define CODEC_SADC_VOL (7)

#define MAX_DIG_DAC_REGVAL (21)

extern const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY];

#define CFG_AUD_EQ_IIR_NUM_BANDS (4)

//battery info
#define APP_BATTERY_MIN_MV (3200)
#define APP_BATTERY_PD_MV   (3100)

#define APP_BATTERY_MAX_MV (4200)

extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg;

extern bool is_ota_remap_enabled(void);

#ifdef __cplusplus
}
#endif

#endif