/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2023-2024, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SPI_H__
#define __SPI_H__

#include <Library/SpiLib.h>

#define SPI_HOST_SIGNATURE  SIGNATURE_32 ('C', 'S', 'P', 'I')
#define SPI_HOST_FROM_SPI_HOST_PROTOCOL(a)  CR (a, SPI_HOST, HostProtocol, SPI_HOST_SIGNATURE)

typedef struct _SPI_HOST_PROTOCOL SPI_HOST_PROTOCOL;

typedef enum {
  SPI_MODE0 = SPI_MODE_0, // CPOL = 0 & CPHA = 0
  SPI_MODE1 = SPI_MODE_1, // CPOL = 0 & CPHA = 1
  SPI_MODE2 = SPI_MODE_2, // CPOL = 1 & CPHA = 0
  SPI_MODE3 = SPI_MODE_3  // CPOL = 1 & CPHA = 1
} SPI_MODE;

typedef enum {
  CHIP_0_SELECTED  = SPI_SS0,  // Slave Select 0
  CHIP_1_SELECTED  = SPI_SS1,  // Slave Select 1
  NO_CHIP_SELECTED = SPI_NOSS  // No Slave select
} CS_MODE;

#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH          Vendor;
  UINT64                      MmioBase;
  UINT8                       Bus;
  EFI_DEVICE_PATH_PROTOCOL    End;
} SPI_DEVICE_PATH;
#pragma pack()

typedef
EFI_STATUS
(EFIAPI *SPI_CHIP_SELECT)(
  IN SPI_HOST_PROTOCOL  *This,
  IN SPI_DEVICE         *Device
  );

typedef
EFI_STATUS
(EFIAPI *SPI_CHIP_UNSELECT)(
  IN SPI_HOST_PROTOCOL  *This,
  IN SPI_DEVICE         *Device
  );

typedef
EFI_STATUS
(EFIAPI *SPI_TRANSFER)(
  IN SPI_HOST_PROTOCOL  *This,
  IN SPI_DEVICE         *Device
  );

typedef
SPI_DEVICE *
(EFIAPI *SPI_SETUP_DEVICE)(
  IN SPI_HOST_PROTOCOL  *This,
  IN CS_MODE            ChipSelect,
  IN SPI_MODE           Mode,
  IN UINTN              Clock
  );

typedef
EFI_STATUS
(EFIAPI *SPI_FREE_DEVICE)(
  IN SPI_HOST_PROTOCOL  *This,
  IN SPI_DEVICE         *Device
  );

struct _SPI_HOST_PROTOCOL {
  SPI_CHIP_SELECT     ChipSelect;
  SPI_CHIP_UNSELECT   ChipUnselect;
  SPI_TRANSFER        Transfer;
  SPI_SETUP_DEVICE    Setup;
  SPI_FREE_DEVICE     Free;
};

#endif // __SPI_H__
