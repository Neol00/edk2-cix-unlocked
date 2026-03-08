/** @file
  SocInitLib.c

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/ArmSmcLib.h>
#include <IndustryStandard/ArmStdSmc.h>
#include <Library/RebootReason.h>
#include <Library/CixSipLib.h>

void
SetRebootReason (
  unsigned int  RebootReason
  )
{
  ARM_SMC_ARGS  ArmSmcArgs;
  unsigned int  Value = 0;

  Value          &= REBOOT_REASON_MASK;
  Value          |= RebootReason;
  ArmSmcArgs.Arg0 = CIX_SIP_SVC_SET_REBOOT_REASON;
  ArmSmcArgs.Arg1 = Value;
  ArmCallSmc (&ArmSmcArgs);
}

/*
 * Description : Obtaining the reboot reason
 */
unsigned int
GetRebootReason (
  void
  )
{
  unsigned int  Value                = 0xFF;
  UINT32        g_reboot_reason_addr = PcdGet32 (PcdRebootReasonRegisterAddr);

  if (g_reboot_reason_addr != 0) {
    Value  = MmioRead32 (g_reboot_reason_addr);
    Value &= 0xFF;
    DEBUG ((DEBUG_INFO, "%a : 0x%x \n", __FUNCTION__, Value));
  }

  return Value;
}

EFI_STATUS
EFIAPI
LibResetSystem (
  IN EFI_RESET_TYPE  ResetType,
  IN EFI_STATUS      ResetStatus,
  IN UINTN           DataSize,
  IN CHAR16          *ResetData OPTIONAL
  )
{
  ARM_SMC_ARGS  ArmSmcArgs;

  switch (ResetType) {
    case EfiResetPlatformSpecific:
    // Map the platform specific reset as reboot
    case EfiResetWarm:
    // Map a warm reset into a cold reset
    case EfiResetCold:
      // Send a PSCI 0.2 SYSTEM_RESET command
      ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_SYSTEM_RESET;
      break;
    case EfiResetShutdown:
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
