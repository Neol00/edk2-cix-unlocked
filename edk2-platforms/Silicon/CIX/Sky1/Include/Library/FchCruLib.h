/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef FCH_CRU_LIB_H__
#define FCH_CRU_LIB_H__

#include <Base.h>
#include <Uefi.h>
#include "IoConfig.h"
#include "MemoryMap.h"

typedef enum _FCH_BLOCK {
  FCH_I2C0  = 0x00,
  FCH_I2C1  = 0x01,
  FCH_I2C2  = 0x02,
  FCH_I2C3  = 0x03,
  FCH_I2C4  = 0x04,
  FCH_I2C5  = 0x05,
  FCH_I2C6  = 0x06,
  FCH_I2C7  = 0x07,
  FCH_SPI0  = 0x08,
  FCH_SPI1  = 0x09,
  FCH_UART0 = 0x0A,
  FCH_UART1 = 0x0B,
  FCH_UART2 = 0x0C,
  FCH_UART3 = 0x0D,
  FCH_I3C0  = 0x0E,
  FCH_I3C1  = 0x0F,
  FCH_TIMER = 0x10,
  FCH_GPIO  = 0x11,
  FCH_IO_S0 = 0x12,
  FCH_XSPI  = 0x13,
  FCH_DMA   = 0x14,
} FCH_BLOCK;

EFI_STATUS
FchCruClkCtrl (
  IN FCH_BLOCK  Block,
  IN BOOLEAN    ClkEn
  );

EFI_STATUS
FchCruRstCtrl (
  IN FCH_BLOCK  Block,
  IN BOOLEAN    Assert
  );

#endif // FCH_CRU_LIB_H__
