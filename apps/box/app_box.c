//========================================================================
//  add by guyu for laiyuan mcu communication
//  time:2020-6-30
//========================================================================

#define __HDS_COMM_LY_C__

#include "cmsis_os.h"
#include "hal_i2c.h"
#include "hal_trace.h"
#include "pmu.h"
#include "app_box.h"
#include "hds_apply_thread.h"
#include "hds_sys_status_manage.h"
#include "apps.h"
bool i2c_int_flg = false;

//#define ICP1205_DVCID 0xC2

uint8_t box_battery = 0;




void i2c_event_disableint(enum HAL_GPIO_PIN_T pin);
#if 0
unsigned char I2C_MasterWriteData(unsigned char add, unsigned char reg, unsigned char *data, unsigned char length)
{
    uint8_t buf[20];
    buf[0] = reg;
	
    for (uint8_t i=1;i<length+1;i++)
		buf[i] = data[i-1]; 

	return hal_i2c_send ( HAL_I2C_ID_1, add, buf, 1, length, 0, 0 );
}


unsigned char I2C_MasterReadData(unsigned char add, unsigned char reg, unsigned char *data, unsigned char length)
{
	uint8_t buf[20];
	buf[0] = reg;
	
	uint8_t ret = hal_i2c_recv ( HAL_I2C_ID_1, add, buf, 1, length, HAL_I2C_RESTART_AFTER_WRITE, 0, 0 );

	for (uint8_t i=0;i<length;i++)
		data[i] = buf[i+1];
	
	return ret;
}
#endif


unsigned char I2C_MasterWriteData(unsigned char add, unsigned char reg, unsigned char *data, unsigned char length)
{
   // uint8_t buf[20];
   // for (uint8_t i=0;i<length+1;i++)
	//	buf[i] = data[i]; 




 //  hal_gpio_i2c_simple_send2(add, reg, buf,length);

   hal_gpio_i2c_simple_send2(add, reg, data,length);

   return 0;

	
}


unsigned char I2C_MasterReadData(unsigned char add, unsigned char reg, unsigned char *data, unsigned char length)
{
	uint8_t buf[100];
//	buf[0] = reg;
	
//	uint8_t ret = hal_i2c_recv ( HAL_I2C_ID_1, add, buf, 1, length, HAL_I2C_RESTART_AFTER_WRITE, 0, 0 );

	hal_gpio_i2c_read_data(add,reg,1,buf,length);

	for (uint8_t i=0;i<length;i++)
		data[i] = buf[i];
	
	return 0;
}





/******************************************************************************								
 * Local pre-processor symbols/macros ('#define')								
 ******************************************************************************/								
								
#define show_ref_dbg 1								
extern uint8_t gU8RxData[16];
extern volatile uint8_t gU8RxFlg,gU8RxCnt,gU8RxStFlg,gU8RecCmdFlg;
extern volatile uint8_t gU8CurRecdat;
extern volatile uint16_t gU16RecCnt;
extern void SetDefChrgEn(void);
/*******************************************************************************								
 * Function implementation - global ('extern') and local ('static')								
 ****/	
 /*
void Icp1205TrcomTest(void)
{  
	uint8_t u8RegTable[40],i,yyy,u8dat1,u8tmp;	
	uint32_t u32cnt,u32Vbat;
		OLED_ClearArea(0, 0,127, 3);		
			i=32;		
			OLED_ShowCHinese(i,0,py_tou);					
			i +=16;					
			OLED_ShowCHinese(i,0,py_chuan);				
			i +=16;					
			OLED_ShowString(i,0,(uint8_t*)"...",16);		
			OLED_ShowString(0,2,(uint8_t*)"Dat=  ",16);		
			OLED_ShowString(56,2,(uint8_t*)"Cnt=:",16);		
			u8dat1=0x11;
			 I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8dat1,1); 
			 u8dat1=0x10;
			 I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8dat1,1);  
			 u8dat1=0x00;
			 I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8dat1,1);  
			gU8RxFlg=0;
			gU16RecCnt=0;		
			gU8CurRecdat=0;
			gU8RxStFlg=0;
			 Uart_ClrStatus(UARTCH1,UartRxFull);
					 OLED_ShowHex(32,2,gU8CurRecdat,16);
					 OLED_ShowNum(88,2,gU16RecCnt,5,16);	
			do{
			 u32cnt=0; 
				while(u32cnt<200000)
				{
					u32cnt++;
				 if(Uart_GetStatus(UARTCH1,UartRxFull))
				 {
					
	
					  Uart_ClrStatus(UARTCH1,UartRxFull);
					  gU8CurRecdat=Uart_ReceiveData(UARTCH1);
						
						
					 gU16RecCnt++;
					 gU8RxFlg=0;
					 u32cnt=0;
					 OLED_ShowHex(32,2,gU8CurRecdat,16);
					 OLED_ShowNum(88,2,gU16RecCnt,5,16);	
						
						if(gU8RxStFlg)
						{
							gU8RxData[gU8RxCnt]=gU8CurRecdat;
							gU8RxCnt++;
							if(gU8RxCnt==2)
							{
								gU8RecCmdFlg = 1;
								gU8RxStFlg=0;
								gU8RxCnt=0;
							}
						}
						else
						{
							if(gU8CurRecdat==0xaa)gU8RxCnt++;
							else gU8RxCnt=0;
							if(gU8RxCnt==4)
							{
								gU8RxStFlg=1;
								gU8RxCnt=0;
							}
						}
				 }
				 
				 if(gU8RecCmdFlg)
				 {
					  gU8RecCmdFlg=0;
					 
          
           	if(gU8RxData[0]=='V')	
						{	
							Gpio_SetIO(3,4,1); //发送
							OLED_ShowString(90,0,(uint8_t*)"cmd",8);
							 
							 GetBatVoltag(&u32Vbat);
							 Gpio_InitIOExt(3,5,GpioDirOut,TRUE,FALSE,FALSE,FALSE);   
               Gpio_SetFunc_UART1TX_P35();
							 for(i=0;i<20;i++)
							{
							// delay1ms(2);
							 Uart_SendData(UARTCH1, 0xAA);
								
							 delay100us(1);
							 Uart_SendData(UARTCH1, 0xAA);
							 delay100us(1);
							 Uart_SendData(UARTCH1, 0xAA);
							 delay100us(1);
							 Uart_SendData(UARTCH1, 0xAA);
							 delay100us(1);
							 Uart_SendData(UARTCH1, 0xcc);
							 delay100us(1);
							 Uart_SendData(UARTCH1, u32Vbat>>8);
							 delay100us(1);
							 Uart_SendData(UARTCH1, u32Vbat);
							 OLED_ShowNum(90,1,u32Vbat,4,8);
							}
							 delay1ms(10);
							 Gpio_SetIO(3,4,0); //发送
//							 u8dat1=0x88;
//					     I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8dat1,1); 
//						
//							 delay100us(2);
//							 u8dat1=0x00;
//					     I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8dat1,1); 
							 Gpio_InitIOExt(3,5,GpioDirIn,TRUE,FALSE,FALSE,FALSE);   
							 M0P_GPIO->P35_SEL_f.SEL = 0;
							 delay1ms(1000);
						}							
				 }
				 			
			 }
				//OLED_ShowString(90,1,(uint8_t*)"   ",8);	
				OLED_ShowNum(90,0,i++,3,8);	
        I2C_MasterReadData(ICP1205_DVCID,ICP1205_COMM_CON,&u8dat1,1);	
      }while(u8dat1&COMMCON_BUSY_STS);
			
		 	
			 u8dat1=0x00;
			 I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8dat1,1);
			 OLED_ShowString(72,0,(uint8_t*)" End   ",16);		
			 Icp1205SetChrgFunEnable();//		
			
	
}
*/

