/***************************************************************************************************************
 *File:   aw_i2c.c
 *Auther: duanqianshuai@awinic.com.cn
 *Copyright (c) Shanghai awinic technology Co.,Ltd. 2008-2019. All right reserved
 ***************************************************************************************************************/
#include "hal_i2c.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "string.h"


//extern I2C_HandleTypeDef hi2c1;

//clock = 72Hz
#if 0
void delay(unsigned int us){
    int i=10;
    while(us!=0){
        i=10;
        while(i!=0)
            i--;
        us--;
    }
}

uint8_t aw22xxx_i2c_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data)
{
    uint8_t res = 0;
    uint8_t cnt = 0;
    
    while (cnt < I2C_RETRY_TIMES) {
        res = HAL_I2C_Mem_Read(&hi2c1,i2c_addr<<1,reg_addr,I2C_MEMADD_SIZE_8BIT,reg_data,1,1000);
        if (res != I2C_OK) {
            //printf("%s: i2c_write cnt = %d, error = %d\r\n", __func__, cnt, res);
        } else {
            break;
        }
        cnt ++;
    }
    return res;
}
#endif

uint8_t aw22xxx_i2c_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data)
{
	hal_gpio_i2c_read_data(i2c_addr,reg_addr,1,reg_data,1);
	return 0;
}


uint8_t aw22xxx_i2c_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_data)
{
    int ret = -1;
    //uint8_t cnt = 0;
	ret = hal_gpio_i2c_simple_send2(i2c_addr, reg_addr, &reg_data, 1);
	return ret;
}



uint8_t aw22xxx_i2c_writes(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data,uint8_t data_num)
{
    int ret = -1;
    //uint8_t cnt = 0;
	ret = hal_gpio_i2c_simple_send2(i2c_addr, reg_addr, reg_data, data_num);
	return ret;

}



uint8_t aw22xxx_i2c_writes2(uint8_t i2c_addr, uint8_t reg_addr, const uint8_t *reg_data,uint8_t data_num)
{
    int ret = -1;
    //uint8_t cnt = 0;
	ret = hal_gpio_i2c_simple_send3(i2c_addr, reg_addr, reg_data, data_num);
	return ret;

}




#if 0
uint8_t aw22xxx_i2c_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_data)
{
    uint8_t res = 0;
    uint8_t cnt = 0;
    
    while (cnt < I2C_RETRY_TIMES) {
        res = HAL_I2C_Mem_Write(&hi2c1,i2c_addr<<1,reg_addr,I2C_MEMADD_SIZE_8BIT,&reg_data,1,1000);
        if (res != I2C_OK) {
            //printf("%s: i2c_write cnt = %d, error = %d\r\n", __func__, cnt, res);
        } else {
            break;
        }
        cnt ++;
    }
    return res;		
}


uint8_t aw22xxx_i2c_writes(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data,uint8_t data_num)
{
	  uint8_t res = 0;
    uint8_t cnt = 0;
    
    while (cnt < I2C_RETRY_TIMES) {
        res = HAL_I2C_Mem_Write(&hi2c1,i2c_addr<<1,reg_addr,I2C_MEMADD_SIZE_8BIT,reg_data,data_num,1000);
        if (res != I2C_OK) {
            //printf("%s: i2c_write cnt = %d, error = %d\r\n", __func__, cnt, res);
        } else {
            break;
        }
        cnt ++;
    }
    return res;	
}
#endif






