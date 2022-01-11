
#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AW22127_LED_EN HAL_GPIO_PIN_P0_4
#define AW22127_CHIP_ADDR (0X6A)
//#define AW22127_CHIP_ADDR (0X35)
#define AW22127_IIC_SCK_GPIO HAL_GPIO_PIN_P0_6
#define AW22127_IIC_SDK_GPIO HAL_GPIO_PIN_P0_7


#define LEDONOFF_SYNC                0x01
#define VOLUME_SYNC                  0x02
#define BTCONNECTSTATIC_SYNC         0x03
#define BTDISCONNECTSTATIC_SYNC      0x04
#define RHYTHMTYPE_SYNC              0x05
#define THEME_SYNC                   0x06
#define EARINOUT_SYNC                0x07
#define LED2AUTO_SYNC                0x08
#define POWEROFF_SYNC                0x09
#define TWSBATTERY_SYNC              0x0A



typedef enum APP_STATUS_INDICATION_T_AW {
    APP_STATUS_INDICATION_BOXIN = 0,
    APP_STATUS_INDICATION_BOXOPEN,
    APP_STATUS_INDICATION_BOXCLOSE,
    APP_STATUS_INDICATION_AWCHARGENEED,
    APP_STATUS_INDICATION_AWCONNECT,
    APP_STATUS_INDICATION_INCOMCALL,
    APP_STATUS_INDICATION_INCALL,
    APP_STATUS_INDICATION_AWHANGUPCALL,
    APP_STATUS_INDICATION_AWDISCONNECT,

    APP_STATUS_INDICATION_AW_NUM
}APP_STATUS_INDICATION_T_AW;


void aw22127_init_2(void);
void aw22127_init(void);
void aw22127_theme_colors(uint8_t mode,bool bleconnetedflg);
void aw22127_oneled_colors(uint8_t RR,uint8_t GG,uint8_t BB,uint8_t mode);
void aw22127_twoled_colors(uint8_t RR1,uint8_t GG1,uint8_t BB1,uint8_t RR2,uint8_t GG2,uint8_t BB2,uint8_t mode);
void aw22127_led2_colors(uint8_t RR1,uint8_t GG1,uint8_t BB1,uint8_t RR2,uint8_t GG2,uint8_t BB2,uint8_t RR3,uint8_t GG3,uint8_t BB3,uint8_t RR4,uint8_t GG4,uint8_t BB4,uint8_t RR5,uint8_t GG5,uint8_t BB5,uint8_t RR6,uint8_t GG6,uint8_t BB6,uint8_t luminance,uint8_t mode,bool bleconnetedflg);
void aw22127_led2_onoff(uint8_t ledon,bool bleconnetedflg);

int app_status_indication_aw22127set(APP_STATUS_INDICATION_T_AW status);
uint8_t Accessoriestype_check(void);
void aw22127_led_en(bool enflg);
void aw22127_rhythm_colors(uint8_t mode,bool bleconnetedflg);

uint8_t Rhythmtype_check(void);
void aw22127_volume_set(uint8_t volume_data,bool bleconnetedflg);

uint8_t ledonooff_check(void);
uint8_t volume_check(void);
uint8_t theme_colors_check(void);

void aw22127_btconnected_sync(void);
void aw22127_btdisconnected_sync(void);

extern uint8_t twssyncdata[21];
bool read_led_on_status(void);
uint8_t luminance_colors_check(void);

void ccessories_delay_set_time(uint32_t millisec);






#ifdef __cplusplus
}
#endif

#endif

