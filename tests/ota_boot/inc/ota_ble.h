#ifndef __OTA_BLE_H
#define __OTA_BLE_H

#include "hci.h"

#include "ota_main.h"

#define GATT_SHORT_UUID 1
#define GATT_LONG_UUID  2

#define OTA_ATT_OPCODE_WRITE_REQ                0x12
#define OTA_ATT_OPCODE_WRITE_CMD                0x52
#define OTA_ATT_OPCODE_READ_REQ                 0x0A
#define OTA_ATT_OPCODE_READ_RESP                0x0B
#define OTA_ATT_OPCODE_CONFIRMATION             0x1E
#define OTA_ATT_OPCODE_INDICATION               0x1D
#define OTA_ATT_OPCODE_NOTIFY                   0x1B
#define OTA_ATT_OPCODE_EXEC_WRITE_RESP          0x19
#define OTA_ATT_OPCODE_EXEC_WRITE_REQ           0x18
#define OTA_ATT_OPCODE_PREPARE_WRITE_RESP       0x17
#define OTA_ATT_OPCODE_PREPARE_WRITE_REQ        0x16
#define OTA_ATT_OPCODE_WRITE_RESP               0x13
#define OTA_ATT_OPCODE_READ_BY_GRP_TYPE_RESP    0x11
#define OTA_ATT_OPCODE_READ_BY_GRP_TYPE_REQ     0x10
#define OTA_ATT_OPCODE_READ_BY_TYPE_REQ         0x08
#define OTA_ATT_OPCODE_READ_BY_TYPE_RESP        0x09
#define OTA_ATT_OPCODE_FIND_BY_TYPE_REQ         0x06
#define OTA_ATT_OPCODE_FIND_INFOMRATION_REQ     0x04
#define OTA_ATT_OPCODE_FIND_INFOMRATION_RSP     0x05


#define OTA_ATT_OPCODE_MTU_RESPONSE             0x03
#define OTA_ATT_OPCODE_MTU_REQUEST              0x02
#define OTA_ATT_OPCODE_ERR_RESPONSE             0x01

#define GATT_PERM_READ                  (1 << 0)
#define GATT_CHAR_PROP_BIT_READ         (1 << 1)
#define GATT_CHAR_PROP_BIT_WRITE_NO_RSP (1 << 2)
#define GATT_CHAR_PROP_BIT_WRITE        (1 << 3)
#define GATT_CHAR_PROP_BIT_NOTIFY       (1 << 4)

#define OTA_TIMER_REPORT    ((void *)0)

#if defined(CHIP_BEST2300)||defined(CHIP_BEST2300P)||defined(CHIP_BEST1400)
//bt address not ble address
#define HCI_SET_BD_ADDR_CMD_OPCODE                	0xFC72
#else
#define HCI_SET_BD_ADDR_CMD_OPCODE                	0xfc40 // bt address
#endif

#define HCI_LE_REM_CON_PARAM_REQ_RPLY_CMD_OPCODE	0x2020
#define HCI_LE_SET_EVENT_MASK_CMD_OPCODE			0x2001

#define OTA_BLE_DEFAULT_MTU_SIZE			23

#define OTA_BLE_CONNECTION_INTERVAL_MIN		10
#define OTA_BLE_CONNECTION_INTERVAL_MAX		20
#define OTA_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS	5000

extern void otaBuildRecvPkt(uint8_t* data, uint16_t len);
extern void otaBleL2capParse(uint8_t* data, uint16_t len, uint16_t hciHandle);
extern void sendAttPrepareWriteResp(void);
extern void sendBleConnRaramReqNeplyCommand(void);
extern void fillRemoteConnParam(uint8_t* data);
extern void ota_timer_callback(void *param);
extern void updateBleConnectionIntervel(uint32_t intervelMin, uint32_t intervalMax, uint32_t timeoutInMs);
extern void sendAttNotify(uint8_t* data, uint16_t len);

#endif
