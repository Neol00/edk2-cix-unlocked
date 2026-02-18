/** @file
   Hooks for Platform populate different function and Platform only ACPI Table.

  @copyright
  Copyright 2013 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiDxe.h>
#include <Library/PlatformSpecificAcpiTableLib.h>

/**
  This function will check ACPI Table is active or not active.

  @param [in, out]      Table       The table to be udated.

  @retval EFI_SUCCESS  -  The Table is active.

**/
EFI_STATUS
PlatformAcpiReportHooksTableIsActive (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  )
{
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader;

  TableHeader   = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
  if (TableHeader->Signature == EFI_ACPI_6_2_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_SIGNATURE) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

/**

  This function will patch to update platform level Acpi Table information.

  @param [in, out]      Table       The table to be udated.

  @retval EFI_SUCCESS  -  The function completed successfully.

**/
EFI_STATUS
PatchPlatformSpecificAcpiTableHooks (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  )
{
  return EFI_SUCCESS;
}
