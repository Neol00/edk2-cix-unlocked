/*
  Internal header file for S3 Boot Script Saver state driver.

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef _INTERNAL_S3_SAVE_STATE_H_
#define _INTERNAL_S3_SAVE_STATE_H_
#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CsuPmIpcLib.h>
#include <Protocol/CixS3SaveState.h>
#include <Guid/EventGroup.h>

/**
  Adds a record into S3 boot script table.

  This function is used to store a boot script record into S3 boot
  script table. If the S3 boot script table  is nonexistent in the
  system, a new table will automatically be created and then the script record
  will be added into the new table. This function is responsible for allocating
  necessary memory for the script.

  This function has a variable parameter list. The exact parameter list depends on
  the OpCode that is passed into the function. If an unsupported OpCode or illegal
  parameter list is passed in, this function returns EFI_INVALID_PARAMETER.
  If there are not enough resources available for storing more scripts, this function returns
  EFI_OUT_OF_RESOURCES.

  @param  This                  A pointer to the EFI_S3_SAVE_STATE_PROTOCOL instance.
  @param  OpCode                The operation code (opcode) number.
  @param  ...                   Argument list that is specific to each opcode.

  @retval EFI_SUCCESS           The operation succeeded. A record was added into the
                                specified script table.
  @retval EFI_INVALID_PARAMETER The parameter is illegal or the given boot script is not supported.
                                If the opcode is unknow or not supported because of the PCD
                                Feature Flags.
  @retval EFI_OUT_OF_RESOURCES  There is insufficient memory to store the boot script.

**/
EFI_STATUS
EFIAPI
BootScriptWrite (
  IN CONST CIX_S3_SAVE_STATE_PROTOCOL  *This,
  IN       UINT8                       OpCode,
  ...
  );

/**
  Dump S3 boot script infomation.
**/
EFI_STATUS
EFIAPI
BootScriptDump (
  VOID
  );

#endif //_INTERNAL_S3_SAVE_STATE_H_
