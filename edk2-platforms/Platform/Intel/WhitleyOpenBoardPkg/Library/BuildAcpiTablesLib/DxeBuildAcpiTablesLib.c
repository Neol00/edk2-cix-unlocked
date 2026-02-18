/** @file
  Library for building ACPI Tables.

  @copyright
  Copyright 2016 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiDxe.h>
#include <Acpi/Madt.h>
#include <Acpi/Srat.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <UncoreCommonIncludes.h>
#include <Library/BuildAcpiTablesLib.h> // library class being implemented

STRUCTURE_HEADER mMadtStructureTable[] = {
  {EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC,          sizeof (EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_STRUCTURE)},
  {EFI_ACPI_6_2_IO_APIC,                       sizeof (EFI_ACPI_6_2_IO_APIC_STRUCTURE)},
  {EFI_ACPI_6_2_INTERRUPT_SOURCE_OVERRIDE,     sizeof (EFI_ACPI_6_2_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE)},
  {EFI_ACPI_6_2_NON_MASKABLE_INTERRUPT_SOURCE, sizeof (EFI_ACPI_6_2_NON_MASKABLE_INTERRUPT_SOURCE_STRUCTURE)},
  {EFI_ACPI_6_2_LOCAL_APIC_NMI,                sizeof (EFI_ACPI_6_2_LOCAL_APIC_NMI_STRUCTURE)},
  {EFI_ACPI_6_2_LOCAL_APIC_ADDRESS_OVERRIDE,   sizeof (EFI_ACPI_6_2_LOCAL_APIC_ADDRESS_OVERRIDE_STRUCTURE)},
  {EFI_ACPI_6_2_IO_SAPIC,                      sizeof (EFI_ACPI_6_2_IO_SAPIC_STRUCTURE)},
  {EFI_ACPI_6_2_LOCAL_SAPIC,                   sizeof (EFI_ACPI_6_2_PROCESSOR_LOCAL_SAPIC_STRUCTURE)},
  {EFI_ACPI_6_2_PLATFORM_INTERRUPT_SOURCES,    sizeof (EFI_ACPI_6_2_PLATFORM_INTERRUPT_SOURCES_STRUCTURE)},
  {EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC,        sizeof (EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_STRUCTURE)},
  {EFI_ACPI_6_2_LOCAL_X2APIC_NMI,              sizeof (EFI_ACPI_6_2_LOCAL_X2APIC_NMI_STRUCTURE)}
};

STRUCTURE_HEADER mSratStructureTable[] = {
  {EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY, sizeof (EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE)},
  {EFI_ACPI_6_2_MEMORY_AFFINITY,                     sizeof (EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE)},
  {EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY,     sizeof (EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY_STRUCTURE)}
};

/**
  Get the size of the ACPI table.

  This function calculates the size needed for the ACPI Table based on the number and
  size of the sub-structures that will compose it.

  @param[in]  TableSpecificHdrLength  Size of the table specific header, not the ACPI standard header size.
  @param[in]  Structures              Pointer to an array of sub-structure pointers.
  @param[in]  StructureCount          Number of structure pointers in the array.

  @return     Total size needed for the ACPI table.
**/
UINT32
GetTableSize (
  IN  UINTN                 TableSpecificHdrLength,
  IN  STRUCTURE_HEADER      **Structures,
  IN  UINTN                 StructureCount
  )
{
  UINT32  TableLength;
  UINT32  Index;

  //
  // Compute size of the ACPI table; header plus all structures needed.
  //
  TableLength = (UINT32) TableSpecificHdrLength;

  for (Index = 0; Index < StructureCount; Index++) {
    ASSERT (Structures[Index] != NULL);
    if (Structures[Index] == NULL) {
      return 0;
    }

    TableLength += Structures[Index]->Length;
  }

  return TableLength;
}

