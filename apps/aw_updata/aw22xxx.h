#ifndef __AW22XXX_H__
#define __AW22XXX_H__

#define AW221XX_SRSTR       0x76
#define AW22XXX_SRSTW       0x55
#define AW22118_CHIPID      0x18
#define AW22127_CHIPID      0x27
#define AW_READ_CHIPID_RETRIES	5
#define AW22XXX_FLASH_I2C_WRITES 1
#define MAX_FLASH_WRITE_BYTE_SIZE   128

/******************************************************
 *
 * type
 *
 *****************************************************/
#ifndef bool
typedef unsigned char bool;
#endif

#ifndef true
#define true  (1)
#endif

#ifndef false
#define false (0)
#endif

/******************************************************
 *
 * Register List
 *
 *****************************************************/
#define REG_CHIPID      0x00
#define REG_SRST        0x01
#define REG_GCR         0x02
#define REG_CLKCTR      0x03
#define REG_MCUCTR      0x04
#define REG_TASK0       0x05
#define REG_TASK1       0x06
#define REG_PST         0x07
#define REG_INTCFG      0x08
#define REG_INTEN       0x09
#define REG_INTST       0x0a
#define REG_IMAX        0x0b
#define REG_AUDCTR      0x0c
#define REG_IGAIN       0x0d
#define REG_GAIN        0x0e
#define REG_UVLO        0x0f
#define REG_UVLOTHR     0x10
#define REG_DBGCTR      0x20
#define REG_ADDR1       0x21
#define REG_ADDR2       0x22
#define REG_DATA        0x23
#define REG_BSTCTR      0x24
#define REG_FLSBSTD0    0x25
#define REG_FLSBSTD1    0x26
#define REG_FLSCTR      0x30
#define REG_FLSCFG1     0x31
#define REG_FLSCFG2     0x32
#define REG_FLSCFG3     0x33
#define REG_FLSCFG4     0x34
#define REG_FLSWP1      0x35
#define REG_FLSWP2      0x36
#define REG_FLSRP       0x37
#define REG_DECCFG      0x38
#define REG_FLSWP3      0x39
#define REG_PAGE        0xff
/******************************************************
 *
 * Register Write/Read Access
 *
 *****************************************************/
#define REG_NONE_ACCESS                 0
#define REG_RD_ACCESS                   1 << 0
#define REG_WR_ACCESS                   1 << 1
#define AW221XX_REG_MAX                 0x100

/******************************************************
 *
 * Register Detail
 *
 *****************************************************/
#define BIT_GCR_ADPDN_MASK              (~(1<<7))
#define BIT_GCR_ADPDN_DISABLE           (1<<7)
#define BIT_GCR_ADPDN_ENABLE            (0<<7)
#define BIT_GCR_ENPDN_MASK              (~(1<<6))
#define BIT_GCR_ENPDN_DISABLE           (1<<6)
#define BIT_GCR_ENPDN_ENABLE            (0<<6)
#define BIT_GCR_OTMD_MASK               (~(1<<5))
#define BIT_GCR_OTMD_DISABLE            (1<<5)
#define BIT_GCR_OTMD_ENABLE             (0<<5)
#define BIT_GCR_OTPD_MASK               (~(1<<4))
#define BIT_GCR_OTPD_DISABLE            (1<<4)
#define BIT_GCR_OTPD_ENABLE             (0<<4)
#define BIT_GCR_UVLOME_MASK             (~(1<<3))
#define BIT_GCR_UVLOME_DISABLE          (1<<3)
#define BIT_GCR_UVLOME_ENABLE           (0<<3)
#define BIT_GCR_UVLOEN_MASK             (~(1<<2))
#define BIT_GCR_UVLOEN_ENABLE           (1<<2)
#define BIT_GCR_UVLOEN_DISABLE          (0<<2)
#define BIT_GCR_OSCDIS_MASK             (~(1<<1))
#define BIT_GCR_OSCDIS_DISABLE          (1<<1)
#define BIT_GCR_OSCDIS_ENABLE           (0<<1)
#define BIT_GCR_CHIPEN_MASK             (~(1<<0))
#define BIT_GCR_CHIPEN_ENABLE           (1<<0)
#define BIT_GCR_CHIPEN_DISABLE          (0<<0)

