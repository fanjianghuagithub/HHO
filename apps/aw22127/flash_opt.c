
#include "hal_uart.h"
#include "hal_trace.h"
#include "string.h"
#include "hal_timer.h"
#include "cmsis_os.h"
#include "hal_key.h"
//#include "btapp.h"
#include "app_thread.h"
#include "norflash_api.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"
#include "nvrecord_env.h"
//#include "app_com.h"
#include "tgt_hardware.h"
#include "app_audio.h"
#include "app_bt.h"
#include "App_bt_stream.h"
#include "apps.h"
#include "hal_spdif.h"
#include "app_battery.h"
#include "flash_opt.h"
#include "hal_i2c.h"
#include "led.h"



void aw22127_i2c_start(void)
{
   struct HAL_GPIO_I2C_CONFIG_T i2c_cfg={AW22127_IIC_SCK_GPIO,AW22127_IIC_SDK_GPIO,20,0};
   hal_gpio_i2c_open(&i2c_cfg);
}



static int aw22127_write_byte(uint8_t device_addr,uint8_t reg_addr, uint8_t value)
{
    int ret = -1;
    uint8_t cnt = 0;
	//uint8_t write_data[2];

	ret = hal_gpio_i2c_simple_send2(device_addr, reg_addr, &value, 1);
	return ret;
		
    while(cnt < 2) {
        //ret = I2C_Write_Bytes(I2C1, device_addr, reg_addr, &value, 1);
        ret = hal_gpio_i2c_simple_send2(device_addr, reg_addr, &value, 1);
        if(ret == 0) {
            TRACE(1,"aw9523_i2c_write: i2c_write cnt=%d error=%d\n", cnt, ret);
        } else {
            break;
        }
        cnt ++;
    }

    return ret;
}
/*
static int aw22127_write_bytes(uint8_t device_addr, uint8_t reg_addr, uint8_t*value, uint8_t len)
{
    int ret = -1;
    uint8_t cnt = 0;
	//uint8_t write_data[2];
		
    while(cnt < 2) {
        //ret = I2C_Write_Bytes(I2C1, device_addr, reg_addr, value, len);
        ret = hal_gpio_i2c_simple_send2(device_addr, reg_addr, value, len);
        if(ret == 0) {
            TRACE(1,"aw9523_i2c_write: i2c_write cnt=%d error=%d\n", cnt, ret);
        } else {
            break;
        }
        cnt ++;
    }

    return ret;
}
*/

void aw9523_read_byte(uint8_t device_addr, uint8_t reg_addr, unsigned char *data)
{
	hal_gpio_i2c_read_data(device_addr,reg_addr,1,data,1);
}


#if 1
unsigned char readdata_app[22] = {0,0,0,};
void read_flash(void)
{
  unsigned int i = 0;
  unsigned int tmp_addr = 0;
#if 1
  aw22127_write_byte(AW22127_CHIP_ADDR, 0xff, 0x00);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x01, 0x55);
  osDelay(2);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x02, 0x01);
  osDelay(2);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x04, 0x01);
  osDelay(2);
  
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x80, 0xec);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x35, 0x29);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x36, 0xcd);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x37, 0xba);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x38, 0x00);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x39, 0xf2);
  
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x30, 0x00);
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x20, 0x03);
#endif

  for(i=0; i<22; i++)
  {
	 tmp_addr = 0x4400 + i;
	 aw22127_write_byte(AW22127_CHIP_ADDR, 0x22, (unsigned char)(tmp_addr>>8));
	 aw22127_write_byte(AW22127_CHIP_ADDR, 0x21, (unsigned char)(tmp_addr>>0));
	 aw9523_read_byte(AW22127_CHIP_ADDR, 0x23, &readdata_app[i]);
	 TRACE(1,"readdata_app = %x\n",readdata_app[i]);
  }
  aw22127_write_byte(AW22127_CHIP_ADDR, 0x20, 0x00);
}
#endif





