/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/EcLib.h>
#include <Library/PinMuxLib.h>
#include <Library/PinMuxTableLib.h>
#include <Library/GpioLib.h>
#include <Library/GpioTableLib.h>
#include <Library/PlatformEnvHookLib.h>
#include <Protocol/EcPlatformProtocol.h>
#include <Protocol/I2cDevicePath.h>
#include <Library/CixSipLib.h>
#include <Library/EcLib.h>
#include <Library/MailBoxLib.h>
#include <Library/CixFwBootPerfLib.h>
#include <Library/CixPostCodeLib.h>
#include <Guid/NetworkStackSetup.h>
#include <PlatformSetupVar.h>

// {3F7B73C7-FB70-4e91-86E7-34EAD76AC74D}
EFI_GUID  gEfiFarmEnableFlagGuid = {
  0x3f7b73c7, 0xfb70, 0x4e91, { 0x86, 0xe7, 0x34, 0xea, 0xd7, 0x6a, 0xc7, 0x4d }
};

EFI_STATUS
EFIAPI
PlatformEnvHookLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS
EFIAPI
InitGpio (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  GpioInit (mGpioCfgTable, mGpioCfgTableLength);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InitPinmux (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  PinMuxInit (mPinMuxCfgTable, mPinMuxCfgTableLength);

  return EFI_SUCCESS;
}

VOID
EFIAPI
InitEcDefaultSetting (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  EC_PLATFORM_PROTOCOL  *Ec;
  EC_PARAM              Params;

  Status = gBS->LocateProtocol (&gCixEcPlatformProtocolGuid, NULL, (VOID **)&Ec);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "EC platform not ready.\n"));
    return;
  }

  Params.ThermalAutoFanCtl.FanAutoFlg = 0; // manual mode
  Status                              = Ec->Transfer (Ec, EC_COMMAND_SET_THERMAL_AUTO_FAN_CTL, &Params, NULL);
  DEBUG ((DEBUG_INFO, "EC Set Fan control manual mode Status:%r\n", Status));

  Params.PwmSetDuty.Duty    = 100;
  Params.PwmSetDuty.Index   = 0;
  Params.PwmSetDuty.PwmType = 0;
  Status                    = Ec->Transfer (Ec, EC_COMMAND_SET_FAN_DUTY, &Params, NULL);

  DEBUG ((DEBUG_INFO, "EC Set Pwm Duty %d Status:%r\n", Params.PwmSetDuty.Duty, Status));
}

VOID
EFIAPI
InstallRtcProtocol (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_HANDLE                *Handle;
  UINTN                     HandleNum;
  UINTN                     Index;

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
          if (CompareGuid (&(((VENDOR_DEVICE_PATH *)DevicePath)->Guid), &gCixI2cDevicePathGuid) &&
              (((I2C_DEVICE_PATH *)DevicePath)->Bus == FixedPcdGet8 (PcdRa8900ceI2cBus)))
          {
            //
            // Install the ra8900ce I2C Master protocol on this handle so the RTC driver
            // can identify it as the I2C master it can invoke directly, rather than
            // through the I2C driver stack (which cannot be used at runtime)
            //
            Status = gBS->InstallProtocolInterface (
                            &Handle[Index],
                            &gRa8900ceRealTimeClockLibI2cMasterProtocolGuid,
                            EFI_NATIVE_INTERFACE,
                            NULL
                            );
            ASSERT_EFI_ERROR (Status);
            if (Event != NULL) {
              gBS->CloseEvent (Event);
            }
          }
        }
      }
    }

    gBS->FreePool (Handle);
  }

  return;
}