#define BIT_CLKCTR_LOCS                 (1<<7)
#define BIT_CLKCTR_LOCS_DETECTED        (1<<7)
#define BIT_CLKCTR_LOCS_CLKEXIST        (0<<7)
#define BIT_CLKCTR_LOCPD_MASK           (~(1<<6))
#define BIT_CLKCTR_LOCPD_DISABLE        (1<<6)
#define BIT_CLKCTR_LOCPD_ENABLE         (0<<6)
#define BIT_CLKCTR_CLKSEL_MASK          (~(3<<4))
#define BIT_CLKCTR_CLKSEL_EXTCLK        (3<<4)
#define BIT_CLKCTR_CLKSEL_INTCLK_OP     (1<<4)
#define BIT_CLKCTR_CLKSEL_INTCLK_HZ     (0<<4)
#define BIT_CLKCTR_FREQ_MASK            (~(15<<0))
#define BIT_CLKCTR_FREQ_1MHZ            (6<<0)
#define BIT_CLKCTR_FREQ_2MHZ            (5<<0)
#define BIT_CLKCTR_FREQ_4MHZ            (4<<0)
#define BIT_CLKCTR_FREQ_6MHZ            (3<<0)
#define BIT_CLKCTR_FREQ_8MHZ            (2<<0)
#define BIT_CLKCTR_FREQ_12MHZ           (1<<0)
#define BIT_CLKCTR_FREQ_24MHZ           (0<<0)


#define BIT_MCUCTR_MCU_WAKEUP_MASK      (~(1<<2))
#define BIT_MCUCTR_MCU_WAKEUP_ENABLE    (1<<2)
#define BIT_MCUCTR_MCU_WAKEUP_DISABLE   (0<<2)
#define BIT_MCUCTR_MCU_RESET_MASK       (~(1<<1))
#define BIT_MCUCTR_MCU_RESET_DISABLE    (1<<1)
#define BIT_MCUCTR_MCU_RESET_ENABLE     (0<<1)
#define BIT_MCUCTR_MCU_WORK_MASK        (~(1<<0))
#define BIT_MCUCTR_MCU_WORK_ENABLE      (1<<0)
#define BIT_MCUCTR_MCU_WORK_DISABLE     (0<<0)

#define BIT_INTCFG_INTWTH_MASK          (~(127<<1))
#define BIT_INTCFG_INTMD_MASK           (~(1<<0))
#define BIT_INTCFG_INTMD_NEG_PULSE      (1<<0)
#define BIT_INTCFG_INTMD_LOW_LEVEL      (0<<0)

#define BIT_INTEN_LOC_MASK              (~(1<<7))
#define BIT_INTEN_LOC_ENABLE            (1<<7)
#define BIT_INTEN_LOC_DISABLE           (0<<7)
#define BIT_INTEN_UVLO_MASK             (~(1<<6))
#define BIT_INTEN_UVLO_ENABLE           (1<<6)
#define BIT_INTEN_UVLO_DISABLE          (0<<6)
#define BIT_INTEN_OT_MASK               (~(1<<5))
#define BIT_INTEN_OT_ENABLE             (1<<5)
#define BIT_INTEN_OT_DISABLE            (0<<5)
#define BIT_INTEN_WTD_MASK              (~(1<<4))
#define BIT_INTEN_WTD_ENABLE            (1<<4)
#define BIT_INTEN_WTD_DISABLE           (0<<4)
#define BIT_INTEN_FWVER_MASK            (~(1<<3))
#define BIT_INTEN_FWVER_ENABLE          (1<<3)
#define BIT_INTEN_FWVER_DISABLE         (0<<3)
#define BIT_INTEN_FLASH_MASK            (~(1<<2))
#define BIT_INTEN_FLASH_ENABLE          (1<<2)
#define BIT_INTEN_FLASH_DISABLE         (0<<2)
#define BIT_INTEN_MCUCHK_MASK           (~(1<<1))
#define BIT_INTEN_MCUCHK_ENABLE         (1<<1)
#define BIT_INTEN_MCUCHK_DISABLE        (0<<1)
#define BIT_INTEN_FUNCMPE_MASK          (~(1<<0))
#define BIT_INTEN_FUNCMPE_ENABLE        (1<<0)
#define BIT_INTEN_FUNCMPE_DISABLE       (0<<0)

#define BIT_INTST_LOC                   (1<<7)
#define BIT_INTST_UVLO                  (1<<6)
#define BIT_INTST_OT                    (1<<5)
#define BIT_INTST_WTD                   (1<<4)
#define BIT_INTST_FWVER                 (1<<3)
#define BIT_INTST_FLASH                 (1<<2)
#define BIT_INTST_MCUCHK                (1<<1)
#define BIT_INTST_FUNCMPE               (1<<0)

