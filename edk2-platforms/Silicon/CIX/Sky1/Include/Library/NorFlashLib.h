/** @file  NorFlashLib.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022-2023, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __NOR_FLASH_LIB_H__
#define __NOR_FLASH_LIB_H__

#include <Library/DebugLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Pi/PiFirmwareVolume.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include "MemoryMap.h"

#define XSPI_FLASH_DMA_MMIO_ADDR     XSPI_DMA_MMIO_BASE
#define XSPI_FLASH_DIRECT_MMIO_ADDR  XSPI_MMIO_BASE
#define XSPI_CONTROLLER_BASE         XSPI_BASE

#define GET_NOR_BLOCK_ADDRESS(BaseAddr, Lba, LbaSize)  ( BaseAddr + (UINTN)((Lba) * LbaSize) )

#define GENMASK(high, low)  (((~(0)) - ((1) << (low)) + 1) & (~(0) >> (32 - 1 - (high))))

/* Supported SPI protocols */
#define SNOR_MODE_IS_DTR      BIT24                        /* Double Transfer Rate */
#define SNOR_MODE_INST_MASK   GENMASK(23, 16)
#define SNOR_MODE_INST_SHIFT  16
#define SNOR_MODE_INST(bits)  ((((UINT32)(bits)) << SNOR_MODE_INST_SHIFT) & SNOR_MODE_INST_MASK)

#define SNOR_MODE_ADDR_MASK   GENMASK(15, 8)
#define SNOR_MODE_ADDR_SHIFT  8
#define SNOR_MODE_ADDR(bits)  ((((UINT32)(bits)) << SNOR_MODE_ADDR_SHIFT) & SNOR_MODE_ADDR_MASK)

#define SNOR_MODE_DATA_MASK   GENMASK(7, 0)
#define SNOR_MODE_DATA_SHIFT  0
#define SNOR_MODE_DATA(bits)  ((((UINT32)(bits)) << SNOR_MODE_DATA_SHIFT) & SNOR_MODE_DATA_MASK)

#define SNOR_MODE_STR(inst_bits, addr_bits, data_bits)  \
                                    (SNOR_MODE_INST(inst_bits) | SNOR_MODE_ADDR(addr_bits) | SNOR_MODE_DATA(data_bits))
#define SNOR_MODE_DTR(inst_bits, addr_bits, data_bits)  \
                                    (SNOR_MODE_IS_DTR | SNOR_MODE_STR(inst_bits, addr_bits, data_bits))

typedef enum _SPI_NOR_MODE {
  SNOR_1_1_1_MODE = SNOR_MODE_STR (1, 1, 1),
  SNOR_1_1_2_MODE = SNOR_MODE_STR (1, 1, 2),
  SNOR_1_1_4_MODE = SNOR_MODE_STR (1, 1, 4),
  SNOR_1_1_8_MODE = SNOR_MODE_STR (1, 1, 8),
  SNOR_1_2_2_MODE = SNOR_MODE_STR (1, 2, 2),
  SNOR_1_4_4_MODE = SNOR_MODE_STR (1, 4, 4),
  SNOR_1_8_8_MODE = SNOR_MODE_STR (1, 8, 8),
  SNOR_2_2_2_MODE = SNOR_MODE_STR (2, 2, 2),
  SNOR_4_4_4_MODE = SNOR_MODE_STR (4, 4, 4),
  SNOR_8_8_8_MODE = SNOR_MODE_STR (8, 8, 8),

  SNOR_1_1_1_DTR_MODE = SNOR_MODE_DTR (1, 1, 1),
  SNOR_1_2_2_DTR_MODE = SNOR_MODE_DTR (1, 2, 2),
  SNOR_1_4_4_DTR_MODE = SNOR_MODE_DTR (1, 4, 4),
  SNOR_1_8_8_DTR_MODE = SNOR_MODE_DTR (1, 8, 8),
} SPI_NOR_MODE;

#pragma pack (1)
typedef struct {
  VENDOR_DEVICE_PATH          Vendor;
  UINT8                       Index;
  EFI_DEVICE_PATH_PROTOCOL    End;
} NOR_FLASH_DEVICE_PATH;

