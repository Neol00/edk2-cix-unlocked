/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 1996 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"
#include <Library/CrcLib.h>
#include <BdatSchema.h>
#include <Guid/MemoryMapData.h>
#include <Library/CompressedVariableLib.h>
#include <Protocol/DynamicSiLibraryProtocol2.h>
#include <Protocol/DynamicSiLibraryProtocol2.h>

extern struct SystemMemoryMapHob   *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL        *mIioUds;

#include <Acpi/Bdat.h>

#ifndef MAX_HOB_ENTRY_SIZE
#define MAX_HOB_ENTRY_SIZE  60*1024
#endif

/**
  Save BSSA results to BDAT

  @param[in,out]   BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]       OffsetFromLastSchema- Offset (in bytes) from the last schema. Need it to update the schema offsets array.
  @param[in]       SchemaStartAddress  - Starting address where the BSSA result schema will be added
  @param[in, out]  SchemaIndex         - Current schema index inside the BDAT. Need it to update the schema offsets array.
  @param[in]       SchemaSize          - The size of the BSSA results schema.
  @param[out]      SchemaSpaceUsed     - The numebr bytes were filled in all schema
  @param[out]      LastSchemaSpaceUsed - The numebr bytes were filled in the last schema

  @retval EFI_SUCCESS  - BSSA BDAT scehma created successfully
  @retval !EFI_SUCCESS - BSSA BDAT scehma creation failed
**/
EFI_STATUS
SaveBssaResultsToBdat (
  IN OUT BDAT_STRUCTURE          *BdatHeaderStructPtr,
  IN     UINT32                  OffsetFromLastSchema,
  IN     EFI_PHYSICAL_ADDRESS    *SchemaStartAddress,
  IN OUT UINT8                   *SchemaIndex,
  IN     UINT32                  SchemaSize,
  OUT    UINT32                  *SchemaSpaceUsed,
  OUT    UINT32                  *LastSchemaSpaceUsed
  );

/**
  Save EWL results to BDAT

  @param[in,out]   BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]       OffsetFromLastSchema- Offset (in bytes) from the last schema. Need it to update the schema offsets array.
  @param[in]       SchemaStartAddress  - Starting address where the EWL schema will be added
  @param[in, out]  SchemaIndex         - Current schema index inside the BDAT. Need it to update the schema offsets array.
  @param[out]      SchemaSpaceUsed     - The numebr bytes were filled

  @retval EFI_SUCCESS  - EWL BDAT scehma created successfully
  @retval !EFI_SUCCESS - EWL BDAT scehma creation failed
**/
EFI_STATUS
SaveEwlToBdat (
  IN OUT BDAT_STRUCTURE          *BdatHeaderStructPtr,
  IN     UINT32                  OffsetFromLastSchema,
  IN     EFI_PHYSICAL_ADDRESS    *SchemaStartAddress,
  IN OUT UINT8                   *SchemaIndex,
  OUT    UINT32                  *SchemaSpaceUsed
  );

/**
  Save SPD date structure to BDAT

  @param[in,out]   BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]       OffsetFromLastSchema- Offset (in bytes) from the last schema. Need it to update the schema offsets array.
  @param[in]       SchemaStartAddress  - Starting address where the SPD data schema will be added
  @param[in, out]  SchemaIndex         - Current schema index inside the BDAT. Need it to update the schema offsets array.
  @param[out]      SchemaSpaceUsed     - The numebr bytes were filled

  @retval EFI_SUCCESS  - SPD BDAT scehma created successfully
  @retval !EFI_SUCCESS - SPD BDAT scehma creation failed
**/
EFI_STATUS
SaveSpdToBdat (
  IN OUT BDAT_STRUCTURE          *BdatHeaderStructPtr,
  IN     UINT32                  OffsetFromLastSchema,
  IN     EFI_PHYSICAL_ADDRESS    *SchemaStartAddress,
  IN OUT UINT8                   *SchemaIndex,
  OUT    UINT32                  *SchemaSpaceUsed
  );

/**
  Save memory training date structure to BDAT

  @param[in,out]   BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]       OffsetFromLastSchema- Offset (in bytes) from the last schema. Need it to update the schema offsets array.
  @param[in]       SchemaStartAddress  - Starting address where the memory training data schema will be added
  @param[in, out]  SchemaIndex         - Current schema index inside the BDAT. Need it to update the schema offsets array.
  @param[out]      SchemaSpaceUsed     - The numebr bytes were filled

  @retval EFI_SUCCESS  - Memory training data BDAT scehma created successfully
  @retval !EFI_SUCCESS - Memory training data BDAT scehma creation failed
**/
EFI_STATUS
SaveTrainingDataToBdat (
  IN OUT BDAT_STRUCTURE          *BdatHeaderStructPtr,
  IN     UINT32                  OffsetFromLastSchema,
  IN     EFI_PHYSICAL_ADDRESS    *SchemaStartAddress,
  IN OUT UINT8                   *SchemaIndex,
  OUT    UINT32                  *SchemaSpaceUsed
  );

/**
  Get the size of SPD data structure not include the SPD BDAT schema header.

  @retval UINT32 - Size of SPD data structure in bytes
**/
UINT32
GetSpdDataSize (
  VOID
  );

/**
  Read the SPD data for one dimm and fill up SPD entry inside SPD BDAT schema space.

  @param[out] Address           - Start Address of the buffer where the SPD entry to be filled
  @param[in]  Socket            - Socket number
  @param[in]  Channel           - Channel number inside the Socket
  @param[in]  Dimm              - Dimm slot number
  @param[in]  MaxSpdByteOffset  - The max SPD byte offset. DDR4 is 512

  @retval EFI_SUCCESS  - SPD Structure filled successfully
  @retval !EFI_SUCCESS - SPD structure creation failed
**/
EFI_STATUS
FillSpdPerDimmEntry (
  OUT EFI_PHYSICAL_ADDRESS    Address,
  IN UINT8                    Socket,
  IN UINT8                    Channel,
  IN UINT8                    Dimm,
  IN UINT16                   MaxSpdByteOffset
  );

/**
  Fill the SPD data structure inside the BDAT schema space.

  @param[out] StartAddress           - Start Address of the buffer where SPD data structure to be filled
  @param[in]  SpdDataSize            - Size of SPD data structure includes the header

  @retval EFI_SUCCESS  - SPD Structure filled successfully
  @retval !EFI_SUCCESS - SPD structure creation failed
**/
EFI_STATUS
FillSpdSchema (
  OUT EFI_PHYSICAL_ADDRESS    StartAddress,
  IN UINT32                   SpdDataSize
  );

/**
  Displays SPD content for debugging.

  @param[in] Address  - The starting address of the SPD entry

  @retval N/A
**/
VOID
DisplaySpdContents (
  IN EFI_PHYSICAL_ADDRESS  Address
  );

/**
  Get Number of Schemas From BSSA HOB

  @retval UINT16 - Number of BSSA Schemas
**/
UINT16
GetNumberOfSchemasFromBssaHob (
  VOID
  )
{
  UINT32                          GuidIdx = 0;
  UINT16                          NumberOfBssaSchemas = 0;
  EFI_STATUS                      Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL      *DynamicSiLibraryProtocol = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocolGuid, NULL, (VOID **) &DynamicSiLibraryProtocol);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return 0;
  }

  if (mSystemMemoryMap == NULL) {
    mSystemMemoryMap = DynamicSiLibraryProtocol->GetSystemMemoryMapData ();
  }

  ASSERT (mSystemMemoryMap != NULL);
  if (mSystemMemoryMap == NULL) {
    return 0;
  }

  for (GuidIdx = 0; GuidIdx < mSystemMemoryMap->Reserved9; GuidIdx++) {
    //
    // No. of HOBS per GUID added up for all GUIDs created from calls to saveToBdat ()
    //
    NumberOfBssaSchemas += mSystemMemoryMap->Reserved7[GuidIdx];
    DEBUG ((DEBUG_VERBOSE, "GuidIdx = %d, total num hobs: %d\n", GuidIdx,  mSystemMemoryMap->Reserved7[GuidIdx]));
  }
  return NumberOfBssaSchemas;
}

