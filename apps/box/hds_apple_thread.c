//========================================================================
//  add by guyu for laiyuan mcu communication
//  time:2020-6-30
//========================================================================

#define __HDS_THREAD_C_

#include "cmsis_os.h"
#include "hal_i2c.h"
#include "hal_trace.h"
#include "pmu.h"
#include "app_box.h"
#include "hds_apply_thread.h"
#include "hds_sys_status_manage.h"
#include "app_thread.h"
#include "app_ibrt_ui.h"
//#include "app_tws_if.h"
#include "apps.h"
#include "app_status_ind.h"
//#include "app_ibrt_if.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"
#include "nvrecord_env.h"
#include "norflash_api.h"
#include "apps.h"
#include "app_anc.h"
#include "app_battery.h"
#include "factory_section.h"
#include "communication_svr.h"
#include "app_ibrt_ui_test.h"
//#include "app_ibrt_if.h"
#include "app_ibrt_ui_test.h"
#include "app_ibrt_customif_ui.h"
#include "app_tws_if.h"
#ifdef AW2217
#include "led.h"
#endif


extern void analog_aud_codec_nomute(void);
extern void analog_aud_codec_mute(void);



uint8_t msgid_fjh = 0xff;
static void hds_delay_timehandler(void const * param);
osTimerDef(HDS_DELAY, hds_delay_timehandler);
static osTimerId       hds_delay_timer = NULL;
//static void hds_thread_msg_answer_btaddr(uint32_t param1,uint32_t param2);
//static void hds_thread_msg_finish_answer(uint32_t param1,uint32_t param2);
void hds_delay_msg_add(uint8_t eventmode,uint16_t timedelay,bool enflg);


uint8_t sysbtmode = sysbtmode_bt;
uint8_t box_status = box_status_unknow;
bool tws_role_switch_flg = false;
bool tws_pairsuceful_flg = false;
uint8_t data_testwrite1205 = 0;
extern uint8_t lastbatteylevle;
extern unsigned char I2C_MasterWriteData(unsigned char add, unsigned char reg, unsigned char *data, unsigned char length);
extern unsigned char I2C_MasterReadData(unsigned char add, unsigned char reg, unsigned char *data, unsigned char length);
int hds_thread_msg_handler(APP_MESSAGE_BODY * msg)
{
   // app_ibrt_ui_t *p_ui_ctrl = app_ibrt_ui_get_ctx();
   TRACE(5,"message_id = %x",msg->message_id);
  // TRACE(5,"p_ui_ctrl->box_state-------> = %x",p_ui_ctrl->box_state);

   
  
   switch(msg->message_id)
   {

      case THREAD_MSG_EARPHONE_BATTERY:
		 // data_testwrite1205 = app_battery_is_charging();
		  if(tws_pairsuceful_flg == true)
		  {
              data_testwrite1205 = 0x10;
		  }
		  data_testwrite1205 |= lastbatteylevle;
		  
		  //data_testwrite1205  = 0x71;
		  
		  TRACE(0,"rlastbatteylevle = 0x%x",data_testwrite1205);
		  I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_testwrite1205,1);//回写
		  I2C_MasterReadData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_testwrite1205,1);
		  TRACE(0,"read data_testwrite1205 = 0x%x",data_testwrite1205);	
	  	break;


	  case THREAD_MSG_INPUT_EVENT_I2CINT:
	  	TRACE(5,"message_id----->THREAD_MSG_INPUT_EVENT_I2CINT");
	  	Icp1205UpdataIntSts();
		i2c_event_enableint(ICP1205_IIC_INT_GPIO);
	  	break;

   
      case THREAD_MSG_INPUT_EVENT_INBOX:
	  	TRACE(5,"message_id----->THREAD_MSG_INPUT_EVENT_INBOX");
		 if(box_status == box_status_power_off)
		  return 0;
		 if(box_status == box_status_in)
		  return 0;	

		 if(sysbtmode == sysbtmode_dut)
		 return 0;


#ifdef AW2217
		app_status_indication_aw22127set(APP_STATUS_INDICATION_BOXIN);
#endif

		 
		box_status = box_status_in;
		if(tws_role_switch_flg == false)
		{
		   tws_role_switch_flg = true;
		   app_ibrt_ui_event_entry(IBRT_PUT_IN_EVENT);
		}
	  	break;

		
	  case THREAD_MSG_INPUT_EVENT_OUTBOX:
	  	TRACE(5,"message_id----->THREAD_MSG_INPUT_EVENT_OUTBOX");
		if(box_status == box_status_out)
		  return 0;
		 if(sysbtmode == sysbtmode_dut)
		 return 0;		
		box_status = box_status_out;
		if(tws_role_switch_flg == true)
		{
		   tws_role_switch_flg = false;
		   app_ibrt_ui_event_entry(IBRT_FETCH_OUT_EVENT);
		}
		break;


		
	  case THREAD_MSG_UART_CMD_OPENCANG:
	  	TRACE(5,"message_id----->THREAD_MSG_UART_CMD_OPENCANG");
		if(sysbtmode == sysbtmode_dut)
		return 0;
	    if(box_status == box_status_power_off)
	    {
	  	    break;
	    }

				#ifdef AW2217
                aw22127_init();
				#endif		

