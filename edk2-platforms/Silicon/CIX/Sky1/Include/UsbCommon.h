/** @file
 *
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 **/

#ifndef _USBCOMMON_H
#define _USBCOMMON_H

#include <Uefi.h>
#include <Protocol/UsbFunctionIo.h>
#include <Protocol/UsbIo.h>
#include <IndustryStandard/Usb.h>
#include <Library/DebugLib.h>

#define TRB_TO_EP_ID(p)  (((p) & GENMASK(20, 16)) >> 16)

#define COMP_INVALID                           0
#define COMP_SUCCESS                           1
#define COMP_DATA_BUFFER_ERROR                 2
#define COMP_BABBLE_DETECTED_ERROR             3
#define COMP_TRB_ERROR                         5
#define COMP_RESOURCE_ERROR                    7
#define COMP_NO_SLOTS_AVAILABLE_ERROR          9
#define COMP_INVALID_STREAM_TYPE_ERROR         10
#define COMP_SLOT_NOT_ENABLED_ERROR            11
#define COMP_ENDPOINT_NOT_ENABLED_ERROR        12
#define COMP_SHORT_PACKET                      13
#define COMP_RING_UNDERRUN                     14
#define COMP_RING_OVERRUN                      15
#define COMP_VF_EVENT_RING_FULL_ERROR          16
#define COMP_PARAMETER_ERROR                   17
#define COMP_CONTEXT_STATE_ERROR               19
#define COMP_EVENT_RING_FULL_ERROR             21
#define COMP_INCOMPATIBLE_DEVICE_ERROR         22
#define COMP_MISSED_SERVICE_ERROR              23
#define COMP_COMMAND_RING_STOPPED              24
#define COMP_COMMAND_ABORTED                   25
#define COMP_STOPPED                           26
#define COMP_STOPPED_LENGTH_INVALID            27
#define COMP_STOPPED_SHORT_PACKET              28
#define COMP_MAX_EXIT_LATENCY_TOO_LARGE_ERROR  29
#define COMP_ISOCH_BUFFER_OVERRUN              31
#define COMP_EVENT_LOST_ERROR                  32
#define COMP_UNDEFINED_ERROR                   33
#define COMP_INVALID_STREAM_ID_ERROR           34

#define USB_CLASS_PER_INTERFACE    0
#define USB_DT_DEVICE_QUALIFIER    0x06
#define USB_DT_OTHER_SPEED_CONFIG  0x07
#define USB_DT_INTERFACE_POWER     0x08
/* these are from a minor usb 2.0 revision (ECN) */
#define USB_DT_OTG                    0x09
#define USB_DT_DEBUG                  0x0a
#define USB_DT_INTERFACE_ASSOCIATION  0x0b
/* these are from the Wireless USB spec */
#define USB_DT_SECURITY                0x0c
#define USB_DT_KEY                     0x0d
#define USB_DT_ENCRYPTION_TYPE         0x0e
#define USB_DT_BOS                     0x0f
#define USB_DT_DEVICE_CAPABILITY       0x10
#define USB_DT_WIRELESS_ENDPOINT_COMP  0x11

#define USB_EP_MAXP_MULT_SHIFT  11
#define USB_EP_MAXP_MULT_MASK   (3 << USB_EP_MAXP_MULT_SHIFT)
#define USB_EP_MAXP_MULT(m) \
  (((m) & USB_EP_MAXP_MULT_MASK) >> USB_EP_MAXP_MULT_SHIFT)

#define OS_STRING_IDX          0xEE
#define OS_STRING_QW_SIGN_LEN  14

#define USB_DIR_OUT  0    /* to device */
#define USB_DIR_IN   0x80 /* to host */

#define PRINTFCH9(Expression)  DEBUG(Expression)
#define DEBUG_CH9_INFO   DEBUG_INFO
#define DEBUG_CH9_ERROR  DEBUG_ERROR

#define USB_TYPE_MASK             (0x03 << 5)
#define USB_RECIP_MASK            0x1f
#define USB_RECIP_DEVICE          0x00
#define USB_RECIP_INTERFACE       0x01
#define USB_RECIP_ENDPOINT        0x02
#define USB_DEVICE_REMOTE_WAKEUP  1

#define USB_ENDPOINT_NUMBER_MASK  0x0f
#define USB_ENDPOINT_DIR_MASK     0x80
#define USB_ENDPOINT_MAXP_MASK    0x07ff