/**
  Create BDAT Header with necessary information.
  Allocate memory with BdatSize and if failure return status.
  If Success return the pointer address for copying the schema information

  @param[out] BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]  BdatSize            - Size of BDAT Structure
  @param[in]  NumberOfSchema      - Total nunber of of schema

  @retval EFI_SUCCESS  - BDAT Structure created successfully
  @retval !EFI_SUCCESS - BDAT structure creation failed
**/
EFI_STATUS
CreateBdatHeader (
  OUT BDAT_STRUCTURE **BdatHeaderStructPtr,
  IN UINT32          BdatSize,
  IN UINT16          NumberOfSchema
  )
{
  EFI_TIME                      EfiTime;
  UINT64                        Address = 0xffffffff;
  EFI_STATUS                    Status = EFI_SUCCESS;

  //
  // Allocating RealTime Memory for BDAT.
  //

  Status = gBS->AllocatePages (
    AllocateMaxAddress,
    EfiACPIMemoryNVS,
    EFI_SIZE_TO_PAGES(BdatSize),
    &Address
  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // The memory location where the HOB's are to be copied to
  //
  ZeroMem ((VOID *)Address, BdatSize);

  *BdatHeaderStructPtr = (BDAT_STRUCTURE *)Address;

  DEBUG ((DEBUG_VERBOSE, "\nBDAT Allocated Address = %x\n", Address));

  //
  // Create BIOS Data Signature
  //
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataSignature[0] = 'B';
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataSignature[1] = 'D';
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataSignature[2] = 'A';
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataSignature[3] = 'T';
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataSignature[4] = 'H';
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataSignature[5] = 'E';
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataSignature[6] = 'A';
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataSignature[7] = 'D';
  //
  // Structure size
  //
  (*BdatHeaderStructPtr)->BdatHeader.BiosDataStructSize = BdatSize;
  //
  // Primary Version
  //
  (*BdatHeaderStructPtr)->BdatHeader.PrimaryVersion = BDAT_PRIMARY_VER;
  //
  // Secondary Version
  //
  (*BdatHeaderStructPtr)->BdatHeader.SecondaryVersion = BDAT_SECONDARY_VER;
  //
  // CRC16 value of the BDAT_STRUCTURE
  //
  (*BdatHeaderStructPtr)->BdatHeader.Crc16 = 0;
  Status = CalculateCrc16 (
    (VOID *)(*BdatHeaderStructPtr),
    BdatSize,
    &(*BdatHeaderStructPtr)->BdatHeader.Crc16
  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    (*BdatHeaderStructPtr)->BdatHeader.Crc16 = 0xFFFF;
  }
  (*BdatHeaderStructPtr)->BdatSchemas.SchemaListLength = NumberOfSchema;
  (*BdatHeaderStructPtr)->BdatSchemas.Reserved = 0;
  (*BdatHeaderStructPtr)->BdatSchemas.Reserved1 = 0;
  //
  // Initialize the Time parameters in the SCHEMA_LIST_STRUCTURE
  //
  Status = gRT->GetTime (&EfiTime, NULL);
  if (!EFI_ERROR (Status)) {
    (*BdatHeaderStructPtr)->BdatSchemas.Year = EfiTime.Year;
    (*BdatHeaderStructPtr)->BdatSchemas.Month = EfiTime.Month;
    (*BdatHeaderStructPtr)->BdatSchemas.Day = EfiTime.Day;
    (*BdatHeaderStructPtr)->BdatSchemas.Hour = EfiTime.Hour;
    (*BdatHeaderStructPtr)->BdatSchemas.Minute = EfiTime.Minute;
    (*BdatHeaderStructPtr)->BdatSchemas.Second = EfiTime.Second;
  }
  return Status;
}

/**
  Dump BDAT Table to serial log

  Example 1: There are 2 schema: BSSA RMT and EWL.

  Print BDAT Table
  Address     0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
  00000000 0x42 0x44 0x41 0x54 0x48 0x45 0x41 0x44 0x30 0x18 0x00 0x00 0xD7 0x7A 0x00 0x00
  00000010 0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  00000020 0x02 0x00 0x00 0x00 0xE4 0x07 0x08 0x0E 0x00 0x29 0x03 0x00 0x34 0x00 0x00 0x00
  00000030 0xFA 0x07 0x00 0x00 0x28 0xE9 0xF4 0x08 0x5F 0x0F 0xD4 0x46 0x84 0x10 0x47 0x9F
  00000040 0xDA 0x27 0x9D 0xB6 0xC6 0x07 0x00 0x00 0x57 0x7F 0x00 0x00 0x00 0x00 0x00 0xA5
  00000050 0x98 0x88 0xFE 0x4C 0x00 0x00 0x00 0x0D 0xEB 0x7A 0x47 0x07 0xDE 0x77 0x42 0xA4
  00000060 0xE7 0x87 0x81 0x0B 0x10 0x00 0x31 0xF1 0x98 0x88 0xFE 0xF2 0x45 0x81 0xD9 0xF4
  ...
  ...
  000007F0 0x00 0x00 0x00 0x5A 0xA5 0x5A 0xA5 0x5A 0xA5 0x5A 0x2F 0x53 0xFE 0xBF 0x3B 0xCA
  00000800 0x6C 0x41 0xA0 0xF6 0xFF 0xE4 0xE7 0x1E 0x3A 0x0D 0x36 0x10 0x00 0x00 0x9F 0xEF
  00000810 0x70 0x33 0x71 0x75 0x05 0x38 0xB0 0x46 0x9F 0xED 0x60 0xF2 0x82 0x48 0x6C 0xFC
  00000820 0x20 0x10 0x00 0x00 0x12 0x00 0x00 0x00 0x2E 0x81 0x50 0x51 0x00 0x00 0x00 0x00
  00000830 0x01 0x00 0x00 0x00 0x12 0x00 0x01 0x00 0x00 0x00 0x7E 0x07 0x17 0x18 0x01 0xFF
  00000840 0xFF 0xFF 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00

  Example 2: There is 1 schema: EWL
  Address     0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
  00000000 0x42 0x44 0x41 0x54 0x48 0x45 0x41 0x44 0x66 0x10 0x00 0x00 0xA9 0x44 0x00 0x00
  00000010 0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  00000020 0x01 0x00 0x00 0x00 0xE4 0x07 0x08 0x0E 0x01 0x0F 0x2C 0x00 0x30 0x00 0x00 0x00
  00000030 0x2F 0x53 0xFE 0xBF 0x3B 0xCA 0x6C 0x41 0xA0 0xF6 0xFF 0xE4 0xE7 0x1E 0x3A 0x0D
  00000040 0x36 0x10 0x00 0x00 0x9F 0xEF 0x70 0x33 0x71 0x75 0x05 0x38 0xB0 0x46 0x9F 0xED
  00000050 0x60 0xF2 0x82 0x48 0x6C 0xFC 0x20 0x10 0x00 0x00 0x12 0x00 0x00 0x00 0x2E 0x81
  00000060 0x50 0x51 0x00 0x00 0x00 0x00 0x01 0x00 0x00 0x00 0x12 0x00 0x01 0x00 0x00 0x00
  00000070 0x7E 0x07 0x17 0x18 0x01 0xFF 0xFF 0xFF 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  00000080 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00

  @param[out] BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]  BdatSize            - Size of BDAT Structure

  @retval None
**/
VOID
DumpBdatTable(
  IN BDAT_STRUCTURE **BdatHeaderStructPtr,
  IN UINT32 BdatSize
)
{
  UINT32  i = 0;
  UINT8   *Table = NULL;

  Table = (UINT8 *)(*BdatHeaderStructPtr);
  DEBUG ((DEBUG_VERBOSE, "\nPrint BDAT Table\n"));

  //
  // Print address header
  //
  DEBUG ((DEBUG_VERBOSE, "Address "));
  for (i = 0; i < 16; i++) {
    DEBUG ((DEBUG_VERBOSE, "    %01x", i));
  }

  i = 0;
  while (i < BdatSize) {
    if ((i % 16) == 0) {
      DEBUG ((DEBUG_VERBOSE, "\n%08x ", (i / 16) * 16));
    } else {
      DEBUG ((DEBUG_VERBOSE, " "));
    }

    DEBUG ((DEBUG_VERBOSE, "0x%02x", Table[i]));

    if (i == BdatSize - 1) {
      DEBUG ((DEBUG_VERBOSE, "\n"));
    }

    i++;

  }
  DEBUG ((DEBUG_VERBOSE, "\n"));
}

/**
  Copy BDAT Table pointer to scratchpad 5 register

  @param[in] BdatAddress - Bdat Table Address to be copied to Scratchpad 5 register

  @retval None
**/
VOID
CopyBdatPointerToScratchPad5 (
  IN UINT64 BdatAddress
  )
{
  UINT8                           Socket = 0;
  EFI_STATUS                      Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL      *DynamicSiLibraryProtocol = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocolGuid, NULL, (VOID **) &DynamicSiLibraryProtocol);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return;
  }

  //
  // Copy BDAT base address to ScratchPad5
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].Valid) {
      DynamicSiLibraryProtocol->WriteScratchpad5 (Socket, (UINT32) BdatAddress);
      DEBUG ((DEBUG_VERBOSE, "Scratchpad_Debug PatchBdaAcpiTable: Verify Non Sticky Scratchpad5 0x%08x\n", BdatAddress));
    }
  }
}

