
#ifndef __APP_BOX_H__
#define __APP_BOX_H__
#undef _EXT_

#ifdef __HDS_COMM_LY_C__
#define _EXT_ 
#else 
#define _EXT_ extern 
#endif 

#ifdef __cplusplus
extern "C" {
#endif


#define ICP1205_DVCID 0x61

#define ICP1205_IIC_SCK_GPIO HAL_GPIO_PIN_P0_6
#define ICP1205_IIC_SDK_GPIO HAL_GPIO_PIN_P0_7
#define ICP1205_IIC_INT_GPIO HAL_GPIO_PIN_P2_0


_EXT_ void i2c_init(void);
_EXT_ void hds_check_cmd_at_boot(void);
_EXT_ void hds_start_i2c_int(void);
_EXT_ void Icp1205UpdataIntSts(void);
_EXT_ void Icp1205ShipReset(void);
_EXT_ void Icp1205ShipEnable(void);
_EXT_ void Icp1205IntEnable(void);
_EXT_ void Icp1205SetChrgFunDisable(void);
_EXT_ void Icp1205SetChrgFunEnable(void);
_EXT_ void Icp1205ClearIntFlag(void);
_EXT_ unsigned char I2C_MasterReadData(unsigned char add, unsigned char reg, unsigned char *data, unsigned char length);
_EXT_ void i2c_event_enableint(enum HAL_GPIO_PIN_T pin);
_EXT_ void INT_1205_INIT(void);
_EXT_ void test1205_2(void);
_EXT_ void sing_dowload_init(void);
_EXT_ void hds_trace_test(void);
_EXT_ void test_event_irq(void);
_EXT_ void test_box();
_EXT_ void ICP1205_COMM_COM_EN(void);
_EXT_ void ICP1205_COMM_COM_DIS(void);
_EXT_ void ICP1205_COMM_COM_READ(void);
_EXT_ void ICP1205_COMM_COM_WRITE(void);
_EXT_ void test_box_send(void);
_EXT_ void test_box_read(void);



///////////////////////////////////////////
//       ICP1205
///////////////////////////////////////////
#define      ICP1205_CHRG_CON1                    0x0000
#define      ICP1205_CHRG_CON2                    0x0001
#define      ICP1205_CHRG_CON3                    0x0002
#define      ICP1205_CHRG_CON4                    0x0003
#define      ICP1205_CHRG_STS1                    0x0004
#define      ICP1205_CHRG_STS2                    0x0005
#define      ICP1205_CHRG_STS3                    0x0006
#define      ICP1205_CHRG_STS4                    0x0007
#define      ICP1205_WDT_CON                      0x0008
#define      ICP1205_SHIP_CON                     0x0009
#define      ICP1205_CRCOM_CON1                   0x000a
#define      ICP1205_CRCOM_CON2                   0x000b
#define      ICP1205_CRCOM_CHR_DAT                0x000c
#define      ICP1205_CRCOM_WORD_DAT                0x000d
#define      ICP1205_CRCOM_TDAT                   0x000f
#define      ICP1205_COMM_CON                     0x0010
#define      ICP1205_GPIO_CON                       0x0011
#define      ICP1205_REV_DAT1                     0x0014
#define      ICP1205_REV_DAT2                     0x0015
#define      ICP1205_REV_DAT3                     0x0016
#define      ICP1205_REV_DAT4                     0x0017
#define      ICP1205_INT_EN                       0x0020
#define      ICP1205_INT_STAT1                    0x0021
#define      ICP1205_INT_STAT2                    0x0022
#define      ICP1205_INT_STAT3                    0x0023
#define      ICP1205_INT_MASK1                    0x0024
#define      ICP1205_INT_MASK2                    0x0025
#define      ICP1205_INT_MASK3                    0x0026
#define      ICP1205_CHIP_INFO                    0x0030

//////////////////////////////////////////////////////////////////

//#define      ICP1205_DVCID                    0xC2


#define CHRGCON1_CHRG_EN        (0x1u)           /*!< 充电使能 */
#define CHRGCON1_RECHRG_EN      (0x2u)           /*!< 重新充电使能 */
#define CHRGCON1_NTC_EN         (0x4u)           /*!< NTC使能 */
#define CHRGCON1_ITERM_SEL      (0x8u)           /*!< 充电截止比例选择。0:1/10；   1:1/5 */



#define CHRGSTS1_CHRGSTS_BITS         (0x7u)         /*!< 充电状态：0空闲/2涓流模式/3快充模式/4慢充模式/5充电延时/6充满/7错误状态*/
#define CHRGSTS1_CHRG_IDLE_STS       (0x0u)         /*!< 充电状态：0空闲*/
#define CHRGSTS1_CHRG_TRCKL1_STS     (0x1u)         /*!< 充电状态：2涓流模式*/
#define CHRGSTS1_CHRG_TRCKL_STS      (0x2u)         /*!< 充电状态：2涓流模式*/
#define CHRGSTS1_CHRG_FAST_STS       (0x3u)         /*!< 充电状态：3快充模式态*/
#define CHRGSTS1_CHRG_NOR_STS        (0x4u)         /*!< 充电状态：4慢充模式*/
#define CHRGSTS1_CHRG_DLYTIME_STS    (0x5u)         /*!< 充电状态：5充电延时*/
#define CHRGSTS1_CHRG_FINSH_STS      (0x6u)         /*!< 充电状态：6充满/7错误状态*/
#define CHRGSTS1_CHRG_ERR_STS        (0x7u)             /*!< 7错误状态*/

#define CHRGSTS1_ERR_STS          (0x1u<<3)         /*!< 充电错误*/
#define CHRGSTS1_ANA_CHRGSTS      (0x1u<<4)         /*!< 模拟电路实际的充电状态 */
#define CHRGSTS1_ANA_RDY_STS      (0x1u<<5)        /*!< 模拟电路就绪 */
#define CHRGSTS1_ANA_CHRGCTRL_SG  (0x1u<<6)        /*!< 模拟充电控制信号状态  */
#define CHRGSTS1_RECHRG_CMP_STS   (0x1u<<7)         /*!< 再充电比较器结果  */

#define CHRGSTS2_VIN_STS         (0x1u)           /*!< 0：Vin无电；1：Vin>1.5V有电 ，有100mS防抖*/
#define CHRGSTS2_VIN_1P0_CMP     (0x1u<<1)        /*!< 输入1.0V比较器结果 0:小于；1:大于*/
#define CHRGSTS2_VIN_3P3_CMP     (0x1u<<2)        /*!< 输入3.3V比较器结果 0:小于；1:大于 */
#define CHRGSTS2_VIN_4P4_CMP     (0x1u<<3)        /*!< 输入4.4V比较器结果 0:小于；1:大于 */
#define CHRGSTS2_BAT_3P0_CMP     (0x1u<<4)        /*!< 电池3.3V比较器结果 0:小于；1:大于*/
#define CHRGSTS2_SHORT_STS       (0x1u<<5)        /*!< 输出短路状态 0:正常; 1:短路 */
#define CHRGSTS2_VIN_OVP_STS     (0x1u<<6)        /*!< 输入过压（大于6.0V） 0:正常; 1:短路 */
#define CHRGSTS2_VIN_UVLO_STS    (0x1u<<7)        /*!< 输入欠压（小于3.3V） 0:正常; 1:短路 */

#define CHRGSTS3_ITERM_STS       (0x1u)           /*!< 截止电流状态 0:未截止;1:截止 */
#define CHRGSTS3_VIN2BAT_0P15CMP (0x1u<<1)        /*!< 充电压差0p15伏比较器 0:小于；1:大于*/
#define CHRGSTS3_VIN2BAT_0P35CMP (0x1u<<2)        /*!< 充电压差0p35伏比较器 0:小于；1:大于 */
#define CHRGSTS3_CV_STS          (0x1u<<3)        /*!< 充电恒压状态 0:其他充电状态；1:恒压充电状态（可快充）*/
#define CHRGSTS3_TRICKLE_STS     (0x1u<<4)        /*!< 充电涓流状态 0:其他充电状态；1:涓流充电状态 */
#define CHRGSTS3_NTC_STS         (0x1u<<5)        /*!< NTC状态 */
#define CHRGSTS3_NTC_L_STS       (0x1u<<6)        /*!< 低温NTC状态 */
#define CHRGSTS3_NTC_H_STS       (0x1u<<7)        /*!< 高温NTC状态 */


#define CRCOMCON2_CHR_DATA_UPDT_F   (0x1u)        /*!< 载波接收char数据寄存器更新标志，0:没有更新；1:更新*/  
#define CRCOMCON2_INT_DATA_UPDT_F   (0x1u<<1)     /*!< 载波接收整型数据寄存器更新标志，0:没有更新；1:更新*/  


#define COMMCON_BUSY_STS       	(0x1u<<4)       /*!< 透传模式标志 0:非透传；1:透传*/

#define GPIOCON_GP0_STS       	(0x1u<<0)       /*!< GPIO0状态 可通过I2C读写，也可以通过透传指令控制*/
#define GPIOCON_KEY_STS       	(0x1u<<1)       /*!< 按键状态 仅可读*/

#define INT1_PLGIN_FLG       		(0x1u<<0)       /*!< Vin插入中断标志*/
#define INT1_PLGOUT_FLG         (0x1u<<1)       /*!< Vin拔出中断标志*/
#define INT1_CHRGERR_FLG        (0x1u<<2)       /*!< 充电错误中断标志*/
#define INT1_ANARDY_FLG         (0x1u<<3)       /*!< 模拟就绪中断标志*/
#define INT1_WDTOVTIME_FLG      (0x1u<<4)       /*!< 看门狗中断标志*/
#define INT1_BATLOW_FLG         (0x1u<<5)       /*!< 充电结束中断*/
#define INT1_GP0CH_FLG          (0x1u<<6)       /*!< GP0端口变化中断标志*/
#define INT1_CHRGFIN_FLG        (0x1u<<7)       /*!< 充电完成中断标志*/

#define INT2_CHRGSTS_CH_FLG     (0x1u<<0)       /*!< 充电状态改变中断标志*/
#define INT2_CCCV_TIMEOUT_FLG   (0x1u<<1)       /*!< CCCV充电超时中断*/
#define INT2_TRCKL_TIMEOUT_FLG  (0x1u<<2)       /*!< 涓流充电超时中断标志*/
#define INT2_NTC_FLG            (0x1u<<3)       /*!< 电池过温或者低温中断标志*/
#define INT2_CVCHRG_FLG         (0x1u<<4)       /*!< 恒压向恒流充电转换中断标志*/
#define INT2_0P15CMP_FLG        (0x1u<<5)       /*!< 0.15比较器变化中断标志*/
#define INT2_0P35CMP_FLG        (0x1u<<6)       /*!< 0.35比较器变化中断标志*/
#define INT2_ITERM_FLG          (0x1u<<7)       /*!< 充电电流ITERM中断标志*/

#define INT3_KEYDN_FLG       		(0x1u<<0)       /*!< 按键按下标志*/
#define INT3_KEYUP_FLG       		(0x1u<<1)       /*!<  按键抬起标志*/
#define INT3_CRRXCMD_FLG       	(0x1u<<2)       /*!< 载波接收成功中断标志*/
#define INT3_CRRXERR_FLG       	(0x1u<<3)       /*!< 载波接收错误中断标志*/
#define INT3_TRCOM_FLG      		(0x1u<<4)       /*!< 透传模式中断*/


#define INT1_PLGIN_MSK       		(0x1u<<0)       /*!< Vin插入中断标志*/
#define INT1_PLGOUT_MSK         (0x1u<<1)       /*!< Vin拔出中断标志*/
#define INT1_CHRGERR_MSK        (0x1u<<2)       /*!< 充电错误中断标志*/
#define INT1_ANARDY_MSK         (0x1u<<3)       /*!< 模拟就绪中断标志*/
#define INT1_WDTOVTIME_MSK      (0x1u<<4)       /*!< 看门狗中断标志*/
#define INT1_BATLOW_MSK         (0x1u<<5)       /*!< 充电结束中断*/
#define INT1_GP0CH_MSK          (0x1u<<6)       /*!< GP0端口变化中断标志*/
#define INT1_CHRGFIN_MSK        (0x1u<<7)       /*!< 充电完成中断标志*/

#define INT2_CHRGSTS_CH_MSK     (0x1u<<0)       /*!< 充电状态改变中断标志*/
#define INT2_CCCV_TIMEOUT_MSK   (0x1u<<1)       /*!< CCCV充电超时中断*/
#define INT2_TRCKL_TIMEOUT_MSK  (0x1u<<2)       /*!< 涓流充电超时中断标志*/
#define INT2_NTC_MSK            (0x1u<<3)       /*!< 电池过温或者低温中断标志*/
#define INT2_CVCHRG_MSK         (0x1u<<4)       /*!< 恒压向恒流充电转换中断标志*/
#define INT2_0P15CMP_MSK        (0x1u<<5)       /*!< 0.15比较器变化中断标志*/
#define INT2_0P35CMP_MSK        (0x1u<<6)       /*!< 0.35比较器变化中断标志*/
#define INT2_ITERM_MSK          (0x1u<<7)       /*!< 充电电流ITERM中断标志*/

#define INT3_KEYDN_MSK       		(0x1u<<0)       /*!< 按键按下标志*/
#define INT3_KEYUP_MSK       		(0x1u<<1)       /*!<  按键抬起标志*/
#define INT3_CRRXCMD_MSK       	(0x1u<<2)       /*!< 载波接收成功中断标志*/
#define INT3_CRRXERR_MSK       	(0x1u<<3)       /*!< 载波接收错误中断标志*/
#define INT3_TRCOM_MSK      		(0x1u<<4)       /*!< 透传模式中断*/


//载波命令
#define CR_REC_CMD_QUE_CHRG_STS   (0x08<<4)      /*!< 载波接收到查询充电状态命令，无需回复*/
#define CR_REC_CMD_READ_DATA      (0x09<<4)      /*!< 载波接收到查询数据寄存器数据，可以更新新的数据*/
#define CR_REC_CMD_FRC_CHRG_FNSH  (0xAu<<4)      /*!< 载波强制充电完成*/
#define CR_REC_CMD_SHIP_MOD       (0xBu<<4)      /*!< 载波强制运输模式*/
#define CR_REC_CMD_CHAR_DATA      (0xCu<<4)      /*!< 载波发送1字节数据*/
#define CR_REC_CMD_WORD_DATA       (0xDu<<4)      /*!< 载波发送2字节数据*/
#define CR_REC_CMD_CLR_GP0        (0xEu<<4)      /*!< 载波强制GP0=0*/
#define CR_REC_CMD_SET_GP0        (0xFu<<4)      /*!< 载波强制GP0=1*/

//void hds_trace_test(void);



#ifdef __cplusplus
}
#endif

#endif