/**
 *******************************************************************************
 ** \brief 关闭CHRG功能
 **  
 ** \param [in]  无
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205SetChrgFunDisable(void)
{    
	TRACE(3,"%s",__func__);
  uint8_t dat;
   I2C_MasterReadData(ICP1205_DVCID,ICP1205_CHRG_CON1,&dat,1);
	 dat &=0XFC;//(~CHRGCON1_CHRG_EN);  
	 I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CHRG_CON1,&dat,1);
}
/**
 *******************************************************************************
 ** \brief 打开CHRG功能
 **  
 ** \param [in]  无
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205SetChrgFunEnable(void)
{
	TRACE(3,"%s",__func__);
  uint8_t dat;
   I2C_MasterReadData(ICP1205_DVCID,ICP1205_CHRG_CON1,&dat,1);
	 dat |=0x03;  
	 I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CHRG_CON1,&dat,1);
}

/**
 *******************************************************************************
 ** \brief 屏蔽CMP0.15中断
 **  
 ** \param [in]  无
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205Cmp0P15FunDisable(void)
{
	TRACE(3,"%s",__func__);
  uint8_t dat;
     I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_MASK2,&dat,1);
	 dat |=INT2_0P15CMP_FLG;  
	 I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK2,&dat,1);
}
/**
 *******************************************************************************
 ** \brief 打开CMP0.15中断
 **  
 ** \param [in]  无
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205Cmp0P15FunEnable(void)
{
/*
	TRACE("%s",__func__);
  uint8_t dat;
     I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_MASK2,&dat,1);
	 dat &=(~INT2_0P15CMP_FLG);  
	 I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK2,&dat,1);
	 */
}



void test1205_2(void)
{

uint8_t u8RegTable[5];

I2C_MasterReadData(ICP1205_DVCID,0X21,u8RegTable,3);
TRACE(3,"Icp1205UpdataIntSts--->test21 %d %d %d",u8RegTable[0],u8RegTable[1],u8RegTable[2]);


I2C_MasterReadData(ICP1205_DVCID,0X05,u8RegTable,1);
TRACE(3,"Icp1205UpdataIntSts--->test05 %d",u8RegTable[0]);


}




//extern "C" int app_shutdown(void);
unsigned char data_tmp[2]={0,0};
extern uint8_t box_status;
bool Auto_Cang_Power_Off;
//extern bool cangout_poweroff;

void Icp1205UpdataIntSts(void)						
{								
	   uint8_t u8RegTable[5],u8dat1,u8tmp;	
//	   uint32_t u32cnt,u32Vbat;
//	   static uint8_t CrSendDat;
	   uint8_t repeat_flag;
//	   bool full_chger_flg = false;
	   uint8_t repeat_num = 0;


	  // uint8_t i = 0;
       //,i  ,u32Vbat

	do {
	  	//I2C_MasterReadData(ICP1205_DVCID,0X00,u8RegTable,40);
		//add by guyu 20200709
		//读3个指令寄存器
	   	I2C_MasterReadData(ICP1205_DVCID,0X21,u8RegTable,3);
		TRACE(3,"Icp1205UpdataIntSts %d %d %d",u8RegTable[0],u8RegTable[1],u8RegTable[2]);

		//add by guyu 20200805
		uint8_t tmp1=0xff;					
		I2C_MasterReadData(ICP1205_DVCID,ICP1205_CHRG_STS2,&tmp1,1);
		TRACE(3,"ICP1205_CHRG_STS2 %d(d)",tmp1);
		
//		if(u8RegTable[ICP1205_INT_STAT1]==0x00 && (u8RegTable[ICP1205_INT_STAT2]==INT2_CHRGSTS_CH_FLG)
//			  && u8RegTable[ICP1205_INT_STAT3]==0x00)
//		 {
//			 u8dat1=INT2_CHRGSTS_CH_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8dat1,1);
//			 return;
//		 }
//		i=8;
//		OLED_ShowString(i,0,(uint8_t*)"*Note:",16);				
	
//		i=0;
//		OLED_ShowHex(i++*18,0,u8RegTable[4],8);
//		OLED_ShowHex(i++*18,0,u8RegTable[5],8);
//		OLED_ShowHex(i++*18,0,u8RegTable[6],8);
//	
//		OLED_ShowHex(i++*18,0,u8RegTable[7],8);
//		OLED_ShowHex(i++*18,0,u8RegTable[8],8);
//		OLED_ShowHex(i++*18,0,u8RegTable[9],8);
//		i=0;
//		OLED_ShowHex(i++*18,1,u8RegTable[0x10],8);
//		OLED_ShowHex(i++*18,1,u8RegTable[0x21],16);
//		OLED_ShowHex(i++*18,1,u8RegTable[0x22],16);
//		OLED_ShowHex(i++*18,1,u8RegTable[0x23],16);
//		OLED_ShowHex(i++*18,1,u8RegTable[0x24],16);	
//		OLED_ShowHex(i++*18,1,u8RegTable[0x25],16);
//		OLED_ShowHex(i++*18,1,u8RegTable[0x26],16);			
//		delay1ms(200);
//	
		//del by guyu 20200821
		/*
		if(u8RegTable[1]&INT2_0P15CMP_FLG)		//??CHARGE				
		{	
			i=8;								
		
			u8dat1=INT2_0P15CMP_FLG;					
			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8dat1,1);				

			I2C_MasterReadData(ICP1205_DVCID,ICP1205_CHRG_STS3,&u8tmp,1);
			if ((u8tmp&CHRGSTS3_VIN2BAT_0P15CMP) == 0)
				Icp1205SetChrgFunDisable();

		}
	
		if(u8RegTable[1]&INT2_0P35CMP_FLG)		//??CHARGE				
		{	
			i=8;								
		
			u8dat1=INT2_0P35CMP_FLG;					
			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8dat1,1);				

			I2C_MasterReadData(ICP1205_DVCID,ICP1205_CHRG_STS3,&u8tmp,1);
			if ((u8tmp&CHRGSTS3_VIN2BAT_0P35CMP) == 1)
				Icp1205SetChrgFunEnable();

		}
		*/


		//add by guyu 20200709
		//入仓
		if(u8RegTable[0]&INT1_PLGIN_FLG)							
		{						
		//	i=8;					
			//OLED_ShowString(i,2,(uint8_t*)"Vin plug in.",16);				
      		Icp1205SetChrgFunEnable();//test				
			//delay1ms(100);							
			u8dat1=INT1_PLGIN_FLG;					
			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8dat1,1); //??
			//hds_status_set_chargemode(true);
			Icp1205SetChrgFunEnable();
			//del by guyu 20200824
			//Icp1205Cmp0P15FunEnable();	
			//hds_thread_msg_send(THREAD_MSG_INPUT_EVENT_INBOX);

		}

		//add by guyu 20200709
		//出仓			
		if(u8RegTable[0]&INT1_PLGOUT_FLG)							
		{			
			//i=8;					
			//OLED_ShowString(i,2,(uint8_t*)"Vin plug out.",16);	
      		        Icp1205SetChrgFunEnable();			
			u8dat1=INT1_PLGOUT_FLG;					
			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8dat1,1); //回写
			//add by guyu 20200722
			//if (hds_status_get_cang_onoff()==true)
			if(box_status == box_status_off)
			{
			        Auto_Cang_Power_Off = false;
		               // cangout_poweroff = false;
				app_shutdown();			//关仓状态下出仓。关机
				box_status = box_status_power_off;
			}else{
				hds_status_set_chargemode(false);
				Icp1205SetChrgFunEnable();
				hds_thread_msg_send(THREAD_MSG_INPUT_EVENT_OUTBOX);
			}
			//return;//DEBUG					
		}						
								