/**
    Update the BDAT ACPI table: Multiple instances of the BDAT DATA HOB are placed into one contiguos memory range

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success
**/
EFI_STATUS
PatchBdatAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  EFI_PHYSICAL_ADDRESS              Address = 0;
  UINT32                            Idx = 0;
  UINT8                             Checksum = 0;
  BDAT_STRUCTURE                    *BdatHeaderStructPtr = NULL;
  UINT32                            TotalBDATstructureSize = 0;
  UINT32                            BdatHeaderSize = 0;
  EFI_HOB_GUID_TYPE                 *GuidHob = NULL;
  UINT16                            NumberBssaSchemas = 0;
  UINT16                            TotalNumberSchemas = 0;
  UINT32                            BssaSchemaSize = 0;
  EFI_BDAT_ACPI_DESCRIPTION_TABLE   *BdatAcpiTable = NULL;
  EWL_PRIVATE_DATA                  *EwlPrivateData = NULL;
  EFI_GUID                          EWLDataGuid = EWL_ID_GUID;
  UINT32                            EwlSchemaSize = 0;
  MEM_TRAINING_DATA_STRUCTURE       *MemTrainingData = NULL;
  MEM_TRAINING_DATA_HOB_HEADER      *TrainingDataHobHeader = NULL;
  UINT32                            TrainingDataSchemaSize = 0;
  UINT32                            RemainingSchemaSpace = 0;
  UINT32                            SpdDataSize = 0;
  UINT32                            SpdSchemaSize = 0;
  UINT8                             SchemaIndex = 0;
  UINT32                            SpaceUsed = 0;
  UINT32                            LastSchemaSpaceUsed = 0;
  UINT32                            OffsetFromLastSchema = 0;

  BdatAcpiTable = (EFI_BDAT_ACPI_DESCRIPTION_TABLE *)Table;
  DEBUG ((DEBUG_INFO, "\nPatchBdatAcpiTable Started\n"));

  //
  // Gather BSSA schema info
  //
  NumberBssaSchemas = GetNumberOfSchemasFromBssaHob ();

  BssaSchemaSize = mSystemMemoryMap->Reserved6 + (NumberBssaSchemas * sizeof (BDAT_SCHEMA_HEADER_STRUCTURE)); //Total size of all HOBs created by SaveToBdat() + NumberBssaSchemas*headerPerSchema
  DEBUG ((DEBUG_VERBOSE, "NumberBssaSchemas = %d, total BSSA schema size: %d\n", NumberBssaSchemas, BssaSchemaSize));

  TotalNumberSchemas = NumberBssaSchemas;

  TotalBDATstructureSize += BssaSchemaSize;

  //
  // Gather EWL schema info
  //
  GuidHob = GetFirstGuidHob (&EWLDataGuid);
  if (GuidHob != NULL) {
    DEBUG ((DEBUG_VERBOSE, "Found EWL with GUID  %g\n", &EWLDataGuid));

    TotalNumberSchemas += 1;

    EwlPrivateData = GET_GUID_HOB_DATA (GuidHob);

    EwlSchemaSize = sizeof (BDAT_SCHEMA_HEADER_STRUCTURE) + EwlPrivateData->status.Header.Size;

    DEBUG ((DEBUG_VERBOSE, "EWL schema size: %d\n", EwlSchemaSize));

    TotalBDATstructureSize += EwlSchemaSize;
  }

  //
  // Gather SPD schema info
  //
  SpdSchemaSize = 0;
  if (PcdGetBool (SaveSpdToBdat)) {
    SpdDataSize = GetSpdDataSize ();

    if (SpdDataSize > 0) {
      TotalNumberSchemas += 1;
      SpdSchemaSize = sizeof (BDAT_SCHEMA_HEADER_STRUCTURE) + SpdDataSize;
      DEBUG ((DEBUG_VERBOSE, "SPD data schema size: %d\n", SpdSchemaSize));

      TotalBDATstructureSize += SpdSchemaSize;
    }
  } // PcdGetBool (SaveSpdToBdat)

  //
  // Gather Memory training data schema info
  //
  TrainingDataSchemaSize = 0;
  if (PcdGetBool (SaveMrcTrainingDataToBdat)) {
    GuidHob = GetFirstGuidHob (&gMemTrainingDataHobGuid);
    if (GuidHob != NULL) {
      DEBUG ((DEBUG_VERBOSE, "Found memory training data HOB with GUID  %g\n", &gMemTrainingDataHobGuid));

      TotalNumberSchemas += 1;

      TrainingDataHobHeader = GET_GUID_HOB_DATA (GuidHob);

      MemTrainingData = (MEM_TRAINING_DATA_STRUCTURE *)((EFI_PHYSICAL_ADDRESS)TrainingDataHobHeader + sizeof (MEM_TRAINING_DATA_HOB_HEADER));

      TrainingDataSchemaSize = sizeof (BDAT_SCHEMA_HEADER_STRUCTURE) + MemTrainingData->Header.Size;

      DEBUG ((DEBUG_VERBOSE, "Memory training data schema size: %d\n", TrainingDataSchemaSize));

      TotalBDATstructureSize += TrainingDataSchemaSize;
    }
  } // PcdGetBool (SaveMrcTrainingDataToBdat)

  BdatHeaderSize = sizeof (BDAT_STRUCTURE) + (TotalNumberSchemas * (sizeof (UINT32)));
  TotalBDATstructureSize += BdatHeaderSize;

  //
  // This variable is used to keep track the remain space in the BDAT payload (schema section) to
  // prevent overflow the allocated RT BDAT buffer.
  //
  RemainingSchemaSpace = TotalBDATstructureSize - BdatHeaderSize;

  DEBUG ((DEBUG_INFO, "Total BDAT size:%d, BDAT header size: %d, Total schema:%d \n", TotalBDATstructureSize, BdatHeaderSize, TotalNumberSchemas));

  Status = CreateBdatHeader (&BdatHeaderStructPtr, TotalBDATstructureSize, TotalNumberSchemas);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "BdatRegionAddress = 0x%x\n", (UINT64)BdatHeaderStructPtr));
  CopyBdatPointerToScratchPad5 ((UINT64)BdatHeaderStructPtr);

  //
  // Update BDAT ACPI table
  //
  BdatAcpiTable->BdatGas.Address = (UINT64)BdatHeaderStructPtr;

  //
  // Starting address of the first schema
  //
  Address = (EFI_PHYSICAL_ADDRESS)BdatHeaderStructPtr + BdatHeaderSize;

  //
  // Saving to RT Memory BDAT Data received from HOBs generated due to BSSA call/calls to SaveToBdat()
  //
  SpaceUsed = 0;

  //
  // The first schema starts right after the BDAT header structure
  //
  LastSchemaSpaceUsed = BdatHeaderSize;  // It will used as the OffsetFromLastSchema for the next schema if BSSA is not available
  OffsetFromLastSchema = BdatHeaderSize;

  if (BssaSchemaSize > 0) {

    Status = SaveBssaResultsToBdat (BdatHeaderStructPtr, OffsetFromLastSchema, &Address, &SchemaIndex, BssaSchemaSize, &SpaceUsed, &LastSchemaSpaceUsed);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "Faield to add BSSA result to BDAT\n"));
    }
  }

  //
  // Update the starting address of next schema and remaining space.
  //
  Address += SpaceUsed;
  RemainingSchemaSpace -= SpaceUsed;
  OffsetFromLastSchema = LastSchemaSpaceUsed;

  //
  // Saving to RT Memory BDAT Data received from EWL HOB
  //
  SpaceUsed = 0;

  if (EwlSchemaSize > 0) {

    if (RemainingSchemaSpace < EwlSchemaSize) {
      DEBUG ((DEBUG_ERROR, "Not enough space to add EWL schema.\n"));
      goto End;
    }

    Status = SaveEwlToBdat (BdatHeaderStructPtr, OffsetFromLastSchema ,&Address, &SchemaIndex, &SpaceUsed);

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "Failed to add EWL to BDAT.\n"));
    }

  }

  //
  // Update the starting address of next schema and remaining space.
  //
  Address += SpaceUsed;
  RemainingSchemaSpace -= SpaceUsed;
  OffsetFromLastSchema = SpaceUsed;

  //
  // Add SPD schema
  //
  SpaceUsed = 0;

  if (SpdSchemaSize > 0) {

    if (RemainingSchemaSpace < SpdSchemaSize) {
      DEBUG ((DEBUG_ERROR, "Not enough space to add SPD schema.\n"));
      goto End;
    }

    Status = SaveSpdToBdat (BdatHeaderStructPtr, OffsetFromLastSchema, &Address, &SchemaIndex, &SpaceUsed);

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "Failed to add SPD to BDAT.\n"));
    }

  }

  //
  // Update the starting address of next schema and remaining space.
  //
  Address += SpaceUsed;
  RemainingSchemaSpace -= SpaceUsed;
  OffsetFromLastSchema = SpaceUsed;

  //
  // Add memory training data schema
  //
  SpaceUsed = 0;

  if (TrainingDataSchemaSize > 0) {

    if (RemainingSchemaSpace < TrainingDataSchemaSize) {
      DEBUG ((DEBUG_ERROR, "Not enough space to add memory training data schema.\n"));
      goto End;
    }

    Status = SaveTrainingDataToBdat (BdatHeaderStructPtr, OffsetFromLastSchema, &Address, &SchemaIndex, &SpaceUsed);

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "Failed to add memory training data to BDAT.\n"));
    }
  }

  //
  // Update the starting address of next schema and remaining space.
  //
  Address += SpaceUsed;
  RemainingSchemaSpace -= SpaceUsed;
  OffsetFromLastSchema = SpaceUsed;

  DEBUG ((DEBUG_VERBOSE, "Final SchemaIndex:%d RemainingSchemaSpace:%d\n", SchemaIndex, RemainingSchemaSpace));

  End:

  //
  // Update checksum
  //
  BdatAcpiTable->Header.Checksum = 0;
  Checksum = 0;
  for(Idx = 0; Idx < sizeof(EFI_BDAT_ACPI_DESCRIPTION_TABLE); Idx++) {
    Checksum = Checksum + (UINT8) (((UINT8 *)(BdatAcpiTable))[Idx]);
  }
  BdatAcpiTable->Header.Checksum = (UINT8) (0 - Checksum);

  DumpBdatTable (&BdatHeaderStructPtr, TotalBDATstructureSize);

  return Status;
}


