/***************************************************************************************************************
 *File:   aw_i2c.h
 *Auther: duanqianshuai@awinic.com.cn
 *Copyright (c) Shanghai awinic technology Co.,Ltd. 2008-2019. All right reserved
 ***************************************************************************************************************/
#ifndef __AW_I2C_H__
#define __AW_I2C_H__
#include "stdint.h"
//#include "aw_type.h"
#include "string.h"


#define I2C_RETRY_TIMES	                    0x03U
#define AW_RESET_VALUE                      0x55AAU

#if 0
#define LOBYTE(x) (x&0x00ff)
#define HIBYTE(x) ((x&0xff00)>>8)
#define MERGE_TWO_BYTE(x,y) ( (x<<8) | y)
#endif

typedef enum 
{
  I2C_OK       = 0x00U,
  I2C_ERROR    = 0x01U,
  I2C_BUSY     = 0x02U,
  I2C_TIMEOUT  = 0x03U
} AW_I2C_StatusTypeDef;

//void delay(unsigned int us);
uint8_t aw22xxx_i2c_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data);
uint8_t aw22xxx_i2c_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_data);
uint8_t aw22xxx_i2c_writes(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data,uint8_t data_num);
uint8_t aw22xxx_i2c_writes2(uint8_t i2c_addr, uint8_t reg_addr, const uint8_t *reg_data,uint8_t data_num);


#endif

