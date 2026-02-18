/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef __CIX_S3_SAVE_STATE_H__
#define __CIX_S3_SAVE_STATE_H__

#define CIX_S3_SAVE_STATE_PROTOCOL_GUID \
    {0xe5faf065, 0x1f34, 0x46a4, {0x80, 0xc5, 0xff, 0x66, 0xeb, 0x2b, 0x6a, 0xa1}}

// *******************************************
// CIX Boot Script Opcode definitions
// *******************************************
#define CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE       0x00
#define CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE  0x01

///
/// S3 Boot Script Width.
///
typedef enum {
  S3BootScriptWidthUint8,        ///< 8-bit operation.
  S3BootScriptWidthUint16,       ///< 16-bit operation.
  S3BootScriptWidthUint32,       ///< 32-bit operation.
  S3BootScriptWidthUint64,       ///< 64-bit operation.
  S3BootScriptWidthFifoUint8,    ///< 8-bit FIFO operation.
  S3BootScriptWidthFifoUint16,   ///< 16-bit FIFO operation.
  S3BootScriptWidthFifoUint32,   ///< 32-bit FIFO operation.
  S3BootScriptWidthFifoUint64,   ///< 64-bit FIFO operation.
  S3BootScriptWidthFillUint8,    ///< 8-bit Fill operation.
  S3BootScriptWidthFillUint16,   ///< 16-bit Fill operation.
  S3BootScriptWidthFillUint32,   ///< 32-bit Fill operation.
  S3BootScriptWidthFillUint64,   ///< 64-bit Fill operation.
  S3BootScriptWidthMaximum
} S3_BOOT_SCRIPT_WIDTH;

typedef struct _CIX_S3_SAVE_STATE_PROTOCOL CIX_S3_SAVE_STATE_PROTOCOL;

/**
  Record operations that need to be replayed during an S3 resume.

  This function is used to store an OpCode to be replayed as part of the S3 resume boot path. It is
  assumed this protocol has platform specific mechanism to store the OpCode set and replay them
  during the S3 resume.

  @param[in]    This    A pointer to the CIX_S3_SAVE_STATE_PROTOCOL instance.
  @param[in]    OpCode  The operation code (opcode) number.
  @param[in]    ...     Argument list that is specific to each opcode. See the following subsections for the
                        definition of each opcode.

  @retval EFI_SUCCESS           The operation succeeded. A record was added into the specified
                                script table.
  @retval EFI_INVALID_PARAMETER The parameter is illegal or the given boot script is not supported.
  @retval EFI_OUT_OF_RESOURCES  There is insufficient memory to store the boot script.
**/
typedef
  EFI_STATUS
(EFIAPI *CIX_S3_SAVE_STATE_WRITE)(
  IN CONST CIX_S3_SAVE_STATE_PROTOCOL  *This,
  IN       UINT8                       OpCode,
  ...
  );

/**
  Dump S3 boot script infomation.
**/
typedef
  EFI_STATUS
(EFIAPI *CIX_S3_SAVE_STATE_DUMP)(
  VOID
  );

struct _CIX_S3_SAVE_STATE_PROTOCOL {
  CIX_S3_SAVE_STATE_WRITE    Write;
  CIX_S3_SAVE_STATE_DUMP     Dump;
};

extern EFI_GUID  gCixS3SaveStateProtocolGuid;

#endif // __CIX_S3_SAVE_STATE_H__
