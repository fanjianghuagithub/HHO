//#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include "aw22xxx_fw.h"
#include "aw22xxx_cfg.h"
#include "aw22xxx.h"
#include "aw_i2c.h"
#include "led.h"
#include "hal_timer.h"
#include "cmsis_os.h"
#include "hal_uart.h"
#include "hal_trace.h"



#if 0
#ifndef AW_MSDELAY
#define AW_MSDELAY(a)	\
	do {	\
		HAL_Delay(a);	\
	}while( 0 )
#endif
#endif

static void aw22xxx_hw_reset(void)
{
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);//U4
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);//U5
	//AW_MSDELAY(3);
	aw22127_led_en(true);
	osDelay(3);
}

static void aw22xxx_sw_reset(struct aw22xxx *aw22xxx)
{
	aw22xxx_i2c_write(aw22xxx->i2c_addr, REG_SRST, AW22XXX_SRSTW);
	//AW_MSDELAY(2);
	osDelay(2);
}

static int aw22xxx_read_chipid(struct aw22xxx *aw22xxx)
{
	unsigned char cnt = 0;
	unsigned char ret = 0;
	unsigned char reg_val = 0;
	while(cnt < AW_READ_CHIPID_RETRIES){
		aw22xxx_i2c_read(aw22xxx->i2c_addr, REG_CHIPID, &reg_val);
		//printf("%s: chip id is %d\n",__func__, reg_val");
	    
		if(reg_val == aw22xxx->chipid){
			ret = 1;
			break;
		}
		cnt++;
	}
	return ret;
}

static void aw22xxx_chip_enable(struct aw22xxx *aw22xxx,bool flag)
{
	unsigned char reg_val = 0;
	aw22xxx_i2c_read(aw22xxx->i2c_addr, REG_GCR, &reg_val);
	if (flag) {
		reg_val |= 0x01;
	} else {
		reg_val &= ~(0x01);
	}
	aw22xxx_i2c_write(aw22xxx->i2c_addr, REG_GCR, reg_val);
	/* dely 2 ms at least */
	//AW_MSDELAY(2);
	osDelay(2);
}

static void aw22xxx_mcu_enable(struct aw22xxx *aw22xxx,bool flag)
{
   
	unsigned char reg_val = 0;
	aw22xxx_i2c_read(aw22xxx->i2c_addr, REG_MCUCTR, &reg_val);
	if (flag) {
		reg_val |= 0x01;
	} else {
		reg_val &= ~(0x01);
	}
	aw22xxx_i2c_write(aw22xxx->i2c_addr, REG_MCUCTR, reg_val);
	
	/* delay 2 ms */
	//AW_MSDELAY(2);
	osDelay(2);
}

/*****************************************************
 *
 * aw22xxx firmware update
 *
 *****************************************************/
static int aw22xxx_container_update(struct aw22xxx *aw22xxx,
        struct aw22xxx_container *aw22xxx_fw)
{
    unsigned int i;
    unsigned char reg_val;
    unsigned int tmp_bist;
	unsigned int tmp_len;

    /* chip enable*/
    aw22xxx_sw_reset(aw22xxx);
    aw22xxx_chip_enable(aw22xxx,true);
    aw22xxx_mcu_enable(aw22xxx,true);

    /* flash cfg */
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x80, 0xec);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x35, 0x29);
    //aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x37, 0xba);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x38, 0x00);//aw22xxx_fw->key);

    /* flash erase*/
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x22, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x21, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x20, 0x03);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x03);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x23, 0x00);
    //AW_MSDELAY(40);
    osDelay(40);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x22, 0x40);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x21, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x02);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x23, 0x00);
    //AW_MSDELAY(6);
    osDelay(6);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x22, 0x42);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x21, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x02);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x23, 0x00);
    //AW_MSDELAY(6);
    osDelay(6);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x22, 0x44);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x21, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x02);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x23, 0x00);
    //AW_MSDELAY(6);
    osDelay(6);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x00);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x20, 0x00);

    /* flash write */