/**
  Displays SPD content for debugging.

  @param[in] Address  - The starting address of the SPD entry

  @retval N/A
**/
VOID
DisplaySpdContents (
  IN EFI_PHYSICAL_ADDRESS  Address
  )
{
  UINT16                  Index;
  MEM_SPD_ENTRY_TYPE0    *EntryHeaderPtr;

  EntryHeaderPtr = (MEM_SPD_ENTRY_TYPE0  *)Address;

  //
  // Print the Socket, Channel and Dimm information.
  //
  DEBUG ((DEBUG_VERBOSE, "START_PRINT_SPD S%dC%dD%d:\n",
    EntryHeaderPtr->MemoryLocation.Socket,
    EntryHeaderPtr->MemoryLocation.Channel,
    EntryHeaderPtr->MemoryLocation.Dimm));

  //
  // Print the Column Number of the SPD data.
  //
  for (Index = 0; Index < 0x10; Index++) {
    DEBUG ((DEBUG_VERBOSE, " %02x", Index));
  }

  Address += sizeof (MEM_SPD_ENTRY_TYPE0);

  for (Index = 0; Index < EntryHeaderPtr->NumberOfBytes; Index++) {

    //
    // Print the Carriage Return and Byte Index of SPD data.
    //
    if ((Index % 0x10) == 0) {

      //
      // Split the SPD data for every 256 bytes
      //
      if (((Index % 0x100) == 0) && (Index != 0)) {
        DEBUG ((DEBUG_VERBOSE, "\n"));
      }

      DEBUG ((DEBUG_VERBOSE, "\n%02x:", (UINT8) (Index & 0x00F0)));
    }

    DEBUG ((DEBUG_VERBOSE, " %02x", *(UINT8 *)(Address + (EFI_PHYSICAL_ADDRESS)Index)));
  }

  DEBUG ((DEBUG_VERBOSE, "\n"));


  DEBUG ((DEBUG_VERBOSE, "STOP_PRINT_SPD\n"));

} // DisplaySpdContents

/**
  Get the size of SPD data structure not include the SPD BDAT schema header.

  @retval UINT32 - Size of SPD data structure in bytes
**/
UINT32
GetSpdDataSize (
  VOID
  )
{
  UINT32  SchemaSize = 0;
  UINT8   Socket;
  UINT8   Channel;
  UINT8   Dimm;
  UINT32  SpdBytesPerDimm = 0;
  UINT8   NumberOfDimmPresent = 0;

  EFI_STATUS                  Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL  *DynamicSiLibraryProtocol = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocolGuid, NULL, (VOID **) &DynamicSiLibraryProtocol);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return 0;
  }

  if (mSystemMemoryMap == NULL) {
    mSystemMemoryMap = DynamicSiLibraryProtocol->GetSystemMemoryMapData ();
  }

  if (mSystemMemoryMap == NULL) {
    return 0;
  }

  ASSERT (mSystemMemoryMap->DramType == SPD_TYPE_DDR4);
  SpdBytesPerDimm = MAX_SPD_BYTE_DDR4;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Channel = 0; Channel < MAX_CH; Channel++) {
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].Present != 0) {
          NumberOfDimmPresent += 1;
        }
      }
    }
  }

  //
  // Total entries
  //
  SchemaSize = (SpdBytesPerDimm + sizeof (MEM_SPD_ENTRY_TYPE0)) * NumberOfDimmPresent;

  //
  // Add the SPD raw data header
  //
  SchemaSize += sizeof (MEM_SPD_RAW_DATA_HEADER);

  return SchemaSize;

}


