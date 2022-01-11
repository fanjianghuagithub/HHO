#include "cmsis_os.h"
#include "cmsis.h"
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_dma.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_wdt.h"
#include "hal_sleep.h"
#include "hal_bootmode.h"
#include "hal_chipid.h"
#include "hal_cache.h"
#include "hwtimer_list.h"
#include "pmu.h"
#include "analog.h"

#include "bt_drv_interface.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"

// for init
#include "apps.h"
#include "app_thread.h"
#include "app_key.h"
#include "app_pwl.h"
#include "app_battery.h"
#include "app_utils.h"

// for progress
#include "hal_uart.h"
#include "tool_msg.h"

#include "cmsis_nvic.h"

#include "intersyshci.h"

#include "ota_main.h"

#include "ota_ble.h"

#include "ota_common.h"

#include "ota_control.h"

#include "ota_me.h"

#include "platform_deps.h"

#define LOG_TAG "[OTA_MAIN] "
#define OTA_MAIN_DEBUG
#ifdef OTA_MAIN_DEBUG
#define LOG_DBG(num,str,...)   TRACE(num,LOG_TAG"DBG:"""str, ##__VA_ARGS__)             // DEBUG OUTPUT
#define LOG_MSG(num,str,...)   TRACE(num,LOG_TAG"MSG:"""str, ##__VA_ARGS__)             // MESSAGE OUTPUT
#define LOG_ERR(num,str,...)   TRACE(num,LOG_TAG"ERR:"""str, ##__VA_ARGS__)             // ERROR OUTPUT

#define LOG_FUNC_LINE()    TRACE(2,LOG_TAG"%s:%d\n", __FUNCTION__, __LINE__)
#define LOG_FUNC_IN()      TRACE(1,LOG_TAG"%s ++++\n", __FUNCTION__)
#define LOG_FUNC_OUT()     TRACE(1,LOG_TAG"%s ----\n", __FUNCTION__)

#define LOG_DUMP		   DUMP8
#else
#define LOG_DBG(num,str,...)        TRACE(num,LOG_TAG""str, ##__VA_ARGS__)
#define LOG_MSG(num,str,...)   TRACE(num,LOG_TAG""str, ##__VA_ARGS__)
#define LOG_ERR(num,str,...)   TRACE(num,LOG_TAG"ERR:"""str, ##__VA_ARGS__)

#define LOG_FUNC_LINE()
#define LOG_FUNC_IN()
#define LOG_FUNC_OUT()

#define LOG_DUMP
#endif

#define DelayMs(a)      hal_sys_timer_delay(MS_TO_TICKS(a))
#define DelayS(a)       DelayMs(1000 * a)

void OtaReceiveThread(void const *argument);
void OtaSendThread(void const *argument);
void OS_NotifyOtaSendThread(void);

extern uint8_t needRsp;

extern "C" void RXBUFF_Free(btif_hci_buffer_t* rxBuffHandle);


/********************** Receive Thread *********************/
static osThreadId ota_receive_tid;
osMessageQDef(ota_receive_queue, 64, message_t);
osMessageQId  ota_receive_queue_id;

osThreadDef(OtaReceiveThread, osPriorityHigh, 1, OTA_RECV_STACK_SIZE, "OtaReceiveThread");

/************************************************************/

/*********************Send Thread***************************/
static osThreadId ota_send_tid;
osMessageQDef(ota_send_queue, 64, message_t);
osMessageQId  ota_send_queue_id;

osThreadDef(OtaSendThread, osPriorityHigh, 1, OTA_SEND_STACK_SIZE, "OtaSendThread");

/***********************************************************/

osMutexId ota_app_mutex_id = NULL;
osMutexDef(ota_app_mutex);

uint16_t ControllerTotalAclBufNum;
static uint16_t rxAclPktsNum = 0; 

uint8_t sendBleConnRaramReqNeplyFlag = 0;

OtaHciContext ota;

uint8_t sendConfigReq = 0;
uint8_t sendModemStatusReq = 0;
uint8_t ble_name_set = 0;

static void ota_app_lock_thread(void)
{
    osMutexWait(ota_app_mutex_id, osWaitForever);
}

static void ota_app_unlock_thread(void)
{
    osMutexRelease(ota_app_mutex_id);
}

void otaRxBufInit(void)
{
    uint8_t i;
    
    InitializeListHead(&ota.txBuffFreeList);

    for (i = 0; i < OTA_ATT_ACL_BUF; i++)
    {
        InsertTailList(&ota.txBuffFreeList, &ota.aclTxbuf[i].node);
    }

    ota.freeTxBuffCount = OTA_ATT_ACL_BUF;
    ota.txBuffLeft = OTA_ATT_ACL_BUF;
}

