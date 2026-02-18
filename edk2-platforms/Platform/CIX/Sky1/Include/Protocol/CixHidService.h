#/** @file CixHidService.h
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#ifndef __CIX_HID_SERVICE_H__
#define __CIX_HID_SERVICE_H__
  #include <Uefi.h>
  #include <Library/BaseLib.h>
  #include <Library/BaseMemoryLib.h>
  #include <Library/DebugLib.h>
  #include <Library/MemoryAllocationLib.h>
  #include <Library/UefiBootServicesTableLib.h>
  #include <Library/UefiDriverEntryPoint.h>
  #include <Library/UefiLib.h>
  #include <Library/DevicePathLib.h>
  #include <Library/BaseHidParseReportLib.h>
  #include <Protocol/DriverBinding.h>
  #include <Protocol/ComponentName2.h>
  #include <Protocol/ComponentName.h>
  #include <Protocol/DevicePath.h>
  #include <Protocol/I2cHidDeviceProtocol.h>

#define CIX_HID_DEVICE_PROTOCOL_GUID \
  { \
    0xa6fdad2a, 0xa56f, 0x45f8, { 0x80, 0xd3, 0x2c, 0xff, 0xf4, 0x91, 0xcd, 0xc2 } \
  }

#define CIX_HID_SERVICE_PROTOCOL_GUID \
  { \
    0xe3f5cc15, 0x9635, 0x4346, { 0x86, 0xa1, 0x83, 0x27, 0xde, 0x48, 0x23, 0x1c } \
  }

#define HID_BCD_VERSION  0x0100
#define DELAY_5MS        5000
#define DELAY_10MS       10000
#define DELAY_100MS      100000

//
// Command Register for MSB - Op Code
//
#define HID_RESET              0x01
#define HID_SET_POWER          0x08
#define HID_DESCROPTOR_LENGTH  0x1E

#define HID_POWER_ON  0x00

//
// Command Register for LSB - Report Type
//
#define TYPE_CMD_RESERVED  0x00
#define TYPE_CMD_INPUT     0x01
#define TYPE_CMD_OUTPUT    0x02
#define TYPE_CMD_FEATURE   0x03

#define COMMAND_SET_ARRAY(OpCode, RptType, RptId)  (OpCode << 8 | ((RptType << 4) | RptId))

#define COMMAND_SET_ARRAY_LENGTH  2
#define COMMAND_RQ_MAX_LENGTH     8

#define I2C_FLAG_WRITE           0
#define I2C_WR_OP_COUNT          1
#define I2C_RD_OP_COUNT          1
#define I2C_WR_THEN_RD_OP_COUNT  2

typedef struct _CIX_I2C_HID_PRIVATE CIX_I2C_HID_PRIVATE;

  #pragma pack(1)
typedef struct {
  UINT16    wHidDescLength;
  UINT16    bcdVersion;
  UINT16    wReportDescLength;
  UINT16    wReportDescRegister;
  UINT16    wInputRegister;
  UINT16    wMaxInputLength;
  UINT16    wOutputRegister;
  UINT16    wMaxOutputLength;
  UINT16    wCommandRegister;
  UINT16    wDataRegister;
  UINT16    wVendorId;
  UINT16    wProductId;
  UINT16    wVersionId;
  UINT32    Reserved;
} I2C_HID_DESCRIPTOR;
  #pragma pack()

typedef struct {
  UINT16    Len;
  UINT8     Id;
  UINT8     Raw[1];
} HID_MULTI_REPORT;

typedef struct {
  UINT16    Len;
  UINT8     Raw[1];
} HID_SINGLE_REPORT;

#define I2C_HID_DEV_SIGNATURE  SIGNATURE_32('I','H','D','S')

#define I2C_HID_DEV_FROM_THIS(a, b)  CR(a, CIX_I2C_HID_PRIVATE, b, I2C_HID_DEV_SIGNATURE)

/**
    Get report descriptor

    @param[in] This            - A pointer to the CIX I2C HID IO instance.
    @param[in, out] ReportDescriptor   Point to the ReportDescriptor start address.
    @param[out] ReportLength           The length of Report descriptor

    @retval EFI_SUCCESS
    @retval EFI_UNSUPPORTED

**/
typedef
EFI_STATUS
(EFIAPI *GET_REPORT_DESCRIPTOR)(
  IN       CIX_I2C_HID_PRIVATE             *This,
  IN OUT   VOID                            **ReportDescriptor,
  OUT      UINTN                           *ReportLength
  );

