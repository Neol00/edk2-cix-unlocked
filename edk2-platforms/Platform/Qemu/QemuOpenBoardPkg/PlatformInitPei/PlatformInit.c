/** @file PlarformInit.c
  Platform initialization PEIM for QEMU

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PlatformInit.h"
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include "Library/DebugLib.h"
#include <Library/PlatformInitLib.h>
#include <Library/HobLib.h>
#include <Library/PciCf8Lib.h>
#include <IndustryStandard/Pci.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <IndustryStandard/Q35MchIch9.h>

EFI_STATUS
EFIAPI
PlatformInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS             Status;
  UINT16                 DeviceId;
  EFI_HOB_PLATFORM_INFO  *EfiPlatformInfo;

  //
  // Install permanent memory
  //
  Status = InstallMemory (PeiServices);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Memory installation failed\n"));
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "Memory installation success\n"));
  }

  //
  // Report CPU core count to MPInitLib
  //
  MaxCpuInit ();

  EfiPlatformInfo = AllocateZeroPool (sizeof (EFI_HOB_PLATFORM_INFO));
  if (EfiPlatformInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate pool for EFI_HOB_PLATFORM_INFO\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Report gUefiOvmfPkgPlatformInfo HOB with only the necessary data for OVMF
  //
  DeviceId = PciCf8Read16 (PCI_CF8_LIB_ADDRESS (0, 0, 0, PCI_DEVICE_ID_OFFSET));
  DEBUG ((DEBUG_INFO, "Building gUefiOvmfPkgPlatformInfoGuid with Host bridge dev ID %x \n", DeviceId));
  (*EfiPlatformInfo).HostBridgeDevId = DeviceId;

  BuildGuidDataHob (&gUefiOvmfPkgPlatformInfoGuid, EfiPlatformInfo, sizeof (EFI_HOB_PLATFORM_INFO));

  PcdSet16S (PcdOvmfHostBridgePciDevId, DeviceId);

  //
  // Initialize PCI or PCIe based on current emulated system
  //
  if (DeviceId == INTEL_Q35_MCH_DEVICE_ID) {
    DEBUG ((DEBUG_INFO, "Q35: Initialize PCIe\n"));
    return InitializePcie ();
  } else {
    DEBUG ((DEBUG_INFO, "PIIX4: Initialize PCI\n"));
    return InitializePciPIIX4 ();
  }
}
