#ifndef _OTA_MAIN_H_
#define _OTA_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ota_common.h"
#include "ota_spp.h"
#include "ota_me.h"
//#include "hcitrans.h"
#include "hci_api.h"
#include "hci.h"
#include "l2cap.h"

#define USER_DATA_BASE (FLASH_NC_BASE+100*0x400)

#define OTA_RECV_STACK_SIZE 1024 * 6

#define OTA_SEND_STACK_SIZE 1024 * 4

#define OTA_BUF_SIZE 1024 * 1

#define OTA_HCI_CMD_TYPE 0x01
#define OTA_ACL_DATA_TYPE 0x02
#define OTA_HCI_EVENT_TYPE 0x03

/* Events type define */
#define OTA_HCI_COMMAND_COMPLETE_EVENT      0x0E
#define OTA_HCI_COMMAND_COMPLETE_STATUS     0x0F
#define OTA_HCI_DISCONNECT_EVENT            0x05
#define OTA_HCI_NUM_OF_COMPLETED_PKTS_EVENT 0x13
#define OTA_HCE_CONNECT_REQUEST             0x04
#define OTA_HCI_CONNECTION_COMPLETE_EVENT   0x03
#define OTA_HCI_PIN_CODE_REQ_EVENT          0x16
#define OTA_HCI_LINK_KEY_REQ                0x17
#define OTA_HCI_LINK_KEY_NOTIFY             0x18
#define OTA_HCI_IO_CAP_REQ                  0x31
#define OTA_HCI_IO_CAP_RSP                  0x32
#define OTA_HCI_USER_CONF_REQ               0x33
#define OTA_HCI_SSP_COMPLETE                0x36

#define OTA_HCI_BLE_EVENT                   0x3E
#define OTA_HCI_BLE_CONNECTION_EVENT        0x01
#define OTA_HCI_BLE_ENHANCED_CONNECTION_EVENT        0x0A

#define OTA_HCI_BLE_CONN_PARAM_REQ_EVENT        0x06

#define HCI_BUFTYPE_COMMAND      0x01
#define HCI_BUFTYPE_EVENT        0x02 /* Never sent as an HciPacket */
#define HCI_BUFTYPE_ACL_DATA     0x04
#define HCI_BUFTYPE_SCO_DATA     0x08

#define OTA_DEFAULT_ATTR_HANDLE         42
#define OTA_CCC_HANDLE					43

#define OTA_ATT_CID                     4

#define OTA_ATT_MAX_MTU_LENGTH          512 //23
#define OTA_L2CAP_HEADER                4
#define OTA_HCI_HEADER                  4

#define OTA_ATT_ACL_BUF                 6

#define OTA_ATT_FREE_ACL_BUF_LOW_LEVEL_THRESHOLD	2	

#define HCI_ERR_UNACCEPT_CONN_INTERVAL  0x2021

#define CMD_NONE            0
#define CMD_ACCEPT_CON_REQ  1
#define CMD_CON_COMPLETE    2
#define CMD_PIN_CODE_REQ_REPL     3
#define CMD_LINK_KEY_REQ_REPL     4
#define CMD_IO_CAP_REQ_REPL       5
#define CMD_USER_CONF_REQ_REPL    6
#define CMD_LINK_KEY_REQ_NEG_REPL 7
enum {
    OTA_BLE_DISCONNECTED, 
    OTA_BLE_CONNECTED, 
};

enum {
    BLE_DATA_STATE_IDLE,
    BLE_DATA_STATE_RECV,
    BLE_DATA_STATE_SEND,
};

enum {
    BLE_INIT_START = 0x00,
    BLE_INIT_HCI_RESET,
    BLE_INIT_HCI_READ_BUFFER_SIZE,
    BLE_INIT_HOST_BUFFER_SIZE,
    BLE_INIT_HOST_FLOW_CONTROL_ENABLE,
    BLE_INIT_WRITE_BD_ADDRESS,
    BLE_INIT_SET_BT_EVENT_MASK,
    BLE_INIT_SET_BLE_EVENT_MASK,
    BLE_INIT_READ_LOCAL_FEATURES,
    BLE_INIT_READ_LOCAL_EXT_FEATURES_0,
    BLE_INIT_READ_LOCAL_EXT_FEATURES_1,
    BLE_INIT_READ_LOCAL_EXT_FEATURES_2,

    BLE_INIT_SET_BLE_ADV_PARAM,
    BLE_INIT_SET_BLE_ADV_DATA,
    BLE_INIT_SET_BLE_SCAN_RSP_DATA,

    BT_INIT_WRITE_BT_ADDRESS,
    BT_INIT_WRITE_LOCAL_NAME,
    BT_INIT_DISABLE_AUTHEN,
    BT_INIT_WRITE_EIR,
    BT_INIT_WRITE_INQ_SCAN_TYPE,
    BT_INIT_WRITE_INQ_SCAN_ACTIVITY,
    BT_INIT_WRITE_PAGE_SCAN_TYPE,
    BT_INIT_WRITE_PAGE_SCAN_ACTIVITY,
    BT_INIT_WRITE_SCAN_ENABLE,
    
    BLE_INIT_SET_BLE_ADV_ENABLE,

    BLE_INIT_RADIO_END //19
};