#define USB_DEVICE_REMOTE_WAKEUP  1
#define USB_DEVICE_TEST_MODE      2
#define USB_DEVICE_U1_ENABLE      48
#define USB_DEVICE_U2_ENABLE      49
#define USB_DEVICE_LTM_ENABLE     50

#define USB_INTRF_FUNC_SUSPEND     0
#define USB_INTRF_FUNC_SUSPEND_LP  (1 << (8 + 0))
#define USB_INTRF_FUNC_SUSPEND_RW  (1 << (8 + 1))

#define USB_REQ_SET_ISOCH_DELAY  0x31
#define USB_REQ_SET_SEL          0x30

#define EP_STATE_MASK      (0x7)
#define EP_STATE_DISABLED  0
#define EP_STATE_RUNNING   1
#define EP_STATE_HALTED    2
#define EP_STATE_STOPPED   3
#define EP_STATE_ERROR     4
#define GET_EP_CTX_STATE(ctx)  (((ctx)->EpInfo) & EP_STATE_MASK)

#define TRB_NORMAL        1
#define TRB_SETUP         2
#define TRB_DATA          3
#define TRB_STATUS        4
#define TRB_ISOC          5
#define TRB_LINK          6
#define TRB_EVENT_DATA    7
#define TRB_TR_NOOP       8
#define TRB_ENABLE_SLOT   9
#define TRB_DISABLE_SLOT  10
#define TRB_ADDR_DEV      11
#define TRB_CONFIG_EP     12
#define TRB_EVAL_CONTEXT  13
#define TRB_RESET_EP      14
#define TRB_STOP_RING     15
#define TRB_SET_DEQ       16
#define TRB_RESET_DEV     17
#define TRB_FORCE_EVENT   18
#define TRB_FORCE_HEADER  22
#define TRB_CMD_NOOP      23
#define TRB_TRANSFER      32
#define TRB_COMPLETION    33
#define TRB_PORT_STATUS   34
#define TRB_HC_EVENT      37
#define TRB_MFINDEX_WRAP  39

#define UsbBusSpeedSuperPlus  (UsbBusSpeedSuper + 1)

#define USB_EP_CAPS_TYPE_CONTROL  0x01
#define USB_EP_CAPS_TYPE_ISO      0x02
#define USB_EP_CAPS_TYPE_BULK     0x04
#define USB_EP_CAPS_TYPE_INT      0x08
#define USB_EP_CAPS_TYPE_ALL \
  (USB_EP_CAPS_TYPE_ISO | USB_EP_CAPS_TYPE_BULK | USB_EP_CAPS_TYPE_INT)
#define USB_EP_CAPS_DIR_IN   0x01
#define USB_EP_CAPS_DIR_OUT  0x02
#define USB_EP_CAPS_DIR_ALL  (USB_EP_CAPS_DIR_IN | USB_EP_CAPS_DIR_OUT)

#define _Packed  __attribute__ ((__packed__))

typedef struct _Packed {
  UINT8     Length;
  UINT8     DescriptorType;
  UINT16    TotalLength;
  UINT8     NumDeviceCaps;
} USB_BOS_DESCRIPTOR;

typedef struct _Packed {
  UINT8    BLength;
  UINT8    BDescriptorType;
  UINT8    BcdUSB;
  UINT8    BDeviceClass;
  UINT8    BDeviceSubClass;
  UINT8    BDeviceProtocol;
  UINT8    BMaxPacketSize0;
  UINT8    BNumConfigurations;
  UINT8    BRESERVED;
} USB_QUALIFIER_DESCRIPTOR;

typedef struct _Packed {
  UINT8     Length;
  UINT8     DescriptorType;
  #define USB_CAP_TYPE_EXT  2
  #define USB_SS_CAP_TYPE   3
  UINT8     DevCapabilityType;
  UINT32    Attributes;
  #define USB_LPM_SUPPORT          (1 << 1) /* supports LPM */
  #define USB_BESL_SUPPORT         (1 << 2) /* supports BESL */
  #define USB_BESL_BASELINE_VALID  (1 << 3) /* Baseline BESL valid*/
  #define USB_BESL_DEEP_VALID      (1 << 4) /* Deep BESL valid */
  #define USB_SET_BESL_BASELINE(p)  (((p) & 0xf) << 8)
  #define USB_SET_BESL_DEEP(p)      (((p) & 0xf) << 12)
  #define USB_GET_BESL_BASELINE(p)  (((p) & (0xf << 8)) >> 8)
  #define USB_GET_BESL_DEEP(p)      (((p) & (0xf << 12)) >> 12)
} USB_EXT_CAP_DESCRIPTOR;

