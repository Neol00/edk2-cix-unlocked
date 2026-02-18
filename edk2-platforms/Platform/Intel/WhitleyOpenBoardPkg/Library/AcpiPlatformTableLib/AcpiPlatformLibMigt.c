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

extern BIOS_ACPI_PARAM      *mAcpiParameter;

/**
    Update the MIGT ACPI table

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success
**/
EFI_STATUS
PatchMigtAcpiTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  )
{
  EFI_STATUS        Status;
  UINT64            Address;
  UINTN             idx;
  UINT8             checksum;
  EFI_MIGT_ACPI_DESCRIPTION_TABLE *MigtAcpiTable;

  MigtAcpiTable = (EFI_MIGT_ACPI_DESCRIPTION_TABLE *)Table;
  Address = 0xffffffff;

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   1,     //page
                   &Address
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // update MIGT ACPI table
  //
  MigtAcpiTable->ActionRegion.Address = Address;

  //
  // update checksum
  //
  MigtAcpiTable->Header.Checksum = 0;
  checksum = 0;
  for(idx = 0; idx < sizeof(EFI_MIGT_ACPI_DESCRIPTION_TABLE); idx++) {
    checksum = checksum + (UINT8) (((UINT8 *)(MigtAcpiTable))[idx]);
  }
  MigtAcpiTable->Header.Checksum = (UINT8) (0 - checksum);

  //
  // Update Migration Action Region GAS address
  //
  mAcpiParameter->MigrationActionRegionAddress = Address;

  return Status;
}
