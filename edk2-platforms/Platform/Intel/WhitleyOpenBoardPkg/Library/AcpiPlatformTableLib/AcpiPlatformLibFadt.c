/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 1996 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"
#include <Library/PchPcieRpLib.h>

extern UINT8                 mKBPresent;
extern UINT8                 mMousePresent;
extern SOCKET_PROCESSORCORE_CONFIGURATION   mSocketProcessorCoreConfiguration;
extern SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;


EFI_STATUS
PatchFadtTable (
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   )
{
  UINT16                                    LegacyDevice;
  EFI_ACPI_6_2_FIXED_ACPI_DESCRIPTION_TABLE *FadtHeader;
  EFI_STATUS                                Status;
  UINT8                                     PcieGlobalAspm;

  Status = GetOptionData (&gEfiSocketIioVariableGuid, OFFSET_OF(SOCKET_IIO_CONFIGURATION, PcieGlobalAspm), &PcieGlobalAspm, sizeof(UINT8));
  if (EFI_ERROR (Status)) {
    PcieGlobalAspm = 0x2;
  }

  //
  // Patch FADT for legacy free
  //
  LegacyDevice  = 0;
  FadtHeader    = (EFI_ACPI_6_2_FIXED_ACPI_DESCRIPTION_TABLE *) Table;

  //
  // Control of setting ASPM disabled bit in FADT
  //
  switch (mSocketPowermanagementConfiguration.NativeAspmEnable) {

  case 0:
    LegacyDevice |= (1 << 4);
    break;

  case 1:
    LegacyDevice &= ~(1 << 4);
    break;

  case 2:
    if (PcieGlobalAspm == 0) {
      LegacyDevice |= (1 << 4);
    } else {
      LegacyDevice &= ~(1 << 4);
    }
    break;

  default:
    LegacyDevice &= ~(1 << 4);
    DEBUG ((DEBUG_ERROR, "\n Native ASPM = %d is not valid (expected values are 0, 1, 2). \n", mSocketPowermanagementConfiguration.NativeAspmEnable ));
    ASSERT (0);
    break;
  }

  FadtHeader->IaPcBootArch = LegacyDevice;
  FadtHeader->Flags |= (mSocketProcessorCoreConfiguration.ForcePhysicalModeEnable) ? EFI_ACPI_6_2_FORCE_APIC_PHYSICAL_DESTINATION_MODE : 0;

  return EFI_SUCCESS;
}
