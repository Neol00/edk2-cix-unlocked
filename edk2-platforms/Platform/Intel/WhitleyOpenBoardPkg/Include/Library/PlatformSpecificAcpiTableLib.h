/** @file
  This library provides a set of platform only ACPI tables and functions.

  @copyright
  Copyright 2012 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PLATFORM_SPECIFIC_ACPI_TABLE_LIB_H_
#define _PLATFORM_SPECIFIC_ACPI_TABLE_LIB_H_

#include <IndustryStandard/Acpi.h>
#include <Library/AcpiPlatformLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SerialIo.h>
#include <Protocol/SuperIo.h>
#include <Guid/GlobalVariable.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>

/**
  This function will check ACPI Table is active or not active.
  This allows boards to prevent publication of unused tables.

  @param Table  -  The table to check

  @retval EFI_SUCCESS  -  The Table is active.

**/
EFI_STATUS
PlatformAcpiReportHooksTableIsActive (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  );

/**
  This function will patch to update platform ACPI Table information.

  @param [in, out]      Table       The table to be udated.

  @retval EFI_SUCCESS  -  The function completed successfully.

**/
EFI_STATUS
PatchPlatformSpecificAcpiTableHooks (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  );

/**
  This function will patch to update SPCR Table information.

  @param [in, out]      Table       The table to be udated.

  @retval EFI_SUCCESS  -  The function completed successfully.

**/
EFI_STATUS
PatchSpcrAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  );

/**
  Update the HMAT table.

  @param [in, out]      Table       The table to be udated.

  @retval EFI SUCCESS   Procedure returned successfully.
**/
EFI_STATUS
PatchHmatAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  );

/**
  Update the PMTT ACPI table

  @param [in, out]      Table       The table to be udated.

  @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchPlatformMemoryTopologyTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  );

/**
  Update the MSCT ACPI table

  @param [in, out]      Table       The table to be udated.

  @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchMsctAcpiTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

/**

  Update the MIGT ACPI table

  @param [in, out]      Table       The table to be udated.

  @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchMigtAcpiTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

/**
  Update the BDAT ACPI table: Multiple instances of the BDAT DATA HOB are placed into one contiguos memory range

  @param [in, out]      Table       The table to be udated.

  @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchBdatAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  );

#endif
