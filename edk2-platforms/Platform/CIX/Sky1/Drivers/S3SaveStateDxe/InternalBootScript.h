/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef __INTERNAL_BOOT_SCRIPT__
#define __INTERNAL_BOOT_SCRIPT__

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/CixS3SaveState.h>

#include "BootScriptInternalFormat.h"

#define S3_BOOT_SCRIPT_DYNAMIC_ALLOCATE_PAGE_NUMBER  2

//
// The S3 boot script data.
//
typedef struct {
  UINT8     *TableBase;
  UINT32    TableLength;            // Record the actual memory length
  UINT16    TableMemoryPageNumber;  // Record the page number Allocated for the table
  UINT32    ScriptNumber;           // Record the script number in the table
} S3_BOOT_SCRIPT_TABLE_DATA;

extern S3_BOOT_SCRIPT_TABLE_DATA  *mS3BootScriptTablePtr;

/**
  Adds a record for a memory write operation into boot script table.

  @param[in] Width     The width of the I/O operations.
  @param[in] Address   The base address of the memory operations
  @param[in] Count     The number of memory operations to perform.
  @param[in] Buffer    The source buffer from which to write the data.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveMemWrite (
  IN  S3_BOOT_SCRIPT_WIDTH  Width,
  IN  UINT64                Address,
  IN  UINT8                 Count,
  IN  VOID                  *Buffer
  );

/**
  Adds a record for a memory modify operation into boot script table.

  @param[in] Width      The width of the I/O operations.
  @param[in] Address    The base address of the memory operations. Address needs
                        alignment, if required
  @param[in] Data       A pointer to the data to be OR-ed.
  @param[in] DataMask   A pointer to the data mask to be AND-ed with the data
                        read from the register.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveMemReadWrite (
  IN  S3_BOOT_SCRIPT_WIDTH  Width,
  IN  UINT64                Address,
  IN  VOID                  *Data,
  IN  VOID                  *DataMask
  );

#endif //__INTERNAL_BOOT_SCRIPT__