#if 1 //continuous multi byte write
    /* flash writes */
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x20, 0x03);
		for(i = 0; i < aw22xxx_fw->len; i+=tmp_len){
			aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x22, ((i>>8)&0xff));
			aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x21, ((i>>0)&0xff));
			aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x11, 0x01);
			aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x04);
			if((aw22xxx_fw->len - i) < MAX_FLASH_WRITE_BYTE_SIZE){
				tmp_len = aw22xxx_fw->len - i;
			}else{
				tmp_len = MAX_FLASH_WRITE_BYTE_SIZE;
			}
#if UPDATA_CODE_FRAMEWORK
			aw22xxx_i2c_writes2(aw22xxx->i2c_addr, 0x23, &aw22xxx_fw_code[i], tmp_len);
#else
			//aw22xxx_i2c_writes(aw22xxx->i2c_addr, 0x23, &aw22xxx_fw_code[40+i], tmp_len);
			aw22xxx_i2c_writes2(aw22xxx->i2c_addr, 0x23, &aw22xxx_fw_code[40+i], tmp_len);


#endif
			aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x11, 0x00);
			aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x00);
    }
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x20, 0x00);
#else//single byte write
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x20, 0x03);
    for(i=0; i<aw22xxx_fw->len; i++) {
        aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x22, ((i>>8)&0xff));
        aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x21, ((i>>0)&0xff));
        aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x04);
#if UPDATA_CODE_FRAMEWORK
			aw22xxx_i2c_writes(aw22xxx->i2c_addr, 0x23, &aw22xxx_fw_code[i], tmp_len);
#else
			aw22xxx_i2c_writes(aw22xxx->i2c_addr, 0x23, &aw22xxx_fw_code[40+i], tmp_len);
#endif
        aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x30, 0x00);
    }
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x20, 0x00);
#endif
    /* bist check */
    aw22xxx_sw_reset(aw22xxx);
    aw22xxx_chip_enable(aw22xxx, true);
    aw22xxx_mcu_enable(aw22xxx, true);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x22, (((aw22xxx_fw->len-1)>>8)&0xff));
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x21, (((aw22xxx_fw->len-1)>>0)&0xff));
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x24, 0x07);
    //AW_MSDELAY(5);
    osDelay(5);
    aw22xxx_i2c_read(aw22xxx->i2c_addr, 0x24, &reg_val);
    if(reg_val == 0x05) {
        aw22xxx_i2c_read(aw22xxx->i2c_addr, 0x25, &reg_val);
        tmp_bist = reg_val;
        aw22xxx_i2c_read(aw22xxx->i2c_addr, 0x26, &reg_val);
        tmp_bist |= (reg_val<<8);
    } 
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x24, 0x00);

    return 0;
}


static void aw22xxx_fw_update(struct aw22xxx *aw22xxx)
{
    struct aw22xxx_container *aw22xxx_fw;
    int ret = -1;
    unsigned char reg_val = 0;
    unsigned int tmp_bist = 0;

    /* get fw info */
    aw22xxx_fw = (struct aw22xxx_container *)malloc(sizeof(struct aw22xxx_container));
    if (!aw22xxx_fw) {

	//	TRACE(1,"....................get fw info return \n");
        return;
    }

    aw22xxx_fw->version = (aw22xxx_fw_code[27]<<24) | (aw22xxx_fw_code[28]<<16) |
            (aw22xxx_fw_code[29]<<8) | (aw22xxx_fw_code[30]<<0);
    aw22xxx_fw->bist = (aw22xxx_fw_code[34]<<8) | (aw22xxx_fw_code[35]<<0);
    aw22xxx_fw->key = aw22xxx_fw_code[36];
    aw22xxx_fw->len = (aw22xxx_fw_code[38]<<8) | (aw22xxx_fw_code[39]<<0);
    aw22xxx_fw->p_data = (unsigned char *)&aw22xxx_fw_code[40];


//	TRACE(1,"....................get fw info aw22xxx_fw->version = %d \n",aw22xxx_fw->version);

    aw22xxx_sw_reset(aw22xxx);
    aw22xxx_chip_enable(aw22xxx, true);
    aw22xxx_mcu_enable(aw22xxx, true);
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x22, (((aw22xxx_fw->len-1)>>8)&0xff));
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x21, (((aw22xxx_fw->len-1)>>0)&0xff));
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x24, 0x07);
    //AW_MSDELAY(5);
    osDelay(5);
	
		/* bist check */
    aw22xxx_i2c_read(aw22xxx->i2c_addr, 0x24, &reg_val);
    if(reg_val == 0x05) {
        aw22xxx_i2c_read(aw22xxx->i2c_addr, 0x25, &reg_val);
        tmp_bist = reg_val;
        aw22xxx_i2c_read(aw22xxx->i2c_addr, 0x26, &reg_val);
        tmp_bist |= (reg_val<<8);
    }
    aw22xxx_i2c_write(aw22xxx->i2c_addr, 0x24, 0x00);
    if(tmp_bist == aw22xxx_fw->bist) {
        aw22xxx->fw_flags = AW22XXX_FLAG_FW_OK;

          TRACE(1,".................... fw info old \n");
		
    } else if (aw22xxx->fw_update == 1) {
        /* fw update */
        ret = aw22xxx_container_update(aw22xxx, aw22xxx_fw);
        if(ret) {
            aw22xxx->fw_flags = AW22XXX_FLAG_FW_FAIL;
        } else {
            aw22xxx->fw_flags = AW22XXX_FLAG_FW_OK;
			TRACE(1,".................... fw info new \n");
        }
    }

    free(aw22xxx_fw);
}


