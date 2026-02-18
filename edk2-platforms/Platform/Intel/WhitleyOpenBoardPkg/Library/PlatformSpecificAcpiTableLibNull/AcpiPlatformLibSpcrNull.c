/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 1996 - 2016 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include <Library/PlatformSpecificAcpiTableLib.h>

EFI_STATUS
PatchSpcrAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  return Status;
}
