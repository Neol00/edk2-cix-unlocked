/** @file
  EFI ACPI Platform Protocol

  @copyright
  Copyright 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_PLATFORM_PROTOCOL_H
#define _ACPI_PLATFORM_PROTOCOL_H

#include <IndustryStandard/Acpi62.h>

///
/// ACPI Platform protocol provided for DXE phase
///
typedef struct _ACPI_PLATFORM_PROTOCOL  ACPI_PLATFORM_PROTOCOL;

typedef EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE ACPI_MEMORY_AFFINITY_DATA;
#define ACPI_MEMORY_NONVOLATILE   EFI_ACPI_6_2_MEMORY_NONVOLATILE

/**
  Function retrieves selected data of ACPI SRAT Memory Affinity Structures
  (please note that data will not be available until SRAT table installation)

  @param[out] *MemAffData         ACPI Memory Affinity Data
  @param[out] *MemAffDataLength   ACPI Memory Affinity Data Length

  @retval EFI_SUCCESS             ACPI Memory Affinity Data retrieved successfully
  @retval EFI_NOT_FOUND           ACPI Memory Affinity Data not found (SRAT ACPI table was not published)
  @retval EFI_INVALID_PARAMETER   One or more of input arguments is NULL
**/
typedef
EFI_STATUS
(EFIAPI *GET_ACPI_MEMORY_AFFINITY_DATA) (
  OUT ACPI_MEMORY_AFFINITY_DATA **MemAffData,
  OUT UINTN                     *MemAffDataLength
  );


/**
  ACPI Platform protocol provided for DXE phase
**/
struct _ACPI_PLATFORM_PROTOCOL {
  GET_ACPI_MEMORY_AFFINITY_DATA GetAcpiMemoryAffinityData;
};

extern EFI_GUID gAcpiPlatformProtocolGuid;

#endif // _ACPI_PLATFORM_PROTOCOL_H
