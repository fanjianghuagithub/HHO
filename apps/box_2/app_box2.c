/******************************************************************************
 *
 * Filename:       app_box.cpp
 *
 * Description:    Source file for communication with the charging box. 
 *
 * date_created:  2019-08-23
 * version:       1.0
 * authors:       fanjianghua
 *
 *****************************************************************************/
#include "cmsis_os.h"
#include "tgt_hardware.h"
#include "pmu.h"
#include "hal_timer.h"
#include "hal_gpadc.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_chipid.h"
#include "app_thread.h"
#include "apps.h"
#include "app_status_ind.h"
#ifdef BT_USB_AUDIO_DUAL_MODE
#include "btusb_audio.h"
#endif
#include <stdlib.h>
	 
#ifdef __INTERCONNECTION__
#include "app_ble_mode_switch.h"
#endif
#include "communication_svr.h"
//#include "app_ibrt_ui.h"
#include "app_thread.h"
#include "app_battery.h"
//#include "app_ibrt_ui.h"
#include "app_box.h"
#ifdef AW2217
#include "led.h"
#endif

#include "app_box2.h"

//box protocol
//airho


#define POWERON_HEADSET         0xF1
#define POWEROFF_HEADSET        0xF2
#define CLOSEBOX_HEADSET        0xF4

#define TWS_PAIR                0xE1
#define ENTER_DUT               0xE2

#define ENTER_OTA               0XD1
#define ENTER_BT_PAIR_CLR       0xD2

#define STATUS_HEADSET          0xD4


#define ENTER_TEST              0XC7

#define POWEROFF_HEADSET_TEST   0XC1



#define BOX_DEBUG
#ifdef BOX_DEBUG
#define BOX_TRACE TRACE
#else
#define BOX_TRACE(...)
#endif

enum
{
    box_status_unknow = 0,
    box_poweron,
    box_ota_fjh,
    box_btclear,
    box_status_open,
    box_status_close,
    box_status_poweroff,
    box_delay_test,
    box_cmd,
};


uint8_t box_status_fjh = box_status_unknow;
bool boxin_flg = true;


bool box_poweroff_flg_test = false;

uint8_t box_old_cmd = 0;

bool otamode_en = false;
bool power_on_test = false;
bool clear_test = false;
bool box_delay_test_flg = false;
bool enter_test_mode = false;

int app_uart_send2box(uint8_t * buf, uint8_t len);




extern void analog_aud_codec_nomute(void);
static void inoutbox_det_timehandler(void const * param);
osTimerDef(INOUTBOX_GENSORDELAY,inoutbox_det_timehandler);
static osTimerId inoutboxdet_timer = NULL;
static void inoutbox_det_timehandler(void const * param)
{

	if(boxin_flg == true)
	{
	    boxin_flg = false;
	    analog_aud_codec_nomute();
	}
}



void inoutbox_set_time(uint32_t millisec)
{
  if (inoutboxdet_timer == NULL)
  {
	 inoutboxdet_timer	= osTimerCreate(osTimer(INOUTBOX_GENSORDELAY), osTimerOnce, NULL);
	 osTimerStop(inoutboxdet_timer);
  } 
     osTimerStart(inoutboxdet_timer, millisec);
}


void inoutbox_close_time(void)
{
  if (inoutboxdet_timer == NULL)
  {
     inoutboxdet_timer  = osTimerCreate(osTimer(INOUTBOX_GENSORDELAY), osTimerOnce, NULL);
     osTimerStop(inoutboxdet_timer);
  } 
}










static void delay_timehandler(void const * param);
osTimerDef(DELAY_GENSORDELAY,delay_timehandler);
static osTimerId       delay_timer = NULL;
void c_Send_Cmd(uint32_t idtest);
extern bool uart_cmd_enable;
static void delay_timehandler(void const * param)
{
	//if(power_on_test == true)
	{
      // power_on_test = false;
	   if(uart_cmd_enable == false)
	   {
             BOX_TRACE(8,"delay1500---->uart_cmd_enable error = %d",uart_cmd_enable); 
	   }
	   else
	   {
          app_poweron_box();
	   }
	}
}



void delay_set_time(uint32_t millisec)
{
 osStatus tt;
  if (delay_timer == NULL)
  {
	 delay_timer	= osTimerCreate(osTimer(DELAY_GENSORDELAY), osTimerOnce, NULL);
	 osTimerStop(delay_timer);
  } 
      tt = osTimerStart(delay_timer, millisec);
	  TRACE(9,"osTimerStart:= %08x \n",tt);
}

