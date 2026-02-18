/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/EcLib.h>

I2C_HOST_DESCRIPTOR  *mHost;

EFI_STATUS
EFIAPI
GetEcInfo (
  IN      UINT16  Command,
  IN      VOID    *RequestBuffer,
  IN      UINTN   RequestSize,
  OUT     VOID    *ResponseBuffer,
  IN OUT  UINTN   *ResponseSize
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetEcVersion (
  IN OUT EC_RESPONSE_EC_VERSION  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetBoardId (
  IN OUT EC_RESPONSE_BOARD_ID  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetBatteryStaticInfo (
  IN OUT EC_RESPONSE_BATTERY_STATIC_INFO  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetBatteryDynamicInfo (
  IN OUT EC_RESPONSE_BATTERY_DYNAMIC_INFO  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetFarmId (
  IN OUT EC_RESPONSE_FARM_ID  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
SetGpio (
  IN EC_PARAMS_GPIO  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetGpio (
  IN OUT EC_RESPONSE_GPIO  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetPmicVersion (
  IN OUT EC_RESPONSE_PMIC_VER  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetPdVersion (
  IN OUT EC_RESPONSE_PD_VER  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetGreenPakVersion (
  IN OUT EC_RESPONSE_GREENPAK_VER  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetChagerInfo (
  IN OUT EC_RESPONSE_CHARGER_INFO  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
SetFanRpm (
  IN EC_PARAMS_FAN_SET_RPM  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetFanRpm (
  IN OUT EC_RESPONSE_FAN_GET_RPM  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
SetPwmDuty (
  IN OUT EC_PARAMS_PWM_SET_DUTY  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetPwmDuty (
  IN  EC_PARAMS_PWM_GET_DUTY    *Param,
  OUT EC_RESPONSE_PWM_GET_DUTY  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
SetThermalFanAutoCtl (
  IN EC_PARAMS_THERMAL_AUTO_FAN_CTL  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetPvtTemp (
  IN OUT EC_RESPONSE_PVT_TEMP  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetPoweroffRsn (
  IN OUT EC_RESPONSE_POWER_OFF_RSN  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
SetECAutoPowerOn (
  IN EC_PARAMS_EC_AUTO_POWER_ON  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetAcpiIntEvent (
  IN OUT EC_RESPONSE_ACPI_INT_EVENT  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
SetPdFwUpdateInfo (
  IN EC_PARAMS_PD_FW_UPDATE_INFO  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
GetPdFwUpdateState (
  IN  EC_PARAMS_PD_FW_UPDATE_STATE    *Param,
  OUT EC_RESPONSE_PD_FW_UPDATE_STATE  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

#pragma pack(push, 1)
typedef struct {
  UINT8     CmdCode;
  UINT8     PktLen;
  UINT16    StartAddr;
  UINT8     DataLen;
} PD_FW_BIN_TRANS_HEADER;
#pragma pack(pop)

#define FLASH_SIZE_OF_BANK        0x10000
#define MAX_DATA_SIZE_ONCE_TRANS  0x1D
#define MAX_TRY_COUNT_ONCE_TRANS  3

STATIC CONST UINT8  WRITE_FLASH_COMMAND_CODE[] = { 0x04, 0x06, 0x13, 0x14 };

EFI_STATUS
EFIAPI
TransPdFwBin (
  IN  EC_PARAMS_PD_FW_BIN_TRANS_INFO  *Param
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
ForceEcReset (
  IN EC_PARAMS_FORCE_EC_RESET  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
SetAlsMode (
  IN EC_PARAMS_ALS_MODE_CTL  *Info
  )
{
  return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
Get4SForceShutdown (
  IN OUT EC_RESPONSE_GET_4S_FORCE_SHD_EVT  *Info
  )
{
  return EFI_DEVICE_ERROR;
}
