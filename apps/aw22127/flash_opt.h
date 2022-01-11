
#ifndef __FLASH_OPT_H__
#define __FLASH_OPT_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define AW22127_CHIP_ADDR (0X6A)
#define AW22127_CHIP_ADDR (0X6A)
#define AW22127_IIC_SCK_GPIO HAL_GPIO_PIN_P0_6
#define AW22127_IIC_SDK_GPIO HAL_GPIO_PIN_P0_7


void aw22127_i2c_start(void);
void aw22127_flash_erase_write_test(uint8_t RR1,uint8_t GG1,uint8_t BB1,uint8_t RR2,uint8_t GG2,uint8_t BB2,uint8_t RR3,uint8_t GG3,uint8_t BB3,uint8_t RR4,uint8_t GG4,uint8_t BB4,uint8_t RR5,uint8_t GG5,uint8_t BB5,uint8_t RR6,uint8_t GG6,uint8_t BB6,uint8_t effect,uint8_t luminance);
void aw9523_read_byte(uint8_t device_addr, uint8_t reg_addr, unsigned char *data);
void read_flash(void);

extern unsigned char readdata_app[22];




#ifdef __cplusplus
}
#endif

#endif