#ifdef AW2217
		    app_status_indication_aw22127set(APP_STATUS_INDICATION_BOXOPEN);
#endif

		
		box_status = box_status_open;
		app_auto_twspair();
	  	break;
		
		
	  case THREAD_MSG_UART_CMD_CLOSECANG:
	  	TRACE(5,"message_id----->THREAD_MSG_UART_CMD_CLOSECANG");
		if(box_status == box_status_off)
		break;	

        if(sysbtmode == sysbtmode_dut)
        {
          app_shutdown();	
		  break;
		}
#ifdef AW2217
		app_status_indication_aw22127set(APP_STATUS_INDICATION_BOXCLOSE);
#endif		
		box_status = box_status_off;
		app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
		app_stop_10_second_timer(APP_PAIR_TIMER_ID);
		app_ibrt_ui_event_entry(IBRT_CLOSE_BOX_EVENT);
		hds_status_set_cang_onoff(true);
		hds_status_set_chargemode(true);
		nv_record_flash_flush();
		norflash_flush_all_pending_op();	

#ifdef AW2217
		  aw22127_led_en(0);
#endif
	  break;
	  

	  case THREAD_MSG_BATTRY_EVENT_FULLCHARGE:
	  	TRACE(5,"message_id----->THREAD_MSG_BATTRY_EVENT_FULLCHARGE");
		if(box_status !=box_status_power_off)
		{
	  	  app_shutdown();			//充满电,关机
	  	  box_status = box_status_power_off;
		}
	  	break;



	   //DUT
	   case THREAD_MSG_UART_CMD_TEST:
			 TRACE(5,"message_id----->DUT");
			 app_poweron_factorymode_fjh(); 	   
			 break;
	  
	  //BT TEST
	   case THREAD_MSG_UART_CMD_ANC_TEST:
		   TRACE(5,"message_id----->BT_TEST");
		   app_bt_sing_enter_pair();	   
		   //app_bt_enter_pair();	
		   break;

			  
	  //5S
	  case THREAD_MSG_UART_CMD_BTPAIR:
	  TRACE(5,"message_id----->THREAD_MSG_UART_CMD_BTPAIR");
	  app_bt_enter_pair();
	  break;
			  	  
			  
	  //恢复出厂....  10S	
	  case THREAD_MSG_UART_CMD_RESET:
	  TRACE(5,"message_id----->RESET");
	  break;	 


/*********************************************************************************************************************/
      //单线升级或OTA
	    case THREAD_MSG_UART_CMD_UPGRADE:
		TRACE(5,"message_id----->OTA");
	  	break;
/*********************************************************************************************************************/
        case THREAD_MSG_ENTER_SING_UART_MODE:
		box_status = box_status_power_off;
		break;

		case THREAD_MSG_TWS_PAIRING_MODE:
		break;
/*****************用户区盒子定义*******************/
        //shuangji
	    case THREAD_MSG_UART_EVENT_MOBILE_DISCONNECTED:
	    TRACE(5,"message_id----->THREAD_MSG_UART_EVENT_MOBILE_DISCONNECTED");
		break;




		case THREAD_MSG_TWS_PAIRING_SUCEESFULL_MODE:
			
			break;
			






/********************************************************/
		
	  default:break;


   }
   return 0;
}





void Entet_Singleline(void)
{
  //  hal_iomux_set_uart1();
	sing_dowload_init();
	//app_singleline_enter_fan();
}



uint8_t hds_thead_msg_findid(uint32_t msgid)
{
   return 0;
}

uint32_t hds_get_delaytime(uint32_t start)
{

   return 0;
}

void hds_thread_msg_putwith_param(uint32_t msgid,uint32_t param1,uint32_t param2)
{

}

static void hds_delay_timehandler(void const * param)
{
    hds_thread_msg_send(msgid_fjh);
}

void hds_delay_set_time(uint32_t millisec)
{
  if (hds_delay_timer == NULL)
  {
	 hds_delay_timer	= osTimerCreate(osTimer(HDS_DELAY), osTimerOnce, NULL);
	 osTimerStop(hds_delay_timer);
  } 
     osTimerStart(hds_delay_timer, millisec);
}



void hds_delay_msg_add(uint8_t eventmode,uint16_t timedelay,bool enflg)
{
    msgid_fjh = eventmode;
    hds_delay_set_time(timedelay);
}



void hds_thread_msg_send(uint32_t msgid)
{
    APP_MESSAGE_BLOCK msg;
    msg.mod_id          = APP_MODUAL_BOX;
    msg.msg_body.message_id = msgid;
    msg.msg_body.message_ptr = (uint32_t) NULL;
    app_mailbox_put(&msg);
}


void hds_thread_init(void)
{
  app_set_threadhandle(APP_MODUAL_BOX, hds_thread_msg_handler);
}


void hds_tread_exit()
{

}



uint8_t box_status_test(void)
{
    return box_status;
}



