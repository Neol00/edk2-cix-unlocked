/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef _EC_LIB_H_
#define _EC_LIB_H_

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/I2cLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/I2cMaster.h>

#define I2C_FLAG_WRITE           0
#define I2C_WR_THEN_RD_OP_COUNT  2

#define EC_VER_TEXT_SIZE         (sizeof ("V0.00-0123456789AB") + 0)
#define EC_COMM_TEXT_MAX         0x08
#define EC_COMMAND_PROTOCOL_3    0xDA
#define EC_HOST_REQUEST_VERSION  0x03

#define EC_RES_SUCCESS                 0
#define EC_RES_INVALID_COMMAND         1
#define EC_RES_ERROR                   2
#define EC_RES_INVALID_PARAM           3
#define EC_RES_ACCESS_DENIED           4
#define EC_RES_INVALID_RESPONSE        5
#define EC_RES_INVALID_VERSION         6
#define EC_RES_INVALID_CHECKSUM        7
#define EC_RES_IN_PROGRESS             8  /* Accepted, command in progress */
#define EC_RES_UNAVAILABLE             9  /* No response available */
#define EC_RES_TIMEOUT                 10 /* We got a timeout */
#define EC_RES_OVERFLOW                11 /* Table / data overflow */
#define EC_RES_INVALID_HEADER          12 /* Header contains invalid data */
#define EC_RES_REQUEST_TRUNCATED       13 /* Didn't get the entire request */
#define EC_RES_RESPONSE_TOO_BIG        14 /* Response was too big to handle */
#define EC_RES_BUS_ERROR               15 /* Communications bus error */
#define EC_RES_BUSY                    16 /* Up but too busy.  Should retry */
#define EC_RES_INVALID_HEADER_VERSION  17 /* Header version invalid */
#define EC_RES_INVALID_HEADER_CRC      18 /* Header CRC invalid */
#define EC_RES_INVALID_DATA_CRC        19 /* Data CRC invalid */
#define EC_RES_DUP_UNAVAILABLE         20 /* Can't resend response */

#define EC_CMD_BATTERY_GET_STATIC      0x0600
#define EC_CMD_BATTERY_GET_DYNAMIC     0x0601
#define EC_CMD_PWM_GET_FAN_TARGET_RPM  0x0020
#define EC_CMD_PWM_SET_FAN_TARGET_RPM  0x0021
#define EC_CMD_PWM_SET_DUTY            0x0025
#define EC_CMD_PWM_GET_DUTY            0x0026
#define EC_CMD_THERMAL_AUTO_FAN_CTRL   0x0052
#define EC_CMD_INT_GET_INFO            0x0055
#define EC_CMD_INT_WRITE_GPIO          0x0092
#define EC_CMD_INT_READ_GPIO           0x0093
#define EC_CMD_GET_BOARD_ID            0x3E01
#define EC_CMD_GET_CHARGER_INFO        0x3E02
#define EC_CMD_GET_PMIC_VERSION        0x3E03
#define EC_CMD_GET_GREENPAK_VERSION    0x3E06
#define EC_CMD_GET_PD_VERSION          0x3E07
#define EC_CMD_GET_PWROFFRSN           0x3E09
#define EC_CMD_GET_FRAM_ID             0x3E0A
#define EC_CMD_EC_AUTO_POWER_ON        0x3E0B
#define EC_CMD_GET_PVT_TEMP            0x3E0C
#define EC_CMD_SET_PD_UPDATE_INFO      0x3E0D
#define EC_CMD_GET_PD_UPDATE_STATE     0x3E0E
#define EC_CMD_TRANS_PD_FW_BIN         0x3E10
#define EC_CMD_FORCE_EC_RESET          0x3E19
#define EC_CMD_SET_AUTO_ALS_CTRL       0x3E21
#define EC_CMD_GET_4S_FORCE_SHD_EVT    0x3E23
#define EC_CMD_GET_EC_VERSION          0x3FFF

#define EC_BATT_FLAG_AC_PRESENT      0x01
#define EC_BATT_FLAG_BATT_PRESENT    0x02
#define EC_BATT_FLAG_DISCHARGING     0x04
#define EC_BATT_FLAG_CHARGING        0x08
#define EC_BATT_FLAG_LEVEL_CRITICAL  0x10
#define EC_BATT_FLAG_INVALID_DATA    0x20

#define EC_FAN_MODE_AUTO  0x01
#define EC_FAN_MODE_MUTE  0x02
#define EC_FAN_MODE_PERF  0x04

enum {
  PD_FW_UPDATE_STATE_IDLE = 0,
  PD_FW_UPDATE_STATE_IN_PROCESS,
  PD_FW_UPDATE_STATE_DONE,
  PD_FW_UPDATE_STATE_FAIL,
};

#pragma pack(push, 1)

typedef struct  {
  UINT8     StructVersion;
  UINT8     Checksum;
  UINT16    Command;
  UINT8     CommandVersion;
  UINT8     Reserved;
  UINT16    DataLen;
} EC_HOST_REQUEST;