otaTxAclBuf* otaRxBufAlloc(void)
{
    OS_StopHardware();
    otaTxAclBuf* buf = NULL;
    if (!IsListEmpty(&ota.txBuffFreeList) && ota.txBuffLeft)
    {
        buf = (otaTxAclBuf*)RemoveHeadList(&ota.txBuffFreeList);
        ota.freeTxBuffCount--;
        ota.txBuffLeft--;
        TRACE(1,"Free tx buf -- to %d", ota.freeTxBuffCount);
    }
    OS_ResumeHardware();

    return buf;
}

void otaRxBufFree(otaTxAclBuf* buf)
{
    OS_StopHardware();
    if (!IsNodeOnList(&ota.txBuffFreeList, &buf->node))
    {
        InsertTailList(&ota.txBuffFreeList, &buf->node);
        ota.freeTxBuffCount++;
        TRACE(1,"Free tx buf ++ to %d", ota.freeTxBuffCount);
    }
    OS_ResumeHardware();
}

void OS_NotifyOtaSendThread(void)
{
	osMessagePut(ota_send_queue_id, 0xFF, 0);
    LOG_DBG(1,"%s status", __func__);
}

void OS_NotifyOtaRecvThread(void)
{
	osMessagePut(ota_receive_queue_id, 0xFF, 0);
}

void OtaHciBufferInd(void)
{
    LOG_FUNC_IN();

    OS_NotifyOtaRecvThread();

    LOG_FUNC_OUT();
}

void inesrtAclToBufList()
{
    
}

void tran_init(void)
{
    ota.tranEntry.sendHciCmd = (TranSendCmdFunc)BESHCI_SendData;
    ota.tranEntry.sendAclData = (TranSendAclDataFunc)BESHCI_SendBuffer;
}

void clearRxAclPktsNum(void)
{
    ota_app_lock_thread();
    rxAclPktsNum = 0; 
    ota_app_unlock_thread();
}

void decreaseRxAclPktsNum(uint16_t cnt)
{
    ota_app_lock_thread();
    rxAclPktsNum -= cnt;
    ota_app_unlock_thread();  
}

void increaseRxAclPktsNum(uint16_t cnt)
{
    ota_app_lock_thread();
    rxAclPktsNum += cnt;
    ota_app_unlock_thread();    
}

void setRxAclPktsNum(uint16_t cnt)
{
    ota_app_lock_thread();
    rxAclPktsNum = cnt;
    ota_app_unlock_thread();
}

uint16_t getRxAclPktsNum(void)
{
    return rxAclPktsNum;
}

void handleHciEvent(uint8_t* data, uint16_t len)
{
    uint8_t type;    
    uint8_t* p;
    uint8_t payloadLen;
	
    LOG_FUNC_IN();

    p = data;

    STREAM_TO_UINT8(type, p);
    STREAM_TO_UINT8(payloadLen, p);

    LOG_DBG(2,"%s receive event %02x", __func__, type);

    switch(type) {
        case OTA_HCI_COMMAND_COMPLETE_EVENT:
            handleHciCmdCompleteEvent(p, payloadLen);
            break;
        case OTA_HCI_COMMAND_COMPLETE_STATUS:
            LOG_DBG(1,"%s , Hci command complete status", __func__);
            break;
        case OTA_HCI_DISCONNECT_EVENT:
            handleHciDisconnectionEvent(p, payloadLen);
            break;
        case OTA_HCI_NUM_OF_COMPLETED_PKTS_EVENT:
            handleHciCompletedPktEvent(p, payloadLen);
            break;
        case OTA_HCI_BLE_EVENT:
            handleHciBleEvent(p, payloadLen);
            break;
        case OTA_HCE_CONNECT_REQUEST:
            handleHciConnectReqEvent(p, payloadLen);
            break;
        case OTA_HCI_CONNECTION_COMPLETE_EVENT:
            LOG_DBG(0,"Connection Complete Event");
            handleHciConCompleteEvent(p, payloadLen);
            break;
        case OTA_HCI_PIN_CODE_REQ_EVENT:
            LOG_DBG(0,"OTA_HCI_PIN_CODE_REQ_EVENT");
            handleHciPinCodeReqEvent(p, payloadLen);
            break;
        case OTA_HCI_LINK_KEY_REQ:
            LOG_DBG(0,"OTA_HCI_LINK_KEY_REQ");
            handleHciLinkKeyReqEvent(p, payloadLen);
            break;
        case OTA_HCI_LINK_KEY_NOTIFY:
            LOG_DBG(0,"OTA_HCI_LINK_KEY_NOTIFY");
            handleHciLinkKeyNotifyEvent(p, payloadLen);
            break;
        case OTA_HCI_IO_CAP_REQ:
            handleHciIOCapabilityReq(p, payloadLen);
            break;
        case OTA_HCI_USER_CONF_REQ:
            handleHciUserConfirmReq(p, payloadLen);
            break;
        default:
            LOG_ERR(2,"%s receive unknown event 0x%02x", __func__, type);
            break;
    }

    LOG_FUNC_OUT();
}

