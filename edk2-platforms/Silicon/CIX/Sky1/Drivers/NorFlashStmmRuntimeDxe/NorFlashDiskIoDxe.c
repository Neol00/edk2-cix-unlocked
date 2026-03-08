/** @file  NorFlashBlockIoDxe.c

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "NorFlashDiskIoDxe.h"

/*
  Although DiskIoDxe will automatically install the DiskIO protocol whenever
  we install the BlockIO protocol, its implementation is sub-optimal as it reads
  and writes entire blocks using the BlockIO protocol. In fact we can access
  NOR flash with a finer granularity than that, so we can improve performance
  by directly producing the DiskIO protocol.
*/

/**
  Read BufferSize bytes from Offset into Buffer.

  @param  This                  Protocol instance pointer.
  @param  MediaId               Id of the media, changes every time the media is replaced.
  @param  Offset                The starting byte offset to read from
  @param  BufferSize            Size of Buffer
  @param  Buffer                Buffer containing read data

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not match the current device.
  @retval EFI_INVALID_PARAMETER The read request contains device addresses that are not
                                valid for the device.

**/
EFI_STATUS
EFIAPI
NorFlashDiskIoReadDisk (
  IN EFI_DISK_IO_PROTOCOL  *This,
  IN UINT32                MediaId,
  IN UINT64                DiskOffset,
  IN UINTN                 BufferSize,
  OUT VOID                 *Buffer
  )
{
  EFI_STATUS          Status;
  NOR_FLASH_INSTANCE  *Instance;
  UINT32              BlockSize;
  UINT32              BlockOffset;
  EFI_LBA             Lba;

  Instance = INSTANCE_FROM_DISKIO_THIS (This);

  if (MediaId != Instance->Media.MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  BlockSize = Instance->Media.BlockSize;
  Lba       = (EFI_LBA)DivU64x32Remainder (DiskOffset, BlockSize, &BlockOffset);

  Status = NorFlashRead (Instance, Lba, BlockOffset, BufferSize, Buffer);

  return Status;
}

/**
  Writes a specified number of bytes to a device.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    ID of the medium to be written.
  @param  Offset     The starting byte offset on the logical block I/O device to write.
  @param  BufferSize The size in bytes of Buffer. The number of bytes to write to the device.
  @param  Buffer     A pointer to the buffer containing the data to be written.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not match the current device.
  @retval EFI_INVALID_PARAMETER The write request contains device addresses that are not
                                 valid for the device.

**/
EFI_STATUS
EFIAPI
NorFlashDiskIoWriteDisk (
  IN EFI_DISK_IO_PROTOCOL  *This,
  IN UINT32                MediaId,
  IN UINT64                DiskOffset,
  IN UINTN                 BufferSize,
  IN VOID                  *Buffer
  )
{
  EFI_STATUS          Status;
  NOR_FLASH_INSTANCE  *Instance;
  UINT32              BlockSize;
  UINT32              BlockOffset;
  EFI_LBA             Lba;

  Instance = INSTANCE_FROM_DISKIO_THIS (This);

  if (MediaId != Instance->Media.MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  BlockSize = Instance->Media.BlockSize;
  Lba       = (EFI_LBA)DivU64x32Remainder (DiskOffset, BlockSize, &BlockOffset);

  Status = NorFlashWrite (Instance, Lba, BlockOffset, BufferSize, Buffer);

  return Status;
}
