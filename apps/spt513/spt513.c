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
#include "spt513.h"
#include "ble_cmd.h"
#include "app_tws_if.h"
#include "led.h"
#include "hal_key.h"

#define LOG_MODULE HAL_TRACE_MODULE_APP

void spt513_irq_en(void);
void spt513_Send_Cmd(uint32_t id);

static void spt513_delay_timehandler(void const * param);
osTimerDef(SPT513_DELAY, spt513_delay_timehandler);
static osTimerId       spt513_delay_timer = NULL;
#define SPT513_EINT_EVT               1
#define SPT513_IN_EINT_EVT            2
#define SPT513_OUT_EINT_EVT           3


bool earin_left_flg = false;
bool earin_right_flg = false;
extern bool earin_flg;

const struct HAL_IOMUX_PIN_FUNCTION_MAP spt513_cfg_gpio[1] =
{
    {
        SPT513_IRQ_GPIO, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
    },
};



void spt513_irqhandler(enum HAL_GPIO_PIN_T pin)          //中断产生后执行
{
    TRACE(9,"spt513_irqhandler>>>>>>>>>>>>>>>> \n");
    spt513_Send_Cmd(SPT513_EINT_EVT);
}


static void spt513_gpio_enable_irq(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_IRQ_POLARITY_T polarity)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;
    hal_gpio_pin_set_dir(pin, HAL_GPIO_DIR_IN, 1);

    gpiocfg.irq_enable  = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_polarity = polarity;
    gpiocfg.irq_handler = spt513_irqhandler;
    gpiocfg.irq_type    = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;

    hal_gpio_setup_irq(pin, &gpiocfg);
}




static void spt513_gpio_disable_irq(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_IRQ_POLARITY_T polarity)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;
    hal_gpio_pin_set_dir(pin, HAL_GPIO_DIR_IN, 1);

    gpiocfg.irq_enable  = false;
    gpiocfg.irq_debounce = false;
    gpiocfg.irq_polarity = polarity;
    gpiocfg.irq_handler = NULL;
    gpiocfg.irq_type    = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;

    hal_gpio_setup_irq(pin, &gpiocfg);
}



static void spt513_delay_timehandler(void const * param)
{
    if(hal_gpio_pin_get_val(SPT513_IRQ_GPIO))
    {
       TRACE(9,"high.......... \n");
       spt513_irq_en();
	   
      if(earin_flg == true)
      {
          earin_flg = false;
		  if(app_tws_is_left_side())
		  {
			 earin_left_flg = false;
		  }
		  else if(app_tws_is_right_side())
		  {
			 earin_right_flg = false;
		  }

#ifdef SPT513
	  send_key_event(HAL_KEY_CODE_FN1,HAL_KEY_EVENT_EAROUT);
#endif			
	  TRACE(9,"SPT513_OUT_EINT_EVT.......... \n");


	  twssyncdata[0] = EARINOUT_SYNC;
	  twssyncdata[1] = earin_left_flg;
	  twssyncdata[2] = earin_right_flg;
	  app_ibrt_customif_test_fjh(twssyncdata,10);

      earphone_inout_res(earin_left_flg,earin_right_flg);

	#ifdef __EARIN_KEY_APP_T__
	  hal_gpiokey_close_app();
    #endif


	  
      }


	   
	  // if(earin_flg == true)
      // spt513_Send_Cmd(SPT513_OUT_EINT_EVT);
	}
	else
	{
	   TRACE(9,"low.......... \n");
	   spt513_irq_en();
	   if(earin_flg == false)
	   {
	   earin_flg = true;

       if(app_tws_is_left_side())
       {
          earin_left_flg = true;
       }
       else if(app_tws_is_right_side())
       {
          earin_right_flg = true;
       }		

	   
#ifdef SPT513
	   send_key_event(HAL_KEY_CODE_FN1,HAL_KEY_EVENT_EARIN);
#endif		
	   TRACE(9,"SPT513_IN_EINT_EVT.......... \n");

       twssyncdata[0] = EARINOUT_SYNC;
	   twssyncdata[1] = earin_left_flg;
	   twssyncdata[2] = earin_right_flg;
	   app_ibrt_customif_test_fjh(twssyncdata,10);

	   earphone_inout_res(earin_left_flg,earin_right_flg);
#ifdef __EARIN_KEY_APP_T__
		 hal_gpiokey_open_app();
#endif

	   
	   }
	   
	   //if(earin_flg == false)
       //spt513_Send_Cmd(SPT513_IN_EINT_EVT);
	}
}




