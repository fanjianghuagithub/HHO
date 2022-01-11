#ifndef _OTA_SPP_H_
#define _OTA_SPP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* RFCOMM Control Fields and events */
#define RFC_SABM    0x3F
#define RFC_UA_F    0x73
#define RFC_DM_F    0x1F
#define RFC_DM      0x0F
#define RFC_DISC    0x53
#define RFC_UIH     0xEF
#define RFC_UIH_F   0xFF


/* -- L2CAP Signal Codes -----------------------------------------------------
 *
 * The following defines are used as events to the L2CAP state machine. The 
 * codes 0x01 through 0x0B match the opcodes of the respective L2CAP signalling
 * commands. The remainder of the defines are for events used internally.
 */
#define LLC_COMMAND_REJ     0x01
#define LLC_CONN_REQ        0x02
#define LLC_CONN_RSP        0x03
#define LLC_CONFIG_REQ      0x04
#define LLC_CONFIG_RSP      0x05
#define LLC_DISC_REQ        0x06
#define LLC_DISC_RSP        0x07
#define LLC_ECHO_REQ        0x08
#define LLC_ECHO_RSP        0x09
#define LLC_INFO_REQ        0x0A
#define LLC_INFO_RSP        0x0B

/*---------------------------------------------------------------------------
 * L2ExtendedFeatures type
 *
 *     Defines the L2CAP Extended Feature Mask values found in an L2CAP 
 *     Information Response. The response value is sent in Little Endian and
 *     may need conversion before being compared with the mask values. If a
 *     bit is set in the response, the corresponding feature is present.
 */
typedef U32 L2ExtendedFeatures;

/* L2CAP Flow Control is supported */
#define L2EXTENDED_FEATURE_FLOW_CONTROL         0x00000001

/* L2CAP Retransmission is supported */
#define L2EXTENDED_FEATURE_RETRANSMISSIONS      0x00000002

/* L2CAP bi-directional QoS is supported */
#define L2EXTENDED_FEATURE_BIDIRECTIONAL_QOS    0x00000004

/* L2CAP Enhanced Retransmission is supported */
#define L2EXTENDED_FEATURE_ERTM                 0x00000008

/* L2CAP Streaming is supported */
#define L2EXTENDED_FEATURE_STREAMING            0x00000010

/* L2CAP FCS Options are supported */
#define L2EXTENDED_FEATURE_FCS                  0x00000020

/* End of L2ExtendedFeatures */


/*---------------------------------------------------------------------------
 * L2capInfoType type
 *
 *     Defines the types of information that can be queried from a remote
 *     L2CAP entity with L2CAP_GetInfo.
 */
typedef U16 L2capInfoType;

#define L2INFO_CONNECTIONLESS_MTU   0x0001

#define L2INFO_EXTENDED_FEATURES    0x0002

/* End of L2capInfoType */


/*---------------------------------------------------------------------------
 * L2capConfigFlags type
 * 
 */
typedef U16 L2capConfigFlags;

/* No flags. */
#define L2CFG_FLAG_NONE     0x0000

/* More/Continuation bit in configuration packets. */
#define L2CFG_FLAG_MORE     0x0001

/* End of L2capConfigFlags */

/*---------------------------------------------------------------------------
 * L2capConfigResult type
 * 
 *     Describes the result of configuration negotiation. This type is
 *     indicated during L2EVENT_CONFIG_CNF events. It is also used when
 *     sending a L2CAP Configure Response.
 */
typedef U16 L2capConfigResult;

/* Requested parameters are acceptable */
#define L2CFG_RESULT_ACCEPTED      0x0000

/* Requested parameters are unacceptable */
#define L2CFG_RESULT_UNACCEPTABLE  0x0001

/* Request rejected - unspecified reason */
#define L2CFG_RESULT_REJECTED      0x0002

/* Unknown option(s) were present */
#define L2CFG_RESULT_UNKN_OPTIONS  0x0003   

/* End of L2capConfigResult */


//#define L2CAP_SIG_INFO_REQ  0x0a
//#define L2CAP_SIG_INFO_RES  0x0b


/* L2CAP Predefined CIDs  (0x0004-0x003E Reserved)
*/
#define L2CAP_SIGNALLING_CID            0x0001
#define L2CAP_BASE_APPL_CID             0x0040

#define OTA_SPP_DEFAULT_MTU_SIZE			672


typedef uint16_t L2capChannelId;

#define PSM_SDP     0x0001
#define PSM_RFCOMM  0x0003

extern void L2capSendConfigReq(void);
extern void RfcSendMSReq(void);
void RfcSendData(uint8_t *data, uint16_t len);

extern void otaBtL2capParse(uint8_t* data, uint16_t len, uint16_t hciHandle);

#ifdef __cplusplus
}
#endif

#endif
