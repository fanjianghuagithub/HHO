#ifndef _OTA_ME_H_
#define _OTA_ME_H_


#ifdef __cplusplus
extern "C" {
#endif

//#include "bttypes.h"


#define MAXIMUM_BLE_ADV_DATA_LEN    	32
#define BLE_ADV_DATA_LEN    			(3 + 18 + 5)
#define BLE_ADDR_OFFSET_IN_LOCAL_NAME   10

#define MAXIMUM_BT_NAME_LEN    			248
#define BT_ADDR_OFFSET_IN_LOCAL_NAME   	8

extern uint8_t adv_data_buf[MAXIMUM_BLE_ADV_DATA_LEN];
extern uint8_t bt_name[MAXIMUM_BT_NAME_LEN];

typedef U8 HciBufFlags;

/*---------------------------------------------------------------------------
 * HciFragment structure
 *
 *     Represents a fragment of data that may appear in an HciPacket.
 */
typedef struct _HciFragment {
   const U8     *buffer;
   U16           len;
} HciFragment;

/* End of HciFragment */

typedef struct _HciPacket HciPacket;

/*---------------------------------------------------------------------------
 * HciPacket structure
 *
 *     Represents a complete HCI packet to be transmitted.  All data passed
 *     from the HCI driver to the HCI transport driver are passed in an
 *     HciPacket structure.  The actual packet data consists of the four
 *     HCI header bytes, followed by any number of data fragments. These
 *     fragments must be reassembled by the transport layer when sending data
 *     to the hardware driver.
 *
 *     There is no length field for header information, but the length can be
 *     determined by the type of data in the packet (stored in "flags").
 *     ACL data headers always have a length of 4.  SCO data and command
 *     packets have a header length of 3.
 */
struct _HciPacket {
    ListEntry    node;       /* Node element for linked list management. */

    U8           header[4];  /* HCI packet header. Transmit these bytes
                              * first. This array contains three valid bytes
                              * for commands and SCO data packets. For ACL
                              * data it contains four valid bytes.
                              */

    U8           fCount;     /* The number of fragments present. Zero to
                              * three fragments may be specified. */
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
    HciFragment f[4];       /* Array of fragments. */
#else
    HciFragment f[3];       /* Array of fragments. */
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */

    HciBufFlags flags;      /* Flags that identify the type of the packet */
};

/* End of HciPacket */
#define BD_ADDR_SIZE    6

typedef struct _BT_BD_ADDR {
    U8    addr[BD_ADDR_SIZE];
} BT_BD_ADDR;

/*---------------------------------------------------------------------------
 * BtLinkKeyType type
 */

typedef U8 OtaBtLinkKeyType;

/*---------------------------------------------------------------------------
 * BtDeviceRecord structure
 *
 *     Represents a Bluetooth Device Database record as seen by the
 *     Security manager.
 */
typedef struct _OtaBtDeviceRecord
{
    BT_BD_ADDR      bdAddr;
    U8              linkKey[16];
} OtaBtDeviceRecord;

uint8_t ota_store_link_key(void);
uint8_t ota_get_link_key(void);

void sendHostNumOfCompletedPktsCmd(void);
void HciSendSetAdvParam(void);
void HciSendSetAdvData(void);
void HciSendSetScanRspData(void);
void HciSendReadLocalExtFeaturesCommand(uint8_t pageNum);
void HciSendSetEventMaskCommand(void);
void HciSendSetBleEventMaskCommand(void);
void HciReadLocalFeaturesCommand(void);
void HciSendWriteBleAddr(void);
void HciSendWriteBtAddr(void);
void HciSendHostFlowControlCommand(void);
void HciSendHostBufSizeCommand(void);
void HciSendReadBufSizeCommand(void);
void HciSendResetCommand(void);
void HciSendEnableAdv(BOOL listen);
void HciSendWriteInqScanType(void);
void HciSendWriteInqScanActivity(void);
void HciSendWritePageScanType(void);
void HciSendWritePageScanActivity(void);
void HciSendWriteScanEnable(void);
void HciSendWriteLocalNameCommand(void);
void HciSendDisableAuthenticCommand(void);
void HciSendAcceptConReq(uint8_t *bt_addr);
void HciSendPinCodeReqRepy(void);
void HciSendLinkKeyReqRepy(void);
void HciSendIOCapabilityReqRepy(void);
void HciSendUserConfirmReqRepy(void);
void HciSendWriteSspCommand(void);
void HciSendWriteEIRCommand(void);
void HciSendLinkKeyReqNegRepy(void);

void handleBleConnParmReqEvent(uint8_t* data, uint16_t len);
void handleBleConnectionEvent(uint8_t* data, uint16_t len);
void handleHciBleEvent(uint8_t* data, uint16_t len);
void handleHciDisconnectionEvent(uint8_t* data, uint16_t len);
void handleHciCmdCompleteEvent(uint8_t* data, uint16_t len);
void handleHciCompletedPktEvent(uint8_t* data, uint16_t len);
void handleHciPinCodeReqEvent(uint8_t* data, uint16_t len);
void handleHciLinkKeyReqEvent(uint8_t* data, uint16_t len);
void handleHciLinkKeyNotifyEvent(uint8_t* data, uint16_t len);

void handleHciConnectReqEvent(uint8_t* data, uint16_t len);
void handleHciConCompleteEvent(uint8_t* data, uint16_t len);
void handleHciIOCapabilityReq(uint8_t* data, uint16_t len);
void handleHciUserConfirmReq(uint8_t* data, uint16_t len);
bool is_bt_init_done(void);

#ifdef __cplusplus
}
#endif

#endif
