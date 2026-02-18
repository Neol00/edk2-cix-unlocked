/** @file Cpu.c
  CPU Count initialization

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PlatformInit.h"
#include <IndustryStandard/Pci.h>
#include <Library/PciCf8Lib.h>
#include <Library/QemuOpenFwCfgLib.h>
#include <IndustryStandard/QemuFwCfg.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>

/**
  Probe Qemu FW CFG device for current CPU count and report to MpInitLib.

  @return EFI_SUCCESS      Detection was successful.
  @retval EFI_UNSUPPORTED  QEMU FW CFG device is not present.
 */
EFI_STATUS
EFIAPI
MaxCpuInit (
  VOID
  )
{
  UINT16      BootCpuCount;
  EFI_STATUS  Status;

  Status = QemuFwCfgIsPresent ();

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "QemuFwCfg not present, unable to detect CPU count \n"));
    ASSERT_EFI_ERROR (Status);
    return EFI_UNSUPPORTED;
  }

  //
  //  Probe Qemu FW CFG device for CPU count
  //

  Status = QemuFwCfgSelectItem (QemuFwCfgItemSmpCpuCount);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  QemuFwCfgReadBytes (sizeof (BootCpuCount), &BootCpuCount);

  //
  //  Report count to MpInitLib
  //

  PcdSet32S (PcdCpuBootLogicalProcessorNumber, BootCpuCount);

  PcdSet32S (PcdCpuMaxLogicalProcessorNumber, 64);

  return EFI_SUCCESS;
}
