
//hds_sys_status_manage.h
#ifndef __HDS_SYS_STATUS_MSG_H__
#define __HDS_SYS_STATUS_MSG_H__

#undef _EXT_

#ifdef __HDS_SYS_STATUS_MSG_C_
#define _EXT_ 
#else 
#define _EXT_ extern 
#endif 


enum HDS_KEY_ID_T {
	HDS_KEY_ID_NONE,
    HDS_KEY_ID_DIALLOG_ANSWER,
    HDS_KEY_ID_DIALLOG_HUNGUP,
	HDS_KEY_ID_REDIAL,
	HDS_KEY_ID_THREEWAY,
	HDS_KEY_ID_PLAYPAUSE,
	HDS_KEY_ID_SIRI,
	HDS_KEY_ID_BACKWARD,
	HDS_KEY_ID_FORWARD,
	HDS_KEY_ID_VOLUMEUP,
	HDS_KEY_ID_VOLUMEDOWN,
	HDS_KEY_ID_BTPAIR,
	HDS_KEY_ID_BTCONN,
	HDS_KEY_ID_RESET,
	//add by guyu 20200608
	HDS_KEY_ID_GAMEMODE_SWITCH,
	
	HDS_KEY_ID_NUM,
};

typedef struct GPIO_KEY_CONFIG_S
{
	uint32_t left_key_inbox[8];	
	uint32_t left_key_outbox[8];
	uint32_t right_key_inbox[8];
	uint32_t right_key_outbox[8];
}GPIO_KEY_CONFIG;

#define POWER_OFF_CHARGEFULL_CHECK_TIME 10000
#define POWER_ON_READY_TIME  400
//add by guyu 20200604
//????2??????????
#define KEY_OUTCANG_READY_TIME  2000
//add by guyu 20200630
//???????MTU?
#define A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU_NORMAL 9
#define A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU_NORMAL 50
//???????MTU?
#define A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU_GAME 6
#define A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU_GAME 15
//add by guyu 20200615
//??????????????
#define KEY_POWER		0
#define KEY_GPIO_CHARGE	1

typedef struct SYS_STATUS_CELL_CTRL_S
{
	bool m_cang_onoff; //false:close true:open
	bool m_cang_inout; // false:in true:out 
	bool m_test_mode;  //flase:non-test true:testmode 
	bool m_charge_mode;//flase:plug-out true:plug-in
	bool m_bt_isready; //flase: not ready true:ready 
	bool m_need_reboot;
	bool m_force_poweroff;
	//add by guyu 20200608
	bool m_game_mode; //true:game mode false:normal mode
	uint8_t m_poweron_ready_flag; //0:power on is not ready 1:power on read
	uint8_t m_l_r_mode; //0:left 1:right
	uint32_t m_cang_off_time; //close cang time
	uint32_t m_power_on_time; //power on time
	uint32_t m_cang_out_time; //out cang time
	//add by guyu 20200608
	uint8_t m_aac_mtu_game; //AAC MTU in game mode
	uint8_t m_sbc_mtu_game; //SBC MTU in game mode
	//add by guyu 20200615
	uint8_t m_project;		//project code
	//add by guyu 20200709
	uint8_t m_peer_btaddr[6];
}SYS_STATUS_CELL;

#ifdef __cplusplus
extern "C" {
#endif
//bool status 
//false:close true:open 
_EXT_ void hds_status_set_cang_onoff(bool status);
_EXT_ bool hds_status_get_cang_onoff();

//flase:out  true:in 
_EXT_ void hds_status_set_cang_inout(bool status);
_EXT_ bool hds_status_get_cang_inout();



//flase:plug-out true:plug-in
_EXT_ void hds_status_set_chargemode(bool charge);
_EXT_ bool hds_status_get_chargemode();

//flase:non-test true:testmode 
_EXT_ void hds_status_set_testmode(bool mode);
_EXT_ bool hds_status_get_testmode(void);

//flase: not ready true:ready 
_EXT_ void hds_status_set_btready(bool ready);
_EXT_ bool hds_status_get_btready(void);

//flase: don't need reboot true:need reboot 
_EXT_ void hds_status_set_reboot(bool reboot);
_EXT_ bool hds_status_get_reboot(void);

//flase: don't need force power off true:need force power off 
_EXT_ void hds_status_set_force_poweroff(bool poweroff);
_EXT_ bool hds_status_get_force_poweroff(void);


//check charge full need power off
_EXT_ bool hds_status_check_charge_pwroff(void);
//true:ready  false:not ready 
_EXT_ bool hds_power_on_isready(void);
//add by guyu 20200604
//true: ready false:not ready
_EXT_ bool hds_status_key_isready(void);

//add by guyu 20200608
//true:game mode false:normal mode
_EXT_ void hds_status_set_game_mode(bool enable);
_EXT_ bool hds_status_get_game_mode(void);

//get mtu value in game mode
_EXT_ uint8_t hds_status_get_aac_mtu_game(void);
_EXT_ uint8_t hds_status_get_sbc_mtu_game(void);

//add by guyu 20200615
//set project code
_EXT_ void hds_status_set_project(uint8_t project);
//get project code
_EXT_ uint8_t hds_status_get_project(void);
_EXT_ void hds_status_set_charfull_status(bool status);
_EXT_ bool hds_status_get_charfull_status();

//add by guyu 20200709
//?????????
void hds_status_set_peer_btaddr(uint8_t *bt_addr);
//?????????
void hds_status_get_peer_btaddr(uint8_t *bt_addr);

_EXT_ void hds_status_init(void);

#ifdef __cplusplus
}
#endif

#endif