//		if(u8RegTable[ICP1205_INT_STAT1]&INT1_CHRGERR_FLG)						
//		{						
//			i=8;					
//			OLED_ShowString(i,2,(uint8_t*)"Charger error.",16);	
//			u8dat1=INT1_CHRGERR_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8dat1,1);					
//			//return;//DEBUG					
//		}						
//								
//	  if(u8RegTable[ICP1205_INT_STAT1]&INT1_WDTOVTIME_FLG)							
//		{						
//			i=8;			
//			OLED_ShowString(i,2,(uint8_t*)"WDT over time.",16);				
//			delay1ms(100);					
//			u8dat1=INT1_WDTOVTIME_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8dat1,1);					
//					
//		}						
//								
//		if(u8RegTable[ICP1205_INT_STAT1]&INT1_BATLOW_FLG)						
//		{						
//			i=8;					
//			
//			i=8;			
//			OLED_ShowString(i,2,(uint8_t*)"Batter low",16);					
//			//delay1ms(100);					
//			u8dat1=INT1_BATLOW_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8dat1,1);								
//		}						
//								
								
//		if(u8RegTable[ICP1205_INT_STAT1]&INT1_GP0CH_FLG)						
//		{						
//			i=8;
//      if(u8RegTable[ICP1205_GPIO_CON]&GPIOCON_GP0_STS)OLED_ShowString(i,2,(uint8_t*)"GPIO0 set 'H'",16);		
//			else OLED_ShowString(i,2,(uint8_t*)"GPIO0 Clr 'L'",16);		
//			
//			delay1ms(100);					
//			u8dat1=INT1_GP0CH_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8dat1,1);										
//		}						
//								
								
//		if(u8RegTable[0]&INT1_CHRGFIN_FLG)						
//		{						
//			i=8;							
//							
//			u8dat1=INT1_CHRGFIN_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8dat1,1);
//		}						
///*******************/							
//								
//		if((u8RegTable[ICP1205_INT_STAT2]&INT2_CHRGSTS_CH_FLG) && 
//      (u8RegTable[ICP1205_INT_MASK2]&INT2_CHRGSTS_CH_MSK)==0X00)   			
//		{						
//			i=8;	

//      u8tmp=u8RegTable[ICP1205_CHRG_STS1]&0x07;		
//			switch(u8tmp)
//			{
//				case CHRGSTS1_CHRG_IDLE_STS:
//					 OLED_ShowString(i,2,(uint8_t*)"Charge Idle.",16);		
//				   break;
//				
//				case CHRGSTS1_CHRG_TRCKL_STS:
//					 OLED_ShowString(i,2,(uint8_t*)"Charge Trickle.",16);		
//				   break;
//				
//				case CHRGSTS1_CHRG_FAST_STS:
//					 OLED_ShowString(i,2,(uint8_t*)"Charge Fast STS.",16);		
//				   break;
//				
//				case CHRGSTS1_CHRG_NOR_STS:
//					 OLED_ShowString(i,2,(uint8_t*)"Charge normal STS.",16);		
//				   break;
//				
//				case CHRGSTS1_CHRG_DLYTIME_STS:
//					 OLED_ShowString(i,2,(uint8_t*)"Charge extend.",16);		
//				   break;
//				
//				case CHRGSTS1_CHRG_FINSH_STS:
//					 OLED_ShowString(i,2,(uint8_t*)"Charge finish.",16);		
//				   break;
//	
//				case CHRGSTS1_CHRG_ERR_STS:
//				  	i=0;
//					 OLED_ShowString(i,2,(uint8_t*)"Charge error.",16);	
//						if(u8RegTable[ICP1205_CHRG_STS2]&CHRGSTS2_VIN_OVP_STS)						
//							{						
//								i +=sizeof("Charge error.");					
//								OLED_ShowString(i,2,(uint8_t*)"OVP!",16);						
//	
//							}						
//													
//							if(u8RegTable[ICP1205_CHRG_STS2]&CHRGSTS2_VIN_UVLO_STS)						
//							{
//								i +=sizeof("Charge error.");	
//								OLED_ShowString(i,2,(uint8_t*)"UVLO",16);						
//										
//							}		
//				   break;
//        default:
//					break;
//				
//			}
//							
//			//delay1ms(100);					
//			u8dat1=INT2_CHRGSTS_CH_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8dat1,1);								
//		}	
//		
///***************************/		
//		
//		
//		
		
