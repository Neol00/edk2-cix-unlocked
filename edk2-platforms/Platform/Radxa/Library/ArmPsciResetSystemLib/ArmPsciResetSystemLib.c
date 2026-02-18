/** @file
  Support ResetSystem Runtime call using PSCI calls
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

  Note: A similar library is implemented in
  ArmVirtPkg/Library/ArmVirtualizationPsciResetSystemLib
  So similar issues might exist in this implementation too.

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2013-2015, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2014, Linaro Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/EfiResetSystemLib.h>
#include <Library/ArmSmcLib.h>
#include <Library/EcLib.h>
#include <Library/GpioLib.h>

#include <IndustryStandard/ArmStdSmc.h>

/**
  Resets the entire platform.

  @param  ResetType             The type of reset to perform.
  @param  ResetStatus           The status code for the reset.
  @param  DataSize              The size, in bytes, of WatchdogData.
  @param  ResetData             For a ResetType of EfiResetCold, EfiResetWarm, or
                                EfiResetShutdown the data buffer starts with a Null-terminated
                                Unicode string, optionally followed by additional binary data.

**/
EFI_STATUS
EFIAPI
LibResetSystem (
  IN EFI_RESET_TYPE  ResetType,
  IN EFI_STATUS      ResetStatus,
  IN UINTN           DataSize,
  IN CHAR16          *ResetData OPTIONAL
  )
{
  ARM_SMC_ARGS              ArmSmcArgs;
  EC_PARAMS_FORCE_EC_RESET  Params;
  CHAR16 *                  SystemProductName;

  SystemProductName = (CHAR16 *)FixedPcdGetPtr (PcdSystemProductName);

  switch (ResetType) {
    case EfiResetPlatformSpecific:
      // Map the platform specific reset as reboot
      if (!StrCmp (L"Radxa Orion O6", SystemProductName)) {
        Params.Reserved = 0;
        ForceEcReset (&Params);

        DEBUG ((DEBUG_INFO, "%a: force EC reset\n", __FUNCTION__));
        CpuDeadLoop ();
      }
    case EfiResetWarm:
    // Map a warm reset into a cold reset
    case EfiResetCold:
      // Cycle USB VBUS power off before reset so devices re-enumerate cleanly
      if (!StrCmp (L"Radxa Orion O6", SystemProductName)) {
        GpioConfig (19, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
        GpioConfig (20, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
        GpioConfig (21, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
        GpioConfig (40, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
        GpioConfig (41, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
        GpioConfig (42, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
        MicroSecondDelay (50000);  // 50ms for USB devices to see VBUS drop
      }
      // Send a PSCI 0.2 SYSTEM_RESET command
      ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_SYSTEM_RESET;
      break;
    case EfiResetShutdown:
      // Turn off power LED (GPIO 15)
      GpioSet (15, INOUT_LOW);
      GpioConfig (FixedPcdGet8 (PcdPcieRootPort0PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
      GpioConfig (FixedPcdGet8 (PcdPcieRootPort1PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
      GpioConfig (FixedPcdGet8 (PcdPcieRootPort2PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
      GpioConfig (FixedPcdGet8 (PcdPcieRootPort3PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
      GpioConfig (FixedPcdGet8 (PcdPcieRootPort4PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);

      DEBUG ((DEBUG_INFO, "%a: disable additional GPIOs for %a\n", __FUNCTION__, SystemProductName));
      if (!StrCmp (L"Radxa Orion O6", SystemProductName)) {
        GpioConfig (10, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of LOM_PWREN
        GpioConfig (12, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of M2_SSD_PWREN
        GpioConfig (13, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of WLAN_PWREN
        GpioConfig (17, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of VGFX_PWREN
        GpioConfig (21, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of USB02F_DRIVE_VBUS
        GpioConfig (22, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of 5GPHY2_PWR_EN
        GpioConfig (40, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of USB2_DRIVE_VBUS
        GpioConfig (41, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of USB_DRIVE_VBUS4
        GpioConfig (42, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of USB_DRIVE_VBUS5
        GpioConfig (81, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of CAM_PWREN
      } else if (!StrCmp (L"Radxa Orion O6N", SystemProductName)) {
        GpioConfig ( 1, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of GPIO001_MKEY_EN
        GpioConfig ( 7, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output high of PD_RESET
        GpioConfig (13, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of WLAN_PWREN
        GpioConfig (14, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of UFS_5V_EN
        GpioConfig (23, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of TPM_PWR_EN
        GpioConfig (40, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of USB2_DRIVE_VBUS
        GpioConfig (41, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of USB_DRIVE_VBUS4
        GpioConfig (42, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of USB_DRIVE_VBUS5
        GpioConfig (73, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of BKEY_PWR_EN
        GpioConfig (81, OUTPUT,  INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);  //  output low of CAM_PWREN
      }

      // Send a PSCI 0.2 SYSTEM_OFF command
      ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_SYSTEM_OFF;
      break;
    default:
      ASSERT (FALSE);
      return EFI_UNSUPPORTED;
  }

  ArmCallSmc (&ArmSmcArgs);

  // We should never be here
  DEBUG ((DEBUG_ERROR, "%a: PSCI Reset failed\n", __FUNCTION__));
  CpuDeadLoop ();
  return EFI_UNSUPPORTED;
}

/**
  Initialize any infrastructure required for LibResetSystem () to function.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
LibInitializeResetSystem (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return EFI_SUCCESS;
}
