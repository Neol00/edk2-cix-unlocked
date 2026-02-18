/** @file

  @copyright
  Copyright 1996 - 2017 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_PLATFORM_LIB_H_
#define _ACPI_PLATFORM_LIB_H_

//
// Statements that include other header files
//
#include <PiDxe.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

#include <IndustryStandard/Acpi.h>

#include <Protocol/MpService.h>

#include <Protocol/AcpiSystemDescriptionTable.h>


EFI_STATUS
AcpiPlatformHooksIsActiveTable (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  );

/*++

Routine Description:

  Called for every ACPI table found in the BIOS flash.
  Returns whether a table is active or not. Inactive tables
  are not published in the ACPI table list. This hook can be
  used to implement optional SSDT tables or enabling/disabling
  specific functionality (e.g. SPCR table) based on a setup
  switch or platform preference. In case of optional SSDT tables,
  the platform flash will include all the SSDT tables but will
  return EFI_SUCCESS only for those tables that need to be
  published.
  This hook can also be used to update the table data. The header
  is updated by the common code. For example, if a platform wants
  to use an SSDT table to export some platform settings to the
  ACPI code, it needs to update the data inside that SSDT based
  on platform preferences in this hook.

Arguments:

  None

Returns:

  Status  - EFI_SUCCESS if the table is active
  Status  - EFI_UNSUPPORTED if the table is not active
*/

/**

  This function will update any runtime platform specific information.
  This currently includes:
    Setting OEM table values, ID, table ID, creator ID and creator revision.
    Enabling the proper processor entries in the APIC tables.

  @param Table  -  The table to update

  @retval EFI_SUCCESS  -  The function completed successfully.

**/
EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  ,IN OUT EFI_ACPI_TABLE_VERSION     *Version
  );

/**
  Give the platform a chance to build tables.

  Some tables can be built from scratch more efficiently than being prebuilt
  and updated. This function builds any such tables for the platform.

  @retval EFI_SUCCESS   Any platform tables were successfully built.
**/
EFI_STATUS
PlatformBuildTables (
  VOID
  );

/**
  Platform hook to initialize Platform Specific ACPI Parameters

  @retval EFI_SUCCESS            Platform specific parameters in mAcpiParameter
                                 initialized successfully.
  @retval EFI_INVALID_PARAMETER  mAcpiParameter global was NULL.
**/
EFI_STATUS
PlatformHookAfterAcpiParamInit (
  VOID
  );

#endif
