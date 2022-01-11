
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
#include "led.h"
#include "hal_i2c.h"
#include "flash_opt.h"
//#include "app_status_ind.h"
#include "ble_cmd.h"
#include "app_ibrt_customif_cmd.h"

#ifdef __AW_UPDATA__
#include "aw22xxx.h"
#endif


static APP_STATUS_INDICATION_T_AW app_status = APP_STATUS_INDICATION_AW_NUM;
static APP_STATUS_INDICATION_T_AW app_status_ind_filter = APP_STATUS_INDICATION_AW_NUM;

uint8_t luminance_colors_mode = 0xff;

uint8_t theme_colors_mode = 0x1e;
uint8_t Accessoriestype = 0xb0;
uint8_t rhythmtype = 0x1e;
uint8_t ledonofftype = 0x01;
uint8_t volumetype = 0x08;
uint8_t twssyncdata[21]={0};

bool ledon_flg = true;




static void Accessoriestypedet_delay_timehandler(void const * param);
osTimerDef(ACCESSORIES_DETDELAY, Accessoriestypedet_delay_timehandler);
static osTimerId       ccessoriestype_delay_timer = NULL;
void ccessories_delay_set_time(uint32_t millisec);


static void Accessoriestypedet_delay_timehandler(void const * param)
{
   uint8_t testAccessoriestype=0;
   aw9523_read_byte(AW22127_CHIP_ADDR,0x07,&testAccessoriestype);
   #if 0
   if(testAccessoriestype != Accessoriestype)
   {
       if(testAccessoriestype != 0xff)
       {
          aw22127_init();
       }
	   else
	   {
	      Accessoriestype = testAccessoriestype;
		  led_static_res();
          ccessories_delay_set_time(2000);
	   }
   }
   else
   {
       ccessories_delay_set_time(2000);
   }
   #endif
   TRACE(1,"....................Accessoriestype=%x \n", Accessoriestype);
   ccessories_delay_set_time(1000);
}




void ccessories_delay_set_time(uint32_t millisec)
{
  if (ccessoriestype_delay_timer == NULL)
  {
	 ccessoriestype_delay_timer	= osTimerCreate(osTimer(ACCESSORIES_DETDELAY), osTimerOnce, NULL);
	 osTimerStop(ccessoriestype_delay_timer);
  } 
     osTimerStart(ccessoriestype_delay_timer, millisec);
}





const struct HAL_IOMUX_PIN_FUNCTION_MAP led_en[1] =
{
    {
        AW22127_LED_EN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL
    },
};


void aw22127_led_en(bool enflg)
{
	hal_iomux_init(led_en, 1);
	if(enflg == 1)
	{
       hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T) led_en[0].pin, HAL_GPIO_DIR_OUT, 1);
	}
	else
	{
       hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T) led_en[0].pin, HAL_GPIO_DIR_OUT, 0);
	}
}



void aw22127_i2c_start2(void)
{
   struct HAL_GPIO_I2C_CONFIG_T i2c_cfg={AW22127_IIC_SCK_GPIO,AW22127_IIC_SDK_GPIO,20,0};
   hal_gpio_i2c_open(&i2c_cfg);
}



static int aw22127_write_byte2(uint8_t device_addr,uint8_t reg_addr, uint8_t value)
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

void aw9523_read_byte2(uint8_t device_addr, uint8_t reg_addr, unsigned char *data)
{
	hal_gpio_i2c_read_data(device_addr,reg_addr,1,data,1);
}


void aw22127_theme_colors(uint8_t mode,bool bleconnetedflg)
{
   theme_colors_mode = mode;

   if(bleconnetedflg == true)
   {
     twssyncdata[0] = THEME_SYNC;
     twssyncdata[1] = theme_colors_mode;
     app_ibrt_customif_test_fjh(twssyncdata,10);
   }

   
   aw22127_write_byte2(AW22127_CHIP_ADDR,0xff,0x00);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x01);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x03);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x06,mode);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,rhythmtype);
}



void aw22127_theme_colors2(uint8_t mode,uint8_t rhythmtypetest)
{
if(mode == 0x10)
{
   if(Accessoriestype == 0xb0)
   {
      mode = 0x00;
   }
   else if(Accessoriestype == 0xb1)
   {
       mode = 0x04;
   }
   else if(Accessoriestype == 0xb2)
   {
       mode = 0x07;
   }
   else if(Accessoriestype == 0xb3)
   {
       mode = 10;
   }
   else if(Accessoriestype == 0xb4)
   {
       mode = 13;
   }
}

   aw22127_write_byte2(AW22127_CHIP_ADDR,0xff,0x00);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x01);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x06,mode);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x03);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,rhythmtypetest);
}


void aw22127_theme_colors3(uint8_t mode,uint8_t rhythmtypetest)
{
   aw22127_write_byte2(AW22127_CHIP_ADDR,0xff,0x00);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x01);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x06,mode);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x03);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,rhythmtypetest);
}