EFI_STATUS
EFIAPI
UpdatePcdDmaDeviceLimit (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status;
  UINT64      SystemMemSize  = 0;
  UINT64      DmaDeviceLimit = 0;

 #ifdef UPDATE_MEMORY_TOP
  UINT32  *pMemSizeAddr = (UINT32 *)(FixedPcdGet64 (PcdMemInfoBaseAddr) + 0x10);
  SystemMemSize   = *pMemSizeAddr;
  SystemMemSize <<= 20;
 #else
  SystemMemSize = FixedPcdGet64 (PcdSystemMemorySize);
 #endif
  DEBUG ((DEBUG_INFO, "[%a] SystemMemSize :%lx\n", __FUNCTION__, SystemMemSize));
  DmaDeviceLimit += (SystemMemSize + 0x80000000);
  DmaDeviceLimit -= 1;
  Status          = PcdSet64S (PcdDmaDeviceLimit, DmaDeviceLimit);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "PcdDmaDeviceLimit set fail status:%r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "PcdDmaDeviceLimit set success\n"));
  }

  return Status;
}

EFI_STATUS
EFIAPI
WakeupSourceInit (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  // wakeup enable/disable sample
  // WakeupCfg(S5_GPIO_U0,FALSE);//disable GPIO wakeup

  return Status;
}