//		if(u8RegTable[ICP1205_INT_STAT2]&INT2_CCCV_TIMEOUT_FLG)						
//		{						
//			i=8;					
//			OLED_ShowString(i,2,(uint8_t*)"Charge over time!",16);						
//			
//			//delay1ms(100);					
//			u8dat1=INT2_CCCV_TIMEOUT_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8dat1,1);					
//			//return;//DEBUG					
//		}				
//		
//		if(u8RegTable[ICP1205_INT_STAT2]&INT2_TRCKL_TIMEOUT_FLG)						
//		{						
//			i=8;					
//			OLED_ShowString(i,2,(uint8_t*)"Charge bad!",16);						
//			
//			//delay1ms(100);					
//			u8dat1=INT2_TRCKL_TIMEOUT_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8dat1,1);					
//			//return;//DEBUG					
//		}										
//							
//		if(u8RegTable[ICP1205_INT_STAT2]&INT2_NTC_FLG)						
//		{						
//			i=8;	
//      if(u8RegTable[ICP1205_CHRG_STS3]&CHRGSTS3_NTC_L_STS)OLED_ShowString(i,2,(uint8_t*)"Bat Temp 'L'!",16);	
//      else 		OLED_ShowString(i,2,(uint8_t*)"Bat Temp 'H'!",16);	
//			
//			//delay1ms(100);					
//			u8dat1=INT2_NTC_FLG;					
//			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8dat1,1);					
//			//return;//DEBUG					
//		}		
		
	 
		if(u8RegTable[2]&INT3_CRRXCMD_FLG)						
		{
			/*
			i=8;		
			OLED_ShowCHinese(i,2,py_zai);					
			i +=16;					
			OLED_ShowCHinese(i,2,py_bo);				
			i +=16;	
			*/
			
			u8dat1=INT3_CRRXCMD_FLG;
			//add by guyu 20200709
			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT3,&u8dat1,1);	//回写
			
			I2C_MasterReadData(ICP1205_DVCID,ICP1205_CRCOM_CON1,&u8tmp,1);  //读指令

			Icp1205SetChrgFunEnable();

      		switch(u8tmp&0xf0)		
			{
				case CR_REC_CMD_QUE_CHRG_STS:
					//OLED_ShowString(40,2,(uint8_t*)":Read Sts",16);
					break;
				case CR_REC_CMD_READ_DATA:
				  	I2C_MasterReadData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&u8dat1,1);	//回写

