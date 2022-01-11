//========================================================================
//  add by guyu for laiyuan mcu communication
//  time:2020-6-30
//========================================================================

#define __HDS_SYS_STATUS_MSG_C_

#include "cmsis_os.h"
#include "hal_i2c.h"
#include "hal_trace.h"
#include "pmu.h"
#include "app_box.h"
#include "hds_apply_thread.h"
#include "hds_sys_status_manage.h"
#include "app_bt_stream.h"
#include "app_ibrt_customif_cmd.h"
#include "app_status_ind.h"
#include "apps.h"

bool cang_onoff_flg = false;
bool cang_inout_flg = false;
bool charging_flg = false;
bool charfull_flg = false;
void hds_status_set_cang_onoff(bool status)
{
    if(status == true)
    {
		TRACE(6,"set_cang_onoff_status------------> off");
		//仓是关的那肯定在仓内....
		hds_status_set_cang_inout(status); 
    }
	else
	{

	    TRACE(6,"set_cang_onoff_status------------> on");  
	}
        cang_onoff_flg = status;
	if(status == true)
	{
        app_status_indication_set(APP_STATUS_INDICATION_CHARGING);
	}
}
bool hds_status_get_cang_onoff()
{
	if(cang_onoff_flg == true)
	{
        TRACE(6,"get_cang_onoff_status------------> off");
	}
	else
	{
	   TRACE(6,"get_cang_onoff_status------------> on");
       
	}

    return cang_onoff_flg;
}



void hds_status_set_charfull_status(bool status)
{
      charfull_flg = status;
}
bool hds_status_get_charfull_status()
{

    return charfull_flg;
}









//flase:out  true:in 


extern void analog_aud_codec_nomute(void);
extern void analog_aud_codec_mute(void);
//extern bool enter_dut_flg;
//extern bool EnterBTsingpairFlg;
void hds_status_set_cang_inout(bool status)
{ 
   
   cang_inout_flg = status;

   if(status)
   {
	   TRACE(6,"set_cang_inout_status------------> in");

	 //  if((EnterBTsingpairFlg == false)&(enter_dut_flg == false))
	 //  analog_aud_codec_mute();

	   
	  // if(enter_dut_flg==false)
	//   hal_gpio_pin_set(HAL_GPIO_PIN_P1_4);

   }
   else
   {
	  TRACE(6,"set_cang_inout_status------------> out");
	  
          analog_aud_codec_nomute();
	//  hal_gpio_pin_clr(HAL_GPIO_PIN_P1_4);
	  
   }
}

extern void analog_aud_codec_mute(void);
bool hds_status_get_cang_inout()
{
	if(cang_inout_flg)
	{
       TRACE(6,"get_cang_inout_status------------> in");
	 //  analog_aud_codec_mute();	   
       
	}
	else
	{
	   TRACE(6,"get_cang_inout_status------------> out");
       
	}
    return cang_inout_flg;  
}



//flase:plug-out true:plug-in
void hds_status_set_chargemode(bool charge)
{
    TRACE(6,"hds_status_set_chargemode---> :=%d",charge);

	
	if(charge == true)
	{
	  //if(cang_onoff_flg == false)
     // app_status_indication_set(APP_STATUS_INDICATION_CHARGING);
	}
	charging_flg = charge;
}


bool hds_status_get_chargemode()
{
	TRACE(6,"hds_status_get_chargemode---> :=%d",charging_flg);

    return charging_flg;  
}


//flase:non-test true:testmode 
void hds_status_set_testmode(bool mode)
{
	
	
	
}

bool hds_status_get_testmode(void)
{
	
 return 0;		
	
}


//flase: not ready true:ready 
void hds_status_set_btready(bool ready)
{
	
	
	
}

bool hds_status_get_btready(void)
{

return 0;  


}

//flase: don't need reboot true:need reboot 
void hds_status_set_reboot(bool reboot)
{
	
	
	
}

bool hds_status_get_reboot(void)
{

return 0;  


}

//flase: don't need force power off true:need force power off 
void hds_status_set_force_poweroff(bool poweroff)
{
	
	
	
}

bool hds_status_get_force_poweroff(void)
{

return 0;  


}


//check charge full need power off
bool hds_status_check_charge_pwroff(void)
{
	
	
	 return 0;	
}

//true:ready  false:not ready 
bool hds_power_on_isready(void)
{
	
	
 return 0;		
}

//add by guyu 20200604
//true: ready false:not ready
bool hds_status_key_isready(void)
{
	
	
 return 0;		
}


//add by guyu 20200608
//true:game mode false:normal mode
extern uint8_t A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU;
extern uint8_t A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU;
//extern void app_ibrt_customif_test1_cmd_send(uint8_t *p_buff, uint16_t length);
bool game_mode_onflg = false;
void hds_status_set_game_mode(bool enable)
{
	if(enable == true)
	{
          A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU = A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU_GAME;
		  A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU = A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU_GAME;
		  game_mode_onflg = true;
	}
	else
	{    
          A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU = A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU_NORMAL;
		  A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU = A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU_NORMAL;
		  game_mode_onflg = false;
	}
}

bool hds_status_get_game_mode(void)
{
   return game_mode_onflg;
}


//get mtu value in game mode
uint8_t hds_status_get_aac_mtu_game(void)
{
	
 return 0;		
	
}

uint8_t hds_status_get_sbc_mtu_game(void)
{


return 0;  

}

//add by guyu 20200615
//set project code
void hds_status_set_project(uint8_t project)
{
	
	
	
}

//get project code
uint8_t hds_status_get_project(void)
{
	
	
   return 0;	
}


//add by guyu 20200709
//设置对耳地址
void hds_status_set_peer_btaddr(uint8_t *bt_addr)
{
	
	
	
}

//获取对耳地址
void hds_status_get_peer_btaddr(uint8_t *bt_addr)
{
	
		
}


void hds_status_init(void)
{



}