void otaHandleRecvPkts(void)
{
    int ret = 0;
    uint8 rx_empty = 0;
    hci_buff_t *hci_buff = NULL;
    uint16 length;
    uint8 *data = NULL;
    uint16_t hciHandle;
    
    // rx : event acl sco
    Plt_LockHCIBuffer();
    rx_empty = hci_buff_list_is_empty(HBT_RX);
    Plt_UNLockHCIBuffer();
    if (!rx_empty)
    {
        Plt_LockHCIBuffer();
        ret = hci_buff_list_pop(HBT_RX, &hci_buff);
        Plt_UNLockHCIBuffer();
        if (ret == SUCCESS)
        {
            data = HB_DATA(hci_buff);
            length = HB_DATA_LEN(hci_buff);
            switch(HB_TYPE(hci_buff))
            {
                case HBT_RX_ACL:
                    //LOG_DBG(1,"%s acl data", __func__);
                    //LOG_DUMP("%02x ", data, length);
                    hciHandle = HB_HANDLE(hci_buff);

                    /* BT hciHandle: 0x2080, BLE hciHandle: 0x2000 */
                    if ((hciHandle & 0x0fff) != 0x0000)
                    {
                        otaBtL2capParse(data, length, hciHandle);
                    }
                    else
                    {
                        otaBleL2capParse(data, length, hciHandle);
                    }

                    increaseRxAclPktsNum(1);
                    if (getRxAclPktsNum() >= 1) {
                        OS_NotifyOtaSendThread();
                    }
                    break;
                case HBT_EVENT:
                {
                    handleHciEvent(data, length);
                }
                    break;
                default:
                    break;
            }
        }
        hci_buff_free(hci_buff);
    }else{

        TRACE(0,"@");
    }
}

/************ BLE and BT Radio Init ***********************/
void radioInit(void)
{
    LOG_DBG(2,"%s start ota.radioState: %d", __func__, ota.radioState);
    LOG_FUNC_IN();
	
    switch (ota.radioState) {
        case BLE_INIT_HCI_RESET:
            HciSendResetCommand();
            break;
        case BLE_INIT_HCI_READ_BUFFER_SIZE:
            HciSendReadBufSizeCommand();
            break;
        case BLE_INIT_HOST_BUFFER_SIZE:
            HciSendHostBufSizeCommand();
            break;
        case BLE_INIT_HOST_FLOW_CONTROL_ENABLE:
            HciSendHostFlowControlCommand();
            break;
        case BLE_INIT_WRITE_BD_ADDRESS:
            HciSendWriteBleAddr();
            break;
        case BLE_INIT_SET_BT_EVENT_MASK:
            HciSendSetEventMaskCommand();
            break;
        case BLE_INIT_SET_BLE_EVENT_MASK:
            HciSendSetBleEventMaskCommand();
            break;
        case BLE_INIT_READ_LOCAL_FEATURES:
            HciReadLocalFeaturesCommand();
            break;
        case BLE_INIT_READ_LOCAL_EXT_FEATURES_0:
        case BLE_INIT_READ_LOCAL_EXT_FEATURES_1:
        case BLE_INIT_READ_LOCAL_EXT_FEATURES_2:
            HciSendReadLocalExtFeaturesCommand(ota.radioState - BLE_INIT_READ_LOCAL_EXT_FEATURES_0);
            break;

        case BLE_INIT_SET_BLE_ADV_PARAM:
            HciSendSetAdvParam();
            break;
        case BLE_INIT_SET_BLE_ADV_DATA:
            HciSendSetAdvData();
            break;
        case BLE_INIT_SET_BLE_SCAN_RSP_DATA:
            HciSendSetScanRspData();
            break;
        case BLE_INIT_SET_BLE_ADV_ENABLE:
            HciSendEnableAdv(ota.isToEnableBleAdv);
            break;

        case BT_INIT_WRITE_BT_ADDRESS:
            HciSendWriteBtAddr();
            break;
        case BT_INIT_WRITE_LOCAL_NAME:
            HciSendWriteLocalNameCommand();
            break;
        case BT_INIT_DISABLE_AUTHEN:
            HciSendDisableAuthenticCommand();
            break;
        case BT_INIT_WRITE_EIR:
            HciSendWriteEIRCommand();
            break;
        case BT_INIT_WRITE_INQ_SCAN_TYPE:
            HciSendWriteInqScanType();
            break;
        case BT_INIT_WRITE_INQ_SCAN_ACTIVITY:
            HciSendWriteInqScanActivity();
            break;
        case BT_INIT_WRITE_PAGE_SCAN_TYPE:
            HciSendWritePageScanType();
            break;
        case BT_INIT_WRITE_PAGE_SCAN_ACTIVITY:
            HciSendWritePageScanActivity();
            break;    
        case BT_INIT_WRITE_SCAN_ENABLE:
            HciSendWriteScanEnable();
            break;
        case BLE_INIT_RADIO_END:
            ota.radioState++;
            break;
    }

    LOG_FUNC_OUT();
}