#define BIT_IMAX_MASK                   (~(15<<0))
#define BIT_IMAX_75mA                   ( 7<<0)
#define BIT_IMAX_60mA                   ( 6<<0)
#define BIT_IMAX_45mA                   ( 5<<0)
#define BIT_IMAX_40mA                   (14<<0)
#define BIT_IMAX_30mA                   ( 4<<0)
#define BIT_IMAX_20mA                   (12<<0)
#define BIT_IMAX_15mA                   ( 3<<0)
#define BIT_IMAX_10mA                   (11<<0)
#define BIT_IMAX_9mA                    ( 2<<0)
#define BIT_IMAX_6mA                    ( 1<<0)
#define BIT_IMAX_4mA                    ( 9<<0)
#define BIT_IMAX_3mA                    ( 0<<0)
#define BIT_IMAX_2mA                    ( 8<<0)

#define BIT_AUDCTR_PRCHG_MASK           (~(1<<3))
#define BIT_AUDCTR_PRCHG_ENABLE         (1<<3)
#define BIT_AUDCTR_PRCHG_DISABLE        (0<<3)
#define BIT_AUDCTR_PGABP_MASK			(~(1<<2))
#define BIT_AUDCTR_PGABP_ENABEL			(1<<2)
#define BIT_AUDCTR_PGABP_DISABEL		(0<<2)
#define BIT_AUDCTR_AGCEN_MASK           (~(1<<1))
#define BIT_AUDCTR_AGCEN_ENABLE         (1<<1)
#define BIT_AUDCTR_AGCEN_DISABLE        (0<<1)
#define BIT_AUDCTR_AUDEN_MASK           (~(1<<0))
#define BIT_AUDCTR_AUDEN_ENABLE         (1<<0)
#define BIT_AUDCTR_AUDEN_DISABLE        (0<<0)

#define BIT_DBGCTR_MODE_MASK            (~(3<<0))
#define BIT_DBGCTR_FLASH_MODE           (3<<0)
#define BIT_DBGCTR_SFR_MODE             (2<<0)
#define BIT_DBGCTR_IRAM_MODE            (1<<0)
#define BIT_DBGCTR_NORMAL_MODE          (0<<0)

#define BIT_BSTCTR_TYPE_MASK            (~(1<<2))
#define BIT_BSTCTR_TYPE_FLASH           (1<<2)
#define BIT_BSTCTR_TYPE_SRAM            (0<<2)
#define BIT_BSTCTR_BSTRUN_MASK          (~(1<<1))
#define BIT_BSTCTR_BSTRUN_RUN           (1<<1)
#define BIT_BSTCTR_BSTRUN_STOP          (0<<1)
#define BIT_BSTCTR_BSTEN_MASK           (~(1<<0))
#define BIT_BSTCTR_BSTEN_ENABLE         (1<<0)
#define BIT_BSTCTR_BSTEN_DISABLE        (0<<0)

/*********************************************************
 *
 * struct
 *
 ********************************************************/
enum aw22xxx_fw_flags {
    AW22XXX_FLAG_FW_NONE = 0,
    AW22XXX_FLAG_FW_UPDATE = 1,
    AW22XXX_FLAG_FW_OK = 2,
    AW22XXX_FLAG_FW_FAIL = 3,
};
enum aw22xxx_flags {
    AW22XXX_FLAG_NONE = 0,
    AW22XXX_FLAG_SKIP_INTERRUPTS = 1,
};
enum aw22xxx_imax {
    AW22XXX_IMAX_2mA = 8,
    AW22XXX_IMAX_3mA = 0,
    AW22XXX_IMAX_4mA = 9,
    AW22XXX_IMAX_6mA = 1,
    AW22XXX_IMAX_9mA = 2,
    AW22XXX_IMAX_10mA = 11,
    AW22XXX_IMAX_15mA = 3,
    AW22XXX_IMAX_20mA = 12,
    AW22XXX_IMAX_30mA = 4,
    AW22XXX_IMAX_40mA = 14,
    AW22XXX_IMAX_45mA = 5,
    AW22XXX_IMAX_60mA = 6,
    AW22XXX_IMAX_75mA = 7,
};



struct aw22xxx_container {
    unsigned int len;
    unsigned int version;
    unsigned int bist;
    unsigned int key;
    unsigned char *p_data;
};

struct aw22xxx {
    unsigned char chipid;
    unsigned char i2c_addr;
    unsigned char fw_update;
    unsigned char fw_flags;
    unsigned int fw_version;
};


void aw22xxx_play(void);




#endif /* __AW22XXX_H__ */
