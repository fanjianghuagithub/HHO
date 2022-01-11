#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "os_api.h"
#include "bt_if.h"
#include "app_bt.h"
#include "app_spp.h"
#include "spp_api.h"
#include "sdp_api.h"
#include "app_bt_func.h"
#include "app_rfcomm_mgr.h"
#ifdef IBRT
#include "app_tws_ibrt.h"
#include "app_tws_if.h"
#include "app_tws_ctrl_thread.h"
#include "app_ibrt_customif_cmd.h"
#endif
#ifdef MEDIA_PLAYER_SUPPORT
#include "resources.h"
#include "app_media_player.h"
#endif

#ifdef GFPS_ENABLED
#include "app_fp_rfcomm.h"
#include "app_gfps.h"

osMutexDef(fp_rfcomm_mutex);
osMutexDef(fp_rfcomm_credit_mutex);


#define FP_RFCOMM_TX_PKT_CNT 6

/* 128 bit UUID in Big Endian df21fe2c-2515-4fdb-8886-f12c4d67927c */
static const uint8_t FP_RFCOMM_UUID_128[16] = {
    0x7C, 0x92, 0x67, 0x4D, 0x2C, 0xF1, 0x86, 0x88, 0xDB, 0x4F, 0x15, 0x25, 0x2C, 0xFE, 0x21, 0xDF};

typedef struct
{
    uint8_t    isConnected;
    int8_t     serviceIndex;
    uint8_t    isRfcommInitialized;
} FpRFcommServiceEnv_t;

typedef union
{
    struct
    {
        uint8_t     isCompanionAppInstalled :   1;
        uint8_t     isSilentModeSupported   :   1;
        uint8_t     reserve                 :   6;
    } env;
    uint8_t content;
} FpCapabilitiesEnv_t;

typedef struct
{
    uint8_t isRightRinging  :   1;
    uint8_t isLeftRinging   :   1;
    uint8_t reserve         :   6;
} FpRingStatus_t;

static FpRFcommServiceEnv_t fp_rfcomm_service = {false, -1, false};

static FpCapabilitiesEnv_t fp_capabilities = {false, false, 0};

static __attribute__((unused)) FpRingStatus_t fp_ring_status = {false, false, 0};

btif_sdp_record_t* fpSppSdpRecord;

extern "C" void     app_gfps_get_battery_levels(uint8_t *pCount, uint8_t *pBatteryLevel);
extern "C" uint8_t *appm_get_current_ble_addr(void);

static int fp_rfcomm_data_received(void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen);
static void app_fp_msg_send_active_components_rsp(void);
static void app_fp_msg_send_message_ack(uint8_t msgGroup, uint8_t msgCode);
static void app_fp_msg_send_message_nak(uint8_t reason, uint8_t msgGroup, uint8_t msgCode);
static void fp_rfcomm_data_handler(uint8_t* ptr, uint16_t len);
// update this value if the maximum possible tx data size is bigger than current value
#define FP_RFCOMM_TX_BUF_CHUNK_SIZE 64
#define FP_RFCOMM_TX_BUF_CHUNK_CNT FP_RFCOMM_TX_PKT_CNT
#define FP_RFCOMM_TX_BUF_SIZE (FP_RFCOMM_TX_BUF_CHUNK_CNT * FP_RFCOMM_TX_BUF_CHUNK_SIZE)

static uint32_t fp_rfcomm_tx_buf_next_allocated_chunk = 0;
static uint32_t fp_rfcomm_tx_buf_allocated_chunk_cnt  = 0;
static uint8_t  fp_rfcomm_tx_buf[FP_RFCOMM_TX_BUF_CHUNK_CNT][FP_RFCOMM_TX_BUF_CHUNK_SIZE];

static uint8_t *fp_rfcomm_tx_buf_addr(uint32_t chunk)
{
    return fp_rfcomm_tx_buf[chunk];
}

static int32_t fp_rfcomm_alloc_tx_chunk(void)
{
    uint32_t lock = int_lock_global();
    
    if (fp_rfcomm_tx_buf_allocated_chunk_cnt >= FP_RFCOMM_TX_BUF_CHUNK_CNT)
    {
        int_unlock_global(lock);
        return -1;
    }

    uint32_t returnedChunk = fp_rfcomm_tx_buf_next_allocated_chunk; 
    
    fp_rfcomm_tx_buf_allocated_chunk_cnt++;
    fp_rfcomm_tx_buf_next_allocated_chunk++;
    if (FP_RFCOMM_TX_BUF_CHUNK_CNT == fp_rfcomm_tx_buf_next_allocated_chunk)
    {
        fp_rfcomm_tx_buf_next_allocated_chunk = 0;
    }

    int_unlock_global(lock);
    return returnedChunk;
}