void OtaReceiveThread(void const *argument)
{
    LOG_FUNC_IN();

    InitializeListHead(&ota.rxBuffList);

    InitializeListHead(&ota.txBuffList);

    InitializeListHead(&ota.rxBadBuffList);

    LOG_MSG(1,"thread %s start", __func__);
    
    while (1)
    {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_1, APP_SYSFREQ_32K);
        osMessageGet(ota_receive_queue_id, osWaitForever);
        app_sysfreq_req(APP_SYSFREQ_USER_APP_1, APP_SYSFREQ_52M);
    
        LOG_DBG(1,"%s otaHandleRecvPkts", __func__);
        if (0)// (ota.freeTxBuffCount < OTA_ATT_FREE_ACL_BUF_LOW_LEVEL_THRESHOLD)
        {
            TRACE(2,"%s Low level free tx buffer %d!Yield for the send thread", __func__, ota.freeTxBuffCount);
            OS_NotifyOtaSendThread();
        }
        else
        {
            otaHandleRecvPkts();
        }
    }
	
    LOG_FUNC_OUT();
}

void handleHciCmdSendMsg(void)
{
    switch (ota.otaCmdFlag) {
        case CMD_ACCEPT_CON_REQ:
            HciSendAcceptConReq(ota.peerAddr);
            break;
        case CMD_PIN_CODE_REQ_REPL:
            HciSendPinCodeReqRepy();
            break;
        case CMD_CON_COMPLETE:
            break;
        case CMD_LINK_KEY_REQ_REPL:
            HciSendLinkKeyReqRepy();
            break;
        case CMD_NONE:
            break;
        case CMD_IO_CAP_REQ_REPL:
            HciSendIOCapabilityReqRepy();
            break;
        case CMD_USER_CONF_REQ_REPL:
            HciSendUserConfirmReqRepy();
            break;
        case CMD_LINK_KEY_REQ_NEG_REPL:
            HciSendLinkKeyReqNegRepy();
            break;
        default:
            LOG_DBG(2,"%s, Invalid ota.otaCmdFlag: %d", __func__, ota.otaCmdFlag);
            break;
    }
    
    ota.otaCmdFlag = CMD_NONE;
}

void OtaSendThread(void const *argument)
{
    LOG_FUNC_IN();

    BESHCI_Open();

    tran_init();

    LOG_MSG(1,"thread %s start", __func__);

    OS_NotifyOtaSendThread();

    while (1)
    {
        osEvent event;
        otaTxAclBuf  buf;
        otaTxAclBuf* tmpTxBuf;
        otaTxAclBuf* txBuf;
        app_sysfreq_req(APP_SYSFREQ_USER_APP_2, APP_SYSFREQ_32K);
        event = osMessageGet(ota_send_queue_id, osWaitForever);
        app_sysfreq_req(APP_SYSFREQ_USER_APP_2, APP_SYSFREQ_52M);
    
        LOG_DBG(2,"%s status: %d", __func__, event.status);

        if (ota.radioState < BLE_INIT_RADIO_END)
        {
            radioInit();
        }
        
        handleHciCmdSendMsg();

        if (needRsp) 
        {
            sendAttPrepareWriteResp();
        }

        if (sendBleConnRaramReqNeplyFlag) {
            sendBleConnRaramReqNeplyFlag = 0;
            sendBleConnRaramReqNeplyCommand();
        }

        OS_StopHardware();
        if (!IsListEmpty(&ota.txBuffList))
        {
            tmpTxBuf = (otaTxAclBuf*)RemoveHeadList(&ota.txBuffList); 
            OS_ResumeHardware();
            memcpy(&buf, tmpTxBuf, sizeof(otaTxAclBuf));
            txBuf = &buf;

            otaRxBufFree(tmpTxBuf);
            
            ota.tranEntry.sendAclData(0x02, txBuf->buf, txBuf->pktLen);
        }
        else
        {
            OS_ResumeHardware();
        }

        if (sendConfigReq == 1)
        {
            sendConfigReq = 0;
            L2capSendConfigReq();
        }

        if (sendModemStatusReq == 1)
        {
            sendModemStatusReq = 0;
            RfcSendMSReq();
        }
        
        if (getRxAclPktsNum() >= 1) {
            sendHostNumOfCompletedPktsCmd();
        }

        if (!IsListEmpty(&ota.txBuffList))
        {
            OS_NotifyOtaSendThread();
        }

        int8 rx_empty = hci_buff_list_is_empty(HBT_RX);
        if (!rx_empty)
        {
            OS_NotifyOtaRecvThread();
        }
    }

    LOG_FUNC_OUT();
}