typedef struct  {
  /* Always 0xda to backward compatible with v2 struct */
  UINT8              CommandProtocol;
  EC_HOST_REQUEST    EcRequest;
} EC_HOST_REQUEST_I2C;

typedef struct  {
  UINT8     StructVersion;
  UINT8     Checksum;
  UINT16    Result;
  UINT16    DataLen;
  UINT16    Reserved;
} EC_HOST_RESPONSE;

typedef struct  {
  UINT8               Result;
  UINT8               PacketLength;
  EC_HOST_RESPONSE    EcResponse;
} EC_HOST_RESPONSE_I2C;

typedef struct {
  CHAR8    String[EC_VER_TEXT_SIZE];
} EC_RESPONSE_EC_VERSION;

typedef union {
  UINT16    Value;
  struct {
    UINT16    Sku    : 3;
    UINT16    Memory : 3;
    UINT16    SkuExt : 1;
    UINT16    MemExt : 1;
    UINT16    Rev    : 2;
    UINT16    Rsvd   : 6;
  } Id;
} EC_RESPONSE_BOARD_ID;

typedef struct {
  UINT8    Index;
} EC_PARAMS_BATTERY_STATIC_INFO;

typedef struct  {
  UINT16    DesignCapability;
  UINT16    DesignVoltage;
  CHAR8     Manufacturer[EC_COMM_TEXT_MAX];
  CHAR8     Model[EC_COMM_TEXT_MAX];
  CHAR8     Serial[EC_COMM_TEXT_MAX];
  CHAR8     Type[EC_COMM_TEXT_MAX];
  UINT32    CycleCount;
} EC_RESPONSE_BATTERY_STATIC_INFO;

typedef struct {
  UINT8    Index;
} EC_PARAMS_BATTERY_DYNAMIC_INFO;

typedef struct  {
  UINT16    ActualVoltage;
  UINT16    ActualCurrent;
  UINT16    RemainingCapacity;
  UINT16    FullCapacity;
  UINT16    Flags;
  UINT16    DesiredVoltage;
  UINT16    DesiredCurrent;
} EC_RESPONSE_BATTERY_DYNAMIC_INFO;

typedef struct {
  UINT8    Id;
} EC_RESPONSE_FARM_ID;

typedef struct {
  UINT8    GpioNum;
  UINT8    GpioVal;
} EC_PARAMS_GPIO;

typedef struct {
  UINT8    GpioNum;
  UINT8    GpioVal;
} EC_RESPONSE_GPIO;

typedef struct {
  UINT8    Pmic1Ver;
  UINT8    Pmic2Ver;
  UINT8    Pmic3Ver;
} EC_RESPONSE_PMIC_VER;

typedef struct {
  UINT16    Pd1Ver;
  UINT16    Pd2Ver;
} EC_RESPONSE_PD_VER;

typedef struct {
  UINT8    GreenpakVer;
} EC_RESPONSE_GREENPAK_VER;

typedef struct {
  CHAR8    Name[7]; // charger ic name. SC8886
  CHAR8    Type[3]; // PD
  UINT8    Online;  // 0:offline 1:online.
} EC_RESPONSE_CHARGER_INFO;

typedef struct {
  UINT32    Rpm;
  UINT8     FanIdx;
} EC_PARAMS_FAN_SET_RPM;

typedef struct {
  UINT32    Rpm;
} EC_RRESPONSE_PWM_GET_FAN_RPM;

typedef union {
  UINT8                           dat[4];
  EC_RRESPONSE_PWM_GET_FAN_RPM    FanRpm;
} EC_RESPONSE_FAN_GET_RPM;

typedef struct {
  UINT16    Duty;     /* Duty cycle, EC_PWM_MAX_DUTY = 100% */
  UINT8     PwmType;
  UINT8     Index;    /* Type-specific index, or 0 if unique */
} EC_PARAMS_PWM_SET_DUTY;

typedef struct {
  UINT8    PwmType;
  UINT8    Index;    /* Type-specific index, or 0 if unique */
} EC_PARAMS_PWM_GET_DUTY;

typedef struct {
  UINT16    Duty;    /* Duty cycle, EC_PWM_MAX_DUTY = 100% */
} EC_RESPONSE_PWM_GET_DUTY;

typedef struct {
  UINT8    FanAutoFlg;
} EC_PARAMS_THERMAL_AUTO_FAN_CTL;

typedef struct {
  UINT8    PvtTempH;
  UINT8    PvtTempL;
} EC_RESPONSE_PVT_TEMP;

typedef struct {
  UINT8    Reason;
} EC_RESPONSE_POWER_OFF_RSN;

typedef struct {
  UINT8    EcAutoPowerOnFlg;
} EC_PARAMS_EC_AUTO_POWER_ON;

typedef struct {
  UINT8     Type;
  UINT32    Event;
} EC_RESPONSE_ACPI_INT_EVENT;

typedef struct {
  UINT32    PdFwFlashAddr; // reserved, not used now
  UINT32    PdFwSize;
  UINT8     Index;
  UINT8     TriggerFlag;
} EC_PARAMS_PD_FW_UPDATE_INFO;

typedef struct {
  UINT8    Index;
} EC_PARAMS_PD_FW_UPDATE_STATE;