void spt513_delay_set_time(uint32_t millisec)
{
  if (spt513_delay_timer == NULL)
  {
	 spt513_delay_timer	= osTimerCreate(osTimer(SPT513_DELAY), osTimerOnce, NULL);
	 osTimerStop(spt513_delay_timer);
  } 
     osTimerStart(spt513_delay_timer, millisec);
}


/************************************************************************************************/
int app_spt513_process(APP_MESSAGE_BODY * msg)
{
   if (msg->message_id == SPT513_EINT_EVT)
   {
      TRACE(9,"SPT513_EINT_EVT.......... \n");
      spt513_delay_set_time(500);
   }
   else if(msg->message_id == SPT513_IN_EINT_EVT)
   {
   /*
       earin_flg = true;
       #ifdef SPT513
       send_key_event(HAL_KEY_CODE_PWR,HAL_KEY_EVENT_EARIN);
       #endif	   
       TRACE(9,"SPT513_IN_EINT_EVT.......... \n");

       */
   }
   else if(msg->message_id == SPT513_OUT_EINT_EVT)
   {
   /*
       earin_flg = false;
       #ifdef SPT513
       send_key_event(HAL_KEY_CODE_PWR,HAL_KEY_EVENT_EAROUT);
       #endif		   
       TRACE(9,"SPT513_OUT_EINT_EVT.......... \n");
       */
   }
   return 0;
}


void spt513_Send_Cmd(uint32_t id)
{
  APP_MESSAGE_BLOCK msg;
  msg.mod_id			= APP_MODUAL_SPT513;
  msg.msg_body.message_id = id;
  msg.msg_body.message_ptr = (uint32_t) NULL;
  app_mailbox_put(&msg);
}



void spt513_irq_en(void)
{
    if(hal_gpio_pin_get_val(SPT513_IRQ_GPIO))
    {
        //earin_flg = false;
        spt513_gpio_enable_irq(SPT513_IRQ_GPIO, HAL_GPIO_IRQ_POLARITY_LOW_FALLING);
    }
	else
	{
	    //earin_flg = true;
        spt513_gpio_enable_irq(SPT513_IRQ_GPIO, HAL_GPIO_IRQ_POLARITY_HIGH_RISING);
	}
}

void spt513_close(void)
{
   spt513_gpio_disable_irq(SPT513_IRQ_GPIO, HAL_GPIO_IRQ_POLARITY_LOW_FALLING);
}


void spt513_init(void)
{
    app_set_threadhandle(APP_MODUAL_SPT513, app_spt513_process);
    hal_iomux_init(spt513_cfg_gpio, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T) spt513_cfg_gpio[0].pin, HAL_GPIO_DIR_IN, 1);

    if(hal_gpio_pin_get_val(SPT513_IRQ_GPIO))
    {
        if(app_tws_is_left_side())
        {
           earin_left_flg = false;
		}
		else if(app_tws_is_right_side())
        {
           earin_right_flg = false;
		}
        earin_flg = false;

		#ifdef __EARIN_KEY_APP_T__
        hal_gpiokey_close_app();
		#endif
    }
	else
	{
	    if(app_tws_is_left_side())
        {
           earin_left_flg = true;
		}
		else if(app_tws_is_right_side())
        {
           earin_right_flg = true;
		}		
	    earin_flg = true;

        #ifdef __EARIN_KEY_APP_T__
        hal_gpiokey_open_app();
        #endif
	}
	
	spt513_irq_en();
}