static bool fp_rfcomm_free_tx_chunk(void)
{
    uint32_t lock = int_lock_global();
    if (0 == fp_rfcomm_tx_buf_allocated_chunk_cnt)
    {
        int_unlock_global(lock);
        return false;
    }

    fp_rfcomm_tx_buf_allocated_chunk_cnt--;
    int_unlock_global(lock);
    return true;
}

static void fp_rfcomm_reset_tx_buf(void)
{
    uint32_t lock = int_lock_global();
    fp_rfcomm_tx_buf_allocated_chunk_cnt = 0;
    fp_rfcomm_tx_buf_next_allocated_chunk = 0;
    int_unlock_global(lock);
}


#include "app_status_ind.h"
#include "apps.h"
osTimerId ring_timeout_timer_id = NULL;
static void gfps_find_devices_ring_timeout_handler(void const *param);
osTimerDef (GFPS_FIND_DEVICES_RING_TIMEOUT, gfps_find_devices_ring_timeout_handler);
#define GFPS_FIND_MY_BUDS_CMD_STOP_DUAL 0x0
#define GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY 0x1
#define GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY 0x2
#define GFPS_FIND_MY_BUDS_CMD_START_DUAL 0x3

static bool left_is_ring=0;
static bool right_is_ring=0;
static void gfps_set_find_my_buds(uint8_t cmd)
{
    TRACE(2,"%s, cmd = %d", __func__, cmd);
    if(GFPS_FIND_MY_BUDS_CMD_STOP_DUAL == cmd)
    {
        app_gfps_ring_mode_set(GFPS_RING_MODE_BOTH_OFF);
        app_gfps_find_sm(false);
        left_is_ring=0;
        right_is_ring=0;
    }
    else if(GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY == cmd)    //right ring, stop left
    {
        app_gfps_ring_mode_set(GPFS_RING_MODE_RIGHT_ON);
        if(app_tws_is_left_side())
        {
            app_gfps_find_sm(false);
        }
        else
        {
            app_gfps_find_sm(true);
        }
        right_is_ring=1;
        left_is_ring=0;
    }
    else if(GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY == cmd)    //left ring, stop right
    {
        app_gfps_ring_mode_set(GFPS_RING_MODE_LEFT_ON);
        if(app_tws_is_left_side())
        {
            app_gfps_find_sm(true);
        }
        else
        {
            app_gfps_find_sm(false);
        }
        left_is_ring=1;
        right_is_ring=0;
    }
    else if(GFPS_FIND_MY_BUDS_CMD_START_DUAL == cmd)    //both ring
    {
        app_gfps_ring_mode_set(GFPS_RING_MODE_BOTH_ON);
        app_gfps_find_sm(true);
        left_is_ring=1;
        right_is_ring=1;
    }
}
static void gfps_find_devices_ring_timeout_handler(void const *param)
{
    TRACE(0,"gfps_find_devices_ring_timeout_handler");
    app_bt_start_custom_function_in_bt_thread(GFPS_FIND_MY_BUDS_CMD_STOP_DUAL, 0, \
                                (uint32_t)gfps_set_find_my_buds);
}

void fp_rfcomm_ring_timer_set(uint8_t period)
{
    TRACE(2,"%s, period = %d", __func__, period);
    if (ring_timeout_timer_id == NULL)
    {
        ring_timeout_timer_id = osTimerCreate(osTimer(GFPS_FIND_DEVICES_RING_TIMEOUT), osTimerOnce, NULL);
    }
    
    osTimerStop(ring_timeout_timer_id);
    if(period)
    {
        osTimerStart(ring_timeout_timer_id, period*1000);
    }
}

static void fp_rfcomm_ring_request_handling(uint8_t* requestdata, uint16_t datalen)
{
    TRACE(1,"%s,[RFCOMM][FMD] request",__func__);
    DUMP8("%02x ", requestdata, datalen);
    app_fp_msg_send_message_ack(FP_MSG_GROUP_DEVICE_ACTION, FP_MSG_DEVICE_ACTION_RING);
     if (datalen > 1)
    {
        fp_rfcomm_ring_timer_set(requestdata[1]);
    }

    gfps_set_find_my_buds(requestdata[0]);
}