//				 	u8dat1=CrSendDat++;//Bt_Cnt16Get(TIM1);
//				  	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&u8dat1,1);	
					break;
				case CR_REC_CMD_FRC_CHRG_FNSH:
					//OLED_ShowString(40,2,(uint8_t*)":Chrg Fnsh",16);
				  	//del by guyu 20200821
				  	//Icp1205SetChrgFunDisable();//test
					break;


					
				case CR_REC_CMD_SHIP_MOD:
					//OLED_ShowString(40,2,(uint8_t*)":Ship mode",16);
					hds_thread_msg_send(THREAD_MSG_SHIP_MODE);
				break;


					
				case CR_REC_CMD_CHAR_DATA:
					break;
				//add by guyu 20200709
				//收到双字节指令中断，读取两个字节，第一个为电量，第二个为操作指令
				case CR_REC_CMD_WORD_DATA:
					
					I2C_MasterReadData(ICP1205_DVCID,ICP1205_CRCOM_WORD_DAT,data_tmp,2);
					TRACE(3,"data_tmp %x %x",data_tmp[0],data_tmp[1]);
					

					//add by guyu 20200709
					//进入单线升级
					if((data_tmp[1]&0x0f)==0x1)
					{
						TRACE(3,"upgrade cmd: battery %x", data_tmp[0]);
				    	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
						uint8_t	dattmp=0x10;
						I2C_MasterWriteData(ICP1205_DVCID,0x1d,&dattmp,sizeof(dattmp)); 
						hds_thread_msg_send(THREAD_MSG_UART_CMD_UPGRADE);
						//del by guyu 20200824
                        //Icp1205Cmp0P15FunEnable();
					}

					//add by guyu 20200709
					//蓝牙配对
					if((data_tmp[1]&0x0f)==0x2)
					{
						TRACE(3,"bt pair cmd: battery %x", data_tmp[0]);
				    	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
						hds_thread_msg_send(THREAD_MSG_UART_CMD_BTPAIR);
						//del by guyu 20200824
                        //Icp1205Cmp0P15FunEnable();	
					}
					/*
					if((data_tmp[1]&0x0f)==0x4)
					{
						TRACE("reset cmd: battery %x", data_tmp[0]);
				    	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
						hds_thread_msg_send(THREAD_MSG_UART_CMD_RESET);
					}
					*/

					//add by guyu 20200709
					//关仓
					if((data_tmp[1]&0x0f)==0x5)
					{
						TRACE(3,"close cang cmd: battery %x", data_tmp[0]);
						box_battery = data_tmp[0];
				    	        I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
						//hds_status_set_chargemode(true);
						hds_thread_msg_send(THREAD_MSG_UART_CMD_CLOSECANG);	
                                                Icp1205Cmp0P15FunDisable();						
					}

					//add by guyu 20200709
				  	//开仓
				  	if((data_tmp[1]&0x0f)==0x6)
					{
						TRACE(3,"open cang cmd: battery %x", data_tmp[0]);
						box_battery = data_tmp[0];
				    	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
						hds_thread_msg_send(THREAD_MSG_UART_CMD_OPENCANG);
                        Icp1205Cmp0P15FunEnable();				
					}

					//add by guyu 20200709
					//入仓
					if((data_tmp[1]&0x0f)==0x7)
					{
						TRACE(3,"into cang cmd: battery %x", data_tmp[0]);
						box_battery = data_tmp[0];
				    	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
						hds_thread_msg_send(THREAD_MSG_INPUT_EVENT_INBOX);

						//add by guyu 20200805
						/*
						I2C_MasterReadData(ICP1205_DVCID,ICP1205_CHRG_STS3,data_tmp,1);
      					if ((u8tmp&CHRGSTS3_VIN2BAT_0P15CMP) == 0)
       						Icp1205SetChrgFunDisable();
       					*/
					}

					//add by guyu 20200709
					//恢复出厂
					if((data_tmp[1]&0x0f)==0x8)
					{
						TRACE(3,"recovery cmd: battery %x", data_tmp[0]);
				    	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写

						//add by guyu 20200826
						u8tmp=0x01;
 						I2C_MasterWriteData(ICP1205_DVCID,0x10,&u8tmp,1); //??????

						hds_thread_msg_send(THREAD_MSG_UART_CMD_RESET);
						//del by guyu 20200821
						//Icp1205SetChrgFunDisable();
					//	hal_gpio_pin_clr(HAL_GPIO_PIN_P0_4);

					}

					//add by guyu 20200709
					//满电
					if((data_tmp[1]&0x0f)==0x9)
					{
					        i2c_event_disableint(ICP1205_IIC_INT_GPIO);
						TRACE(3,"charge full: battery %x", data_tmp[0]);
				    	        I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
						hds_status_set_chargemode(false);
						hds_thread_msg_send(THREAD_MSG_BATTRY_EVENT_FULLCHARGE);
//						full_chger_flg = true;
						/*************************************************************************************/
						i2c_int_flg = false;
						
						//del by guyu 20200824
                        //Icp1205Cmp0P15FunEnable();	
					}

					//add by guyu 20200709
					//测试模式   DUT
					if((data_tmp[1]&0x0f)==0xA)
					{
						TRACE(3,"test cmd: battery %x", data_tmp[0]);
				    	        I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
						hds_thread_msg_send(THREAD_MSG_UART_CMD_TEST);
					}					
				


                //ANC TEST
				if((data_tmp[1]&0x0f)==0xB)
				{
					TRACE(3,"test cmd: battery %x", data_tmp[0]);
					I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
					hds_thread_msg_send(THREAD_MSG_UART_CMD_ANC_TEST);
				}


				if((data_tmp[1]&0x0f)==0xE)
				{
					TRACE(3,"test cmd: battery %x", data_tmp[0]);
					I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
					hds_thread_msg_send(THREAD_MSG_EARPHONE_BATTERY);
				}			

				if((data_tmp[1]&0x0f)==0xD)
				{
					TRACE(3,"test cmd: 0x0d %x", data_tmp[0]);
					I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_tmp[1],1);//回写
					hds_thread_msg_send(THREAD_MSG_UART_EVENT_MOBILE_DISCONNECTED);
				}					
				break;

					
				case CR_REC_CMD_CLR_GP0:
					//OLED_ShowString(40,2,(uint8_t*)":GP0='0'",16);
					break;
				case CR_REC_CMD_SET_GP0:
					//OLED_ShowString(40,2,(uint8_t*)":GP0='1'",16);
					break;
				default:
					break;
			}	
			//delay1ms(100);		
			//return;//DEBUG					
		}						
		
		//add by guyu 20200709
		//串口透传错误
		if(u8RegTable[2]&INT3_CRRXERR_FLG)						
		{		
			
			u8dat1=INT3_CRRXERR_FLG;					
			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT3,&u8dat1,1);	//回写
		  	//OLED_ShowString(40,2,(uint8_t*)"Crcom Err!",16);
			//delay1ms(100);						
		}						
					
		//add by guyu 20200709
		//进入串口透传
		if(u8RegTable[2]&INT3_TRCOM_FLG)						
		{		
			
			u8dat1=INT3_TRCOM_FLG;					
			I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT3,&u8dat1,1);//回写
			//del by guyu 20200821
			//Icp1205SetChrgFunDisable();
			//Icp1205TrcomTest();
			//return;//DEBUG					
		}						
			

#if 1
  //      if(full_chger_flg == false)
       	{

		u8tmp=0X7c;//保留插入拔出中断
		I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8tmp,1);	
		u8tmp=0XFf;//保留?.15中断
		I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8tmp,1);	
		u8tmp=0X1b;//保留透传、载波错误与成功中断
		I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT3,&u8tmp,1);		

		//add by guyu 20200804
		//检查是否需要有新指令未读
		repeat_flag = 0;
		I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8tmp,1);
		if ((u8tmp&0x83)!=0)
			repeat_flag =1;

		I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8tmp,1);
		if ((u8tmp&0x20)!=0)
			repeat_flag =1;
		
		I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_STAT3,&u8tmp,1);
		if ((u8tmp&0x14)!=0)
			repeat_flag =1;
       	}
	 //   else
	//   	{
        //    repeat_flag = 0;
	//    }
#endif

        repeat_num ++;
        if(repeat_num>3)
			return;




	//	 repeat_flag = 0;
			
		/*
		I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_STAT1,&u8tmp,1);
		TRACE("ICP1205_INT_STAT1= %x",u8tmp);
		I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8tmp,1);
		TRACE("ICP1205_INT_STAT2= %x",u8tmp);
		I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_STAT3,&u8tmp,1);
		TRACE("ICP1205_INT_STAT3= %x",u8tmp);
		*/
	
		}while(repeat_flag == 1);
 }								

void i2c_event_disableint(enum HAL_GPIO_PIN_T pin);
void i2c_event_enableint(enum HAL_GPIO_PIN_T pin);


static void i2c_event_irqhandler(enum HAL_GPIO_PIN_T pin)
{
	
	TRACE(3,"hal_gpio_pin_get_val(pin) %d",hal_gpio_pin_get_val(pin));
	if(hal_gpio_pin_get_val(pin) == 0)
	{
		i2c_event_disableint(pin);
		hds_thread_msg_send(THREAD_MSG_INPUT_EVENT_I2CINT);
	}
	
}