void c_Send_Cmd(uint32_t idtest)
{
  APP_MESSAGE_BLOCK msg;
  msg.mod_id			= APP_MODUAL_DELAY;
  msg.msg_body.message_id = idtest;
  msg.msg_body.message_ptr = (uint32_t) NULL;
  app_mailbox_put(&msg);
}


int app_delay_process(APP_MESSAGE_BODY * msg)
{
   uint8_t cmd_data = 0xd4;
   if(msg->message_id == box_cmd)
   {
      app_uart_send2box(&cmd_data,1);
   }
   return 0;
}


void init_delay_thread(void)
{
	app_set_threadhandle(APP_MODUAL_DELAY, app_delay_process);
}






int app_box_uart_open(void)
{
	BOX_TRACE(8,"---------------------------app_box_uart_open-----------------------------------");
	//app_set_threadhandle(APP_MODUAL_BOX, app_uart_rx_process);
    return 0;
}

extern bool tws_connect_status;
uint8_t buf_send[5]={0};
int app_uart_send2box(uint8_t * buf, uint8_t len)
{
	test_box_send();
	//osDelay(10);
    buf_send[0] = 0x55;
	buf_send[1] = * buf;
	buf_send[2] = batter_lever_read();
	if(tws_connect_status == 0)
	{
	   buf_send[3] = 0x00;
	}
	else
	{
       buf_send[3] = 0x11;
	}
	buf_send[4] = 0xAA;
    communication_send_buf(buf_send,5);
	
    osDelay(10);
	test_box_read();
    return 0;
}





//extern bool box_poweroff_flg;
extern bool uart_cmd_enable;
//extern bool report_power_low_flg;
//extern bool connected_flg;