/**

    HID write the output report to device.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.
    @param[in, out] Buffer - A pointer to the buffer to set.
    @param[in] Size - The size, in bytes, of buffer.

    @retval  EFI_SUCCESS       The request executed successfully.
    @retval  EFI_TIMEOUT       A timeout occurred executing the request.
    @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
typedef
EFI_STATUS
(EFIAPI *SENDING_OUTPUT_REPORT)(
  IN      CIX_I2C_HID_PRIVATE        *This,
  IN      UINT8                 *Buffer,
  IN      UINTN                  Size
  );

/**

    Get I2C HID device Interrupt Status.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.

    @retval  TRUE           Has Interrupt.
    @retval  FALSE          No Interrupt.

**/
typedef
BOOLEAN
(EFIAPI *GET_INTERRUPT_STATUS)(
  IN      CIX_I2C_HID_PRIVATE        *This
  );

/**

    Start reterieve the HID device raw data.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.

    @retval  EFI_SUCCESS       The request executed successfully.

**/
typedef
EFI_STATUS
(EFIAPI *START_RETRIEVE_EVENT)(
  IN      CIX_I2C_HID_PRIVATE        *This
  );

/**

    Stop reterieve the HID device raw data.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.

    @retval  EFI_SUCCESS       The request executed successfully.

**/
typedef
EFI_STATUS
(EFIAPI *STOP_RETRIEVE_EVENT)(
  IN      CIX_I2C_HID_PRIVATE        *This
  );

/**

    Handle when retrieve data from slave device
    and invoked this callback funciton.

    @param     Context      Pointer to the Hid device instance context
    @param     Data         Read data for HID device driver processed.

    @retval    EFI_SUCCESS  Request transfer complete.

**/
typedef
VOID
(EFIAPI *HANDLER_DATA_FUCNTION)(
  IN     VOID                       *Context,
  IN     UINT8                      *Data
  );

/**

    Register a handler function to be called when a given report id
    is retrieve.

    @param[in]   This           - A pointer to the CIX I2C HID IO instance.
    @param[in]   ReportId       Hid report id.
    @param[in]   Context        A pointer to points hid device instance
    @param[in]   Handler        Callback function to process data.


    @retval EFI_SUCCESS           The callback function register success.
    @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to
                                  a lack of resources.
    @retval EFI_INVALID_PARAMETER The callback function has been registered.

**/
typedef
EFI_STATUS
(EFIAPI *REGISTER_CALLBACK)(
  IN   CIX_I2C_HID_PRIVATE          *This,
  IN   UINT8                        ReportId,
  IN   VOID                         *Context,
  IN   HANDLER_DATA_FUCNTION        Handler
  );

/**

    Cancel a handler function when a given report id
    is retrieve.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.
    @param[in]   ReportId       Hid report id.

    @retval EFI_SUCCESS         The callback function unregister success.
    @retval EFI_NOT_FOUND       The callback function can not found.

**/
typedef
EFI_STATUS
(EFIAPI *CANCEL_CALLBACK)(
  IN   CIX_I2C_HID_PRIVATE          *This,
  IN   UINT8                        ReportId
  );

typedef struct {
  UINTN                    Signature;
  UINT8                    ReportId;
  VOID                     *Context;
  HANDLER_DATA_FUCNTION    HandleFunction;
  LIST_ENTRY               NotifyEntry;
} HANDLER_NOTIFY;

typedef struct {
  LIST_ENTRY        NotifyList;
  HANDLER_NOTIFY    *DispatchNotify[HID_MAX_REPORT_IDS];
} HANDLER_NOTIFY_ENUM;

struct _CIX_I2C_HID_PRIVATE {
  UINTN                      Signature;
  UINT16                     VendorId;
  UINT16                     DeviceId;
  EFI_I2C_MASTER_PROTOCOL    *I2cMaster;
  GET_REPORT_DESCRIPTOR      GetReportDescriptor;
  SENDING_OUTPUT_REPORT      SendingOutputReport;
  REGISTER_CALLBACK          RegisterCallback;
  CANCEL_CALLBACK            CancelCallback;
  START_RETRIEVE_EVENT       StartRetrieveData;
  STOP_RETRIEVE_EVENT        StopRetrieveData;
  GET_INTERRUPT_STATUS       GetInterruptStatus;
  HANDLER_NOTIFY_ENUM        HandlerNotifyEnum;
  BOOLEAN                    ReportNumbered[HID_REPORT_TYPES_MAX];
  I2C_HID_DESCRIPTOR         HidDescriptor;
  VOID                       *ReportDescriptor;
  UINT8                      *I2cReadData;
  EFI_EVENT                  TimerEvent;
};

