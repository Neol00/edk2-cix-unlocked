#/**
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#ifndef _EC_PLATFORM_PROTOCOL_H_
#define _EC_PLATFORM_PROTOCOL_H_

  #include <Library/EcLib.h>
  #include <Protocol/I2cDevicePath.h>

#define CIX_EC_PLATFORM_PROTOCOL_VERSION           0x45435001    //   'E' 'C' 'P' '(version ID)'
#define CIX_EC_PLATFORM_PROTOCOL_VERSION_STR_MASK  0xFFFFFF00
#define CIX_EC_PLATFORM_PROTOCOL_VERSION_NUM_MASK  0x000000FF

typedef enum {
  EC_COMMAND_GET_EC_VERSION            = EC_CMD_GET_EC_VERSION,
  EC_COMMAND_GET_BOARD_ID              = EC_CMD_GET_BOARD_ID,
  EC_COMMAND_GET_BATTTERY_STATIC_INFO  = EC_CMD_BATTERY_GET_STATIC,
  EC_COMMAND_GET_BATTTERY_DYNAMIC_INFO = EC_CMD_BATTERY_GET_DYNAMIC,
  EC_COMMAND_GET_FARM_ID               = EC_CMD_GET_FRAM_ID,
  EC_COMMAND_SET_GPIO                  = EC_CMD_INT_WRITE_GPIO,
  EC_COMMAND_GET_GPIO                  = EC_CMD_INT_READ_GPIO,
  EC_COMMAND_GET_PMIC_VERSION          = EC_CMD_GET_PMIC_VERSION,
  EC_COMMAND_GET_PD_VERSION            = EC_CMD_GET_PD_VERSION,
  EC_COMMAND_GET_GREENPAK_VERSION      = EC_CMD_GET_GREENPAK_VERSION,
  EC_COMMAND_GET_CHARGER_INFO          = EC_CMD_GET_CHARGER_INFO,
  EC_COMMAND_SET_FAN_RPM               = EC_CMD_PWM_SET_FAN_TARGET_RPM,
  EC_COMMAND_GET_FAN_RPM               = EC_CMD_PWM_GET_FAN_TARGET_RPM,
  EC_COMMAND_SET_FAN_DUTY              = EC_CMD_PWM_SET_DUTY,
  EC_COMMAND_GET_FAN_DUTY              = EC_CMD_PWM_GET_DUTY,
  EC_COMMAND_SET_THERMAL_AUTO_FAN_CTL  = EC_CMD_THERMAL_AUTO_FAN_CTRL,
  EC_COMMAND_GET_PVT_TEMP              = EC_CMD_GET_PVT_TEMP,
  EC_COMMAND_GET_POWER_OFF_RSN         = EC_CMD_GET_PWROFFRSN,
  EC_COMMAND_SET_AUTO_POWER_ON         = EC_CMD_EC_AUTO_POWER_ON,
  EC_COMMAND_GET_ACPI_INT_EVENT        = EC_CMD_INT_GET_INFO,
  EC_COMMAND_SET_PD_FW_UPDATE_INFO     = EC_CMD_SET_PD_UPDATE_INFO,
  EC_COMMAND_GET_PD_FW_UPDATE_STATE    = EC_CMD_GET_PD_UPDATE_STATE,
  EC_COMMAND_TRANS_PD_FW_BIN           = EC_CMD_TRANS_PD_FW_BIN,
  EC_COMMAND_FORCE_EC_RESET            = EC_CMD_FORCE_EC_RESET,
  EC_COMMAND_SET_ALS_MODE              = EC_CMD_SET_AUTO_ALS_CTRL,
} EC_COMMAND_ID;

typedef union {
  EC_PARAMS_GPIO                    Gpio;
  EC_PARAMS_FAN_SET_RPM             FanSetRpm;
  EC_PARAMS_PWM_SET_DUTY            PwmSetDuty;
  EC_PARAMS_PWM_GET_DUTY            PwmGetDuty;
  EC_PARAMS_THERMAL_AUTO_FAN_CTL    ThermalAutoFanCtl;
  EC_PARAMS_PD_FW_UPDATE_INFO       PdFwUpdateInfo;
  EC_PARAMS_PD_FW_UPDATE_STATE      PdFwUpdateState;
  EC_PARAMS_EC_AUTO_POWER_ON        EcAutoPowerOn;
  EC_PARAMS_PD_FW_BIN_TRANS_INFO    PdFwBinTransInfo;
  EC_PARAMS_FORCE_EC_RESET          ForceReset;
  EC_PARAMS_ALS_MODE_CTL            AlsModeCtl;
} EC_PARAM;

typedef union {
  EC_RESPONSE_EC_VERSION              Version;
  EC_RESPONSE_BOARD_ID                BoardId;
  EC_RESPONSE_BATTERY_STATIC_INFO     BatteryStaticInfo;
  EC_RESPONSE_BATTERY_DYNAMIC_INFO    BatteryDynamicInfo;
  EC_RESPONSE_FARM_ID                 FarmId;
  EC_RESPONSE_GPIO                    Gpio;
  EC_RESPONSE_PMIC_VER                PmicVer;
  EC_RESPONSE_PD_VER                  PdVer;
  EC_RESPONSE_GREENPAK_VER            GreenPakVer;
  EC_RESPONSE_CHARGER_INFO            ChargerInfo;
  EC_RESPONSE_FAN_GET_RPM             FanGetRpm;
  EC_RESPONSE_PWM_GET_DUTY            PwmGetDuty;
  EC_RESPONSE_PVT_TEMP                PvtTemp;
  EC_RESPONSE_POWER_OFF_RSN           PowerOffRsn;
  EC_RESPONSE_ACPI_INT_EVENT          AcpiIntEvent;
  EC_RESPONSE_PD_FW_UPDATE_STATE      PdFwUpdateState;
} EC_RESPONSE;

typedef struct _EC_PLATFORM_PROTOCOL EC_PLATFORM_PROTOCOL;

typedef
  EFI_STATUS
(EFIAPI *EC_INFO_TRANSFER)(
  IN     EC_PLATFORM_PROTOCOL  *This,
  IN     EC_COMMAND_ID         Command,
  IN     EC_PARAM              *Params,
  OUT    EC_RESPONSE           *Response
  );

struct _EC_PLATFORM_PROTOCOL {
  EFI_HANDLE          Handle;
  UINT32              Version;
  EC_INFO_TRANSFER    Transfer;
};

extern EFI_GUID  gCixEcPlatformProtocolGuid;
#endif