bool poweron_err_flg = false;
uint8_t cmd_data = 0;
extern uint8_t box_battery;
extern bool poweon_ok_flg;
extern void analog_aud_codec_mute(void);
uint8_t buf_powerof[5]={0};
extern uint8_t box_charging_n;
void communication_receive__callback(uint8_t *buf, uint8_t len)
{
	//uint8_t uartrxcmd = 0;
    //命令解析...
	uint8_t ii=0;

	//while(1)  
	{
	  if(len>=5)
	  {
         while(ii)
       	 {
             if(buf[ii] == 0x55)
             {
                 if((ii+5)<=len)
                 {
                    if(buf[ii+4] == 0xAA)
                    {

                        BOX_TRACE(8,"cmd sucefull--------- = %x",ii);
						break;
					}
					else
					{
                        BOX_TRACE(8,"cmd error--------- = %x",ii);
						return;
					}
                 }
				 else
				 {
                        BOX_TRACE(8,"cmd error--------- = %x",ii);
						return;
				 }
		     }
			 ii++;
			 if(ii>=(len-1))
			 {
			     BOX_TRACE(8,"cmd error--------- = %x",ii);
                 return;
			 }
	     }
		 if((buf[ii+1]) == (255-(buf[ii+2])))
		 {
               cmd_data = (buf[ii+1]);
			   box_battery = (buf[ii+3]);
			   BOX_TRACE(8,"cmd_data--------- = %x",cmd_data);
			   BOX_TRACE(8,"box_battery--------- = %x",(buf[ii+3]));
			   //break;
		 }
	  }
	  else
	  { 
	     BOX_TRACE(8,"cmd len error--------- = %x",ii);
		 return;
	  }
   }

#if 0
	if(uart_cmd_enable == false)
	{
	   BOX_TRACE(8,"---->uart_cmd_enable error = %d",uart_cmd_enable);  
	   if((*buf) ==ENTER_TEST)
	   {
	         enter_test_mode = true;
             return;
	   }
	}
	else
	{
       BOX_TRACE(8,"---->uart_cmd_enable sucefull = %d",uart_cmd_enable);   
	}
#endif	

      if(box_old_cmd!=cmd_data)
      {
           box_old_cmd = cmd_data;
	  }
	  else
	  {
	       if(cmd_data !=STATUS_HEADSET)
	       {
              return;
	       }
	  }

	  if(cmd_data !=STATUS_HEADSET)
	  {
	    if(uart_cmd_enable == false)
	    {
          buf_powerof[0] = (buf[ii]);
		  buf_powerof[1] = (buf[ii+1]);
		  buf_powerof[2] = (buf[ii+2]);
		  buf_powerof[3] = (buf[ii+3]);
		  buf_powerof[4] = (buf[ii+4]);	  
	    }
	  }

	  if(uart_cmd_enable == false)
	  {

           return;
	  }

	  switch(cmd_data)
	  {

                  case CLOSEBOX_HEADSET:
				  	  BOX_TRACE(8,"---------------------------CLOSEBOX_HEADSET-----------------------------------");
					  //connected_flg = false;
					  inoutbox_close_time();

					  if(power_on_test == true)
					  {
                            app_shutdown();
							return;
					  }
					  
					  poweon_ok_flg = false;
					  //power_on_test = false;
					  poweron_err_flg = false;
					  box_status_fjh = box_status_close;
					  
					  //app_uart_send2box(&cmd_data,1);
					  
				      app_close_box();
					  //report_power_low_flg = false;

                     #ifdef AW2217
					 app_status_indication_aw22127set(APP_STATUS_INDICATION_BOXCLOSE);
                     #endif		


					 #ifdef AW2217
		             //aw22127_led_en(0);
					 aw22127_led2_onoff(0,0);
                     #endif
				  	 break;


				  case POWEROFF_HEADSET:					  
					  BOX_TRACE(8,"---------------------------POWEROFF_HEADSET-----------------------------------");			  
					   //是否需要主从切换
					  //connected_flg = false;
					  box_status_fjh = box_status_poweroff;
					  app_uart_send2box(&cmd_data,1);
					  //box_poweroff_flg = true; 
					  app_shutdown();
					  break;

					  
		  
				  case POWERON_HEADSET:
					  BOX_TRACE(8,"---------------------------POWERON_HEADSET-----------------------------------");
					  //app_uart_send2box(&cmd_data,1);
					  box_charging_n = 0;
					  box_status_fjh = box_status_open;
					  app_battery_charger_handle_reset();
					  if(uart_cmd_enable == false)
					  {
					     //power_on_test = true;
					     //c_Send_Cmd(box_poweron);
					     delay_set_time(1500);
					     poweron_err_flg = true;
					  }
					  else
					  {
					      app_poweron_box();
					  }
				      #ifdef AW2217
						  aw22127_init();
				      #endif		
					  
                      #ifdef AW2217
						 app_status_indication_aw22127set(APP_STATUS_INDICATION_BOXOPEN);
                      #endif
					  inoutbox_set_time(1300);
					  break;	  


					  
				  case TWS_PAIR:
					  BOX_TRACE(8,"---------------------------TWS_PAIR-----------------------------------");
					  //app_uart_send2box(&cmd_data,1);
					  //app_dis_mob();
					  app_twspairing_box();
					  //box_delay_test_flg = true;
					  //c_Send_Cmd(box_delay_test); 
					  break;
	  
					  
				  case ENTER_DUT:
					  BOX_TRACE(8,"---------------------------ENTER_DUT-----------------------------------");
					  //app_uart_send2box(&cmd_data,1);
					  app_dutmode_box();
					  break;  
					  

		  
				 case ENTER_OTA:
					  BOX_TRACE(8,"---------------------------ENTER_OTA-----------------------------------");
					  //app_uart_send2box(&cmd_data,1);
					  //otamode_en = true;
					  //c_Send_Cmd(box_ota_fjh);
					  break;
					  
					  
				  case ENTER_BT_PAIR_CLR:
					   BOX_TRACE(8,"---------------------------ENTER_BT_PAIR_CLR-----------------------------------");
					   //app_uart_send2box(&cmd_data,1);
					   //app_close_box_test();
					   //clear_test = true;
					   //c_Send_Cmd(box_btclear); 
					   app_clearpairlist_box();
				  break;


				  case ENTER_TEST:
				  	  BOX_TRACE(8,"---------------------------ENTER_BT_TEST-----------------------------------");
					  //app_uart_send2box(&cmd_data,1);
					  //app_close_box_test();
					  //app_clearpairlist_box();
					  enter_test_mode = false;
				  	break;


		         case POWEROFF_HEADSET_TEST:
			     BOX_TRACE(8,"---------------------------POWEROFF_HEADSET_TEST-----------------------------------");
			     //app_uart_send2box(&cmd_data,1);
			     //box_poweroff_flg = true; 
			     box_poweroff_flg_test = true;
			     app_shutdown();
		         break;


		        case STATUS_HEADSET:
				if(box_status_fjh != box_status_close)	
				{
			      if(boxin_flg == false)
			      {
                    boxin_flg = true;
			        analog_aud_codec_mute();
                    #ifdef AW2217
					app_status_indication_aw22127set(APP_STATUS_INDICATION_BOXIN);
                    #endif
			      }
				  test_box_send();
				  c_Send_Cmd(box_cmd);
				  inoutbox_set_time(1300);
				}
		  	    break;

					
	
	
		default:break;
	  }
}
