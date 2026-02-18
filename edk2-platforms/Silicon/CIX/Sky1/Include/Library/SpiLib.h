/** @file  SpiLib.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2023, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SPI_LIB_H__
#define __SPI_LIB_H__

#define SPI_CPHA    0x01        /* clock phase */
#define SPI_CPOL    0x02        /* clock polarity */
#define SPI_MODE_0  (0|0)
#define SPI_MODE_1  (0|SPI_CPHA)
#define SPI_MODE_2  (SPI_CPOL|0)
#define SPI_MODE_3  (SPI_CPOL|SPI_CPHA)

#define SPI_SS0   BIT0          /* Slave Select 0 */
#define SPI_SS1   BIT1          /* Slave Select 1 */
#define SPI_NOSS  0xF           /* No Slave select */

#define SPI_CTRL_NUM  2

typedef struct _SPI_DEVICE {
  VOID       *HostBase;
  UINTN      ClkRate;
  UINT32     TxBytes;
  UINT32     RxBytes;
  VOID       *TxBuf;
  VOID       *RxBuf;
  BOOLEAN    IsDecodedCs;
  UINT8      ChipSelect;
  UINT32     Mode;
} SPI_DEVICE;

UINT32
GetMmioBaseAddress (
  IN UINT8  Bus
  );

EFI_STATUS
EFIAPI
SpiTransfer (
  IN SPI_DEVICE  *Spi
  );

EFI_STATUS
EFIAPI
SpiInit (
  IN SPI_DEVICE  *Spi
  );

EFI_STATUS
SpiClkConfig (
  IN UINT8  Bus
  );

#endif /* __SPI_LIB_H__ */