typedef struct _XSPI_ACCESS_INFO_PROTOCOL {
  UINTN     BaseAddress;
  UINTN     DmaBaseAddress;
  UINT32    Size;
  UINT32    RemappedOffset;
} XSPI_ACCESS_INFO_PROTOCOL;
#pragma pack ()

typedef struct _NOR_FLASH_INSTANCE {
  UINT32                                 Signature;
  EFI_HANDLE                             Handle;

  // NOR Flash device
  UINTN                                  DeviceBaseAddress;
  UINTN                                  Size;
  VOID                                   *ShadowBuffer;
  EFI_BLOCK_IO_MEDIA                     Media;
  EFI_BLOCK_IO_PROTOCOL                  BlockIoProtocol;
  EFI_DISK_IO_PROTOCOL                   DiskIoProtocol;
  NOR_FLASH_DEVICE_PATH                  DevicePath;
  XSPI_ACCESS_INFO_PROTOCOL              AccessInfo;

  // NOR Flash NVRAM blocks
  UINTN                                  RegionBaseAddress;
  UINTN                                  Blocks;
  EFI_LBA                                StartLba;
  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL    FvbProtocol;
} NOR_FLASH_INSTANCE;

#define NOR_FLASH_SIGNATURE  SIGNATURE_32('n', 'o', 'r', '0')
#define INSTANCE_FROM_FVB_THIS(a)     CR(a, NOR_FLASH_INSTANCE, FvbProtocol, NOR_FLASH_SIGNATURE)
#define INSTANCE_FROM_BLKIO_THIS(a)   CR(a, NOR_FLASH_INSTANCE, BlockIoProtocol, NOR_FLASH_SIGNATURE)
#define INSTANCE_FROM_DISKIO_THIS(a)  CR(a, NOR_FLASH_INSTANCE, DiskIoProtocol, NOR_FLASH_SIGNATURE)

EFI_STATUS
NorFlashReadBlocks (
  IN NOR_FLASH_INSTANCE  *Instance,
  IN EFI_LBA             Lba,
  IN UINTN               Length,
  OUT VOID               *Buffer
  );

EFI_STATUS
NorFlashWriteBlocks (
  IN NOR_FLASH_INSTANCE  *Instance,
  IN EFI_LBA             Lba,
  IN UINTN               Length,
  IN VOID                *Buffer
  );

EFI_STATUS
NorFlashRead (
  IN NOR_FLASH_INSTANCE  *Instance,
  IN EFI_LBA             Lba,
  IN UINTN               Offset,
  IN UINTN               Length,
  OUT VOID               *Buffer
  );

EFI_STATUS
NorFlashWrite (
  IN NOR_FLASH_INSTANCE  *Instance,
  IN EFI_LBA             Lba,
  IN UINTN               Offset,
  IN UINTN               Length,
  IN UINT8               *Buffer
  );

EFI_STATUS
NorFlashErase (
  IN NOR_FLASH_INSTANCE  *Instance,
  IN EFI_LBA             Lba,
  IN UINTN               Length
  );

EFI_STATUS
NorFlashReset (
  IN NOR_FLASH_INSTANCE  *Instance
  );

EFI_STATUS
NorFlashWriteReadTransaction (
  IN SPI_NOR_MODE  Format,
  IN BOOLEAN       DualRate,
  IN UINT8         Command,
  IN UINT32        Address,
  IN UINT32        AddressSize,
  IN UINT8         *DataOut,
  IN UINT32        DataOutSize,
  IN UINT32        DummyCycles,
  OUT UINT8        *DataIn,
  IN UINT32        DataInSize
  );

EFI_STATUS
SpiNorRead (
  IN UINTN   Address,
  IN UINT32  Length,
  OUT UINT8  *Buffer
  );

EFI_STATUS
SpiNorWrite (
  IN UINTN   Address,
  IN UINT32  Length,
  IN UINT8   *Buffer
  );

EFI_STATUS
SpiNorErase (
  IN UINTN    Address,
  IN UINT32   Length,
  IN BOOLEAN  WholeChip
  );

VOID
UpdateNorFlashHostBaseAddr (
  UINTN  Addr
  );

UINTN
GetNorFlashHostBaseAddr (
  );

VOID
UpdateNorFlashSdmaBaseAddr (
  UINTN  Addr
  );

UINTN
GetNorFlashSdmaBaseAddr (
  );

#endif /* __NOR_FLASH_H__ */
