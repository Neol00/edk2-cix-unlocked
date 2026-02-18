/** @file
  ACPI static data update.

  @copyright
  Copyright 2013 - 2022 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "StaticSkuDataDxe.h"
#include <Library/UbaAcpiUpdateLib.h>

#include "AmlOffsetTable.c" // Generated in PreBuild step

#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>

ACPI_FIX_UPDATE_TABLE  FixupTableUpdate10nm =
{
  PLATFORM_ACPI_FIX_UPDATE_SIGNATURE,
  PLATFORM_ACPI_FIX_UPDATE_VERSION,
  &DSDT_EPRP10NM_OffsetTable
};

EFI_STATUS
InstallAcpiFixupTableData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  ACPI_FIX_UPDATE_TABLE                 *PtrTable;
  UINT32                                TableSize;

  GuidHob = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  PtrTable = &FixupTableUpdate10nm;
  TableSize = sizeof(FixupTableUpdate10nm);
  DEBUG ((DEBUG_INFO, "UBA: Loading Acpi table for ICX\n"));

  Status = UbaConfigProtocol->AddData (
                                UbaConfigProtocol,
                                &gPlatformAcpiFixTableGuid,
                                PtrTable,
                                TableSize
                                );
  return Status;
}