enum {
    OTA_STATE_IDLE = 0x00,
    OTA_STATE_BLE_INIT,
    OTA_STATE_UPDATE_CONN_INTERVEL,
    OTA_STATE_MTU_EXCHANGE,
    OTA_STATE_SEND_INDICATION,
    OTA_STATE_SEND_WRITE_RESP,
    OTA_STATE_SEND_PREPARE_WRITE_RESP,
    OTA_STATE_SEND_EXEC_WRITE_RESP,
    OTA_STATE_SEND_READ_RESP,
    OTA_STATE_SEND_READ_BY_TYPE_RESP,
    OTA_STATE_SEND_READ_BY_GRP_TYPE_RESP,
    OTA_STATE_ATT_RECV,
    OTA_STATE_ATT_SEND,
    OTA_STATE_ERR,
};

typedef void (*TranSendCmdFunc)(unsigned char type, unsigned short cmd_conn, unsigned short len, unsigned char *buffer);
typedef int (*TranSendAclDataFunc)(unsigned char packet_type, unsigned char *packet, int size);

typedef struct {
    uint8_t buf[OTA_BUF_SIZE];
    uint16_t count;
    uint16_t offset;
} OtaBuffer;

typedef struct _OtaTranEntry {
    TranSendCmdFunc          sendHciCmd;
    TranSendAclDataFunc      sendAclData;
} OtaTranEntry;


typedef union {
    uint16_t shortUuid; 
    uint8_t longUuid[16]; 
} uuidValue;

typedef struct {
    uint8_t type;
    uuidValue value;
} gattUuid;

typedef struct {
    uint8_t property;
    uint16_t handle; 
    gattUuid uuid;
} gattCharValue;

typedef struct {
    uint8_t property;
    uint16_t handle; 
    gattUuid uuid;
} gattCharCCC;

typedef struct {
    uint16_t attrHandle;
    uint16_t attrType;
    gattCharValue attrValue;
	gattCharCCC  attrCCC;
    uint8_t permission;
} gattChar;

typedef struct {
    uint16_t startHandle;
    uint16_t endHandle;
    gattUuid uuid;
    uint8_t isPrimary;
} gattService;

typedef struct {
    ListEntry     node;
    uint8_t buf[OTA_ATT_MAX_MTU_LENGTH + OTA_L2CAP_HEADER + OTA_HCI_HEADER + 10];
    uint16_t pktLen;
} otaTxAclBuf;

typedef struct _L2CAP_Channel L2CAP_Channel;

struct _L2CAP_Channel {
    U16                 flags;
    U8                  state;          /* L2CAP Channel Connection State */
    U8                  localIdent;     /* Identifier sent in signalling request */

    //BtRemoteDevice     *link;           /* remote device info and data queues */
    evm_timer_t            timer;          /* RTX/ERTX */

    U16                 rxMtu;          /* Max Rx SDU size (MTU) */
    U16                 txMtu;          /* Max Tx SDU size */
    U16                 inUse;          /* Number of signal + data packets outstanding on channel */

    U16                 remotePsm;      /* Remote PSM value */

    /* Channel oriented stuff */
    //const L2capPsm     *psmInfo;        /* PSM hosting this channel */
    L2capChannelId      localCid;       /* Local Channel Id (>= 0x0040) */
    L2capChannelId      remoteCid;      /* Remote devices channel id */
    U8                  remoteIdent;    /* Identifier recv'd in signalling req */
    U16                 result;         /* Configure/Connect response "result" */

    U16                 txFlushTimeout;
    U16                 rxFlushTimeout;
    //U8 sigid_last_send;
};

typedef struct {
    ListEntry    rxBuffList;
    ListEntry    rxBadBuffList;

    ListEntry    txBuffList;
	uint8_t		 freeTxBuffCount;
    ListEntry    txBuffFreeList;
	uint8_t 	 txBuffLeft;

    uint8_t hciCmdPkt[512];
    uint8_t hciCmdPayloadBuf[512];
    uint8_t aclPktBuf[OTA_ATT_MAX_MTU_LENGTH + OTA_L2CAP_HEADER + OTA_HCI_HEADER + 10];

    otaTxAclBuf aclTxbuf[6];

    uint8_t peerAddr[BD_ADDR_LEN];

    uint8_t prepareWriteBuf[OTA_ATT_MAX_MTU_LENGTH];
    uint16_t prepareWriteOffset;
    uint16_t prepareWriteLen;

    uint16_t attMtu;

    uint16_t startHandle;
    uint16_t endHandle;
    uint16_t attrType;

    uint8_t radioState;
	uint8_t	isToEnableBleAdv;

    uint8_t otaState;
    uint8_t bleDataState;

    uint8_t otaBuildPktFlag;
    uint16_t otaBuildPktTotalLen;

    OtaTranEntry tranEntry;

    OtaBuffer otaBuf;

    gattService service;
    gattChar characteristic;
    uint16_t hciHandle;

    uint8_t otaCmdFlag;
    
    OtaBtDeviceRecord  record;
    
    uint8_t dataBuf[1024];
    
    L2CAP_Channel l2capRfcChannel;
    L2CAP_Channel l2capSdpChannel;
    uint16_t curPsm;

	uint16_t cccValue;
} OtaHciContext;

extern __attribute__((aligned(4))) OtaHciContext ota;
extern uint8_t sendConfigReq;
extern uint8_t sendModemStatusReq;

extern void otaMainInit(void);
extern void OS_NotifyOtaSendThread(void);
extern otaTxAclBuf* otaRxBufAlloc(void);

void ota_start_10_second_timer(uint8_t timer_id);
void ota_stop_10_second_timer(uint8_t timer_id);
void ota_reset_timer(uint32_t intervalInMs);
void OtaHciBufferInd(void);

#ifdef __cplusplus
}
#endif

#endif