extern uint8_t luminance_colors_mode;
extern void aw22127_theme_colors(uint8_t mode,bool bleconnetedflg);
extern uint8_t theme_colors_mode;
extern uint8_t rhythmtype;
void aw22127_flash_erase_write_test(uint8_t RR1,uint8_t GG1,uint8_t BB1,uint8_t RR2,uint8_t GG2,uint8_t BB2,uint8_t RR3,uint8_t GG3,uint8_t BB3,uint8_t RR4,uint8_t GG4,uint8_t BB4,uint8_t RR5,uint8_t GG5,uint8_t BB5,uint8_t RR6,uint8_t GG6,uint8_t BB6,uint8_t effect,uint8_t luminance)
{
		//unsigned char flash_data[512]={0x06,0x19,0x03,0x04,0x00,0xff,0xff,0x00,0x00,0x00,0xff,0x00};
		uint8_t flash_data[512]={0x06,effect,0x03,0x04,0x00,luminance,RR1,GG1,BB1,RR2,GG2,BB2,RR3,GG3,BB3,RR4,GG4,BB4,RR5,GG5,BB5,RR6,GG6,BB6};
		//unsigned char readdata[10] = {0,0,0,};
		unsigned int i = 0;
		unsigned int tmp_addr = 0;
		//unsigned char readdata[10] = {0,0,0,};
		//unsigned char fd_data = 0;
	
		/* earse flash */
		aw22127_write_byte(AW22127_CHIP_ADDR, 0xff, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x01, 0x55);
		osDelay(2);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x02, 0x01);
		osDelay(2);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x04, 0x01);
		osDelay(2);
	
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x80, 0xec);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x35, 0x29);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x36, 0xcd);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x37, 0xba);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x38, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x39, 0xf2);
	
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x22, 0x44);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x21, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x20, 0x03);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x30, 0x02);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x23, 0x00);
		osDelay(6);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x30, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x20, 0x00);
	
	
		/* trim flash */
		aw22127_write_byte(AW22127_CHIP_ADDR, 0xff, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x01, 0x55);
		osDelay(2);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x02, 0x01);
		osDelay(2);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x04, 0x01);
		osDelay(2);
	
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x80, 0xec);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x35, 0x29);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x36, 0xcd);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x37, 0xba);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x38, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x39, 0xf2);
	
	
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x20, 0x03);
		for(i=0; i<512; i++)
		{
			tmp_addr = 0x4400 + i;
			aw22127_write_byte(AW22127_CHIP_ADDR, 0x22, (unsigned char)(tmp_addr>>8));
			aw22127_write_byte(AW22127_CHIP_ADDR, 0x21, (unsigned char)(tmp_addr>>0));
			aw22127_write_byte(AW22127_CHIP_ADDR, 0x30, 0x04);
			aw22127_write_byte(AW22127_CHIP_ADDR, 0x23, flash_data[i]);
			aw22127_write_byte(AW22127_CHIP_ADDR, 0x30, 0x00);
		}
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x20, 0x00);
	
		/* check flash */
#if 1
		aw22127_write_byte(AW22127_CHIP_ADDR, 0xff, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x01, 0x55);
		osDelay(2);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x02, 0x01);
		osDelay(2);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x04, 0x01);
		osDelay(2);
	
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x80, 0xec);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x35, 0x29);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x36, 0xcd);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x37, 0xba);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x38, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x39, 0xf2);
	
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x30, 0x00);
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x20, 0x03);
		for(i=0; i<22; i++)
		{
			tmp_addr = 0x4400 + i;
			aw22127_write_byte(AW22127_CHIP_ADDR, 0x22, (unsigned char)(tmp_addr>>8));
			aw22127_write_byte(AW22127_CHIP_ADDR, 0x21, (unsigned char)(tmp_addr>>0));
			aw9523_read_byte(AW22127_CHIP_ADDR, 0x23, &readdata_app[i]);
			TRACE(1,"readdata_app = %d\n",readdata_app[i]);
		}
		aw22127_write_byte(AW22127_CHIP_ADDR, 0x20, 0x00);
#endif
		
	
	 //  aw22127_write_byte(AW22127_CHIP_ADDR, 0x06, effect); 	
	 //  aw22127_write_byte(AW22127_CHIP_ADDR, 0x04, 0x03);
	 //  osDelay(2);


	aw22127_write_byte(AW22127_CHIP_ADDR,0xff,0x00);
	aw22127_write_byte(AW22127_CHIP_ADDR,0x02,0x01);
	aw22127_write_byte(AW22127_CHIP_ADDR,0x0c,0x03);
	aw22127_write_byte(AW22127_CHIP_ADDR,0x0b,0x08);
	aw22127_write_byte(AW22127_CHIP_ADDR,0x05,0x01);
	aw22127_write_byte(AW22127_CHIP_ADDR,0x04,0x01);
	
	aw22127_write_byte(AW22127_CHIP_ADDR,0x04,0x03);
	aw22127_write_byte(AW22127_CHIP_ADDR,0x06,0x10);
	aw22127_write_byte(AW22127_CHIP_ADDR,0x05,effect);

	

	//aw9523_read_byte(AW22127_CHIP_ADDR, 0x07, &fd_data);
	//TRACE(1,"......fd_data = %d\n",fd_data);
	//aw9523_read_byte(AW22127_CHIP_ADDR, 0x04, &fd_data);
	//TRACE(1,"......fd_data = %d\n",fd_data);

	TRACE(1,"......effect = %x\n",effect);
	TRACE(1,"......luminance = %x\n",luminance);


	   

    luminance_colors_mode = luminance;

	rhythmtype = luminance;
	theme_colors_mode = effect;

	//ccessories_delay_set_time(1000);

	//aw22127_theme_colors(0x10,true);
}