#define FP_ACCUMULATED_DATA_BUF_SIZE    128
static uint8_t fp_accumulated_data_buf[FP_ACCUMULATED_DATA_BUF_SIZE];
static uint16_t fp_accumulated_data_size = 0;

static void fp_rfcomm_reset_data_accumulator(void)
{
    fp_accumulated_data_size = 0;
    memset(fp_accumulated_data_buf, 0, sizeof(fp_accumulated_data_buf));
}

static void fp_rfcomm_data_accumulator(uint8_t* ptr, uint16_t len)
{
    ASSERT((fp_accumulated_data_size + len) < sizeof(fp_accumulated_data_buf),
        "fp accumulcate buffer is overflow!");

    memcpy(&fp_accumulated_data_buf[fp_accumulated_data_size], ptr, len);
    fp_accumulated_data_size += len;

    uint16_t msgTotalLen;
    FP_MESSAGE_STREAM_T* msgStream;
    
    while (fp_accumulated_data_size >= FP_MESSAGE_RESERVED_LEN)
    {
        msgStream = (FP_MESSAGE_STREAM_T *)fp_accumulated_data_buf;
        msgTotalLen = ((msgStream->dataLenHighByte << 8)|msgStream->dataLenLowByte) + 
            FP_MESSAGE_RESERVED_LEN;
        ASSERT(msgTotalLen < sizeof(fp_accumulated_data_buf),
            "Wrong fp msg len %d received!", msgTotalLen);
        if (fp_accumulated_data_size >= msgTotalLen)
        {
            fp_rfcomm_data_handler(fp_accumulated_data_buf, msgTotalLen);
            fp_accumulated_data_size -= msgTotalLen;
            memmove(fp_accumulated_data_buf, &fp_accumulated_data_buf[msgTotalLen],
                fp_accumulated_data_size);
        }
        else
        {
            break;
        }
    }
}

