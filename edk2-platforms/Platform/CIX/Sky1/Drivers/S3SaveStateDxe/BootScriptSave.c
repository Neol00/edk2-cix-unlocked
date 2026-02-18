/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "InternalBootScript.h"

/**
  To get the start address from which a new boot time s3 boot script entry will write into.
  If the table is not exist, the function will first allocate a buffer for the table.
  If the table buffer is not enough for the new entry, the funtion will invoke reallocate
  to enlarge buffer.

  @param EntryLength      the new entry length.

  @retval the address from which the a new s3 boot script entry will write into
 **/
UINT8 *
S3BootScriptGetEntryAddAddress (
  UINT8  EntryLength
  )
{
  EFI_PHYSICAL_ADDRESS          S3TableBase;
  EFI_PHYSICAL_ADDRESS          NewS3TableBase;
  UINT8                         *NewEntryPtr;
  UINT32                        TableLength;
  UINT16                        PageNumber;
  EFI_STATUS                    Status;
  CIX_BOOT_SCRIPT_TABLE_HEADER  *ScriptTableInfo;

  S3TableBase = (EFI_PHYSICAL_ADDRESS)(UINTN)(mS3BootScriptTablePtr->TableBase);
  if (S3TableBase == 0) {
    //
    // The table is not exist. This is the first to add entry.
    // Allocate ACPI script table space under 4G memory.
    //
    S3TableBase = 0xffffffff;
    Status      = gBS->AllocatePages (
                         AllocateMaxAddress,
                         EfiReservedMemoryType,
                         S3_BOOT_SCRIPT_DYNAMIC_ALLOCATE_PAGE_NUMBER,
                         (EFI_PHYSICAL_ADDRESS *)&S3TableBase
                         );

    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return 0;
    }

    ZeroMem ((EFI_PHYSICAL_ADDRESS *)S3TableBase, EFI_PAGES_TO_SIZE (S3_BOOT_SCRIPT_DYNAMIC_ALLOCATE_PAGE_NUMBER));

    //
    // Fill Table Header
    //
    ScriptTableInfo                              = (CIX_BOOT_SCRIPT_TABLE_HEADER *)(UINTN)S3TableBase;
    ScriptTableInfo->OpCode                      = CIX_S3_BOOT_SCRIPT_TABLE_OPCODE;
    ScriptTableInfo->Length                      = (UINT8)sizeof (CIX_BOOT_SCRIPT_TABLE_HEADER);
    ScriptTableInfo->Version                     = BOOT_SCRIPT_TABLE_VERSION;
    ScriptTableInfo->TableLength                 = 0; // will be calculate at CloseTable
    mS3BootScriptTablePtr->TableLength           = sizeof (CIX_BOOT_SCRIPT_TABLE_HEADER);
    mS3BootScriptTablePtr->TableBase             = (UINT8 *)(UINTN)S3TableBase;
    mS3BootScriptTablePtr->TableMemoryPageNumber = S3_BOOT_SCRIPT_DYNAMIC_ALLOCATE_PAGE_NUMBER;
  }

  PageNumber  = (UINT16)mS3BootScriptTablePtr->TableMemoryPageNumber;
  TableLength =  mS3BootScriptTablePtr->TableLength;
  if (EFI_PAGES_TO_SIZE ((UINTN)PageNumber) < (TableLength + EntryLength + sizeof (CIX_BOOT_SCRIPT_TERMINATE))) {
    //
    // The buffer is too small to hold the table, Reallocate the buffer
    //
    NewS3TableBase = 0xffffffff;
    Status         = gBS->AllocatePages (
                            AllocateMaxAddress,
                            EfiReservedMemoryType,
                            PageNumber + S3_BOOT_SCRIPT_DYNAMIC_ALLOCATE_PAGE_NUMBER,
                            (EFI_PHYSICAL_ADDRESS *)&NewS3TableBase
                            );

    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return 0;
    }

    ZeroMem ((EFI_PHYSICAL_ADDRESS *)NewS3TableBase, EFI_PAGES_TO_SIZE (PageNumber + S3_BOOT_SCRIPT_DYNAMIC_ALLOCATE_PAGE_NUMBER));

    CopyMem ((VOID *)(UINTN)NewS3TableBase, (VOID *)(UINTN)S3TableBase, TableLength);
    gBS->FreePages (S3TableBase, mS3BootScriptTablePtr->TableMemoryPageNumber);

    mS3BootScriptTablePtr->TableBase             = (UINT8 *)(UINTN)NewS3TableBase;
    mS3BootScriptTablePtr->TableMemoryPageNumber =  (UINT16)(PageNumber + S3_BOOT_SCRIPT_DYNAMIC_ALLOCATE_PAGE_NUMBER);
  }

  //
  // calculate the the start address for the new entry.
  //
  NewEntryPtr = mS3BootScriptTablePtr->TableBase + TableLength;

  //
  // update the table lenghth
  //
  mS3BootScriptTablePtr->TableLength =  TableLength + EntryLength;

  //
  // In the boot time, we will not append the termination entry to the boot script
  // table until the callers think there is no boot time data that should be added and
  // it is caller's responsibility to explicit call the CloseTable.
  //
  //

  return NewEntryPtr;
}