void test_event_irq(void)
{
 // uint8_t u8RegTable[5],u8RegTable05,u8RegTable0d,u8RegTable0e,u8RegTable00;

  if(i2c_int_flg == true)
  {
  /*
	I2C_MasterReadData(ICP1205_DVCID,0X20,u8RegTable,4);
	TRACE(3,"Icp1205UpdataIntSts %d %d %d %d",u8RegTable[0],u8RegTable[1],u8RegTable[2],u8RegTable[3]);

    I2C_MasterReadData(ICP1205_DVCID,0X05,&u8RegTable05,1);
	TRACE(3,"u8RegTable05 %d",u8RegTable05);

    I2C_MasterReadData(ICP1205_DVCID,0X0d,&u8RegTable0d,1);
	TRACE(3,"u8RegTable0d %d",u8RegTable0d);

    I2C_MasterReadData(ICP1205_DVCID,0X0e,&u8RegTable0e,1);
	TRACE(3,"u8RegTable0e %d",u8RegTable0e);

    I2C_MasterReadData(ICP1205_DVCID,0X00,&u8RegTable00,1);
	TRACE(3,"u8RegTable00 %d",u8RegTable00);

  
    TRACE(3,"hal_gpio_pin_get_val(pin) %d",hal_gpio_pin_get_val(HAL_GPIO_PIN_P1_1));*/
    if(hal_gpio_pin_get_val(ICP1205_IIC_INT_GPIO) == 0)
    {
      i2c_event_disableint(ICP1205_IIC_INT_GPIO);
	  hds_thread_msg_send(THREAD_MSG_INPUT_EVENT_I2CINT);
    }
  }
}




void i2c_event_enableint(enum HAL_GPIO_PIN_T pin)
{
	struct HAL_GPIO_IRQ_CFG_T gpiocfg;

    //hal_gpio_pin_set_dir(pin, HAL_GPIO_DIR_IN, 0);

    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    gpiocfg.irq_handler = i2c_event_irqhandler;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_LEVEL_SENSITIVE;

    hal_gpio_setup_irq(pin, &gpiocfg);
}

void i2c_event_disableint(enum HAL_GPIO_PIN_T pin)
{
	struct HAL_GPIO_IRQ_CFG_T gpiocfg;

	gpiocfg.irq_enable = false;
	gpiocfg.irq_debounce = false;
	gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
	gpiocfg.irq_handler = NULL;
	gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_LEVEL_SENSITIVE;
	
    hal_gpio_setup_irq(pin, &gpiocfg);
}

/**
 *******************************************************************************
 ** \brief 通过载波向ICP1106上传数据
 **ICP1106通过载波查询数据指令时，1205会自动把此寄存器的值上报
 **除了查询数据之外，ICP1106还能通过载波指令，得到ICP1205的各种状态
 **如充电状态
 ** \param [in]  功能参数
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205ComInit(void)
{
	uint8_t dat;
	dat=0x11;
	I2C_MasterWriteData(ICP1205_DVCID,0x1d,&dat,sizeof(dat)); 
	dat=0x10;
	I2C_MasterWriteData(ICP1205_DVCID,0x1d,&dat,sizeof(dat)); 
	dat=0x00;
	I2C_MasterWriteData(ICP1205_DVCID,0x1d,&dat,sizeof(dat)); 
			
}


/**
 *******************************************************************************
 ** \brief ICP1205清除所有中断状态
 **
 ** \param [in]  功能参数
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205ClearIntFlag(void)
{
	uint8_t u8dat[3]={0xff,0xff,0xff};

	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT1,u8dat,sizeof(u8dat));		
}

/**
 *******************************************************************************
 ** \brief ICP1205使能中断
 **
 ** \param [in]  功能参数
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205IntEnable(void)
{
	uint8_t u8dat;
	
	//I2C_MasterReadData(ICP1205_DVCID,ICP1205_INT_EN,&u8dat,1);
  u8dat =0X01;
  I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_EN,&u8dat,1);

}

/**
 *******************************************************************************
 ** \brief ICP1205关闭中断
 **
 ** \param [in]  功能参数
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205IntDisable(void)
{
	  uint8_t u8dat;
	
    u8dat =0;
    I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_EN,&u8dat,1);

}


/**
 *******************************************************************************
 ** \brief 进入Ship模式,控制位需要先清零，再写1才能进入
 **
 ** \param [in]  无
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205ShipReset(void)
{
	uint8_t dat;
	//add by guyu 20200821
	//for different MCU chips
	I2C_MasterReadData(ICP1205_DVCID,0X30,&dat,1);
	if(dat==0x00)dat=0x01;
	else dat=0x10;

	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_SHIP_CON,&dat,sizeof(dat));	
   
}



void Icp1205Shipint(void)
{
	uint8_t dat;
    dat=0x04;
	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_SHIP_CON,&dat,sizeof(dat));	
}

/**
 *******************************************************************************
 ** \brief 进入Ship模式,控制位需要先复位，再写1才能进入
 **
 ** \param [in]  无
 **
 ** \retval void            返回无
 ******************************************************************************/
void Icp1205ShipEnable(void)
{
	 uint8_t dat;	
     dat=	0x11;
	 I2C_MasterWriteData(ICP1205_DVCID,ICP1205_SHIP_CON,&dat,1);	
}



const struct HAL_IOMUX_PIN_FUNCTION_MAP PX318J_cfg_gpio[1] =
{

    {
        ICP1205_IIC_INT_GPIO, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
    },


};