static void fp_rfcomm_data_handler(uint8_t* ptr, uint16_t len)
{
    FP_MESSAGE_STREAM_T* pMsg = (FP_MESSAGE_STREAM_T *)ptr;
    uint16_t datalen = 0;
    TRACE(2,"fp rfcomm receives msg group %d code %d", 
        pMsg->messageGroup, pMsg->messageCode);

    DUMP8("%02x ",ptr,len);
    switch (pMsg->messageGroup)
    {
        case FP_MSG_GROUP_DEVICE_INFO:
        {
            switch (pMsg->messageCode)
            {
                case FP_MSG_DEVICE_INFO_ACTIVE_COMPONENTS_REQ:
                    app_fp_msg_send_active_components_rsp();
                    break;
                case FP_MSG_DEVICE_INFO_TELL_CAPABILITIES:
                    fp_capabilities.content = pMsg->data[0];
                    TRACE(3,"cap 0x%x isCompanionAppInstalled %d isSilentModeSupported %d",
                        fp_capabilities.content, fp_capabilities.env.isCompanionAppInstalled,
                        fp_capabilities.env.isSilentModeSupported);
                    break;
                default:
                    break;
            }
        }
        /*FALL THROUGH*/
        
        case FP_MSG_GROUP_DEVICE_ACTION:
        {
            switch (pMsg->messageCode)
            {
                case FP_MSG_DEVICE_ACTION_RING:
                    datalen = (pMsg->dataLenHighByte<<8)+pMsg->dataLenLowByte;
                    fp_rfcomm_ring_request_handling(pMsg->data, datalen);
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

static int fp_rfcomm_data_received(void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen)
{
    TRACE(1,"%s",__func__);
    //DUMP8("0x%02x ",pData, dataLen);
    fp_rfcomm_data_accumulator(pData, dataLen);
    return 0;
}

static void app_fp_sync_ring_status_to_mobile(uint8_t ring_status)
{
    TRACE(2,"%s %d",__func__,ring_status);
    FP_MESSAGE_STREAM_T  ring_cmd;
    ring_cmd.messageGroup = FP_MSG_GROUP_DEVICE_ACTION;
    ring_cmd.messageCode = FP_MSG_DEVICE_ACTION_RING;
    ring_cmd.dataLenHighByte=0x00;
    ring_cmd.dataLenLowByte=0x01;
    ring_cmd.data[0]=ring_status;
    app_fp_rfcomm_send((uint8_t *)&ring_cmd, FP_MESSAGE_RESERVED_LEN+1);
}

void app_fp_master_stop_ring_buds()
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    uint8_t send_cmd=0;
    if(p_ibrt_ctrl->current_role != IBRT_MASTER)
    {
        return;
    }

    if((!app_tws_is_left_side())&&(right_is_ring))//right need stop ring
    {
        if(left_is_ring)
        {
            send_cmd=GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY;
        }
        else
        {
            send_cmd=GFPS_FIND_MY_BUDS_CMD_STOP_DUAL;
        }
        app_gfps_find_sm(false);
        app_fp_sync_ring_status_to_mobile(send_cmd);
        right_is_ring=0;
    }
    else if(app_tws_is_left_side()&&(left_is_ring))//left need stop ring
    {
        if(right_is_ring)
        {
            send_cmd=GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY;
        }
        else
        {
            send_cmd=GFPS_FIND_MY_BUDS_CMD_STOP_DUAL;
        }
        app_gfps_find_sm(false);
        app_fp_sync_ring_status_to_mobile(send_cmd);
        left_is_ring=0;
    }
}

void app_fp_slave_stop_ring_buds()
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    uint8_t ring_status;
    if(p_ibrt_ctrl->current_role != IBRT_SLAVE)
    {
        return;
    }
    TRACE(3,"%s %d %d",__func__,left_is_ring,right_is_ring);
    if((!app_tws_is_left_side())&&(right_is_ring))
    {
        ring_status=1;
        tws_ctrl_send_cmd(APP_TWS_CMD_GFPS_RING_STOP_SYNC, &ring_status, 1);
        app_gfps_find_sm(false);
    }
    else if(app_tws_is_left_side()&&(left_is_ring))
    {
        ring_status=1;
        tws_ctrl_send_cmd(APP_TWS_CMD_GFPS_RING_STOP_SYNC, &ring_status, 1);
        app_gfps_find_sm(false);
    }
}

void app_fp_master_handle_slave_stop_ring_buds()
{
    TRACE(3,"%s left=%d right=%d",__func__,left_is_ring,right_is_ring);
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    uint8_t send_cmd=0;
    if(p_ibrt_ctrl->current_role != IBRT_MASTER)
    {
        return;
    }

    if((!app_tws_is_left_side())&&(left_is_ring))//;left need stop ring
    {
        if(right_is_ring)
        {
            send_cmd=GFPS_FIND_MY_BUDS_CMD_START_MASTER_ONLY;
        }
        else
        {
            send_cmd=GFPS_FIND_MY_BUDS_CMD_STOP_DUAL;
        }
        left_is_ring=0;
        app_fp_sync_ring_status_to_mobile(send_cmd);
    }
    else if(app_tws_is_left_side()&&(right_is_ring))//right need stop ring
    {
        if(left_is_ring)
        {
            send_cmd=GFPS_FIND_MY_BUDS_CMD_START_SLAVE_ONLY;
        }
        else
        {
            send_cmd=GFPS_FIND_MY_BUDS_CMD_STOP_DUAL;
        }
        right_is_ring=0;
        app_fp_sync_ring_status_to_mobile(send_cmd);
    }
}

static void fp_rfcomm_connected_handler(void)
{
    if (!fp_rfcomm_service.isConnected)
    {
        fp_rfcomm_service.isConnected = true;

        fp_rfcomm_reset_data_accumulator();
        app_fp_msg_send_model_id();
        app_fp_msg_send_updated_ble_addr();
        app_fp_msg_send_battery_levels();
    }
}

static bool fp_rfcomm_callback(RFCOMM_EVENT_E event, 
    uint8_t instanceIndex, uint16_t connHandle,
    uint8_t* pBtAddr, uint8_t* pSentDataBuf, uint16_t sentDataLen)
{
    TRACE(2,"%s,event is %d",__func__,event);
    switch (event)
    {
        case RFCOMM_INCOMING_CONN_REQ:
        {
            TRACE(0,"Connected Indication RFComm device info:");
            TRACE(2,"hci handle is 0x%x service index %d", 
                connHandle, instanceIndex);
            if (pBtAddr)
            {
                TRACE(0,"Bd addr is:");
                DUMP8("%02x ", pBtAddr, 6);     
            }

            fp_rfcomm_connected_handler();
            break;
        }
        case RFCOMM_CONNECTED:
        {
            if (pBtAddr)
            {
              TRACE(0,"Bd addr is:");
              DUMP8("%02x ", pBtAddr, 6);
            }
        
            fp_rfcomm_connected_handler();
            break;
        }
        case RFCOMM_DISCONNECTED:
        {
            TRACE(0,"Disconnected Rfcomm device info:");
            TRACE(0,"Bd addr is:");
            DUMP8("%02x ", pBtAddr, 6);
            TRACE(1,"hci handle is 0x%x", connHandle);

            TRACE(1,"::RFCOMM_DISCONNECTED %d", event);
            
            fp_rfcomm_service.isConnected = false;
            fp_rfcomm_reset_tx_buf();
            break;
        }
        case RFCOMM_TX_DONE:
        {
            TRACE(1,"Rfcomm dataLen %d sent out", sentDataLen);
            fp_rfcomm_free_tx_chunk();
            break;
        }
        default:
        {
            TRACE(1,"Unkown rfcomm event %d", event);
            break;
        }
    }

    return true;
}

static void app_fp_disconnect_rfcomm_handler(void)
{
    if (fp_rfcomm_service.isConnected)
    {
        app_rfcomm_close(fp_rfcomm_service.serviceIndex);
    }
}

void app_fp_disconnect_rfcomm(void)
{
    app_bt_start_custom_function_in_bt_thread(0,
                                           0,
                                           ( uint32_t )app_fp_disconnect_rfcomm_handler);
}

static void app_fp_rfcomm_send_handler(uint8_t *ptrData, uint32_t length)
{
    int8_t ret = app_rfcomm_write(fp_rfcomm_service.serviceIndex, ptrData, length);
    if (0 != ret)
    {
        fp_rfcomm_free_tx_chunk();
    }
}

void app_fp_rfcomm_send(uint8_t *ptrData, uint32_t length)
{
    if (!fp_rfcomm_service.isConnected)
    {
        return;
    }

    int32_t chunk = fp_rfcomm_alloc_tx_chunk();
    if (-1 == chunk)
    {
        TRACE(0,"Fast pair rfcomm tx buffer used out!");
        return;
    }

    ASSERT(length < FP_RFCOMM_TX_BUF_CHUNK_SIZE,
           "FP_RFCOMM_TX_BUF_CHUNK_SIZE is %d which is smaller than %d, need to increase!",
           FP_RFCOMM_TX_BUF_CHUNK_SIZE,
           length);

    uint8_t *txBufAddr = fp_rfcomm_tx_buf_addr(chunk);
    memcpy(txBufAddr, ptrData, length);
    app_bt_start_custom_function_in_bt_thread(( uint32_t )txBufAddr,
                                           ( uint32_t )length,
                                           ( uint32_t )app_fp_rfcomm_send_handler);
}

bt_status_t app_fp_rfcomm_init(void)
{
    TRACE(1,"%s",__func__);
    bt_status_t stat = BT_STS_SUCCESS;

    if (!fp_rfcomm_service.isRfcommInitialized)
    {
        osMutexId mid;
        mid = osMutexCreate(osMutex(fp_rfcomm_mutex));
        if (!mid) {
            ASSERT(0, "cannot create mutex");
        }

        fp_rfcomm_service.isRfcommInitialized = true;
        
        RFCOMM_CONFIG_T tConfig;
        tConfig.callback = fp_rfcomm_callback;
        tConfig.tx_pkt_cnt = FP_RFCOMM_TX_PKT_CNT;
        tConfig.rfcomm_128bit_uuid = FP_RFCOMM_UUID_128;
        tConfig.rfcomm_ch = RFCOMM_CHANNEL_FP;
        tConfig.app_id = BTIF_APP_SPP_SERVER_FP_RFCOMM_ID;
        tConfig.spp_handle_data_event_func = fp_rfcomm_data_received;
        tConfig.mutexId = mid;
        tConfig.creditMutexId = osMutexCreate(osMutex(fp_rfcomm_credit_mutex));
        int8_t index = app_rfcomm_open(&tConfig);

        if (-1 == index)
        {            
            TRACE(0,"fast pair rfcomm open failed");
            return BT_STS_FAILED;           
        }

        fp_rfcomm_service.isConnected = false;
        fp_rfcomm_service.serviceIndex = index;
    }
    else
    {
        TRACE(0,"already initialized.");
    }

    return stat;
}

bool app_is_fp_rfcomm_connected(void)
{
    return fp_rfcomm_service.isConnected;
}

// use cases for fp message stream
void app_fp_msg_enable_bt_silence_mode(bool isEnable)
{
    if (fp_capabilities.env.isSilentModeSupported)
    {
        FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_BLUETOOTH_EVENT, 0, 0, 0};
        if (isEnable)
        {
            req.messageCode = FP_MSG_BT_EVENT_ENABLE_SILENCE_MODE;
        }
        else
        {
            req.messageCode = FP_MSG_BT_EVENT_DISABLE_SILENCE_MODE;
        }

        app_fp_rfcomm_send((uint8_t *)&req, FP_MESSAGE_RESERVED_LEN);
    }
    else
    {
        TRACE(0,"fp silence mode is not supported.");
    }
}

void app_fp_msg_send_model_id(void)
{
    TRACE(1,"%s",__func__);
#ifndef IS_USE_CUSTOM_FP_INFO
    uint32_t model_id = 0x2B677D;
#else
    uint32_t model_id = app_bt_get_model_id();
#endif
    uint8_t  modelID[3];
    modelID[0] = (model_id >> 16) & 0xFF;
    modelID[1] = (model_id >> 8) & 0xFF;
    modelID[2] = ( model_id )&0xFF;

    uint16_t rawDataLen = sizeof(modelID);

    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_DEVICE_INFO,
         FP_MSG_DEVICE_INFO_MODEL_ID,
         (uint8_t)(rawDataLen >> 8),
         (uint8_t)(rawDataLen & 0xFF)};
    memcpy(req.data, modelID, sizeof(modelID));

    app_fp_rfcomm_send(( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + rawDataLen);
}

void app_fp_msg_send_updated_ble_addr(void)
{
    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_DEVICE_INFO,
         FP_MSG_DEVICE_INFO_BLE_ADD_UPDATED,
         0,
         6};

    uint8_t *ptr = appm_get_current_ble_addr();

    for (uint8_t index = 0; index < 6; index++)
    {
        req.data[index] = ptr[5 - index];
    }

    app_fp_rfcomm_send(( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + 6);
}

void app_fp_msg_send_battery_levels(void)
{
    TRACE(1,"%s",__func__);
    FP_MESSAGE_STREAM_T req =
        {FP_MSG_GROUP_DEVICE_INFO,
         FP_MSG_DEVICE_INFO_BATTERY_UPDATED,
         0,
         3};

    uint8_t batteryLevelCount = 0;
    app_gfps_get_battery_levels(&batteryLevelCount, req.data);

    app_fp_rfcomm_send(( uint8_t * )&req, FP_MESSAGE_RESERVED_LEN + 3);
}

static __attribute__((unused)) void app_fp_msg_send_active_components_rsp(void)
{
    FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_DEVICE_INFO, FP_MSG_DEVICE_INFO_ACTIVE_COMPONENTS_RSP, 0, 1};

#if defined(IBRT)
    if (app_tws_ibrt_tws_link_connected())
    {
        req.data[0] = FP_MSG_BOTH_BUDS_ACTIVE;
    }
    else
    {
        if (app_tws_is_left_side())
        {   
            req.data[0] = FP_MSG_LEFT_BUD_ACTIVE;
        }
        else
        {
            req.data[0] = FP_MSG_RIGHT_BUD_ACTIVE;
        }
    }
#else
    req.data[0] = FP_MSG_BOTH_BUDS_ACTIVE;
#endif

    app_fp_rfcomm_send((uint8_t *)&req, FP_MESSAGE_RESERVED_LEN+1);
}

static __attribute__((unused)) void app_fp_msg_send_message_ack(uint8_t msgGroup, uint8_t msgCode)
{
    FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_ACKNOWLEDGEMENT, FP_MSG_ACK, 0, 2};

    req.data[0] = msgGroup;
    req.data[1] = msgCode;

    app_fp_rfcomm_send((uint8_t *)&req, FP_MESSAGE_RESERVED_LEN+2);
}

static __attribute__((unused)) void app_fp_msg_send_message_nak(uint8_t reason, uint8_t msgGroup, uint8_t msgCode)
{
    FP_MESSAGE_STREAM_T req = {FP_MSG_GROUP_ACKNOWLEDGEMENT, FP_MSG_NAK, 0, 3};

    req.data[0] = reason;
    req.data[1] = msgGroup;
    req.data[2] = msgCode;

    app_fp_rfcomm_send((uint8_t *)&req, FP_MESSAGE_RESERVED_LEN+3);
}
#endif