/**
  Adds a record for a memory write operation into boot script table.

  @param Width   The width of the I/O operations.Enumerated in S3_BOOT_SCRIPT_WIDTH.
  @param Address The base address of the memory operations
  @param Count   The number of memory operations to perform.
  @param Buffer  The source buffer from which to write the data.

  @retval RETURN_OUT_OF_RESOURCES  Not enough memory for the table do operation.
  @retval RETURN_SUCCESS           Opcode is added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveMemWrite (
  IN  S3_BOOT_SCRIPT_WIDTH  Width,
  IN  UINT64                Address,
  IN  UINT8                 Count,
  IN  VOID                  *Buffer
  )
{
  UINT8                      Length;
  UINT8                      *Script;
  UINT8                      WidthInByte;
  CIX_BOOT_SCRIPT_MEM_WRITE  ScriptMemWrite;

  WidthInByte = (UINT8)(0x01 << (Width & 0x03));

  //
  // Truncation check
  //
  if (WidthInByte * Count > MAX_UINT8 - sizeof (CIX_BOOT_SCRIPT_MEM_WRITE)) {
    return RETURN_OUT_OF_RESOURCES;
  }

  Length = (UINT8)(sizeof (CIX_BOOT_SCRIPT_MEM_WRITE) + (WidthInByte * Count));

  Script = S3BootScriptGetEntryAddAddress (Length);
  if (Script == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }

  //
  // Build script data
  //
  ScriptMemWrite.OpCode  = CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE;
  ScriptMemWrite.Length  = Length;
  ScriptMemWrite.Width   = Width;
  ScriptMemWrite.Address = Address;
  ScriptMemWrite.Count   = Count;

  CopyMem ((VOID *)Script, (VOID *)&ScriptMemWrite, sizeof (CIX_BOOT_SCRIPT_MEM_WRITE));
  CopyMem ((VOID *)(Script + sizeof (CIX_BOOT_SCRIPT_MEM_WRITE)), Buffer, WidthInByte * Count);

  return RETURN_SUCCESS;
}

/**
  Adds a record for a memory modify operation into boot script table.

  @param Width     The width of the I/O operations.Enumerated in S3_BOOT_SCRIPT_WIDTH.
  @param Address   The base address of the memory operations. Address needs alignment if required
  @param Data      A pointer to the data to be OR-ed.
  @param DataMask  A pointer to the data mask to be AND-ed with the data read from the register.

  @retval RETURN_OUT_OF_RESOURCES  Not enough memory for the table do operation.
  @retval RETURN_SUCCESS           Opcode is added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveMemReadWrite (
  IN  S3_BOOT_SCRIPT_WIDTH  Width,
  IN  UINT64                Address,
  IN  VOID                  *Data,
  IN  VOID                  *DataMask
  )
{
  UINT8                           Length;
  UINT8                           *Script;
  UINT8                           WidthInByte;
  CIX_BOOT_SCRIPT_MEM_READ_WRITE  ScriptMemReadWrite;

  WidthInByte = (UINT8)(0x01 << (Width & 0x03));
  Length      = (UINT8)(sizeof (CIX_BOOT_SCRIPT_MEM_READ_WRITE) + (WidthInByte * 2));

  Script = S3BootScriptGetEntryAddAddress (Length);
  if (Script == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }

  //
  // Build script data
  //
  ScriptMemReadWrite.OpCode  = CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE;
  ScriptMemReadWrite.Length  = Length;
  ScriptMemReadWrite.Width   = Width;
  ScriptMemReadWrite.Address = Address;

  CopyMem ((VOID *)Script, (VOID *)&ScriptMemReadWrite, sizeof (CIX_BOOT_SCRIPT_MEM_READ_WRITE));
  CopyMem ((VOID *)(Script + sizeof (CIX_BOOT_SCRIPT_MEM_READ_WRITE)), Data, WidthInByte);
  CopyMem ((VOID *)(Script + sizeof (CIX_BOOT_SCRIPT_MEM_READ_WRITE) + WidthInByte), DataMask, WidthInByte);

  return RETURN_SUCCESS;
}