/**
  Allocate the ACPI Table.

  This function allocates space for the ACPI table based on the number and size of
  the sub-structures that will compose it.

  @param[in]  TableSpecificHdrLength  Size of the table specific header, not the ACPI standard header size.
  @param[in]  Structures  Pointer to an array of sub-structure pointers.
  @param[in]  StructureCount  Number of structure pointers in the array.
  @param[out] Table            Newly allocated ACPI Table pointer.

  @retval EFI_SUCCESS           Successfully allocated the Table.
  @retval EFI_OUT_OF_RESOURCES  Space for the Table could not be allocated.
**/
EFI_STATUS
AllocateTable (
  IN  UINTN                        TableSpecificHdrLength,
  IN  STRUCTURE_HEADER             **Structures,
  IN  UINTN                        StructureCount,
  OUT EFI_ACPI_DESCRIPTION_HEADER  **Table
  )
{
  EFI_STATUS  Status;
  UINT32      Size;
  EFI_ACPI_DESCRIPTION_HEADER *InternalTable;

  //
  // Get the size of the ACPI table and allocate memory.
  //
  Size = GetTableSize (TableSpecificHdrLength, Structures, StructureCount);
  InternalTable = (EFI_ACPI_DESCRIPTION_HEADER *) AllocatePool (Size);

  if (InternalTable == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((
      DEBUG_ERROR,
      "Failed to allocate %d bytes for ACPI Table\n",
      Size
      ));
  } else {
    Status = EFI_SUCCESS;
    *Table = InternalTable;
  }

  return Status;
}

