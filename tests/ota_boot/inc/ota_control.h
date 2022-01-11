#ifndef _OTA_CONTROL_H_
#define _OTA_CONTROL_H_

#define ERACE_LEN_UNIT  4096
#define WRITE_LEN_UNIT  2048

#define BURN_DATA_MSG_HEAD_LEN  16

#define OTA_TOTAL_PREFIX_LEN (4)

#define OTA_SPEC_VERSION	2

#define OTA_SW_VERSION		1
#define OTA_HW_VERSION		1

#define DATA_PATH_BLE   1
#define DATA_PATH_SPP   2

#define IS_REBOOT_WHEN_OTA_FAILED	1

#define NORMAL_BOOT		BOOT_MAGIC_NUMBER
#define COPY_NEW_IMAGE	0x5a5a5a5a

#ifndef NEW_IMAGE_FLASH_OFFSET
#define NEW_IMAGE_FLASH_OFFSET	0x200000
#endif

#define OTA_NON_CACHE_READ_ISSUE_WORKAROUND     0
#if OTA_NON_CACHE_READ_ISSUE_WORKAROUND
#define OTA_FLASH_LOGIC_ADDR        (FLASH_BASE)
#else
#define OTA_FLASH_LOGIC_ADDR        (FLASH_NC_BASE)
#endif

#define OTAUPLOG_HEADSIZE (sizeof(otaUpgradeLog.randomCode) + sizeof(otaUpgradeLog.totalImageSize) + sizeof(otaUpgradeLog.crc32OfImage))
#define OTA_FLASH_OFFSET_OF_IMAGE   __APP_IMAGE_FLASH_OFFSET__
#define OTA_FLASH_ENTRY_OFFSET	    (OTA_FLASH_LOGIC_ADDR + OTA_FLASH_OFFSET_OF_IMAGE)

#if (OTA_SPEC_VERSION == 1)
#define DUAL_IMAGE_COPY_ENABLED		0

#define OTA_PREFIX_OFFSET    0
#define OTA_TYPE_OFFSET      1
#define OTA_PAC_NUM_OFFSET   2
#define OTA_LEN_OFFSET       4
#define OTA_CRC_OFFSET       8
#define OTA_SEQ_OFFSET       12
#define OTA_CHECK_SUM_OFFSET 15

#define OTA_PREFIX          0xBE
#define OTA_TYPE            0x64

#endif

typedef enum {
    OTA_WAIT_NOTIFY,
    OTA_WAIT_HANDSHAKE,
    OTA_WAIT_BURN_HANDSHAKE,
    OTA_NOTIFY,
    OTA_BURN_HANDSHAKE,
    OTA_HANDSHAKE,
    OTA_BURN_DATA,
    OTA_BURN_OK,

    OTA_SYNC_NUM
} OTA_BURN_STEP;

typedef enum {
    NOTI_NONE,
    NOTI_FIRMWARE_ERROR,

    NOTI_NUM
} OTA_NOTIFY_INFO_E;

typedef enum {
    OTA_ERR_NONE     = 0x00,
    OTA_ERR_LEN      = 0x01,
    OTA_ERR_CHECKSUM = 0x02,
    OTA_ERR_NOT_SYNC = 0x03,
    OTA_ERR_MSG_HEAD = 0x04,

    OTA_ERR_CMD          = 0x06,
    OTA_ERR_BURN_PREPARE = 0x60,

    OTA_ERR_BURN_OK = 0x60,
    OTA_ERR_SECTOR_SIZE = 0x61,

    OTA_ERR_BURN_INFO_MISSING = 0x63,
    OTA_ERR_SECTOR_DATA_LEN   = 0x64,
    OTA_ERR_DATA_CRC   = 0x65,
    OTA_ERR_ERASE_FLSH = 0x67,
    OTA_ERR_BURN_FLSH  = 0x68,
    OTA_ERR_PACKET_MISSING  = 0x69,
    OTA_ALREADY_IN_OTA_MODE = 0x6a,
} OTA_ERR_CODE_E;

typedef struct
{
	uint32_t magicNumber;	// NORMAL_BOOT or COPY_NEW_IMAGE
	uint32_t imageSize;
	uint32_t imageCrc;
} FLASH_OTA_BOOT_INFO_T;

typedef struct
{
	uint8_t softwareRevByte0;
	uint8_t softwareRevByte1;
	uint8_t softwareRevByte2;
	uint8_t softwareRevByte3;
} OTA_SOFTWARE_REV_INFO_T;

typedef struct {
    uint8_t     burn_data[WRITE_LEN_UNIT];
    uint16_t    burn_data_offset;
    uint32_t    burn_total_len;
    uint16_t    packetSum;
    uint16_t    packetSeq;

    uint32_t    burn_addr;
    uint32_t    erase_addr;
    uint32_t    burn_count;

    uint8_t     burn_step;
    uint32_t    cur_seq;

	
} OTA_CTR_T;

typedef struct
{
    uint8_t     randomCode[32];
    uint32_t    totalImageSize;
    uint32_t    crc32OfImage;
    uint32_t    upgradeSize[(ERACE_LEN_UNIT - 32 - 2*sizeof(uint32_t)) / 4];
}FLASH_OTA_UPGRADE_LOG_FLASH_T;

#if (OTA_SPEC_VERSION > 1)

#ifndef DUAL_IMAGE_COPY_ENABLED
#define DUAL_IMAGE_COPY_ENABLED		0
#endif


/**
 * @brief Format of the data xfer handler function, to send data to central
 *
 * @param ptrData	Pointer of the data to send
 * @param dataLen 	Length of the data to send
 * 
 */
typedef void (*ota_transmit_data_t)(uint8_t* ptrData, uint16_t dataLen);
typedef void (*ota_transmission_done_t)(void);

void ota_control_register_transmitter(ota_transmit_data_t transmit_handle);
void ota_control_update_MTU(uint16_t mtu);
void ota_check_and_reboot_to_use_new_image(void);
void ota_control_reset_env(void);
void ota_execute_the_final_operation(void);
bool ota_is_in_progress(void);
void ota_data_transmission_done_callback(void);

void ota_control_set_datapath_type(uint8_t datapathType);

#else

uint8_t	ota_burn_data(uint8_t* data, uint32_t len);

#endif // #if (OTA_SPEC_VERSION > 1)

#if DUAL_IMAGE_COPY_ENABLED
void app_copy_new_image(uint32_t srcFlashOffset, uint32_t dstFlashOffset);
#endif

#if GTK_IMAGE_COPY_ENABLED
int gtk_need_copy_new_image(void);
#endif
uint32_t app_get_magic_number(void);

extern OTA_CTR_T gOtaCtr;

#endif
