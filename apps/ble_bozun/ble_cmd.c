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
#include "app_datapath_server.h"
#include "app_ble_cmd_handler.h"
#include "ble_cmd.h"
#include "app_bt_stream.h"
#include "app_anc.h"
#include "led.h"
#include "app_tws_if.h"
#include "flash_opt.h"



#define LOG_MODULE HAL_TRACE_MODULE_APP


uint8_t Left_batteryvolume = 0;
uint8_t Right_batteryvolume = 0;
extern uint8_t box_battery;

void Ble_connect_battery_res(void);
extern bool flag_ble_had_connected;
u16 check_battery(uint16_t battery_val)
{
	u16 battery_value0[11]={3100,3300,3540,3620,3670,3720,3770,3840,3940,4040,4300}; 
	u8  i;
	u8 ret = 0;
	for(i=0; i<10; i++)
    {
           if((battery_val >= battery_value0[i]) & (battery_val <= battery_value0[i+1]))
           {
                ret = (battery_val-battery_value0[i])/((battery_value0[i+1]-battery_value0[i])/10)+(i*10);
				break;	
           }
    }
	if(battery_val<3100)
	{
        ret = 0;
	}
	if(battery_val >= 4170)
    {
          ret = 100;
    }
    TRACE(1,"battery_val = %d\n", battery_val);
    TRACE(1,"ret = %d\n", ret);

	if(app_tws_is_left_side())
	{
       Left_batteryvolume = ret;
	}
	else if(app_tws_is_right_side())
	{
       Right_batteryvolume = ret;
	}

    if(flag_ble_had_connected == true)
    {
       Ble_connect_battery_res();
	}
	
	return ret;
}




void Ble_connect_battery_res(void)
{
	u8 ble_send_data_buff[20]={0};
	ble_send_data_buff[0] = 0xFF;
	ble_send_data_buff[1] = 0x55;
	ble_send_data_buff[2] = 2;
	ble_send_data_buff[3] = 0x04;
	ble_send_data_buff[4] = ERAPHONE_REPLY_BATTERY_CMD;
	ble_send_data_buff[5] = box_battery;
	ble_send_data_buff[6] = Left_batteryvolume;
	ble_send_data_buff[7] = Right_batteryvolume;
	ble_send_data_buff[8] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5]+ ble_send_data_buff[6]+ ble_send_data_buff[7];
	app_datapath_server_send_data_via_notification(ble_send_data_buff, 9);	
}



void Ble_connect_static_res(uint8_t btstatus)
{
	u8 ble_send_data_buff[20]={0};

	ble_send_data_buff[0] = 0xFF;
	ble_send_data_buff[1] = 0x55;
	ble_send_data_buff[2] = 15;
	ble_send_data_buff[3] = 0x02;
	ble_send_data_buff[4] = ERAPHONE_REPLY_EARPHONECONNECT_CMD;
	ble_send_data_buff[5] = btstatus;
	ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
	app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);	
}



void led_static_res(void)
{
	u8 ble_send_data_buff[20]={0};
	ble_send_data_buff[0] = 0xFF;
	ble_send_data_buff[1] = 0x55;
	ble_send_data_buff[2] = 32;
	ble_send_data_buff[3] = 0x02;
	ble_send_data_buff[4] = ERAPHONE_REPLY_PARTS_LIST_CMD;
	ble_send_data_buff[5] = Accessoriestype_check();
	ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
	app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);
}


void earphone_inout_res(uint8_t leftinoutflg,uint8_t rightinoutflg)
{
	u8 ble_send_data_buff[20]={0};
	ble_send_data_buff[0] = 0xFF;
	ble_send_data_buff[1] = 0x55;
	ble_send_data_buff[2] = 32;
	ble_send_data_buff[3] = 0x03;
	ble_send_data_buff[4] = ERAPHONE_REPLY_EARPHONEINOUT_CMD;
	ble_send_data_buff[5] = leftinoutflg;
	ble_send_data_buff[6] = rightinoutflg;
	ble_send_data_buff[7] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5] + ble_send_data_buff[6];
	app_datapath_server_send_data_via_notification(ble_send_data_buff, 8);
}


extern bool earin_left_flg;
extern bool earin_right_flg;
uint8_t RGB_data_buff[30]={0};