void aw22127_rhythm_colors(uint8_t mode,bool bleconnetedflg)
{
/*
   theme_colors_mode = mode;
   aw22127_write_byte2(AW22127_CHIP_ADDR,0xff,0x00);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x01);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x03);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x06,mode);
*/
   rhythmtype = mode;

   if(bleconnetedflg == true)
   {
   twssyncdata[0] = RHYTHMTYPE_SYNC;
   twssyncdata[1] = rhythmtype;
   //app_ibrt_customif_test1_cmd_send(twssyncdata,10);
   app_ibrt_customif_test_fjh(twssyncdata,10);
   }


   aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,rhythmtype);
}









void aw22127_volume_set(uint8_t volume_data,bool bleconnetedflg)
{
   //ledonofftype = volume_data;
   if(bleconnetedflg == true)
   {
   twssyncdata[0] = VOLUME_SYNC;
   twssyncdata[1] = volume_data;
   //app_ibrt_customif_test1_cmd_send(twssyncdata,10);
   app_ibrt_customif_test_fjh(twssyncdata,10);
   }

   app_bt_set_volume_test(volume_data);
}


bool read_led_on_status(void)
{
    return ledon_flg;
}


void aw22127_led2_onoff(uint8_t ledon,bool bleconnetedflg)
{
	ledonofftype = ledon;

    if(bleconnetedflg == true)
    {
	   twssyncdata[0] = LEDONOFF_SYNC;
	   twssyncdata[1] = ledonofftype;
	   //app_ibrt_customif_test1_cmd_send(twssyncdata,10);
	   app_ibrt_customif_test_fjh(twssyncdata,10);
    }
	
    if(ledon == 1)
    {
       //aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x03); 
	   //aw22127_led_en(1);
	   aw22127_init_2();
	   ledon_flg = true;
	}
	else
	{
       //aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x01); 
       aw22127_led_en(0);
	   ledon_flg = false;
	}
}



void aw22127_oneled_colors(uint8_t RR,uint8_t GG,uint8_t BB,uint8_t mode)
{
	//aw22127_flash_erase_write_test(RR,GG,BB,0,0,0,mode,0xff);
}

void aw22127_twoled_colors(uint8_t RR1,uint8_t GG1,uint8_t BB1,uint8_t RR2,uint8_t GG2,uint8_t BB2,uint8_t mode)
{
	//aw22127_flash_erase_write_test(RR1,GG1,BB1,RR2,GG2,BB2,mode,0xff);
}

void aw22127_led2_colors(uint8_t RR1,uint8_t GG1,uint8_t BB1,uint8_t RR2,uint8_t GG2,uint8_t BB2,uint8_t RR3,uint8_t GG3,uint8_t BB3,uint8_t RR4,uint8_t GG4,uint8_t BB4,uint8_t RR5,uint8_t GG5,uint8_t BB5,uint8_t RR6,uint8_t GG6,uint8_t BB6,uint8_t luminance,uint8_t mode,bool bleconnetedflg)
{
    if(bleconnetedflg == true)
    {
	   twssyncdata[0] = LED2AUTO_SYNC;
	   twssyncdata[1] = RR1;
	   twssyncdata[2] = GG1;
	   twssyncdata[3] = BB1;
	   twssyncdata[4] = RR2;
	   twssyncdata[5] = GG2;
	   twssyncdata[6] = BB2;
	   twssyncdata[7] = RR3;
	   twssyncdata[8] = GG3;
	   twssyncdata[9] = BB3;

	   twssyncdata[10] = RR4;
	   twssyncdata[11] = GG4;
	   twssyncdata[12] = BB4;

	   twssyncdata[13] = RR5;
	   twssyncdata[14] = GG5;
	   twssyncdata[15] = BB5;


	   twssyncdata[16] = RR6;
	   twssyncdata[17] = GG6;
	   twssyncdata[18] = BB6;

	   
	   twssyncdata[19] = luminance;
	   twssyncdata[20] = mode;
	   app_ibrt_customif_test_fjh(twssyncdata,21);
    }
	aw22127_flash_erase_write_test(RR1,GG1,BB1,RR2,GG2,BB2,RR3,GG3,BB3,RR4,GG4,BB4,RR5,GG5,BB5,RR6,GG6,BB6,mode,luminance);
}



void aw22127_boxin(void)
{
  // aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x11);
   aw22127_theme_colors2(theme_colors_mode,0x11);
}


void aw22127_closebox(void)
{
  // aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x11);
   aw22127_theme_colors2(theme_colors_mode,0x11);
}


void aw22127_boxopen(void)
{
   //aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x15);
   aw22127_theme_colors2(theme_colors_mode,0x15);
}

void aw22127_btconnected(void)
{
	twssyncdata[0] = BTCONNECTSTATIC_SYNC;
	app_ibrt_customif_test_fjh(twssyncdata,10);
    //aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x16);
    aw22127_theme_colors2(theme_colors_mode,0x16);
}

void aw22127_btdisconnected(void)
{
	twssyncdata[0] = BTDISCONNECTSTATIC_SYNC;
	app_ibrt_customif_test_fjh(twssyncdata,10);
}


void aw22127_btconnected_sync(void)
{
    aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x16);
}

void aw22127_btdisconnected_sync(void)
{

}