void aw22xxx_cfg_update(struct aw22xxx *aw22xxx, const unsigned char *p_cfg_data, unsigned int cfg_size)
{
    unsigned int i = 0;

    for(i=0; i<cfg_size; i+=2) {
        aw22xxx_i2c_write(aw22xxx->i2c_addr, p_cfg_data[i], p_cfg_data[i+1]);
				if(i == 0){
					//AW_MSDELAY(25);
					osDelay(25);
				}else{
					//AW_MSDELAY(5);
					osDelay(5);
				}
    }
}


void aw22xxx_play(void)
{
    unsigned char reg_val = 0;
	unsigned char ret = 0;
	struct aw22xxx aw22xxx;
	
	aw22xxx.chipid = AW22127_CHIPID;
    aw22xxx.i2c_addr = 0x6a;
    aw22xxx.fw_update = 1;
    aw22xxx.fw_flags = AW22XXX_FLAG_FW_FAIL;
    aw22xxx.fw_version = 0;
	
	aw22xxx_hw_reset();
	
	ret = aw22xxx_read_chipid(&aw22xxx);

	
	if(ret <= 0){
		TRACE(1,"....................aw22xxx_read_chipid error \n");
		return;
	}
	
	aw22xxx_sw_reset(&aw22xxx);



	aw22xxx_i2c_read(aw22xxx.i2c_addr, REG_PST, &reg_val);

	TRACE(1,"....................REG_PST = %x \n",reg_val);


	
	#if 1
	aw22xxx_fw_update(&aw22xxx);
	if(aw22xxx.fw_flags != AW22XXX_FLAG_FW_OK){
		TRACE(1,"....................AW22XXX_FLAG_FW_OK error \n");
		return;
	}
	#endif
	
	
#if UPDATA_CODE_FRAMEWORK
	aw22xxx_cfg_update(&aw22xxx, factoryTestLedOnRed, sizeof(factoryTestLedOnRed));
	//AW_MSDELAY(500);
	osDelay(500);
	aw22xxx_cfg_update(&aw22xxx, factoryTestLedOnGreen, sizeof(factoryTestLedOnGreen));   
	//AW_MSDELAY(500);
	osDelay(500);
	aw22xxx_cfg_update(&aw22xxx, factoryTestLedOnBlue, sizeof(factoryTestLedOnBlue));
#else
//	aw22xxx_cfg_update(&aw22xxx, chip_init, sizeof(chip_init) );
//	aw22xxx_cfg_update(&aw22xxx, lamp1_breath, sizeof(lamp1_breath) );
	
	aw22xxx_cfg_update(&aw22xxx, init, sizeof(init) );
	//aw22xxx_cfg_update(&aw22xxx, breath, sizeof(breath) );
#endif

   TRACE(1,"....................AW22XXX__OK  \n");

}