typedef struct _Packed {
  UINT8     Length;
  UINT8     DescriptorType;
  UINT8     DevCapabilityType;
  UINT8     bmAttributes;
  #define USB_LTM_SUPPORT  (1 << 1)   /* supports LTM */
  UINT16    wSpeedSupported;
  #define USB_LOW_SPEED_OPERATION   (1)      /* Low speed operation */
  #define USB_FULL_SPEED_OPERATION  (1 << 1) /* Full speed operation */
  #define USB_HIGH_SPEED_OPERATION  (1 << 2) /* High speed operation */
  #define USB_5GBPS_OPERATION       (1 << 3) /* Operation at 5Gbps */
  UINT8     bFunctionalitySupport;
  #define USB_DEFAULT_U1_DEV_EXIT_LAT  0x01   /* Less then 1 microsec */
  #define USB_DEFAULT_U2_DEV_EXIT_LAT  0x01F4 /* Less then 500 microsec */
  UINT8     bU1devExitLat;
  UINT16    bU2DevExitLat;
} USB_SS_CAP_DESCRIPTOR;

typedef struct {
  UINT8    Length;
  UINT8    DescriptorType;
  UINT8    QwSignature[OS_STRING_QW_SIGN_LEN];
  UINT8    MsVendorCode;
  UINT8    Pad;
} USB_OS_STRING;

typedef enum {
  ENDPOINT_COMPLETE = 0,
  ENDPOINT_INPROCESS,
  ENDPOINT_CONNRESET,
  ENDPOINT_SHUTDONE
} ENDPOINT_STATUS;

typedef enum _USB_DEVICE_STATE {
  /* NOTATTACHED isn't in the USB spec, and this state acts
   * the same as ATTACHED ... but it's clearer this way.
   */
  USB_STATE_NOTATTACHED = 0,
  /* chapter 9 and authentication (wireless) device states */
  USB_STATE_ATTACHED,
  USB_STATE_POWERED,         /* wired */
  USB_STATE_RECONNECTING,    /* auth */
  USB_STATE_UNAUTHENTICATED, /* auth */
  USB_STATE_DEFAULT,         /* limited function */
  USB_STATE_ADDRESS,
  USB_STATE_CONFIGURED,   /* most functions */
  USB_STATE_SUSPENDED

  /* NOTE:  there are actually four different SUSPENDED
   * states, returning to POWERED, DEFAULT, ADDRESS, or
   * CONFIGURED respectively when SOF tokens flow again.
   * At this level there's no difference between L1 and L2
   * suspend states.  (L2 being original USB 1.1 suspend.)
   */
} USB_DEVICE_STATE;

typedef struct {
  UINT32    TypeControl : 1;
  UINT32    TypeIso     : 1;
  UINT32    TypeBulk    : 1;
  UINT32    TypeInt     : 1;
  UINT32    DirIn       : 1;
  UINT32    DirOut      : 1;
} USB_EP_CAPS;

typedef struct _Packed {
  UINT8     BLength;
  #define USB_DT_SS_ENDPOINT_COMP  0x30
  UINT8     BDescriptorType;
  UINT8     BMaxBurst;
  UINT8     BmAttributes;
  UINT16    WBytesPerInterval;
} USB_SS_ENDPOINT_COMP_DES;

typedef struct {
  USB_EP_CAPS                 Caps;
  BOOLEAN                     Claimed;
  BOOLEAN                     Enabled;
  UINT32                      MaxPacket      : 16;
  UINT32                      MaxPacketLimit : 16;
  UINT32                      MaxStreams     : 16;
  UINT32                      Mult           : 2;
  UINT32                      MaxBurst       : 5;
  UINT8                       Address;
  USB_ENDPOINT_DESCRIPTOR     *Des;
  USB_SS_ENDPOINT_COMP_DES    *SsCompDes;
} USB_EP;