/**
  Read the SPD data for one dimm and fill up SPD entry inside SPD BDAT schema space.

  @param[out] Address           - Start Address of the buffer where the SPD entry to be filled
  @param[in]  Socket            - Socket number
  @param[in]  Channel           - Channel number inside the Socket
  @param[in]  Dimm              - Dimm slot number
  @param[in]  MaxSpdByteOffset  - The max SPD byte offset. DDR4 is 512

  @retval EFI_SUCCESS  - SPD Structure filled successfully
  @retval !EFI_SUCCESS - SPD structure creation failed
**/
EFI_STATUS
FillSpdPerDimmEntry (
  OUT EFI_PHYSICAL_ADDRESS    Address,
  IN UINT8                    Socket,
  IN UINT8                    Channel,
  IN UINT8                    Dimm,
  IN UINT16                   MaxSpdByteOffset
  )
{
  UINT16               SpdByteOffset = 0;
  UINT8                SpdData = 0;
  MEM_SPD_ENTRY_TYPE0  *EntryHeaderPtr;
  EFI_STATUS           Status = EFI_SUCCESS;

  DYNAMIC_SI_LIBARY_PROTOCOL2  *DynamicSiLibraryProtocol2 = NULL;

  DEBUG ((DEBUG_VERBOSE, "Fill Spd entry for Socket:%d Channel:%d Dimm:%d at location:0x%x \n", Socket, Channel, Dimm, Address));

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  EntryHeaderPtr = (MEM_SPD_ENTRY_TYPE0  *)Address;

  //
  // Add the entry header. We use type 0.
  //
  EntryHeaderPtr->Header.Type = MemSpdDataType0;
  EntryHeaderPtr->Header.Size = sizeof (MEM_SPD_ENTRY_TYPE0) + MaxSpdByteOffset;

  EntryHeaderPtr->MemoryLocation.Socket = Socket;
  EntryHeaderPtr->MemoryLocation.Channel = Channel;
  EntryHeaderPtr->MemoryLocation.Dimm = Dimm;

  EntryHeaderPtr->NumberOfBytes = MaxSpdByteOffset;

  Address += sizeof (MEM_SPD_ENTRY_TYPE0);

  for (SpdByteOffset = 0; SpdByteOffset < MaxSpdByteOffset; SpdByteOffset++) {
    Status = DynamicSiLibraryProtocol2->SpdReadByte (Socket, Channel, Dimm, SpdByteOffset, &SpdData);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_WARN, "Failed to read SPD data at Socket:%d Channel:%d Dimm:%d SpdByteOffset:%d Status:0x%x\n",
        Socket, Channel, Dimm, SpdByteOffset, Status));
      return Status;
    }

    *(UINT8 *)Address = SpdData;
    Address += 1;
  } // SpdByteOffset

  return Status;
}

/**
  Fill the SPD data structure inside the BDAT schema space.

  @param[out] StartAddress           - Start Address of the buffer where SPD data structure to be filled
  @param[in]  SpdDataSize            - Size of SPD data structure includes the header

  @retval EFI_SUCCESS  - SPD Structure filled successfully
  @retval !EFI_SUCCESS - SPD structure creation failed
**/
EFI_STATUS
FillSpdSchema (
  OUT EFI_PHYSICAL_ADDRESS    StartAddress,
  IN UINT32                   SpdDataSize
  )
{
  UINT8                   Socket;
  UINT8                   Channel;
  UINT8                   Dimm;
  UINT16                  MaxSpdByteOffset = 0;
  UINT32                  RemainedSpace;
  EFI_PHYSICAL_ADDRESS    Address;
  MEM_SPD_RAW_DATA_HEADER *SpdDataHeaderStructPtr;
  EFI_STATUS              Status = EFI_SUCCESS;

  DYNAMIC_SI_LIBARY_PROTOCOL  *DynamicSiLibraryProtocol = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocolGuid, NULL, (VOID **) &DynamicSiLibraryProtocol);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  if (mSystemMemoryMap == NULL) {
    mSystemMemoryMap = DynamicSiLibraryProtocol->GetSystemMemoryMapData ();
  }

  if (mSystemMemoryMap == NULL) {
    return EFI_DEVICE_ERROR;;
  }
  if (StartAddress == 0) {
    return EFI_INVALID_PARAMETER;
  }

  RemainedSpace = SpdDataSize;
  Address = StartAddress;
  SpdDataHeaderStructPtr = (MEM_SPD_RAW_DATA_HEADER *)StartAddress;

  //
  // Fill up the header of the SPD data strcuture
  //
  SpdDataHeaderStructPtr->MemSpdGuid = gSpdVersion1Guid;
  SpdDataHeaderStructPtr->Size = SpdDataSize;
  SpdDataHeaderStructPtr->Reserved = 0;

  Address += sizeof (MEM_SPD_RAW_DATA_HEADER);
  RemainedSpace -= sizeof (MEM_SPD_RAW_DATA_HEADER);

  ASSERT (mSystemMemoryMap->DramType == SPD_TYPE_DDR4);
  MaxSpdByteOffset = MAX_SPD_BYTE_DDR4;

  //
  // Iterate through all populated DIMMs and add them
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Channel = 0; Channel < MAX_CH; Channel++) {
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {

        if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].Present != 0) {

          if (RemainedSpace < sizeof (MEM_SPD_ENTRY_TYPE0) + MaxSpdByteOffset) {
            DEBUG ((DEBUG_ERROR, "Run out of allocated SPD data space. RemainedSpace:%d required space:%d\n",
              RemainedSpace, sizeof (MEM_SPD_ENTRY_TYPE0) + MaxSpdByteOffset));
            return RETURN_OUT_OF_RESOURCES;
          }

          Status = FillSpdPerDimmEntry (Address, Socket, Channel, Dimm, MaxSpdByteOffset);
          if (EFI_ERROR (Status)) {
            DEBUG ((DEBUG_ERROR, "Failed to read SPD data at Socket:%d Channel:%d Dimm:%d Status:0x%x.\n",  Socket, Channel, Dimm, Status));
            return Status;
          }

          DisplaySpdContents (Address);

          Address += sizeof (MEM_SPD_ENTRY_TYPE0) + MaxSpdByteOffset;
          RemainedSpace -= sizeof (MEM_SPD_ENTRY_TYPE0) + MaxSpdByteOffset;

        }
      } // Dimm
    } // Channel
  } // Socket

  //
  // Update CRC
  //
  SpdDataHeaderStructPtr->Crc = 0;
  SpdDataHeaderStructPtr->Crc = CalculateCrc32 ((VOID *) SpdDataHeaderStructPtr, SpdDataHeaderStructPtr->Size);

  return EFI_SUCCESS;

}