void aw22127_incomcall(void)
{
   aw22127_theme_colors2(theme_colors_mode,0x13);
  // aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x13);
}


void aw22127_incall(void)
{
   aw22127_theme_colors2(theme_colors_mode,0x14);
  // aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x14);
}


uint8_t luminance_colors_check(void)
{
   //aw9523_read_byte(AW22127_CHIP_ADDR,0x07,&Accessoriestype);
   return luminance_colors_mode;
}

uint8_t theme_colors_check(void)
{
   //aw9523_read_byte(AW22127_CHIP_ADDR,0x07,&Accessoriestype);
   return theme_colors_mode;
}


uint8_t Accessoriestype_check(void)
{
   //aw9523_read_byte(AW22127_CHIP_ADDR,0x07,&Accessoriestype);
   return Accessoriestype;
}
uint8_t Rhythmtype_check(void)
{
   //aw9523_read_byte(AW22127_CHIP_ADDR,0x07,&Accessoriestype);
   return rhythmtype;
}

uint8_t ledonooff_check(void)
{
   //aw9523_read_byte(AW22127_CHIP_ADDR,0x07,&Accessoriestype);
   return ledonofftype;
}

uint8_t volume_check(void)
{
   //aw9523_read_byte(AW22127_CHIP_ADDR,0x07,&Accessoriestype);
   return app_bt_get_volume_test();
   //return ledonofftype;
}



int app_status_indication_aw22127set(APP_STATUS_INDICATION_T_AW status)
{
return 0;
   // if (app_status == status)
   //     return 0;
    if (app_status_ind_filter == status)
        return 0;

    app_status = status;


    switch (status) {
		case APP_STATUS_INDICATION_BOXIN:
			aw22127_boxin();
			break;
		case APP_STATUS_INDICATION_BOXOPEN:
			aw22127_boxopen();
			break;
		case APP_STATUS_INDICATION_BOXCLOSE:
			aw22127_closebox();
			break;
		case APP_STATUS_INDICATION_AWCHARGENEED:
			break;
		case APP_STATUS_INDICATION_AWCONNECT:
			Ble_connect_static_res(1);
			aw22127_btconnected();
			break;
		case APP_STATUS_INDICATION_INCOMCALL:	
			aw22127_incomcall();
			break;
		case APP_STATUS_INDICATION_INCALL:
			aw22127_incall();
			break;
		case APP_STATUS_INDICATION_AWDISCONNECT:
			Ble_connect_static_res(0);	
			aw22127_btdisconnected();
			break;

		case APP_STATUS_INDICATION_AWHANGUPCALL:
			//aw22127_rhythm_colors(rhythmtype,1);
			aw22127_theme_colors3(theme_colors_mode,rhythmtype);
			break;

		default:break;	
    	}

return 0;

}



void aw22127_init(void)
{
//uint8_t fd_data = 0;
   aw22127_led_en(1);
   osDelay(20);

   aw22127_i2c_start2();

   
#ifdef __AW_UPDATA__
   TRACE(1,"....................aw22xxx_play \n");
   aw22xxx_play();
#endif

   read_flash();

#if 0

   aw22127_led_en(0);
   osDelay(20);
   aw22127_led_en(1);
   osDelay(20);
   #endif

   //readdata_app[1] = 0x1e;


   if((readdata_app[1]==0x1e)||(readdata_app[1]==0x1f)||(readdata_app[1]==0x2e))
   {
       TRACE(1,"....................readdata_app[1]= %x \n",readdata_app[1]);
       theme_colors_mode = readdata_app[1];
	   luminance_colors_mode = readdata_app[5];
#if 1
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0xff,0x00);
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0x02,0x01);
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0x0c,0x03);
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0x0b,0x08);
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x01);
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x01);
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x03);
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0x06,0x10);
	  aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,theme_colors_mode);
#endif   
   }
   else
   {
	   aw22127_flash_erase_write_test(255,100,50,255,100,50,255,60,60,255,90,90,255,30,30,0,0,0,theme_colors_mode,0x5c);
   }


   osDelay(20);
   //aw9523_read_byte(AW22127_CHIP_ADDR,0x07,&Accessoriestype);
   //TRACE(1,"....................Accessoriestype=%d \n", Accessoriestype);   
   led_static_res();
#if 0
	  aw9523_read_byte(AW22127_CHIP_ADDR, 0x07, &fd_data);
	  TRACE(1,"......fd_data = %d\n",fd_data);
	  aw9523_read_byte(AW22127_CHIP_ADDR, 0x04, &fd_data);
	  TRACE(1,"......fd_data = %d\n",fd_data);
#endif
   
 

}





void aw22127_init_2(void)
{
   aw22127_led_en(1);
   osDelay(20);
   aw22127_i2c_start2();
   aw22127_write_byte2(AW22127_CHIP_ADDR,0xff,0x00);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x02,0x01);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x0c,0x03);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x0b,0x08);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,0x01);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x01);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x04,0x03);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x06,0x10);
   aw22127_write_byte2(AW22127_CHIP_ADDR,0x05,theme_colors_mode);
   osDelay(20);
   led_static_res();
 
}


