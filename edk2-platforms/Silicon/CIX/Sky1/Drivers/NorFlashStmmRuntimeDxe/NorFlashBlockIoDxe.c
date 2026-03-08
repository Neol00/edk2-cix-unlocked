/** @file  NorFlashBlockIoDxe.c

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "NorFlashBlockIoDxe.h"

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.Reset
//
EFI_STATUS
EFIAPI
NorFlashBlockIoReset (
  IN EFI_BLOCK_IO_PROTOCOL  *This,
  IN BOOLEAN                ExtendedVerification
  )
{
  NOR_FLASH_INSTANCE  *Instance;

  Instance = INSTANCE_FROM_BLKIO_THIS (This);

  DEBUG ((DEBUG_BLKIO, "%a: media ID 0x%x\n", __FUNCTION__, This->Media->MediaId));

  return NorFlashReset (Instance);
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.ReadBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoReadBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize,
  OUT VOID                   *Buffer
  )
{
  EFI_STATUS          Status;
  NOR_FLASH_INSTANCE  *Instance;
  EFI_BLOCK_IO_MEDIA  *Media;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = INSTANCE_FROM_BLKIO_THIS (This);
  Media    = This->Media;

  if (Media == NULL) {
    Status = EFI_INVALID_PARAMETER;
  } else if (Media->MediaPresent == FALSE) {
    Status = EFI_NO_MEDIA;
  } else if (Media->MediaId != MediaId) {
    Status = EFI_MEDIA_CHANGED;
  } else if ((Media->IoAlign > 2) && (((UINTN)Buffer & (Media->IoAlign - 1)) != 0)) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = NorFlashReadBlocks (Instance, Lba, BufferSize, Buffer);
    DEBUG ((
      DEBUG_INFO,
      "%a: media ID 0x%x, LBA %ld, size 0x%x, buffer @ 0x%08x, status %r\n",
      __FUNCTION__,
      MediaId,
      Lba,
      BufferSize,
      Buffer,
      Status
      ));
  }

  return Status;
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.WriteBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoWriteBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize,
  IN  VOID                   *Buffer
  )
{
  EFI_STATUS          Status;
  NOR_FLASH_INSTANCE  *Instance;

  Instance = INSTANCE_FROM_BLKIO_THIS (This);

  if (This->Media->MediaPresent == FALSE) {
    Status = EFI_NO_MEDIA;
  } else if ( This->Media->MediaId != MediaId ) {
    Status = EFI_MEDIA_CHANGED;
  } else if ( This->Media->ReadOnly ) {
    Status = EFI_WRITE_PROTECTED;
  } else {
    Status = NorFlashWriteBlocks (Instance, Lba, BufferSize, Buffer);
    DEBUG ((
      DEBUG_INFO,
      "%a: media ID 0x%x, LBA %ld, size 0x%x, buffer @ 0x%08x, status %r\n",
      __FUNCTION__,
      MediaId,
      Lba,
      BufferSize,
      Buffer,
      Status
      ));
  }

  return Status;
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.FlushBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  )
{
  // No Flush required for the NOR Flash driver
  // because cache operations are not permitted.

  DEBUG ((DEBUG_INFO, "%a: Function NOT IMPLEMENTED\n", __FUNCTION__));

  // Nothing to do so just return without error
  return EFI_SUCCESS;
}