EFI_STATUS
EFIAPI
PciePowerOffWith4sPowerOverride (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  UINT32          Data32;
  EC_PARAMS_GPIO  GpioInfo;

  Data32  = MmioRead32 (0x16000218); // power override
  Data32 &= 0x00000007;
  if (Data32 == 0x00000007) {
    // PCIE Port 0 PCIE_X8_SLOT
    GpioInfo.GpioNum = 205;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
    // PCIE Port 1 M.2 SSD
    GpioConfig (12, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
    // PCIE Port  2 PCIE_X4_SLOT
    GpioInfo.GpioNum = 214;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
    // PCIE Port  3 RJ45_2.5G
    GpioInfo.GpioNum = 202;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
    // PCIE Port  4 M.2 WLAN/BT
    GpioInfo.GpioNum = 215;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
OnboardDevicePowerOff (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  EC_PARAMS_GPIO  GpioInfo;

  DEBUG ((DEBUG_INFO, "[%a] EntryPoint\n", __FUNCTION__));

  if (ConfigData == NULL) {
    DebugPrint (DEBUG_ERROR, "ConfigData point is NULL");
    return EFI_OUT_OF_RESOURCES;
  }

  if (ConfigData->PlatConfig->GfxPower == 0) {
    // EC GPIO205
    DEBUG ((DEBUG_INFO, "EC GPIO205 Output Low GFX Power Off\n"));
    GpioInfo.GpioNum = 205;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO205
    DEBUG ((DEBUG_INFO, "EC GPIO205 Output High GFX Power On\n"));
    GpioInfo.GpioNum = 205;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->TouchPanelPower == 0) {
    // EC GPIO203
    DEBUG ((DEBUG_INFO, "EC GPIO203 Output Low TouchPanelPower Power Off\n"));
    GpioInfo.GpioNum = 203;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO203
    DEBUG ((DEBUG_INFO, "EC GPIO203 Output High TouchPanelPower Power On\n"));
    GpioInfo.GpioNum = 203;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->TpmPower == 0) {
    // EC GPIO213
    DEBUG ((DEBUG_INFO, "EC GPIO213 Output Low TpmPower Power Off\n"));
    GpioInfo.GpioNum = 213;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO213
    DEBUG ((DEBUG_INFO, "EC GPIO213 Output High TpmPower Power On\n"));
    GpioInfo.GpioNum = 213;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->WwanPower == 0) {
    // EC GPIO215
    DEBUG ((DEBUG_INFO, "EC GPIO215 Output Low WwanPower Power Off\n"));
    GpioInfo.GpioNum = 215;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO215
    DEBUG ((DEBUG_INFO, "EC GPIO215 Output High WwanPower Power On\n"));
    GpioInfo.GpioNum = 215;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->PcieX2SlotPower == 0) {
    // EC GPIO214
    DEBUG ((DEBUG_INFO, "EC GPIO214 Output Low PcieX2SlotPower Power Off\n"));
    GpioInfo.GpioNum = 214;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO214
    DEBUG ((DEBUG_INFO, "EC GPIO214 Output High PcieX2SlotPower Power On\n"));
    GpioInfo.GpioNum = 214;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->FingerPrintPower == 0) {
    // EC GPIO36
    DEBUG ((DEBUG_INFO, "EC GPIO36 Output Low FingerPrintPower Power Off\n"));
    GpioInfo.GpioNum = 36;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO36
    DEBUG ((DEBUG_INFO, "EC GPIO36 Output High FingerPrintPower Power On\n"));
    GpioInfo.GpioNum = 36;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->OnBoardLanPower == 0) {
    // EC GPIO202
    DEBUG ((DEBUG_INFO, "EC GPIO202 Output Low OnBoardLanPower Power Off\n"));
    GpioInfo.GpioNum = 202;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO202
    DEBUG ((DEBUG_INFO, "EC GPIO202 Output High OnBoardLanPower Power On\n"));
    GpioInfo.GpioNum = 202;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->WlanPower == 0) {
    // GPIO13
    DEBUG ((DEBUG_INFO, "GPIO13 Output Low WlanPower Power Off\n"));
    GpioConfig (13, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO13
    DEBUG ((DEBUG_INFO, "GPIO13 Output High WlanPower Power On\n"));
    GpioConfig (13, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->M2SsdPower == 0) {
    // GPIO12
    DEBUG ((DEBUG_INFO, "GPIO12 Output Low M2SsdPower Power Off\n"));
    GpioConfig (12, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO12
    DEBUG ((DEBUG_INFO, "GPIO12 Output High M2SsdPower Power On\n"));
    GpioConfig (12, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->IspCamera0Power == 0) {
    // GPIO91
    DEBUG ((DEBUG_INFO, "GPIO91 Output Low IspCamera0Power Power Off\n"));
    GpioConfig (91, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO91
    DEBUG ((DEBUG_INFO, "GPIO91 Output High IspCamera0Power Power On\n"));
    GpioConfig (91, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->IspCamera1Power == 0) {
    // GPIO93
    DEBUG ((DEBUG_INFO, "GPIO93 Output Low IspCamera1Power Power Off\n"));
    GpioConfig (93, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO93
    DEBUG ((DEBUG_INFO, "GPIO93 Output High IspCamera1Power Power On\n"));
    GpioConfig (93, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->IspCamera2Power == 0) {
    // GPIO83
    DEBUG ((DEBUG_INFO, "GPIO83 Output Low IspCamera2Power Power Off\n"));
    GpioConfig (83, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO83
    DEBUG ((DEBUG_INFO, "GPIO83 Output High IspCamera2Power Power On\n"));
    GpioConfig (83, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->IspCamera3Power == 0) {
    // GPIO86
    DEBUG ((DEBUG_INFO, "GPIO86 Output Low IspCamera3Power Power Off\n"));
    GpioConfig (86, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO86
    DEBUG ((DEBUG_INFO, "GPIO86 Output High IspCamera3Power Power On\n"));
    GpioConfig (86, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetStateAfterG3 (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS                  Status;
  EC_PARAMS_EC_AUTO_POWER_ON  AutoPowerOnInfo;

  AutoPowerOnInfo.EcAutoPowerOnFlg = ConfigData->PlatConfig->StateAfterG3;
  Status                           = SetECAutoPowerOn (&AutoPowerOnInfo);
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "SetECAutoPowerOn Status:%r\n", Status);
  }

  return Status;
}

EFI_STATUS
EFIAPI
RtcWakupEnable (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  if (ConfigData->PlatConfig->RtcWakeup) {
    GpioConfig (11, INPUT, INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE, LEVEL_LOW);
  } else {
    GpioConfig (11, INPUT, INOUT_VALUE_DEFAULT, INTERRUPT_DISABLE, LEVEL_LOW);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FarmFunctionControl (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  UINTN                VarSize;
  NETWORK_STACK        NetworkStack;
  PLATFORM_SETUP_DATA  PlatformSetupVar;
  EC_RESPONSE_FARM_ID  FarmIdInfo;
  UINT8                FarmEnableFlag;

  Status = GetFarmId (&FarmIdInfo);
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Farm Id info fail.\n", __FUNCTION__, __LINE__);
    return Status;
  }

  if (FarmIdInfo.Id == 0) {
    DebugPrint (DEBUG_INFO, "Farm Function Enable.\n");
    VarSize = sizeof (NETWORK_STACK);
    Status  = gRT->GetVariable (
                     NETWORK_STACK_VAR,
                     &gEfiNetworkStackSetupGuid,
                     NULL,
                     &VarSize,
                     &NetworkStack
                     );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Network Stack Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }

    NetworkStack.Enable   = 1;
    NetworkStack.Ipv4Pxe  = 1;
    NetworkStack.Ipv6Pxe  = 1;
    NetworkStack.Ipv4Http = 1;
    NetworkStack.Ipv6Http = 1;
    Status                = gRT->SetVariable (
                                   NETWORK_STACK_VAR,
                                   &gEfiNetworkStackSetupGuid,
                                   EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                   sizeof (NETWORK_STACK),
                                   &NetworkStack
                                   );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Network Stack Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }

    VarSize = sizeof (PLATFORM_SETUP_DATA);
    Status  = gRT->GetVariable (
                     PLATFORM_SETUP_VAR,
                     &gPlatformSetupVariableGuid,
                     NULL,
                     &VarSize,
                     &PlatformSetupVar
                     );
    PlatformSetupVar.SocWatchdogTimer = 0;
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Platform Setup Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }

    Status = gRT->SetVariable (
                    PLATFORM_SETUP_VAR,
                    &gPlatformSetupVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (PLATFORM_SETUP_DATA),
                    &PlatformSetupVar
                    );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Platform Setup Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }

    ConfigData->SocConfig->S5.SocWatchdogTimer = PlatformSetupVar.SocWatchdogTimer; // Update ConfigData parameter
    FarmEnableFlag                             = 1;
    Status                                     = gRT->SetVariable (
                                                        L"FarmEnableFlag",
                                                        &gEfiFarmEnableFlagGuid,
                                                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                                        sizeof (UINT8),
                                                        &FarmEnableFlag
                                                        );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Farm Enable Flag Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }
  } else {
    DebugPrint (DEBUG_INFO, "Farm Function Disable.\n");
    VarSize =  sizeof (UINT8);
    Status  = gRT->GetVariable (
                     L"FarmEnableFlag",
                     &gEfiFarmEnableFlagGuid,
                     NULL,
                     &VarSize,
                     &FarmEnableFlag
                     );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Farm Enable Flag Variable Status %r.\n", __FUNCTION__, __LINE__, Status);
      return Status;
    }

    if (FarmEnableFlag == 1) {
      VarSize = sizeof (NETWORK_STACK);
      Status  = gRT->GetVariable (
                       NETWORK_STACK_VAR,
                       &gEfiNetworkStackSetupGuid,
                       NULL,
                       &VarSize,
                       &NetworkStack
                       );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Network Stack Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }

      NetworkStack.Enable   = FixedPcdGet8 (PcdNetworkStackSupport);
      NetworkStack.Ipv4Pxe  = FixedPcdGet8 (PcdIPv4PXESupport);
      NetworkStack.Ipv6Pxe  = FixedPcdGet8 (PcdIPv6PXESupport);
      NetworkStack.Ipv4Http = FixedPcdGet8 (PcdIPv4HttpSupport);
      NetworkStack.Ipv6Http = FixedPcdGet8 (PcdIPv6HttpSupport);
      Status                = gRT->SetVariable (
                                     NETWORK_STACK_VAR,
                                     &gEfiNetworkStackSetupGuid,
                                     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                     sizeof (NETWORK_STACK),
                                     &NetworkStack
                                     );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Network Stack Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }

      VarSize = sizeof (PLATFORM_SETUP_DATA);
      Status  = gRT->GetVariable (
                       PLATFORM_SETUP_VAR,
                       &gPlatformSetupVariableGuid,
                       NULL,
                       &VarSize,
                       &PlatformSetupVar
                       );
      PlatformSetupVar.SocWatchdogTimer = FixedPcdGet8 (PcdSocWatchdogTimer);
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Platform Setup Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }

      Status = gRT->SetVariable (
                      PLATFORM_SETUP_VAR,
                      &gPlatformSetupVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                      sizeof (PLATFORM_SETUP_DATA),
                      &PlatformSetupVar
                      );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Platform Setup Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }

      ConfigData->SocConfig->S5.SocWatchdogTimer = PlatformSetupVar.SocWatchdogTimer; // Update ConfigData parameter
      FarmEnableFlag                             = 0;
      Status                                     = gRT->SetVariable (
                                                          L"FarmEnableFlag",
                                                          &gEfiFarmEnableFlagGuid,
                                                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                                          sizeof (UINT8),
                                                          &FarmEnableFlag
                                                          );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Farm Enable Flag Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }
    }
  }

  return Status;
}

#ifdef STMM_SUPPORT
EFI_STATUS
EFIAPI
FenceFchXspiHost (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS                     Status;
  MBOX_GASKET_FENCING_PARAMETER  Params;
  MBOX_GASKET_FENCING_RESPONSE   Respon;

  Params.Id = FCH_XSPI_RANGE_ID;
  Status    = MboxEnableGasketFencing (&Params, &Respon);
  DEBUG ((DEBUG_INFO, "[%a] enable FCH XSPI gasket fencing status %r, response error code %d\n", __FUNCTION__, Status, Respon.ErrCode));

  return Status;
}

#endif


STATIC
VOID
EFIAPI
CixFwBootPerfEndNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  POST_CODE (DxeReadyToBoot);
  cix_set_boot_phase (BLOADER_PHASE, RECORD_END);
}

static EFI_EVENT  ReadyToBootEvent;
EFI_STATUS
EFIAPI
CixFwBootPerfInit (
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = gBS->CreateEventEx (
                               EVT_NOTIFY_SIGNAL,
                               TPL_CALLBACK,
                               CixFwBootPerfEndNotify,
                               NULL,
                               &gEfiEventReadyToBootGuid,
                               &ReadyToBootEvent
                               );
  return Status;

}

STATIC PLATFORM_ENV_INIT_TABLE  mPlatformEnvInitTable[] = {
  { NULL,                        NULL,                 InitGpio                        },
  { NULL,                        NULL,                 InitPinmux                      },
  // { NULL,                        NULL,                 PciePowerOffWith4sPowerOverride },
  // { NULL,                        NULL,                 UpdatePcdDmaDeviceLimit         },
  // { NULL,                        NULL,                 WakeupSourceInit                },
  // { NULL,                        NULL,                 OnboardDevicePowerOff           },
  // { NULL,                        NULL,                 SetStateAfterG3                 },
  // { NULL,                        NULL,                 RtcWakupEnable                  },
  // { NULL,                        NULL,                 FarmFunctionControl             },
  // { &gEfiI2cMasterProtocolGuid,  InstallRtcProtocol,   NULL                            },
  // { &gCixEcPlatformProtocolGuid, InitEcDefaultSetting, NULL                            },
 #ifdef STMM_SUPPORT
  { NULL,                        NULL,                 FenceFchXspiHost                },
 #endif
   { NULL,                        NULL,                 CixFwBootPerfInit              },
  // add platform initialization routines on ENV phase BEFORE this line, and they were invoked from top to down.
  { NULL,                        NULL,                 NULL                            }
};

STATIC UINT32  mPlatformEnvInitTableSize = ARRAY_SIZE (mPlatformEnvInitTable);

VOID
EFIAPI
PlatformEnvHook (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      Index  = 0;
  VOID        *Registration;

  if (ConfigData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: platform ENV hook routine failed to get config data\n", __FUNCTION__));
    return;
  }

  while (Index < mPlatformEnvInitTableSize) {
    if (mPlatformEnvInitTable[Index].Guid == NULL) {
      if (mPlatformEnvInitTable[Index].Init != NULL) {
        Status = mPlatformEnvInitTable[Index].Init (ConfigData);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: platform ENV hook routine failed at index %d, status %r\n", __FUNCTION__, Index, Status));
        }
      }
    } else {
      EfiCreateProtocolNotifyEvent (
        mPlatformEnvInitTable[Index].Guid,
        TPL_CALLBACK,
        mPlatformEnvInitTable[Index].Notify,
        ConfigData,
        &Registration
        );
    }

    Index++;
  }
}
