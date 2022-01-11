#ifndef __HDS_THREAD_H__
#define __HDS_THREAD_H__

#undef _EXT_

#ifdef __HDS_THREAD_C_
#define _EXT_ 
#else 
#define _EXT_ extern 
#endif 

#include "app_thread.h"



enum
{
    sysbtmode_unknow = 0,
	sysbtmode_bt,
    sysbtmode_anctest,
    sysbtmode_dut,
    sysbtmode_singdownload,
};

enum
{
    box_status_unknow = 0,
    box_status_in,
    box_status_out,
    box_status_off,
    box_status_open,
    box_status_power_off,
};



//define type of msg
//���������¼�
#define THREAD_MSG_INPUT_EVENT_INBOX     	0x00 //���
#define THREAD_MSG_INPUT_EVENT_OUTBOX     	0x01 //����
#define THREAD_MSG_INPUT_EVENT_WEARUP    	0x02 //���
#define THREAD_MSG_INPUT_EVENT_WEARDOWN    	0x03 //����
//add by guyu 20200629
#define THREAD_MSG_INPUT_EVENT_I2CINT		0x04

//���������¼�
#define THREAD_MSG_KEY_EVENT				0x05 //LOCAL KEY
#define THREAD_MSG_KEY_EVENT_PEER			0x06 //REMOTE KEY

#define THREAD_MSG_EARPHONE_BATTERY			0x07 //REMOTE KEY


//�����¼�
#define THREAD_MSG_BT_EVENT_TIMEOUT				0x20 //δ���������ĳ�ʱ����
#define THREAD_MSG_BT_EVENT_FREEMAN				0x22 //��������ʹ��
#define THREAD_MSG_BT_EVENT_MOBILE_CONNECTED	0x23 //�������ֻ�
#define THREAD_MSG_BT_EVENT_MOBILE_DISCONNECTED	0x24 //�Ͽ��ֻ�
#define THREAD_MSG_BT_EVENT_TWS_CONNECTED 		0x25 //�������
#define THREAD_MSG_BT_EVENT_TWS_DISCONNECTED 	0x26 //�Ͽ�����
#define THREAD_MSG_BT_EVENT_POWERON_WITHROLE 	0x27 //�����ӽ�ɫ����
#define THREAD_MSG_BT_EVENT_POWERON_WITHOUTROLE 0x28 //�����ӽ�ɫ����
#define THREAD_MSG_BT_EVENT_POWERON_TEST 		0x29 //����������
#define THREAD_MSG_BT_EVENT_POWEROFF			0x2A
//add by lrx 20200528
#define THREAD_MSG_BT_EVENT_POWERON_BTREADY            0x2B
//edit by guyu 20200612
#define THREAD_MSG_BT_EVENT_PAIRINGMODE_ENTER 		0x2C	//����pairingģʽ����Ϣ
//add by guyu 20200612
#define THREAD_MSG_BT_EVENT_MOBILE_CONNECTING 		0x2D	//��������ֻ�ģʽ����Ϣ
//add by guyu 20200630
#define THREAD_MSG_BT_EVENT_GAMEMODE_REPORT			0x2E	//������Ϸģʽ�л�����ʾ������Ϣ
//����ָ���¼�
#define THREAD_MSG_UART_CMD_CLOSECANG     	0x100   //0x5a
#define THREAD_MSG_UART_CMD_CLOSECANG2    	0x101   //0x5b
#define THREAD_MSG_UART_CMD_BATERY        	0X102   //0x3c
#define THREAD_MSG_UART_CMD_BTPAIR        	0X103   //0x2D
#define THREAD_MSG_UART_CMD_OPENCANG      	0X104   //0x69
#define THREAD_MSG_UART_CMD_RESET         	0X105   //0x4B
#define THREAD_MSG_UART_CMD_TEST          	0X106   //0xA2
#define THREAD_MSG_UART_CMD_UPGRADE			0X107	//0x78

#define THREAD_MSG_UART_CMD_ANC_TEST         	0X108   //0x4B
#define THREAD_MSG_UART_CMD_IPHONE_PAIRING         	0X109   //0x4B
#define THREAD_MSG_UART_EVENT_MOBILE_DISCONNECTED	0X10A //�Ͽ��ֻ�

//����¼�
#define THREAD_MSG_BATTRY_EVENT_LOWPOWER    0x200
#define THREAD_MSG_BATTRY_EVENT_POWEROFF    0x201
#define THREAD_MSG_BATTRY_EVENT_FULLCHARGE  0x202


#define THREAD_MSG_SHIP_MODE 0x300
#define THREAD_MSG_TWS_PAIRING_MODE 0xc4
#define THREAD_MSG_TWS_PAIRING_SUCEESFULL_MODE 0xc5


//add by guyu 20200702
#define THREAD_MSG_ENTER_SING_UART_MODE     0xc0
#define THREAD_MSG_LY_EVENT_ANSWER_BTADDR   0xc1
#define THREAD_MSG_LY_EVENT_GET_BTADDR   	0xc2
#define THREAD_MSG_LY_EVENT_FINISH_ANSWER	0xc3

typedef void (*msg_rcv_cb)(uint32_t param1,uint32_t param2);


typedef struct THREAD_CONFIG_TCB_S{
	uint32_t m_msg;
	msg_rcv_cb m_msg_handler;
}THREAD_CONFIG_TCB;

#ifdef __cplusplus
extern "C" {
#endif
_EXT_ int hds_thread_msg_handler(APP_MESSAGE_BODY * msg);
_EXT_ uint8_t hds_thead_msg_findid(uint32_t msgid);
_EXT_ uint32_t hds_get_delaytime(uint32_t start);
_EXT_ void hds_thread_msg_putwith_param(uint32_t msgid,uint32_t param1,uint32_t param2);
_EXT_ void hds_thread_msg_send(uint32_t msgid);
_EXT_ void hds_thread_init(void);
_EXT_ void hds_tread_exit();
_EXT_ void Entet_Singleline(void);
_EXT_ void communication_receive__callback(uint8_t *buf, uint8_t len);
_EXT_ void hds_delay_msg_add(uint8_t eventmode,uint16_t timedelay,bool enflg);
_EXT_ uint8_t box_status_test(void);


#ifdef __cplusplus
}
#endif

#endif