/**
  Save BSSA results to BDAT

  @param[in,out]   BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]       OffsetFromLastSchema- Offset (in bytes) from the last schema. Need it to update the schema offsets array.
  @param[in]       SchemaStartAddress  - Starting address where the BSSA result schema will be added
  @param[in, out]  SchemaIndex         - Current schema index inside the BDAT. Need it to update the schema offsets array.
  @param[in]       SchemaSize          - The size of the BSSA results schema.
  @param[out]      SchemaSpaceUsed     - The numebr bytes were filled in all schema
  @param[out]      LastSchemaSpaceUsed - The numebr bytes were filled in the last schema

  @retval EFI_SUCCESS  - BSSA BDAT scehma created successfully
  @retval !EFI_SUCCESS - BSSA BDAT scehma creation failed
**/
EFI_STATUS
SaveBssaResultsToBdat (
  IN OUT BDAT_STRUCTURE          *BdatHeaderStructPtr,
  IN     UINT32                  OffsetFromLastSchema,
  IN     EFI_PHYSICAL_ADDRESS    *SchemaStartAddress,
  IN OUT UINT8                   *SchemaIndex,
  IN     UINT32                  SchemaSize,
  OUT    UINT32                  *SchemaSpaceUsed,
  OUT    UINT32                  *LastSchemaSpaceUsed
  )
{
  EFI_STATUS                        Status                      = EFI_SUCCESS;
  UINT32                            *SchemaAddrLocationArray    = NULL;
  UINT32                            CurrentHobSize              = 0;
  EFI_HOB_GUID_TYPE                 *GuidHob                    = NULL;
  VOID                              *HobData                    = NULL;
  UINT32                            PreviousSchemaSize          = 0;
  EFI_GUID                          gEfiMemoryMapDataHobBdatBssaGuid  = {0};
  UINT32                            GuidIdx                     = 0;
  UINT32                            HobIdx                      = 0;
  UINT32                            RemainingHobSizeBssaSchema  = 0;
  BDAT_SCHEMA_HEADER_STRUCTURE      *BssaSchemaHeaderPtr        = NULL;
  EFI_PHYSICAL_ADDRESS              Address                     = 0;

  if ((BdatHeaderStructPtr == NULL) || (SchemaStartAddress == NULL) || (SchemaIndex == NULL) || (SchemaSpaceUsed == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RemainingHobSizeBssaSchema = SchemaSize;
  Address = *SchemaStartAddress;
  *SchemaSpaceUsed = 0;

  //
  // Update the schema offset array for its first BSSA schema
  //
  SchemaAddrLocationArray = (UINT32 *)((EFI_PHYSICAL_ADDRESS)BdatHeaderStructPtr + sizeof(BDAT_STRUCTURE));

  if (*SchemaIndex < BdatHeaderStructPtr->BdatSchemas.SchemaListLength) {
    if (*SchemaIndex == 0) {
      SchemaAddrLocationArray[*SchemaIndex] = OffsetFromLastSchema;
    }
    else {
      SchemaAddrLocationArray[*SchemaIndex] = SchemaAddrLocationArray[*SchemaIndex - 1] + OffsetFromLastSchema;
    }
  }

  for (GuidIdx = 0; GuidIdx < mSystemMemoryMap->Reserved9; GuidIdx++) {

    gEfiMemoryMapDataHobBdatBssaGuid  = mSystemMemoryMap->Reserved8[GuidIdx]; //get first GUID instance
    GuidHob = GetFirstGuidHob (&gEfiMemoryMapDataHobBdatBssaGuid);

    for (HobIdx = 0; HobIdx < mSystemMemoryMap->Reserved7[GuidIdx]; HobIdx++) { //looping through all HOBs linked to that GUID

      ASSERT (GuidHob != NULL);
      if (GuidHob == NULL) {
        return EFI_NOT_FOUND;
      }

      HobData = GET_GUID_HOB_DATA (GuidHob);
      CurrentHobSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
      DEBUG ((DEBUG_VERBOSE, "Initial HOB size  %d; remaining HOB size %d\n", CurrentHobSize, RemainingHobSizeBssaSchema));

      //
      // Setting the header first
      //
      if (RemainingHobSizeBssaSchema < sizeof(BDAT_SCHEMA_HEADER_STRUCTURE)) {
        //
        // Nothing we can do, break execution
        //
        DEBUG ((DEBUG_WARN, "Not enough space to add schema header to BIOS SSA result\n"));
        RemainingHobSizeBssaSchema = 0;
        break;
      }

      //
      // Each HOB has a header added to it (BDAT_SCHEMA_HEADER_STRUCTURE)
      //
      Address = Address + (EFI_PHYSICAL_ADDRESS)PreviousSchemaSize;

      BssaSchemaHeaderPtr = (BDAT_SCHEMA_HEADER_STRUCTURE *)Address;
      BssaSchemaHeaderPtr->SchemaId = gEfiMemoryMapDataHobBdatBssaGuid;
      RemainingHobSizeBssaSchema -= sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
      *SchemaSpaceUsed = *SchemaSpaceUsed + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);

      Address = Address + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);

      //
      // CRC16 value of the BDAT_SCHEMA_HEADER_STRUCTURE
      //
      BssaSchemaHeaderPtr->Crc16 = 0;
      Status = CalculateCrc16 (
        (VOID *) BssaSchemaHeaderPtr,
        sizeof (BDAT_SCHEMA_HEADER_STRUCTURE),
        &BssaSchemaHeaderPtr->Crc16
        );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        BssaSchemaHeaderPtr->Crc16 = 0xFFFF;
      }

      if (RemainingHobSizeBssaSchema < CurrentHobSize) {
        DEBUG ((DEBUG_WARN, "Not enough space to add complete BIOS SSA result\n"));
        CurrentHobSize = RemainingHobSizeBssaSchema;
      }

      //
      // HOB size won't overflow a UINT32.
      //
      BssaSchemaHeaderPtr->DataSize = (UINT32)CurrentHobSize + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
      DEBUG ((DEBUG_VERBOSE, "Setting schema %g size to %d\n", &(BssaSchemaHeaderPtr->SchemaId), BssaSchemaHeaderPtr->DataSize));
      //
      // HOB size won't overflow a UINT32.
      //
      PreviousSchemaSize = (UINT32)CurrentHobSize + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);

      //
      // Copy HOB to RT Memory
      //
      CopyMem ((VOID *)Address, (VOID *)HobData, (UINT32)CurrentHobSize);
      //
      // HOB size won't overflow a UINT32.
      //
      DEBUG ((DEBUG_VERBOSE, "HOB size  %d; remaining SSA HOB size %d\n", CurrentHobSize, RemainingHobSizeBssaSchema));
      RemainingHobSizeBssaSchema -= (UINT32)CurrentHobSize;
      *SchemaSpaceUsed = *SchemaSpaceUsed + (UINT32)CurrentHobSize;

      *SchemaIndex = *SchemaIndex + 1;

      if (RemainingHobSizeBssaSchema == 0) {
        break;
      }

      GuidHob = GET_NEXT_HOB (GuidHob); // Increment to next HOB
      GuidHob = GetNextGuidHob (&gEfiMemoryMapDataHobBdatBssaGuid, GuidHob);  // Now search for next instance of the BDAT HOB
      if (GuidHob == NULL) {
        break;
      }

      //
      // Update the schema offset arrary
      //
      if (*SchemaIndex < BdatHeaderStructPtr->BdatSchemas.SchemaListLength) {
        SchemaAddrLocationArray[*SchemaIndex] = SchemaAddrLocationArray[*SchemaIndex - 1] + PreviousSchemaSize;
      }
    }

    if (RemainingHobSizeBssaSchema == 0) {
      break;
    }

  }

  *LastSchemaSpaceUsed = PreviousSchemaSize;

  return EFI_SUCCESS;
} // SaveBssaResultsToBdat