void i2c_start(void)
{
#if 0
	hal_i2c_close(HAL_I2C_ID_1);	
	
	hal_gpio_pin_set_dir(ICP1205_IIC_SCK_GPIO, HAL_GPIO_DIR_OUT, 0);
	hal_gpio_pin_set_dir(ICP1205_IIC_SDK_GPIO, HAL_GPIO_DIR_OUT, 0);
	hal_gpio_pin_set_dir(ICP1205_IIC_INT_GPIO, HAL_GPIO_DIR_IN, 0);
	//hal_gpio_pin_set_dir(HAL_GPIO_PIN_P0_4, HAL_GPIO_DIR_OUT, 0);
	//hal_gpio_pin_clr(HAL_GPIO_PIN_P0_4);

	
	struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_iic[] = {
        {ICP1205_IIC_SCK_GPIO, HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {ICP1205_IIC_SDK_GPIO, HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {ICP1205_IIC_INT_GPIO, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
        //{HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };
    hal_iomux_init(pinmux_iic, ARRAY_SIZE(pinmux_iic));
	
	TRACE(3,"init i2c pin");

	
	
	struct HAL_I2C_CONFIG_T i2c_cfg;
	i2c_cfg.mode = HAL_I2C_API_MODE_TASK;
    i2c_cfg.use_dma  = 0;
    i2c_cfg.use_sync = 1;
	i2c_cfg.speed = 40000;
    i2c_cfg.as_master = 1;
    TRACE(3,"I2C initial result: %x", hal_i2c_open(HAL_I2C_ID_1, &i2c_cfg));
#endif

struct HAL_GPIO_I2C_CONFIG_T i2c_cfg={ICP1205_IIC_SCK_GPIO,ICP1205_IIC_SDK_GPIO,20,0};
hal_gpio_i2c_open(&i2c_cfg);

hal_iomux_init(PX318J_cfg_gpio, 1);
hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T) PX318J_cfg_gpio[0].pin, HAL_GPIO_DIR_IN, 1);

//PX318J_gpio_enable_irq(PX318J_IRQ_GPIO, HAL_GPIO_IRQ_POLARITY_LOW_FALLING);





	
}

/**
 *******************************************************************************
 ** \brief ICP1205默认初始化输出配置,注意充电的寄存器没有初始化
 **
 ** \param [in]  功能参数
 **
 ** \retval void            返回无
 ******************************************************************************/
 extern uint8_t data_testwrite1205;
void i2c_init(void)
{
	//TRACE(3,"%s",__func__);
	 TRACE(1,"%s",__func__);

//	hds_thread_init();

	hds_thread_init();
	i2c_start();


	
	//clear reg
	uint8_t u8Icp1205RstVl[16] = {0X23,0x42,0x00,0x99,0x00,0x00,0x00,0x00,0xf0,0xd4,0x03,0x07,0x00,0x00,0x00,0x00};
	uint8_t u8tmp;
	I2C_MasterWriteData(ICP1205_DVCID,0x00,u8Icp1205RstVl,16);	//charge enable

	//check I2C write
	u8Icp1205RstVl[0] = 0x11;
	I2C_MasterReadData(ICP1205_DVCID,0X00,u8Icp1205RstVl,16);
	for (uint8_t i=0;i<8;i++)
		//TRACE(3,"%x",u8Icp1205RstVl[i]);
	 TRACE(1,"%x",u8Icp1205RstVl[i]);

	//read charge status
	//TRACE(3,"charge status: %b",u8Icp1205RstVl[ICP1205_CHRG_STS2]);
	TRACE(3,"charge status: %d",u8Icp1205RstVl[ICP1205_CHRG_STS2]);

	if (u8Icp1205RstVl[ICP1205_CHRG_STS2]&CHRGSTS2_VIN_4P4_CMP)
	{
		//hds_status_set_chargemode(true);
	}
	else
	{	
		//if (u8Icp1205RstVl[ICP1205_CHRG_STS2]&CHRGSTS2_VIN_3P3_CMP)
			//hds_status_set_chargemode(true);
		//else
			//hds_status_set_chargemode(false);
	}

	//add by guyu 20200821


	u8tmp=0x11;
 	I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8tmp,1); //???GP1???
	u8tmp=0x10;
 	I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8tmp,1); //???GP1???
	u8tmp=0x00;
 	I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8tmp,1); //???GP1???

	
	u8tmp=0x34;
 	I2C_MasterWriteData(ICP1205_DVCID,0x12,&u8tmp,1); //???GP1???
	u8tmp=0x24;
 	I2C_MasterWriteData(ICP1205_DVCID,0x12,&u8tmp,1); //???GP1???

	u8tmp=0xF2;
 	I2C_MasterWriteData(ICP1205_DVCID,0x09,&u8tmp,1); //SHIP????????S

	//add by guyu 20200826
	u8tmp=0x00;
 	I2C_MasterWriteData(ICP1205_DVCID,0x10,&u8tmp,1); //??????
	
		
	Icp1205SetChrgFunEnable();
	Icp1205ComInit();
	Icp1205ClearIntFlag();
	Icp1205IntDisable();
	Icp1205ShipReset(); 
//	u8tmp=INT1_GP0CH_MSK | INT1_ANARDY_MSK |INT1_WDTOVTIME_MSK |INT1_BATLOW_MSK;
//	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK1,&u8tmp,1);
//	u8tmp=INT2_0P35CMP_MSK |INT2_CHRGSTS_CH_MSK | INT2_ITERM_MSK |INT2_CVCHRG_MSK |INT2_TRCKL_TIMEOUT_MSK;
//	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK2,&u8tmp,1);
//	u8tmp=INT1_GP0CH_MSK | INT1_ANARDY_MSK |INT1_WDTOVTIME_MSK |INT1_BATLOW_MSK;
//	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK1,&u8tmp,1);
//	u8tmp=INT2_0P35CMP_MSK |INT2_CHRGSTS_CH_MSK | INT2_ITERM_MSK |INT2_CVCHRG_MSK |INT2_TRCKL_TIMEOUT_MSK;
//	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK2,&u8tmp,1);
	
	//测试载波程序
	//u8tmp=0Xfc;//保留插入拔出中断
	u8tmp=0XfC;
	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK1,&u8tmp,1);
	
	u8tmp=0XFF;//保留中断完成、0.15中断
	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK2,&u8tmp,1);

	//edit by guyu 20200805
	u8tmp=0X1B;//保留透传、载波错误与成功中断
	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK3,&u8tmp,1);


    data_testwrite1205 = 0x13;
	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&data_testwrite1205,1);//回写


	//i2c_event_enableint(HAL_GPIO_PIN_P0_2);
	//Icp1205IntEnable();


/*
I2C_MasterReadData(ICP1205_DVCID,0X00,u8Icp1205RstVl,16);
for (uint8_t i=0;i<16;i++)
	TRACE("0x%x /n",u8Icp1205RstVl[i]);
*/

	
	

//	u8tmp=0;
//	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_MASK3,&u8tmp,1);
//#define INT1_PLGIN_MSK       		(0x1u<<0)       /*!< Vin插入中断标志*/
//#define INT1_PLGOUT_MSK         (0x1u<<1)       /*!< Vin拔出中断标志*/
//#define INT1_CHRGERR_MSK        (0x1u<<2)       /*!< 充电错误中断标志*/
//#define INT1_ANARDY_MSK         (0x1u<<3)       /*!< 模拟就绪中断标志*/
//#define INT1_WDTOVTIME_MSK      (0x1u<<4)       /*!< 看门狗中断标志*/
//#define INT1_BATLOW_MSK         (0x1u<<5)       /*!< 充电结束中断*/
//#define INT1_GP0CH_MSK          (0x1u<<6)       /*!< GP0端口变化中断标志*/
//#define INT1_CHRGFIN_MSK        (0x1u<<7)       /*!< 充电完成中断标志*/

