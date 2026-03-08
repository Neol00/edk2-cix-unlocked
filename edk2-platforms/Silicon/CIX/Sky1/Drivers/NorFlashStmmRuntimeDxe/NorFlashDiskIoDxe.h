/** @file  NorFlashBlockIoDxe.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __NOR_FLASH_DISK_IO_DXE_H__
#define __NOR_FLASH_DISK_IO_DXE_H__

#include <Library/BaseLib.h>
#include "NorFlashLib.h"

//
// DiskIO Protocol function EFI_DISK_IO_PROTOCOL.ReadDisk
//
EFI_STATUS
EFIAPI
NorFlashDiskIoReadDisk (
  IN EFI_DISK_IO_PROTOCOL  *This,
  IN UINT32                MediaId,
  IN UINT64                Offset,
  IN UINTN                 BufferSize,
  OUT VOID                 *Buffer
  );

//
// DiskIO Protocol function EFI_DISK_IO_PROTOCOL.WriteDisk
//
EFI_STATUS
EFIAPI
NorFlashDiskIoWriteDisk (
  IN EFI_DISK_IO_PROTOCOL  *This,
  IN UINT32                MediaId,
  IN UINT64                Offset,
  IN UINTN                 BufferSize,
  IN VOID                  *Buffer
  );

#endif /* __NOR_FLASH_DISK_IO_DXE_H__ */