/**
  Save EWL results to BDAT

  @param[in,out]   BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]       OffsetFromLastSchema- Offset (in bytes) from the last schema. Need it to update the schema offsets array.
  @param[in]       SchemaStartAddress  - Starting address where the EWL schema will be added
  @param[in, out]  SchemaIndex         - Current schema index inside the BDAT. Need it to update the schema offsets array.
  @param[out]      SchemaSpaceUsed     - The numebr bytes were filled

  @retval EFI_SUCCESS  - EWL BDAT scehma created successfully
  @retval !EFI_SUCCESS - EWL BDAT scehma creation failed
**/
EFI_STATUS
SaveEwlToBdat (
  IN OUT BDAT_STRUCTURE          *BdatHeaderStructPtr,
  IN     UINT32                  OffsetFromLastSchema,
  IN     EFI_PHYSICAL_ADDRESS    *SchemaStartAddress,
  IN OUT UINT8                   *SchemaIndex,
  OUT    UINT32                  *SchemaSpaceUsed
  )
{
  EFI_STATUS                        Status                      = EFI_SUCCESS;
  UINT32                            *SchemaAddrLocationArray    = NULL;
  EFI_HOB_GUID_TYPE                 *GuidHob                    = NULL;
  EFI_PHYSICAL_ADDRESS              Address                     = 0;
  EWL_PRIVATE_DATA                  *EwlPrivateData             = NULL;
  EFI_GUID                          EWLDataGuid                 = EWL_ID_GUID;
  BDAT_SCHEMA_HEADER_STRUCTURE      *EwlSchemaHeaderPtr         = NULL;
  UINT32                            EwlSize                     = 0;

  if ((BdatHeaderStructPtr == NULL) || (SchemaStartAddress == NULL) || (SchemaIndex == NULL) || (SchemaSpaceUsed == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Address = *SchemaStartAddress;
  *SchemaSpaceUsed = 0;

  DEBUG ((DEBUG_VERBOSE, "\nStarting to copy EWL schema at Address = 0x%x\n", Address));

  //
  // Update the schema offset arrary
  //
  SchemaAddrLocationArray = (UINT32 *)((EFI_PHYSICAL_ADDRESS)BdatHeaderStructPtr + sizeof(BDAT_STRUCTURE));

  if (*SchemaIndex < BdatHeaderStructPtr->BdatSchemas.SchemaListLength) {
    if (*SchemaIndex == 0) {
      SchemaAddrLocationArray[*SchemaIndex] = OffsetFromLastSchema;
    }
    else {
      SchemaAddrLocationArray[*SchemaIndex] = SchemaAddrLocationArray[*SchemaIndex - 1] + OffsetFromLastSchema;
    }
  }

  EwlSchemaHeaderPtr = (BDAT_SCHEMA_HEADER_STRUCTURE *)Address;
  EwlSchemaHeaderPtr->SchemaId = gEwlBdatSchemaGuid;
  *SchemaSpaceUsed = *SchemaSpaceUsed + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);

  //
  // CRC16 value of the BDAT_SCHEMA_HEADER_STRUCTURE
  //
  EwlSchemaHeaderPtr->Crc16 = 0;
  Status = CalculateCrc16 (
    (VOID *)EwlSchemaHeaderPtr,
    sizeof(BDAT_SCHEMA_HEADER_STRUCTURE),
    &EwlSchemaHeaderPtr->Crc16
  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    EwlSchemaHeaderPtr->Crc16 = 0xFFFF;
  }

  GuidHob = GetFirstGuidHob (&EWLDataGuid);
  EwlPrivateData = GET_GUID_HOB_DATA (GuidHob);

  EwlSize = EwlPrivateData->status.Header.Size;

  EwlSchemaHeaderPtr->DataSize = EwlSize + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);

  //
  // Copy EWL HOB to RT Memory
  //
  Address = Address + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
  CopyMem ((VOID *)Address, (VOID *)&(EwlPrivateData->status), EwlSize);
  *SchemaSpaceUsed = *SchemaSpaceUsed + EwlSize;

  *SchemaIndex = *SchemaIndex + 1;

  return EFI_SUCCESS;
} //SaveEwlToBdat

/**
  Check if current boot is slow boot or not

  @retval TRUE  - Slow boot path
  @retval FALSE - not slow boot path
  **/
BOOLEAN
IsSlowBoot (
  VOID
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL2     *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return TRUE;
  }

  return DynamicSiLibraryProtocol2->IsSlowBoot ();
} // IsSlowBoot

/**
  Save SPD date structure to BDAT

  @param[in,out]   BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]       OffsetFromLastSchema- Offset (in bytes) from the last schema. Need it to update the schema offsets array.
  @param[in]       SchemaStartAddress  - Starting address where the SPD data schema will be added
  @param[in, out]  SchemaIndex         - Current schema index inside the BDAT. Need it to update the schema offsets array.
  @param[out]      SchemaSpaceUsed     - The numebr bytes were filled

  @retval EFI_SUCCESS  - SPD BDAT scehma created successfully
  @retval !EFI_SUCCESS - SPD BDAT scehma creation failed
**/
EFI_STATUS
SaveSpdToBdat (
  IN OUT BDAT_STRUCTURE          *BdatHeaderStructPtr,
  IN     UINT32                  OffsetFromLastSchema,
  IN     EFI_PHYSICAL_ADDRESS    *SchemaStartAddress,
  IN OUT UINT8                   *SchemaIndex,
  OUT    UINT32                  *SchemaSpaceUsed
  )
{
  EFI_STATUS                        Status                      = EFI_SUCCESS;
  UINT32                            *SchemaAddrLocationArray    = NULL;
  UINT32                            SpdDataSize                 = 0;
  EFI_PHYSICAL_ADDRESS              Address                     = 0;
  BDAT_SCHEMA_HEADER_STRUCTURE      *SpdSchemaHeaderPtr         = NULL;
//  UINT16                            *SpdVariableName            = L"SpdData";
  VOID                              *VariableData               = NULL;
  BOOLEAN                           SaveToVariable              = TRUE;
  UINTN                             CompareValue;

  if ((BdatHeaderStructPtr == NULL) || (SchemaStartAddress == NULL) || (SchemaIndex == NULL) || (SchemaSpaceUsed == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Address = *SchemaStartAddress;
  *SchemaSpaceUsed = 0;

  //
  // Add SPD schema
  //
  DEBUG ((DEBUG_VERBOSE, "\nStarting to add SPD schema  at Address = 0x%x\n", Address));

  //
  // Update the schema offset arrary
  //
  SchemaAddrLocationArray = (UINT32 *)((EFI_PHYSICAL_ADDRESS)BdatHeaderStructPtr + sizeof(BDAT_STRUCTURE));

  if (*SchemaIndex < BdatHeaderStructPtr->BdatSchemas.SchemaListLength) {
    if (*SchemaIndex == 0) {
      SchemaAddrLocationArray[*SchemaIndex] = OffsetFromLastSchema;
    }
    else {
      SchemaAddrLocationArray[*SchemaIndex] = SchemaAddrLocationArray[*SchemaIndex - 1] + OffsetFromLastSchema;
    }
  }

  SpdSchemaHeaderPtr = (BDAT_SCHEMA_HEADER_STRUCTURE *)Address;
  SpdSchemaHeaderPtr->SchemaId = gSpdBdatSchemaGuid;

  //
  // CRC16 value of the BDAT_SCHEMA_HEADER_STRUCTURE
  //
  SpdSchemaHeaderPtr->Crc16 = 0;
  Status = CalculateCrc16 (
    (VOID *)SpdSchemaHeaderPtr,
    sizeof(BDAT_SCHEMA_HEADER_STRUCTURE),
    &SpdSchemaHeaderPtr->Crc16
  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    SpdSchemaHeaderPtr->Crc16 = 0xFFFF;
  }

  Address = Address + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
  *SchemaSpaceUsed = *SchemaSpaceUsed + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);

  SpdDataSize = GetSpdDataSize ();

  if (IsSlowBoot ()) {
    //
    // Read SPD via Smbus, fill up the SPD data structure.
    //
    FillSpdSchema (Address, SpdDataSize);

    //
    // Save the SPD data structure to EFI variables to save fast boot time
    //
    // Before save variable, read and comapre to current data. If they are the same, then
    // don't save it.
    // If fail to read the variable(the variable doesn't exist), save variable.
    //

    SaveToVariable = TRUE;

    VariableData = AllocatePool (SpdDataSize);
    if (VariableData == NULL) {
      DEBUG ((DEBUG_ERROR, "Not able to allocate space to store SPD variable data.\n"));
      Status = EFI_OUT_OF_RESOURCES;
      goto End;
    }

//    Status = LoadCompressedVariable (SpdVariableName, gSpdVariableGuid, VariableData, SpdDataSize);

    if (!EFI_ERROR (Status)) {
      CompareValue = CompareMem ((VOID *)Address, VariableData, SpdDataSize);
      if (CompareValue == 0) {
        SaveToVariable = FALSE;
        DEBUG ((DEBUG_VERBOSE, "No change to the SPD data, don't save variable.\n"));
      }
    }

    if (SaveToVariable) {
//      Status = CompressAndSaveToVariable (SpdVariableName, gSpdVariableGuid, (VOID *)Address, SpdDataSize);

      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to save SPD data to variable.\n"));
        goto End;
      }

      DEBUG ((DEBUG_VERBOSE, "Save SPD data to EFI variable.\n"));
    }
  } else {
    //
    // Fast boot, read the SPD data from vaiable
    //
//    Status = LoadCompressedVariable (SpdVariableName, gSpdVariableGuid, (VOID *)Address, SpdDataSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to load SPD data from variable.\n"));
      goto End;
    }
    DEBUG ((DEBUG_VERBOSE, "Fill with SPD data from EFI variable.\n"));
  }

  End:

  if (VariableData != NULL) {
    FreePool (VariableData);
  }

  SpdSchemaHeaderPtr->DataSize = SpdDataSize + sizeof (BDAT_SCHEMA_HEADER_STRUCTURE);
  *SchemaSpaceUsed = *SchemaSpaceUsed + SpdDataSize;

  *SchemaIndex = *SchemaIndex + 1;
  return Status;
} //SaveSpdToBdat