static void otaDataInit(void)
{
    LOG_FUNC_IN();

    memset(&ota, 0, sizeof(ota));

    memset(&ota.service, 0, sizeof(ota.service));

    memset(&ota.characteristic, 0, sizeof(ota.characteristic));

    memset(&ota.otaBuf, 0, sizeof(OtaBuffer));

    /*************** Service Initial *********************/
    ota.service.startHandle = 40;
    ota.service.endHandle = OTA_CCC_HANDLE;
    ota.service.uuid.type = GATT_LONG_UUID;
    memset(ota.service.uuid.value.longUuid, 0x66, sizeof(ota.service.uuid.value.longUuid));
    /****************************************************/

    /*************** Characteristic Initial *************/
    ota.characteristic.attrHandle = 41;
    ota.characteristic.attrType = 0x2803;
    ota.characteristic.permission = GATT_PERM_READ;

#if (OTA_SPEC_VERSION == 1)
    ota.characteristic.attrValue.property = GATT_CHAR_PROP_BIT_WRITE | GATT_CHAR_PROP_BIT_NOTIFY;
#else
    ota.characteristic.attrValue.property = GATT_CHAR_PROP_BIT_WRITE_NO_RSP | GATT_CHAR_PROP_BIT_WRITE | GATT_CHAR_PROP_BIT_NOTIFY;
#endif
    ota.characteristic.attrValue.handle = 42;
    ota.characteristic.attrValue.uuid.type = GATT_LONG_UUID;
    memset(ota.characteristic.attrValue.uuid.value.longUuid, 0x77, sizeof(ota.characteristic.attrValue.uuid.value.longUuid));

    ota.characteristic.attrCCC.property = GATT_CHAR_PROP_BIT_WRITE | GATT_CHAR_PROP_BIT_READ;
    ota.characteristic.attrCCC.handle = OTA_CCC_HANDLE;
    ota.characteristic.attrCCC.uuid.type = GATT_SHORT_UUID;
    ota.characteristic.attrCCC.uuid.value.shortUuid = 0x2902;

    
    /****************************************************/

    ota.cccValue = 0;

    ota.radioState = BLE_INIT_HCI_RESET;
    ota.isToEnableBleAdv = true;
    ota.otaState = OTA_STATE_BLE_INIT;
    ota.bleDataState = BLE_DATA_STATE_IDLE;

    ota.otaBuildPktFlag = 0;
    ota.otaBuildPktTotalLen = 0;

    ota.otaCmdFlag = 0;

    memset(&ota.l2capRfcChannel, 0, sizeof(ota.l2capRfcChannel));
    memset(&ota.l2capSdpChannel, 0, sizeof(ota.l2capSdpChannel));
    
    ota.curPsm = 0;
    
    LOG_FUNC_OUT();
}

void otaMainInit(void)
{
    LOG_FUNC_IN();

    otaDataInit();

    otaRxBufInit();

    if (ota_app_mutex_id == NULL)
        ota_app_mutex_id = osMutexCreate((osMutex(ota_app_mutex)));

    ota_receive_queue_id = osMessageCreate(osMessageQ(ota_receive_queue), NULL);
    ota_receive_tid = osThreadCreate(osThread(OtaReceiveThread), NULL);

    ota_send_queue_id = osMessageCreate(osMessageQ(ota_send_queue), NULL);
    ota_send_tid = osThreadCreate(osThread(OtaSendThread), NULL);

    LOG_MSG(1,"OtaReceiveThread: %x\n", (uint32_t)ota_receive_tid);
    LOG_MSG(1,"OtaSendThread: %x\n", (uint32_t)ota_send_tid);

    osDelay(200);

    LOG_FUNC_OUT();
}