typedef struct {
  LIST_ENTRY    Link;
  VOID          *Buf;
  UINT32        Length;
  UINT32        StreamId    : 16;
  UINT32        IsLast      : 1;
  UINT32        NoInterrupt : 1;
  UINT32        Zero        : 1;
  UINT32        ShortNotOk  : 1;
  INT32         Status;
  UINT32        Actual;
} USB_REQUEST;

STATIC
inline
UINT32
UsbEndpointType (
  USB_ENDPOINT_DESCRIPTOR  *Des
  )
{
  return Des->Attributes & USB_ENDPOINT_TYPE_MASK;
}

#define USB_ENDPOINT_MAXP_MASK  0x07ff

UINT32
UsbEndpointMaxPacket (
  USB_ENDPOINT_DESCRIPTOR  *Des
  );

VOID
UsbEpSetMaxpacketLimit (
  USB_EP  *Ep,
  UINT32  MaxpacketLimit
  );

typedef struct _STD_USB_DEVICE_OPS {
  EFI_STATUS (*SetAddress)(
    UINT32  Addr
    );
  EFI_STATUS (*SetConfig)(
    VOID                *ConfigDesc,
    USB_DEVICE_REQUEST  *CtrlRequest
    );
  EFI_STATUS (*ResetConfig)(
    EFI_USB_DEVICE_REQUEST  *CtrlRequest
    );
  EFI_STATUS (*SendCtrlResponse)(
    INT32  BufferLength,
    VOID   *Buffer
    );
  EFI_STATUS (*GetSpeedInfo)(
    UINT8  *MaxSpeed,
    UINT8  *Speed
    );
  VOID (*SetMem)(
    VOID   *Buffer,
    UINTN  Length,
    UINT8  Value
    );
  VOID (*CopyMem)(
    VOID   *Destination,
    VOID   *Source,
    UINTN  Length
    );
  VOID *(*GetCtrlResponseBuffer)(VOID);
} STD_USB_DEVICE_OPS;

typedef struct _USB_STD_DEV {
  USB_DEVICE_DESCRIPTOR        *DeviceDesc;
  VOID                         *ConfigDesc;
  EFI_USB_STRING_DESCRIPTOR    **StringDescritors;
  UINT8                        ValidStringIndex;
  UINT8                        Config;
  USB_DEVICE_STATE             DeviceState;
  STD_USB_DEVICE_OPS           *Ops;
} USB_STD_DEV;

VOID
InitUsbStdDev (
  USB_DEVICE_DESCRIPTOR      *DeviceDesc,
  EFI_USB_STRING_DESCRIPTOR  **StringDescritors,
  UINT8                      ValidStringIndex,
  STD_USB_DEVICE_OPS         *Ops
  );

VOID *
GetConfigDesc (
  VOID
  );

EFI_STATUS
HandleCh9Request (
  USB_DEVICE_REQUEST  *CtrlRequest
  );

EFI_STATUS
HandleEp0NoStdRequest (
  USB_DEVICE_REQUEST  *CtrRequest,
  UINT8               *Buffer
  );

VOID
SetDeviceState (
  USB_DEVICE_STATE  State
  );

USB_DEVICE_STATE
GetDeviceState (
  VOID
  );

VOID
SetBcdUSB (
  UINT16  BcdUSB
  );

VOID
SetMaxPacketSize0 (
  UINT8  MaxPacketSize0
  );

UINT32
UsbEndpointNum (
  USB_ENDPOINT_DESCRIPTOR  *Des
  );

UINT32
UsbEndpointDirIN (
  USB_ENDPOINT_DESCRIPTOR  *Des
  );

UINT32
UsbEndpointXferControl (
  USB_ENDPOINT_DESCRIPTOR  *Des
  );

UINT32
UsbEndpointXferBulk (
  USB_ENDPOINT_DESCRIPTOR  *Des
  );

UINT32
UsbEndpointMaxpMult (
  USB_ENDPOINT_DESCRIPTOR  *Des
  );

UINT32
UsbEndpointXferIsoc (
  USB_ENDPOINT_DESCRIPTOR  *Des
  );

UINT32
UsbSsMaxStreams (
  USB_SS_ENDPOINT_COMP_DES  *CompDes
  );

UINT32
UsbEndpointXferInterrupt (
  USB_ENDPOINT_DESCRIPTOR  *Des
  );

#endif