typedef struct _CIX_HID_SERVICE_BINDING CIX_HID_SERVICE_BINDING;

#define MAX_HID_DEVICE_IN_BINDING  10

struct _CIX_HID_SERVICE_BINDING {
  CIX_I2C_HID_PRIVATE    I2cHidPrivate;
  HID_REPORT_LIST        HidReportList;
  UINT8                  HidDeviceCount;
};

typedef struct _CIX_HID_DEVICE_PROTOCOL CIX_HID_DEVICE_PROTOCOL;

struct _CIX_HID_DEVICE_PROTOCOL {
  EFI_HANDLE                  Handle;
  HID_DEV_TYPE_ENUM           HidDevType;
  UINT8                       ReportId;
  EFI_DEVICE_PATH_PROTOCOL    *HidDp;
  CIX_HID_SERVICE_BINDING     *CixHidService;
};

#define I2C_HID_IO_HANDLER_NOTIFY_SIGNATURE  SIGNATURE_32 ('I', 'H', 'N', 'T')

#define HID_HANDLER_NOTIFY_FROM_NOTIFY_ENTRY(a)  CR(a, HANDLER_NOTIFY, NotifyEntry, I2C_HID_IO_HANDLER_NOTIFY_SIGNATURE)

/**
    Receive data from I2C device.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.
    @param[in, out] Buffer     - A pointer to the buffer to set.
    @param[in] Size            - The size, in bytes, of buffer.

    @retval  EFI_SUCCESS       The request executed successfully.
    @retval  EFI_TIMEOUT       A timeout occurred executing the request.
    @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
ReadDataFromDevice (
  IN       CIX_I2C_HID_PRIVATE  *This,
  IN OUT   UINT8                *Buffer,
  IN       UINTN                Size
  );

/**
    Send data to I2C device.

    @param[in] This        - A pointer to I2c Master protocol
    @param[in, out] Buffer    - A pointer to the buffer to set.
    @param[in] Size           - The size, in bytes, of buffer.

    @retval  EFI_SUCCESS       The request executed successfully.
    @retval  EFI_TIMEOUT       A timeout occurred executing the request.
    @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
WriteDataToDevice (
  IN       EFI_I2C_MASTER_PROTOCOL  *This,
  IN OUT   UINT8                    *Buffer,
  IN       UINTN                    Size
  );

/**
    Get HID descriptor from I2C device.

    @param[in] This         - A pointer to I2c Master protocol
    @param[in, out] Buffer     - A pointer to the Hid descriptor buffer to set.

    @retval  EFI_SUCCESS       The request executed successfully.
    @retval  EFI_TIMEOUT       A timeout occurred executing the request.
    @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
I2cGetHidDescriptor (
  IN      EFI_I2C_MASTER_PROTOCOL  *This,
  IN OUT  VOID                     *Buffer
  );

/**
    Get report descriptor from I2C device.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.

    @retval  EFI_SUCCESS       The request executed successfully.
    @retval  EFI_TIMEOUT       A timeout occurred executing the request.
    @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
GetReportDescriptor (
  IN       CIX_I2C_HID_PRIVATE  *This
  );

/**
    Set hid over i2c device power command.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.
    @param[in] PowerState      - Power state

    @retval  EFI_SUCCESS       The request executed successfully.
    @retval  EFI_TIMEOUT       A timeout occurred executing the request.
    @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
I2cHidSetPowerRequest (
  IN    CIX_I2C_HID_PRIVATE  *This,
  IN    UINT8                PowerState
  );

/**
    Set hid over i2c device reset command.

    @param[in] This            - A pointer to the CIX I2C HID IO instance.

    @retval  EFI_SUCCESS       The request executed successfully.
    @retval  EFI_TIMEOUT       A timeout occurred executing the request.
    @retval  EFI_DEVICE_ERROR  The request failed due to a device error.

**/
EFI_STATUS
EFIAPI
I2cHidSetResetRequest (
  IN    CIX_I2C_HID_PRIVATE  *This
  );

extern EFI_GUID  gCixHidServiceProtocolGuid;
extern EFI_GUID  gCixHidDeviceProtocolGuid;
extern EFI_GUID  gCixI2cHostDevicePathGuid;
#endif