/**
  Save memory training date structure to BDAT

  @param[in,out]   BdatHeaderStructPtr - Pointer to BDAT Structure
  @param[in]       OffsetFromLastSchema- Offset (in bytes) from the last schema. Need it to update the schema offsets array.
  @param[in]       SchemaStartAddress  - Starting address where the memory training data schema will be added
  @param[in, out]  SchemaIndex         - Current schema index inside the BDAT. Need it to update the schema offsets array.
  @param[out]      SchemaSpaceUsed     - The numebr bytes were filled

  @retval EFI_SUCCESS  - Memory training data BDAT scehma created successfully
  @retval !EFI_SUCCESS - Memory training data BDAT scehma creation failed
**/
EFI_STATUS
SaveTrainingDataToBdat (
  IN OUT BDAT_STRUCTURE          *BdatHeaderStructPtr,
  IN     UINT32                  OffsetFromLastSchema,
  IN     EFI_PHYSICAL_ADDRESS    *SchemaStartAddress,
  IN OUT UINT8                   *SchemaIndex,
  OUT    UINT32                  *SchemaSpaceUsed
  )
{
  EFI_STATUS                        Status                      = EFI_SUCCESS;
  UINT32                            *SchemaAddrLocationArray    = NULL;
  EFI_HOB_GUID_TYPE                 *GuidHob                    = NULL;
  EFI_PHYSICAL_ADDRESS              Address                     = 0;
  MEM_TRAINING_DATA_HEADER          *TrainingDataHeader         = NULL;
  MEM_TRAINING_DATA_HOB_HEADER      *TrainingDataHobHeader      = NULL;
  EFI_GUID                          TrainingDataGuid            = gMemTrainingDataHobGuid;
  BDAT_SCHEMA_HEADER_STRUCTURE      *SchemaHeaderPtr            = NULL;
  INT32                             RemainingDataSize           = 0;
  UINT32                            HobSize                     = 0;
  UINT32                            TrainingDataSize            = 0;
  UINT8                             HobIndex                    = 0;

  if ((BdatHeaderStructPtr == NULL) || (SchemaStartAddress == NULL) || (SchemaIndex == NULL) || (SchemaSpaceUsed == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Address = *SchemaStartAddress;
  *SchemaSpaceUsed = 0;

  DEBUG ((DEBUG_VERBOSE, "\nStarting to copy memory training data schema at Address = 0x%x\n", Address));

  //
  // Update the schema offset arrary
  //
  SchemaAddrLocationArray = (UINT32 *)((EFI_PHYSICAL_ADDRESS)BdatHeaderStructPtr + sizeof(BDAT_STRUCTURE));

  if (*SchemaIndex < BdatHeaderStructPtr->BdatSchemas.SchemaListLength) {
    if (*SchemaIndex == 0) {
      SchemaAddrLocationArray[*SchemaIndex] = OffsetFromLastSchema;
    }
    else {
      SchemaAddrLocationArray[*SchemaIndex] = SchemaAddrLocationArray[*SchemaIndex - 1] + OffsetFromLastSchema;
    }
  }

  SchemaHeaderPtr = (BDAT_SCHEMA_HEADER_STRUCTURE *)Address;
  SchemaHeaderPtr->SchemaId = gMemTrainingDataBdatSchemaGuid;
  *SchemaSpaceUsed = *SchemaSpaceUsed + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);

  //
  // CRC16 value of the BDAT_SCHEMA_HEADER_STRUCTURE
  //
  SchemaHeaderPtr->Crc16 = 0;
  Status = CalculateCrc16 (
    (VOID *)SchemaHeaderPtr,
    sizeof(BDAT_SCHEMA_HEADER_STRUCTURE),
    &SchemaHeaderPtr->Crc16
  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    SchemaHeaderPtr->Crc16 = 0xFFFF;
  }

  GuidHob = GetFirstGuidHob (&TrainingDataGuid);

  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "Not found training data HOB with GUID:%g\n", &TrainingDataGuid));
    return EFI_NOT_FOUND;
  }

  TrainingDataHobHeader = GET_GUID_HOB_DATA (GuidHob);
  TrainingDataSize = TrainingDataHobHeader->Size - sizeof (MEM_TRAINING_DATA_HOB_HEADER); // This is the size of data to copy to the RT memory.

  TrainingDataHeader = (MEM_TRAINING_DATA_HEADER *)((EFI_PHYSICAL_ADDRESS)TrainingDataHobHeader + sizeof (MEM_TRAINING_DATA_HOB_HEADER));

  HobSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
  ASSERT (TrainingDataSize <= HobSize);

  HobIndex += 1;
  DEBUG ((DEBUG_VERBOSE, "Memory training data structre size:%d\n", TrainingDataHeader->Size));
  SchemaHeaderPtr->DataSize = TrainingDataHeader->Size + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);

  //
  // This is the total data size in the training data structure.
  //
  RemainingDataSize = (INT32) TrainingDataHeader->Size;

  DEBUG ((DEBUG_VERBOSE, "Remaining training data:%d\n", RemainingDataSize));
  //
  // Copy training data HOBs to RT Memory
  //
  Address = Address + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
  CopyMem ((VOID *)Address, (VOID *)TrainingDataHeader, TrainingDataSize);

  *SchemaSpaceUsed = *SchemaSpaceUsed + TrainingDataSize;

  Address = Address + TrainingDataSize;
  RemainingDataSize -= TrainingDataSize;

 while (RemainingDataSize > 0) {

    GuidHob = GET_NEXT_HOB (GuidHob); // Increment to next HOB
    GuidHob = GetNextGuidHob (&TrainingDataGuid, GuidHob);  // Now search for next instance of the BDAT HOB

    ASSERT (GuidHob != NULL);
    if (GuidHob == NULL) {
      DEBUG ((DEBUG_ERROR, "Not found training data HOB with GUID:%g\n", &TrainingDataGuid));
      return EFI_NOT_FOUND;
    }

    TrainingDataHobHeader = GET_GUID_HOB_DATA (GuidHob);
    TrainingDataSize = TrainingDataHobHeader->Size - sizeof (MEM_TRAINING_DATA_HOB_HEADER); // This is the size of data to copy to the RT memory.

    HobSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
    ASSERT (TrainingDataSize <= HobSize);

    HobIndex += 1;

    //
    // Copy training data HOBs to RT Memory
    //
    CopyMem ((VOID *)Address, (VOID *)((EFI_PHYSICAL_ADDRESS)TrainingDataHobHeader + sizeof (MEM_TRAINING_DATA_HOB_HEADER)), TrainingDataSize);

    *SchemaSpaceUsed = *SchemaSpaceUsed + TrainingDataSize;

    Address = Address + TrainingDataSize;
    RemainingDataSize -= TrainingDataSize;
    DEBUG ((DEBUG_VERBOSE, "Remaining training data:%d after copying Hob:%d \n", RemainingDataSize, HobIndex));
  }

  //
  // Update training data header structure CRC, after all the training data were copied from HOBs to a RT contiguous memory region.
  //
  TrainingDataHeader->Crc = 0;
  TrainingDataHeader->Crc = CalculateCrc32 ((VOID *) TrainingDataHeader, TrainingDataHeader->Size);

  *SchemaIndex = *SchemaIndex + 1;

  return EFI_SUCCESS;
} //SaveTrainingDataToBdat
