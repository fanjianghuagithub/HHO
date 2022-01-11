
#ifndef __BLE_CMD_H__
#define __BLE_CMD_H__
#include "app_ble_cmd_handler.h"


#ifdef __cplusplus
extern "C" {
#endif



#define APP_INQUIRY_LEDONE_CMD         0x43
#define APP_INQUIRY_LEDTWO_CMD         0x44
#define APP_INQUIRY_LEDAUTO_CMD        0x45






/***********************************************************************************************/
#define APP_INQUIRY_BATTERY_CMD               0x40
#define ERAPHONE_REPLY_BATTERY_CMD            0x41
#define APP_INQUIRY_RHYTHM_SET_CMD            0x60
#define APP_INQUIRY_RHYTHM_CMD                0x61
#define APP_INQUIRY_RHYTHM_CMD_RES            0x62
#define APP_INQUIRY_LEDONOFF_SET_CMD          0x48
#define APP_INQUIRY_LEDONOFF_CMD              0x49
#define APP_INQUIRY_LEDONOFF_CMD_RES          0x4A
#define APP_INQUIRY_VOLUME_SET_CMD            0x4B
#define APP_INQUIRY_VOLUME_CMD                0x4C
#define APP_INQUIRY_VOLUME_CMD_RES            0x4D
#define ERAPHONE_REPLY_EARPHONECONNECT_CMD    0x4E
#define APP_INQUIRY_ANC_SET_CMD               0x4F
#define APP_INQUIRY_ANC_CMD                   0x50
#define APP_INQUIRY_ANC_CMD_RES               0x51
#define APP_INQUIRY_PARTS_LIST_CMD            0x52
#define ERAPHONE_REPLY_PARTS_LIST_CMD         0x53

#define APP_INQUIRY_EARPHONEINOUT_CMD         0x54
#define ERAPHONE_REPLY_EARPHONEINOUT_CMD      0x55


#define APP_INQUIRY_CLEAR_EARPHONELIST_CMD    0x56

#define APP_INQUIRY_THEME_SETCOLORS_CMD    0x42
#define APP_INQUIRY_THEMECOLORS_CMD        0x5E
#define APP_INQUIRY_THEMECOLORS_CMD_RES    0x5F

#define APP_INQUIRY_HHO_CMD_SET1      0x63
#define APP_INQUIRY_HHO_CMD_READ      0x64
#define APP_INQUIRY_HHO_CMD_RES       0x65
#define APP_INQUIRY_HHO_CMD_SET2      0x66




/***********************************************************************************************/


BLE_CUSTOM_CMD_RET_STATUS_E BLE_custom_command_receive_data_bozun(uint8_t* ptrData, uint32_t dataLength);
void Ble_connect_static_res(uint8_t btstatus);
void led_static_res(void);
void earphone_inout_res(uint8_t leftinoutflg,uint8_t rightinoutflg);
u16 check_battery(uint16_t battery_val);






#ifdef __cplusplus
}
#endif

#endif