BLE_CUSTOM_CMD_RET_STATUS_E BLE_custom_command_receive_data_bozun(uint8_t* ptrData, uint32_t dataLength)
{
	u8 ble_send_data_buff[30]={0};
	u8 i, checksum=0;
	TRACE(1,"Receive length %d data: ", dataLength);
	DUMP8("0x%02x ", ptrData, dataLength);
	if((ptrData[0] == 0xFF) && (ptrData[1] == 0x55))
	{
				for(i=3; i<dataLength-1; i++)
				{
						checksum += ptrData[i];
				}
				TRACE(1,"checksum is: 0x%02x", checksum);
		
					if(checksum == ptrData[dataLength-1])
					{
                       switch(ptrData[4])
                       {
                                case APP_INQUIRY_BATTERY_CMD:
								ble_send_data_buff[0] = 0xFF;
								ble_send_data_buff[1] = 0x55;
								ble_send_data_buff[2] = ptrData[2];
								
								ble_send_data_buff[3] = 0x04;
								ble_send_data_buff[4] = ERAPHONE_REPLY_BATTERY_CMD;
								ble_send_data_buff[5] = box_battery;
								ble_send_data_buff[6] = Left_batteryvolume;
								ble_send_data_buff[7] = Right_batteryvolume;
								ble_send_data_buff[8] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5]+ ble_send_data_buff[6]+ ble_send_data_buff[7];
								app_datapath_server_send_data_via_notification(ble_send_data_buff, 9);						   	
						        break;


							//	case APP_INQUIRY_THEMECOLORS_CMD:
							//		aw22127_theme_colors(ptrData[5]);
							//		break;

							//	case APP_INQUIRY_LEDONE_CMD:
							//		aw22127_oneled_colors(ptrData[5],ptrData[6],ptrData[7],ptrData[8]);
							//		break;

								//case APP_INQUIRY_LEDTWO_CMD:
								//	aw22127_twoled_colors(ptrData[5],ptrData[6],ptrData[7],ptrData[8],ptrData[9],ptrData[10],ptrData[11]);
								//	break;

								case APP_INQUIRY_LEDAUTO_CMD:
									aw22127_led2_colors(ptrData[5],ptrData[6],ptrData[7],ptrData[8],ptrData[9],ptrData[10],ptrData[11],ptrData[12],ptrData[13],ptrData[14],ptrData[15],ptrData[16],ptrData[17],ptrData[18],ptrData[19],0,0,0,ptrData[20],ptrData[21],true);
									break;

									
									
									


/***************************************************************************************************************************************/
								case APP_INQUIRY_RHYTHM_SET_CMD:
								aw22127_rhythm_colors(ptrData[5],true);	
								ble_send_data_buff[0] = 0xFF;
								ble_send_data_buff[1] = 0x55;
								ble_send_data_buff[2] = ptrData[2];
								ble_send_data_buff[3] = 0x02;
								ble_send_data_buff[4] = APP_INQUIRY_RHYTHM_CMD_RES;
								ble_send_data_buff[5] = ptrData[5];
								ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
								app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);
								break;


								case APP_INQUIRY_RHYTHM_CMD:
									ble_send_data_buff[0] = 0xFF;
									ble_send_data_buff[1] = 0x55;
									ble_send_data_buff[2] = ptrData[2];
									ble_send_data_buff[3] = 0x02;
									ble_send_data_buff[4] = APP_INQUIRY_RHYTHM_CMD_RES;
									ble_send_data_buff[5] = Rhythmtype_check();
									ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
									app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);	
								break;
								


								case APP_INQUIRY_LEDONOFF_SET_CMD:
									aw22127_led2_onoff(ptrData[5],true);
								break;	
									

								case APP_INQUIRY_LEDONOFF_CMD:
									ble_send_data_buff[0] = 0xFF;
									ble_send_data_buff[1] = 0x55;
									ble_send_data_buff[2] = ptrData[2];
									ble_send_data_buff[3] = 0x02;
									ble_send_data_buff[4] = APP_INQUIRY_LEDONOFF_CMD_RES;
									ble_send_data_buff[5] = ledonooff_check();
									ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
									app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);										
								break;


								case APP_INQUIRY_VOLUME_SET_CMD:
									//app_bt_set_volume_test(ptrData[5]);
									aw22127_volume_set(ptrData[5],true);
								break;	
									
								case APP_INQUIRY_VOLUME_CMD:
									ble_send_data_buff[0] = 0xFF;
									ble_send_data_buff[1] = 0x55;
									ble_send_data_buff[2] = ptrData[2];
									ble_send_data_buff[3] = 0x02;
									ble_send_data_buff[4] = APP_INQUIRY_VOLUME_CMD_RES;
									ble_send_data_buff[5] = volume_check();
									ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
									app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);										
								break;


								case APP_INQUIRY_ANC_SET_CMD:
									app_anc_set_mode(ptrData[5]);
									ble_send_data_buff[0] = 0xFF;
									ble_send_data_buff[1] = 0x55;
									ble_send_data_buff[2] = ptrData[2];
									ble_send_data_buff[3] = 0x02;
									ble_send_data_buff[4] = APP_INQUIRY_ANC_CMD_RES;
									ble_send_data_buff[5] = ptrData[5];
									ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
									app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);										
								break;

								case APP_INQUIRY_ANC_CMD:
									ble_send_data_buff[0] = 0xFF;
									ble_send_data_buff[1] = 0x55;
									ble_send_data_buff[2] = ptrData[2];
									ble_send_data_buff[3] = 0x02;
									ble_send_data_buff[4] = APP_INQUIRY_ANC_CMD_RES;
									ble_send_data_buff[5] = app_anc_get_mode();
									ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
									app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);										
								break;
									
								case APP_INQUIRY_PARTS_LIST_CMD:
										ble_send_data_buff[0] = 0xFF;
										ble_send_data_buff[1] = 0x55;
										ble_send_data_buff[2] = ptrData[2];
										ble_send_data_buff[3] = 0x02;
										ble_send_data_buff[4] = ERAPHONE_REPLY_PARTS_LIST_CMD;
										ble_send_data_buff[5] = Accessoriestype_check();
										ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
										app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);	
								break;

                                 case APP_INQUIRY_EARPHONEINOUT_CMD:
										ble_send_data_buff[0] = 0xFF;
										ble_send_data_buff[1] = 0x55;
										ble_send_data_buff[2] = ptrData[2];
										ble_send_data_buff[3] = 0x03;
										ble_send_data_buff[4] = ERAPHONE_REPLY_EARPHONEINOUT_CMD;
										ble_send_data_buff[5] = earin_left_flg;
										ble_send_data_buff[6] = earin_right_flg;
										ble_send_data_buff[7] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5] + ble_send_data_buff[6];
										app_datapath_server_send_data_via_notification(ble_send_data_buff, 8);										
	                             break;
								 
	
                                 case APP_INQUIRY_CLEAR_EARPHONELIST_CMD:
	                             app_bt_enter_pair();
	                             break;	



								case APP_INQUIRY_THEME_SETCOLORS_CMD:
									aw22127_theme_colors(ptrData[5],true);
										ble_send_data_buff[0] = 0xFF;
										ble_send_data_buff[1] = 0x55;
										ble_send_data_buff[2] = ptrData[2];
										ble_send_data_buff[3] = 0x02;
										ble_send_data_buff[4] = APP_INQUIRY_THEMECOLORS_CMD_RES;
										ble_send_data_buff[5] = ptrData[5];
										ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
										app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);										
								break;
									
									
                                case APP_INQUIRY_THEMECOLORS_CMD:
										ble_send_data_buff[0] = 0xFF;
										ble_send_data_buff[1] = 0x55;
										ble_send_data_buff[2] = ptrData[2];
										ble_send_data_buff[3] = 0x02;
										ble_send_data_buff[4] = APP_INQUIRY_THEMECOLORS_CMD_RES;
										ble_send_data_buff[5] = theme_colors_check();
										ble_send_data_buff[6] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5];
										app_datapath_server_send_data_via_notification(ble_send_data_buff, 7);										
								break;


                                case APP_INQUIRY_HHO_CMD_SET1:
									RGB_data_buff[0]= ptrData[5];
									RGB_data_buff[1]= ptrData[6];
									RGB_data_buff[2]= ptrData[7];
									RGB_data_buff[3]= ptrData[8];
									RGB_data_buff[4]= ptrData[9];
									RGB_data_buff[5]= ptrData[10];
									RGB_data_buff[6]= ptrData[11];
									RGB_data_buff[7]= ptrData[12];
									RGB_data_buff[8]= ptrData[13];
							   	break;
								
								
                                case APP_INQUIRY_HHO_CMD_SET2:
									RGB_data_buff[9]= ptrData[5];
									RGB_data_buff[10]= ptrData[6];
									RGB_data_buff[11]= ptrData[7];
									RGB_data_buff[12]= ptrData[8];
									RGB_data_buff[13]= ptrData[9];
									RGB_data_buff[14]= ptrData[10];

									RGB_data_buff[15]= 0;
									RGB_data_buff[16]= 0;
									RGB_data_buff[17]= 0;

									RGB_data_buff[18]= ptrData[11];
									RGB_data_buff[19]= ptrData[12];;
									aw22127_led2_colors(RGB_data_buff[0],RGB_data_buff[1],RGB_data_buff[2],RGB_data_buff[3],RGB_data_buff[4],RGB_data_buff[5],RGB_data_buff[6],RGB_data_buff[7],RGB_data_buff[8],RGB_data_buff[9],RGB_data_buff[10],RGB_data_buff[11],RGB_data_buff[12],RGB_data_buff[13],RGB_data_buff[14],RGB_data_buff[15],RGB_data_buff[16],RGB_data_buff[17],RGB_data_buff[18],RGB_data_buff[19],true);

									#if 0
                                    readdata_app[6] = RGB_data_buff[0];
									readdata_app[7] = RGB_data_buff[1];
									readdata_app[8] = RGB_data_buff[2];

                                    readdata_app[9] = RGB_data_buff[3];
									readdata_app[10] = RGB_data_buff[4];
									readdata_app[11] = RGB_data_buff[5];		


                                    readdata_app[12] = RGB_data_buff[6];
									readdata_app[13] = RGB_data_buff[7];
									readdata_app[14] = RGB_data_buff[8];	


								    readdata_app[15] = RGB_data_buff[9];
								    readdata_app[16] = RGB_data_buff[10];
								    readdata_app[17] = RGB_data_buff[11];	
									
                                    readdata_app[18] = RGB_data_buff[12];
									readdata_app[19] = RGB_data_buff[13];
									readdata_app[20] = RGB_data_buff[14];	

									readdata_app[1] = RGB_data_buff[19];
									readdata_app[5] = RGB_data_buff[18];
									#endif
								break;

								

                                /*
								case APP_INQUIRY_HHO_CMD_SET:
                                           aw22127_led2_colors(ptrData[5],ptrData[6],ptrData[7],ptrData[8],ptrData[9],ptrData[10],ptrData[11],ptrData[12],ptrData[13],ptrData[14],ptrData[15],ptrData[16],ptrData[17],ptrData[18],ptrData[19],0,0,0,ptrData[20],ptrData[21],true);
								break;
								*/




								case APP_INQUIRY_HHO_CMD_READ:
									//read_flash();
									ble_send_data_buff[0] = 0xFF;
									ble_send_data_buff[1] = 0x55;
									ble_send_data_buff[2] = ptrData[2];
									ble_send_data_buff[3] = 0x12;
									ble_send_data_buff[4] = APP_INQUIRY_HHO_CMD_RES;

                                    ble_send_data_buff[5] = readdata_app[6];
								    ble_send_data_buff[6] = readdata_app[7];
									ble_send_data_buff[7] = readdata_app[8];

                                    ble_send_data_buff[8] = readdata_app[9];
								    ble_send_data_buff[9] = readdata_app[10];
									ble_send_data_buff[10] = readdata_app[11];

                                    ble_send_data_buff[11] = readdata_app[12];
								    ble_send_data_buff[12] = readdata_app[13];
									ble_send_data_buff[13] = readdata_app[14];


                                    ble_send_data_buff[14] = readdata_app[15];
								    ble_send_data_buff[15] = readdata_app[16];
									ble_send_data_buff[16] = readdata_app[17];

                                    ble_send_data_buff[17] = readdata_app[18];
								    ble_send_data_buff[18] = readdata_app[19];
									ble_send_data_buff[19] = readdata_app[20];		


									ble_send_data_buff[20] = readdata_app[5];
								    ble_send_data_buff[21] = readdata_app[1];

									ble_send_data_buff[22] = 0;

									for(int i=3;i<22;i++)
									{
                                         ble_send_data_buff[22] = ble_send_data_buff[22]+ble_send_data_buff[i];
									}
									//ble_send_data_buff[5] = theme_colors_check();
									//ble_send_data_buff[6] = luminance_colors_check();
									//ble_send_data_buff[7] = ble_send_data_buff[3] + ble_send_data_buff[4] + ble_send_data_buff[5]+ ble_send_data_buff[6];
									app_datapath_server_send_data_via_notification(ble_send_data_buff, 23);	
									break;
/***************************************************************************************************************************************/

								
								default:break;
					   }
	
						
					}
				    else
					{
						TRACE(0,"checksum error!!!");
					}
	}

	return NO_ERROR;
}





