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

  WakeupCfg (S5_GPIO_U0, FALSE);// disable GPIO wakeup

  /*usb3 typec ports*/
  WakeupCfg(USB_C_SSP_0_HOST_IRQ,FALSE);
  WakeupCfg(USB_C_SSP_1_HOST_IRQ,FALSE);
  WakeupCfg(USB_C_SSP_2_HOST_IRQ,FALSE);
  WakeupCfg(USB_C_SSP_3_HOST_IRQ,FALSE);

  /*usb3 typea ports*/
  WakeupCfg(USB_SSP_0_HOST_IRQ,FALSE);
  WakeupCfg(USB_SSP_1_HOST_IRQ,FALSE);

  /*usb2 ports*/
  WakeupCfg(USB2_0_HOST_IRQ,FALSE);
  WakeupCfg(USB2_1_HOST_IRQ,FALSE);
  WakeupCfg(USB2_2_HOST_IRQ,FALSE);
  WakeupCfg(USB2_3_HOST_IRQ,FALSE);

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
    GpioConfig (37, INPUT, INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE, LEVEL_LOW);
  } else {
    GpioConfig (37, INPUT, INOUT_VALUE_DEFAULT, INTERRUPT_DISABLE, LEVEL_LOW);
  }

  return EFI_SUCCESS;
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
  { NULL,                        NULL,                 UpdatePcdDmaDeviceLimit         },
  { NULL,                        NULL,                 WakeupSourceInit                },
  { NULL,                        NULL,                 SetStateAfterG3                 },
  { NULL,                        NULL,                 RtcWakupEnable                  },
  { &gEfiI2cMasterProtocolGuid,  InstallRtcProtocol,   NULL                            },
  { &gCixEcPlatformProtocolGuid, InitEcDefaultSetting, NULL                            },
 #ifdef STMM_SUPPORT
  { NULL,                        NULL,                 FenceFchXspiHost                },
 #endif
  { NULL,                        NULL,                 CixFwBootPerfInit               },
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
