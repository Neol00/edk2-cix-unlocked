/** @file
  Library for building ACPI Tables.

  @copyright
  Copyright 2016 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _BUILD_ACPI_TABLES_LIB_H_
#define _BUILD_ACPI_TABLES_LIB_H_

#include <IndustryStandard/Acpi.h>

/** Structure of a MADT and SRAT sub-structure header.

  This structure contains the type and length fields, which are common to every
  sub-structure of the MADT and SRAT tables. A pointer to any structure can be cast as this.
**/
typedef struct {
  UINT8 Type;
  UINT8 Length;
} STRUCTURE_HEADER;

/**
  Initialize the MADT header.

  This function fills in the MADT's standard table header with correct values,
  except for the length and checksum fields, which are filled in when building
  the whole table.

  @param[in,out]  MadtHeader    Pointer to the MADT header structure.

  @retval EFI_SUCCESS           Successfully initialized the MADT header.
  @retval EFI_INVALID_PARAMETER Pointer parameter was null.
**/
EFI_STATUS
InitializeMadtHeader (
  IN OUT EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *MadtHeader
  );

/**
  Initialize the SRAT header.

  This function fills in the SRAT's standard table header with correct values,
  except for the length and checksum fields, which are filled in when building
  the whole table.

  @param[in,out]  SratHeader    Pointer to the SRAT header structure.

  @retval EFI_SUCCESS           Successfully initialized the SRAT header.
  @retval EFI_INVALID_PARAMETER Pointer parameter was null.
**/
EFI_STATUS
InitializeSratHeader (
  IN OUT EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER *SratHeader
  );

/**
  Copy an ACPI sub-structure; MADT and SRAT supported

  This function validates the structure type and size of a sub-structure
  and returns a newly allocated copy of it.

  @param[in]  Header            Pointer to the header of the table.
  @param[in]  Structure         Pointer to the structure to copy.
  @param[in]  NewStructure      Newly allocated copy of the structure.

  @retval EFI_SUCCESS           Successfully copied the structure.
  @retval EFI_INVALID_PARAMETER Pointer parameter was null.
  @retval EFI_INVALID_PARAMETER Structure type was unknown.
  @retval EFI_INVALID_PARAMETER Structure length was wrong for its type.
  @retval EFI_UNSUPPORTED       Header passed in is not supported.
**/
EFI_STATUS
CopyStructure (
  IN  EFI_ACPI_DESCRIPTION_HEADER *Header,
  IN  STRUCTURE_HEADER *Structure,
  OUT STRUCTURE_HEADER **NewStructure
  );

/**
  Build ACPI Table. MADT and SRAT tables supported.

  This function builds the ACPI table from the header plus the list of sub-structures
  passed in. The table returned by this function is ready to be installed using
  the ACPI table protocol's InstallAcpiTable function, which copies it into
  ACPI memory. After that, the caller should free the memory returned by this
  function.

  @param[in]  AcpiHeader             Pointer to the header structure.
  @param[in]  TableSpecificHdrLength Size of the table specific header, not the ACPI standard header size.
  @param[in]  Structures             Pointer to an array of sub-structure pointers.
  @param[in]  StructureCount         Number of structure pointers in the array.
  @param[out] NewTable               Newly allocated and initialized pointer to the ACPI Table.

  @retval EFI_SUCCESS           Successfully built the ACPI table.
  @retval EFI_INVALID_PARAMETER Pointer parameter was null.
  @retval EFI_INVALID_PARAMETER Header parameter had the wrong signature.
  @retval EFI_OUT_OF_RESOURCES  Space for the ACPI Table could not be allocated.
**/
EFI_STATUS
BuildAcpiTable (
  IN  EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN  UINTN                        TableSpecificHdrLength,
  IN  STRUCTURE_HEADER             **Structures,
  IN  UINTN                        StructureCount,
  OUT UINT8                        **NewTable
  );

#endif // _BUILD_ACPI_TABLES_LIB_H_