/**
  Initialize the header.

  This function fills in the standard table header with correct values,
  except for the length and checksum fields, which are filled in later.

  @param[in,out]  Header        Pointer to the header structure.

  @retval EFI_SUCCESS           Successfully initialized the header.
  @retval EFI_INVALID_PARAMETER Pointer parameter was null.
**/
EFI_STATUS
InitializeHeader (
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER *Header,
  IN      UINT32                      Signature,
  IN      UINT8                       Revision,
  IN      UINT32                      OemRevision
  )
{
  UINT64 AcpiTableOemId;

  if (Header == NULL) {
    DEBUG ((DEBUG_ERROR, "Header pointer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  Header->Signature  = Signature;
  Header->Length     = 0; // filled in by Build function
  Header->Revision   = Revision;
  Header->Checksum   = 0; // filled in by InstallAcpiTable

  CopyMem (
    (VOID *) &Header->OemId,
    PcdGetPtr (PcdAcpiDefaultOemId),
    sizeof (Header->OemId)
    );

  AcpiTableOemId = PcdGet64 (PcdAcpiDefaultOemTableId);
  CopyMem (
    (VOID *) &Header->OemTableId,
    (VOID *) &AcpiTableOemId,
    sizeof (Header->OemTableId)
    );

  Header->OemRevision     = OemRevision;
  Header->CreatorId       = EFI_ACPI_CREATOR_ID;
  Header->CreatorRevision = EFI_ACPI_CREATOR_REVISION;

  return EFI_SUCCESS;
}

/**
  Initialize the MADT header.

  This function fills in the MADT's standard table header with correct values,
  except for the length and checksum fields, which are filled in later.

  @param[in,out]  MadtHeader    Pointer to the MADT header structure.

  @retval EFI_SUCCESS           Successfully initialized the MADT header.
  @retval EFI_INVALID_PARAMETER Pointer parameter was null.
**/
EFI_STATUS
InitializeMadtHeader (
  IN OUT EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *MadtHeader
  )
{
  EFI_STATUS Status;

  if (MadtHeader == NULL) {
    DEBUG ((DEBUG_ERROR, "MADT header pointer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = InitializeHeader (
    &MadtHeader->Header,
    EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
    EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION,
    EFI_ACPI_OEM_MADT_REVISION
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MadtHeader->LocalApicAddress       = EFI_ACPI_LOCAL_APIC_ADDRESS;
  MadtHeader->Flags                  = EFI_ACPI_6_2_MULTIPLE_APIC_FLAGS;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS Status;

  if (SratHeader == NULL) {
    DEBUG ((DEBUG_ERROR, "SRAT header pointer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = InitializeHeader (
    &SratHeader->Header,
    EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE,
    EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_REVISION,
    EFI_ACPI_OEM_SRAT_REVISION
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SratHeader->Reserved1 = EFI_ACPI_SRAT_RESERVED_FOR_BACKWARD_COMPATIBILITY;
  SratHeader->Reserved2 = EFI_ACPI_RESERVED_QWORD;

  return EFI_SUCCESS;
}

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
  )
{
  STRUCTURE_HEADER      *NewStructureInternal;
  STRUCTURE_HEADER      *StructureTable;
  UINTN                 TableNumEntries;
  BOOLEAN               EntryFound;
  UINT8                 Index;

  //
  // Initialize the number of table entries and the table based on the table header passed in.
  //
  if (Header->Signature == EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE) {
    TableNumEntries = sizeof (mMadtStructureTable) / sizeof (STRUCTURE_HEADER);
    StructureTable = mMadtStructureTable;
  } else if (Header->Signature == EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE) {
    TableNumEntries = sizeof (mSratStructureTable) / sizeof (STRUCTURE_HEADER);
    StructureTable = mSratStructureTable;
  } else {
    return EFI_UNSUPPORTED;
  }

  //
  // Check the incoming structure against the table of supported structures.
  //
  EntryFound = FALSE;
  for (Index = 0; Index < TableNumEntries; Index++) {
    if (Structure->Type == StructureTable[Index].Type) {
      if (Structure->Length == StructureTable[Index].Length) {
        EntryFound = TRUE;
      } else {
        DEBUG ((
          DEBUG_ERROR,
          "Invalid length for structure type %d: expected %d, actually %d\n",
          Structure->Type,
          StructureTable[Index].Length,
          Structure->Length
          ));
        return EFI_INVALID_PARAMETER;
      }
    }
  }

  //
  // If no entry in the table matches the structure type and length passed in
  // then return invalid parameter.
  //
  if (!EntryFound) {
    DEBUG ((
      DEBUG_ERROR,
      "Unknown structure type: %d\n",
      Structure->Type
      ));
    return EFI_INVALID_PARAMETER;
  }

  NewStructureInternal = (STRUCTURE_HEADER *) AllocatePool (Structure->Length);
  if (NewStructureInternal == NULL) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to allocate %d bytes for type %d structure\n",
      Structure->Length,
      Structure->Type
      ));
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (
    (VOID *) NewStructureInternal,
    (VOID *) Structure,
    Structure->Length
    );

  *NewStructure = NewStructureInternal;
  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                  Status;
  EFI_ACPI_DESCRIPTION_HEADER *InternalTable;
  UINTN                       Index;
  UINT8                       *CurrPtr;
  UINT8                       *EndOfTablePtr;

  if (AcpiHeader == NULL) {
    DEBUG ((DEBUG_ERROR, "AcpiHeader pointer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (AcpiHeader->Signature != EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE &&
      AcpiHeader->Signature != EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE) {
    DEBUG ((
      DEBUG_ERROR,
      "MADT or SRAT header signature is expected, actually 0x%08x\n",
      AcpiHeader->Signature
      ));
    return EFI_INVALID_PARAMETER;
  }

  if (Structures == NULL) {
    DEBUG ((DEBUG_ERROR, "Structure array pointer is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < StructureCount; Index++) {
    if (Structures[Index] == NULL) {
      DEBUG ((DEBUG_ERROR, "Structure pointer %d is NULL\n", Index));
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Allocate the memory needed for the table.
  //
  Status = AllocateTable (
    TableSpecificHdrLength,
    Structures,
    StructureCount,
    &InternalTable
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Copy Header and patch in structure length, checksum is programmed later
  // after all structures are populated.
  //
  CopyMem (
    (VOID *) InternalTable,
    (VOID *) AcpiHeader,
    TableSpecificHdrLength
    );

  InternalTable->Length = GetTableSize (TableSpecificHdrLength, Structures, StructureCount);

  //
  // Copy all the sub structures to the table.
  //
  CurrPtr = ((UINT8 *) InternalTable) + TableSpecificHdrLength;
  EndOfTablePtr = ((UINT8 *) InternalTable) + InternalTable->Length;

  for (Index = 0; Index < StructureCount; Index++) {
    ASSERT (Structures[Index] != NULL);
    if (Structures[Index] == NULL) {
      break;
    }

    CopyMem (
      (VOID *) CurrPtr,
      (VOID *) Structures[Index],
      Structures[Index]->Length
      );

    CurrPtr += Structures[Index]->Length;
    ASSERT (CurrPtr <= EndOfTablePtr);
    if (CurrPtr > EndOfTablePtr) {
      break;
    }
  }

  //
  // Update the return pointer.
  //
  *NewTable = (UINT8 *) InternalTable;
  return EFI_SUCCESS;
}