//#define INT2_CHRGSTS_CH_MSK     (0x1u<<0)       /*!< 充电状态改变中断标志*/
//#define INT2_CCCV_TIMEOUT_MSK   (0x1u<<1)       /*!< CCCV充电超时中断*/
//#define INT2_TRCKL_TIMEOUT_MSK  (0x1u<<2)       /*!< 涓流充电超时中断标志*/
//#define INT2_NTC_MSK            (0x1u<<3)       /*!< 电池过温或者低温中断标志*/
//#define INT2_CVCHRG_MSK         (0x1u<<4)       /*!< 恒压向恒流充电转换中断标志*/
//#define INT2_0P15CMP_MSK        (0x1u<<5)       /*!< 0.15比较器变化中断标志*/
//#define INT2_0P35CMP_MSK        (0x1u<<6)       /*!< 0.35比较器变化中断标志*/
//#define INT2_ITERM_MSK          (0x1u<<7)       /*!< 充电电流ITERM中断标志*/

//#define INT3_KEYDN_MSK       		(0x1u<<0)       /*!< 按键按下标志*/
//#define INT3_KEYUP_MSK       		(0x1u<<1)       /*!<  按键抬起标志*/
//#define INT3_CRRXCMD_MSK       	(0x1u<<2)       /*!< 载波接收成功中断标志*/
//#define INT3_CRRXERR_MSK       	(0x1u<<3)       /*!< 载波接收错误中断标志*/
//#define INT3_TRCOM_MSK      		(0x1u<<4)       /*!< 透传模式中断*/



//Icp1205Shipint();




	
}


void hds_check_cmd_at_boot(void)
{
	uint8_t cmd[2];
	//I2C_MasterReadData(ICP1205_DVCID,0x0c,&cmd,sizeof(cmd));
	I2C_MasterReadData(ICP1205_DVCID,ICP1205_CRCOM_WORD_DAT,cmd,2);
	
	TRACE(3,"cmd at boot: %x %x",cmd[0],cmd[1]);

	TRACE(3,"cmd at boot-->box battery: %x",cmd[0]);
	box_battery = cmd[0];
	
	if ((cmd[1]&0x0f) == 0x6)
	{
		hds_status_set_cang_onoff(false);
		I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&cmd[1],1);
	}
	else if ((cmd[1]&0x0f) == 0x5)
	{
		hds_status_set_cang_onoff(true);
		I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&cmd[1],1);
	}
	if ((cmd[1]&0x0f) == 0x7)
	{
		hds_status_set_cang_inout(true);
		I2C_MasterWriteData(ICP1205_DVCID,ICP1205_CRCOM_TDAT,&cmd[1],1);
	}

	if ((cmd[1]&0x0f) == 0x9)
	{
	   TRACE(3,"...............charge full status: %d",cmd[1]);
           hds_status_set_charfull_status(true);
	}
}

void hds_start_i2c_int(void)
{
	hds_check_cmd_at_boot();//鎵撳紑涓柇鏃跺�欙紝妫�娴嬪紑鍏充粨鐘舵�?

	//i2c_event_enableint(ICP1205_IIC_INT_GPIO);

	//feedup in or out cang state for fastest outbox
	uint8_t u8tmp;
	I2C_MasterReadData(ICP1205_DVCID,ICP1205_CHRG_STS2,&u8tmp,1);
	TRACE(3,"charge status: %d",u8tmp);
	if ((u8tmp&CHRGSTS2_VIN_STS) == 0)
	{
		//hds_status_set_cang_onoff(false);
		hds_status_set_cang_inout(false);
	}
	else
	{
        hds_status_set_cang_inout(true);
	}

	I2C_MasterReadData(ICP1205_DVCID,ICP1205_CHRG_STS3,&u8tmp,1);
	TRACE(3,"charge status: %d",u8tmp);
	if ((u8tmp&CHRGSTS3_VIN2BAT_0P15CMP) != 0)
	{
		u8tmp=INT2_0P15CMP_FLG;					
		I2C_MasterWriteData(ICP1205_DVCID,ICP1205_INT_STAT2,&u8tmp,1);	
	}
	//i2c_event_enableint(HAL_GPIO_PIN_P0_2);
	//Icp1205IntEnable();

    //sing_dowload_init();

	//u8tmp=0x13;
	//I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);
    test_box_read();

	
}




void INT_1205_INIT(void)
{
return;
i2c_event_enableint(ICP1205_IIC_INT_GPIO);
Icp1205IntEnable();
i2c_int_flg = true;

}



void hds_trace_test(void)
{

uint8_t u8Icp1205RstVltest[100] = {0x00};


I2C_MasterReadData(ICP1205_DVCID,0X00,u8Icp1205RstVltest,40);

DUMP8(":0x%02x ", u8Icp1205RstVltest, 40);

//for (uint8_t i=0;i<40;i++)
//	TRACE("num=%d :0x%x",i,u8Icp1205RstVltest[i]);


}


void sing_dowload_init(void)
{
uint8_t u8tmp = 0;


u8tmp=0x11;
I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8tmp,1);
osDelay(1);
u8tmp=0x10;
I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8tmp,1);
osDelay(1);
u8tmp=0x0;
I2C_MasterWriteData(ICP1205_DVCID,0x1d,&u8tmp,1);
u8tmp=0x7;
I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);


u8tmp=0x7;
I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);

I2C_MasterReadData(ICP1205_DVCID,0x12,&u8tmp,1);

u8tmp = u8tmp|0x08;

I2C_MasterWriteData(ICP1205_DVCID,0x12,&u8tmp,1); //???GP1???


}


void ICP1205_COMM_COM_EN(void)
{
uint8_t u8tmp = 0x13;
I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);
}

void ICP1205_COMM_COM_DIS(void)
{
uint8_t u8tmp = 0x00;
I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);
}


void ICP1205_COMM_COM_READ(void)
{
uint8_t u8tmp = 0x13;
I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);
}


void ICP1205_COMM_COM_WRITE(void)
{
uint8_t u8tmp = 0x17;
I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);
}


void test_box()
{
 TRACE(1,"%s  i2c_int_flg = %d",__func__,i2c_int_flg);
 if(i2c_int_flg == true)
 hds_thread_msg_send(THREAD_MSG_INPUT_EVENT_I2CINT);

}


void test_box_send(void)
{
	uint8_t u8tmp = 0x17;
	I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);
}

void test_box_read(void)
{
  uint8_t u8tmp = 0x03;
  I2C_MasterWriteData(ICP1205_DVCID,ICP1205_COMM_CON,&u8tmp,1);
}



//end add by guyu 20200630