typedef struct {
  UINT8    PdUpdateState;
} EC_RESPONSE_PD_FW_UPDATE_STATE;

typedef struct {
  UINT32    Address;
  UINT32    Length;
  UINT8     *Data;
} EC_PARAMS_PD_FW_BIN_TRANS_INFO;

typedef struct {
  UINT8    Reserved;
} EC_PARAMS_FORCE_EC_RESET;

typedef struct {
  UINT8    Mode;   // 1:AP, 0:EC
} EC_PARAMS_ALS_MODE_CTL;

typedef struct {
  UINT8    ForceShutdown4S;
} EC_RESPONSE_GET_4S_FORCE_SHD_EVT;

#pragma pack(pop)

EFI_STATUS
EFIAPI
GetEcInfo (
  IN     UINT16  Command,
  IN     VOID    *RequestBuffer,
  IN     UINTN   RequestSize,
  OUT    VOID    *ResponseBuffer,
  IN OUT UINTN   *ResponseSize
  );

EFI_STATUS
EFIAPI
GetEcVersion (
  IN OUT EC_RESPONSE_EC_VERSION  *Info
  );

EFI_STATUS
EFIAPI
GetBoardId (
  IN OUT EC_RESPONSE_BOARD_ID  *Info
  );

EFI_STATUS
EFIAPI
GetBatteryStaticInfo (
  IN OUT EC_RESPONSE_BATTERY_STATIC_INFO  *Info
  );

EFI_STATUS
EFIAPI
GetBatteryDynamicInfo (
  IN OUT EC_RESPONSE_BATTERY_DYNAMIC_INFO  *Info
  );

EFI_STATUS
EFIAPI
GetFarmId (
  IN OUT EC_RESPONSE_FARM_ID  *Info
  );

EFI_STATUS
EFIAPI
SetGpio (
  IN EC_PARAMS_GPIO  *Info
  );

EFI_STATUS
EFIAPI
GetGpio (
  IN OUT EC_RESPONSE_GPIO  *Info
  );

EFI_STATUS
EFIAPI
GetPmicVersion (
  IN OUT EC_RESPONSE_PMIC_VER  *Info
  );

EFI_STATUS
EFIAPI
GetPdVersion (
  IN OUT EC_RESPONSE_PD_VER  *Info
  );

EFI_STATUS
EFIAPI
GetGreenPakVersion (
  IN OUT EC_RESPONSE_GREENPAK_VER  *Info
  );

EFI_STATUS
EFIAPI
GetChagerInfo (
  IN OUT EC_RESPONSE_CHARGER_INFO  *Info
  );

EFI_STATUS
EFIAPI
SetFanRpm (
  IN EC_PARAMS_FAN_SET_RPM  *Info
  );

EFI_STATUS
EFIAPI
GetFanRpm (
  IN OUT EC_RESPONSE_FAN_GET_RPM  *Info
  );

EFI_STATUS
EFIAPI
SetPwmDuty (
  IN OUT EC_PARAMS_PWM_SET_DUTY  *Info
  );

EFI_STATUS
EFIAPI
GetPwmDuty (
  IN  EC_PARAMS_PWM_GET_DUTY    *Param,
  OUT EC_RESPONSE_PWM_GET_DUTY  *Info
  );

EFI_STATUS
EFIAPI
SetThermalFanAutoCtl (
  IN EC_PARAMS_THERMAL_AUTO_FAN_CTL  *Info
  );

EFI_STATUS
EFIAPI
GetPvtTemp (
  IN OUT EC_RESPONSE_PVT_TEMP  *Info
  );

EFI_STATUS
EFIAPI
GetPoweroffRsn (
  IN OUT EC_RESPONSE_POWER_OFF_RSN  *Info
  );

EFI_STATUS
EFIAPI
SetECAutoPowerOn (
  IN EC_PARAMS_EC_AUTO_POWER_ON  *Info
  );

EFI_STATUS
EFIAPI
GetAcpiIntEvent (
  IN OUT EC_RESPONSE_ACPI_INT_EVENT  *Info
  );

EFI_STATUS
EFIAPI
SetPdFwUpdateInfo (
  IN  EC_PARAMS_PD_FW_UPDATE_INFO  *Info
  );

EFI_STATUS
EFIAPI
GetPdFwUpdateState (
  IN  EC_PARAMS_PD_FW_UPDATE_STATE    *Param,
  OUT EC_RESPONSE_PD_FW_UPDATE_STATE  *Info
  );

EFI_STATUS
EFIAPI
TransPdFwBin (
  IN  EC_PARAMS_PD_FW_BIN_TRANS_INFO  *Param
  );

EFI_STATUS
EFIAPI
ForceEcReset (
  IN EC_PARAMS_FORCE_EC_RESET  *Info
  );

EFI_STATUS
EFIAPI
SetAlsMode (
  IN EC_PARAMS_ALS_MODE_CTL  *Info
  );

EFI_STATUS
EFIAPI
Get4SForceShutdown (
  IN OUT EC_RESPONSE_GET_4S_FORCE_SHD_EVT  *Info
  );

#endif
