#/** @file EcPlatformDxe.c
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#include <EcPlatformDxe.h>

EFI_I2C_MASTER_PROTOCOL  *mI2cMaster = NULL;

STATIC
EFI_STATUS
EcInfoTransfer (
  IN     EC_PLATFORM_PROTOCOL  *This,
  IN     EC_COMMAND_ID         Command,
  IN     EC_PARAM              *Param,
  OUT    EC_RESPONSE           *Response
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  switch (Command) {
    case EC_COMMAND_GET_EC_VERSION:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetEcVersion (&Response->Version);
      break;
    case EC_COMMAND_GET_BOARD_ID:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetBoardId (&Response->BoardId);
      break;
    case EC_COMMAND_GET_BATTTERY_STATIC_INFO:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetBatteryStaticInfo (&Response->BatteryStaticInfo);
      break;
    case EC_COMMAND_GET_BATTTERY_DYNAMIC_INFO:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetBatteryDynamicInfo (&Response->BatteryDynamicInfo);
      break;
    case EC_COMMAND_GET_FARM_ID:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetFarmId (&Response->FarmId);
      break;
    case EC_COMMAND_SET_GPIO:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = SetGpio (&Param->Gpio);
      break;
    case EC_COMMAND_GET_GPIO:
      if ((Param == NULL) || (Response == NULL)) {
        return EFI_INVALID_PARAMETER;
      }

      Response->Gpio.GpioNum = Param->Gpio.GpioNum;
      Status                 = GetGpio (&Response->Gpio);
      break;
    case EC_COMMAND_GET_PMIC_VERSION:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetPmicVersion (&Response->PmicVer);
      break;
    case EC_COMMAND_GET_PD_VERSION:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetPdVersion (&Response->PdVer);
      break;
    case EC_COMMAND_GET_GREENPAK_VERSION:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetGreenPakVersion (&Response->GreenPakVer);
      break;
    case EC_COMMAND_GET_CHARGER_INFO:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetChagerInfo (&Response->ChargerInfo);
      break;
    case EC_COMMAND_SET_FAN_RPM:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = SetFanRpm (&Param->FanSetRpm);
      break;
    case EC_COMMAND_GET_FAN_RPM:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetFanRpm (&Response->FanGetRpm);
      break;
    case EC_COMMAND_SET_FAN_DUTY:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = SetPwmDuty (&Param->PwmSetDuty);
      break;
    case EC_COMMAND_GET_FAN_DUTY:
      if ((Param == NULL) || (Response == NULL)) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetPwmDuty (&Param->PwmGetDuty, &Response->PwmGetDuty);
      break;
    case EC_COMMAND_SET_THERMAL_AUTO_FAN_CTL:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = SetThermalFanAutoCtl (&Param->ThermalAutoFanCtl);
      break;
    case EC_COMMAND_GET_PVT_TEMP:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetPvtTemp (&Response->PvtTemp);
      break;
    case EC_COMMAND_GET_POWER_OFF_RSN:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetPoweroffRsn (&Response->PowerOffRsn);
      break;
    case EC_COMMAND_SET_AUTO_POWER_ON:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = SetECAutoPowerOn (&Param->EcAutoPowerOn);
      break;
    case EC_COMMAND_GET_ACPI_INT_EVENT:
      if (Response == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetAcpiIntEvent (&Response->AcpiIntEvent);
      break;
    case EC_COMMAND_SET_PD_FW_UPDATE_INFO:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = SetPdFwUpdateInfo (&Param->PdFwUpdateInfo);
      break;
    case EC_COMMAND_GET_PD_FW_UPDATE_STATE:
      if ((Param == NULL) || (Response == NULL)) {
        return EFI_INVALID_PARAMETER;
      }

      Status = GetPdFwUpdateState (&Param->PdFwUpdateState, &Response->PdFwUpdateState);
      break;
    case EC_COMMAND_TRANS_PD_FW_BIN:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = TransPdFwBin (&Param->PdFwBinTransInfo);
      break;
    case EC_COMMAND_FORCE_EC_RESET:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = ForceEcReset (&Param->ForceReset);
      break;
    case EC_COMMAND_SET_ALS_MODE:
      if (Param == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Status = SetAlsMode (&Param->AlsModeCtl);
      break;
    default:
      break;
  }

  return Status;
}

GLOBAL_REMOVE_IF_UNREFERENCED EC_PLATFORM_PROTOCOL  mEcPlatformProtocolInstance = {
  NULL,
  CIX_EC_PLATFORM_PROTOCOL_VERSION,
  EcInfoTransfer,
};

/**
    This function is the entry point for this DXE.
    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors

**/
EFI_STATUS
EFIAPI
EcPlatformDxeEntry (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                         Status = EFI_SUCCESS;
  EFI_HANDLE                         *Handle;
  UINTN                              HandleNum;
  UINTN                              Index;
  EFI_DEVICE_PATH_PROTOCOL           *DevicePath;
  UINTN                              BusClockHertz = 100000; // 100KHz by default
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  if (PcdGetBool (PcdEcAcpiI2cEn) == TRUE) {
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiI2cMasterProtocolGuid,
                    NULL,
                    &HandleNum,
                    &Handle
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "gEfiI2cMasterProtocolGuid not found yet, status: %r\n", Status));
    } else {
      for (Index = 0; Index < HandleNum; Index++) {
        Status = gBS->HandleProtocol (
                        Handle[Index],
                        &gEfiDevicePathProtocolGuid,
                        (VOID **)&DevicePath
                        );
        if (EFI_ERROR (Status)) {
          continue;
        } else {
          if ((DevicePath->Type == HARDWARE_DEVICE_PATH) && (DevicePath->SubType == HW_VENDOR_DP)) {
            if (((I2C_DEVICE_PATH *)DevicePath)->Bus == FixedPcdGet8 (PcdEcI2cBus)) {
              Status = gBS->LocateProtocol (
                              &gCixConfigParamsManageProtocolGuid,
                              NULL,
                              (VOID **)&ConfigManage
                              );

              if (!EFI_ERROR (Status)) {
                ConfigData    = ConfigManage->Data;
                BusClockHertz = ConfigData->Fch.I2c[FixedPcdGet8 (PcdEcI2cBus)].BusFreq;
              } else {
                DEBUG ((DEBUG_ERROR, "%a: config parameters is invalid %r\n", __FUNCTION__, Status));
              }

              Status = gBS->HandleProtocol (
                              Handle[Index],
                              &gEfiI2cMasterProtocolGuid,
                              (VOID **)&mI2cMaster
                              );
              Status = mI2cMaster->SetBusFrequency (mI2cMaster, &BusClockHertz);

              mEcPlatformProtocolInstance.Handle = Handle[Index];
              Status                             = gBS->InstallProtocolInterface (
                                                          &Handle[Index],
                                                          &gCixEcPlatformProtocolGuid,
                                                          EFI_NATIVE_INTERFACE,
                                                          &mEcPlatformProtocolInstance
                                                          );
            }
          }
        }
      }

      gBS->FreePool (Handle);
    }
  }

  return Status;
}
