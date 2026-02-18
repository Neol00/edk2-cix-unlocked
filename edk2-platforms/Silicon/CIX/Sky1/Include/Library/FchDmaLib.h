/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef FCH_DMA_LIB_H__
#define FCH_DMA_LIB_H__

#include <Base.h>
#include <Uefi.h>
#include "IoConfig.h"
#include "MemoryMap.h"

typedef enum _FCH_DMA_CHANNEL {
  FCH_DMA_CHANNEL0 = 0x00,
  FCH_DMA_CHANNEL1 = 0x01,
  FCH_DMA_CHANNEL2 = 0x02,
  FCH_DMA_CHANNEL3 = 0x03,
  FCH_DMA_CHANNEL4 = 0x04,
  FCH_DMA_CHANNEL5 = 0x05,
  FCH_DMA_CHANNEL6 = 0x06,
  FCH_DMA_CHANNEL7 = 0x07
} FCH_DMA_CHANNEL;

typedef enum _FCH_DMA_TRG_SIG {
  FCH_DMA_TRG_UART0_TX = 0x00,
  FCH_DMA_TRG_UART0_RX = 0x01,
  FCH_DMA_TRG_UART1_TX = 0x02,
  FCH_DMA_TRG_UART1_RX = 0x03,
  FCH_DMA_TRG_UART2_TX = 0x04,
  FCH_DMA_TRG_UART2_RX = 0x05,
  FCH_DMA_TRG_UART3_TX = 0x06,
  FCH_DMA_TRG_UART3_RX = 0x07,
  FCH_DMA_TRG_STM      = 0x08
} FCH_DMA_TRG_SIG;

VOID
DmaClkRstCfg (
  VOID
  );

VOID
StartDmaSwSrcTrigIn (
  IN FCH_DMA_CHANNEL  Channel,
  IN UINTN            SrcAddr,
  IN UINTN            DesAddr,
  IN UINT32           DataBlockSize
  );

VOID
StartDmaHwDesTrigIn (
  IN FCH_DMA_CHANNEL  Channel,
  IN FCH_DMA_TRG_SIG  Trigger,
  IN UINTN            SrcAddr,
  IN UINTN            DesAddr,
  IN UINT32           DataBlockSize
  );

VOID
DisableDmaSrcTrigIn (
  IN FCH_DMA_CHANNEL  Channel
  );

VOID
DisableDmaDesTrigIn (
  IN FCH_DMA_CHANNEL  Channel
  );

EFI_STATUS
DmaChPollComplete (
  IN FCH_DMA_CHANNEL  Channel,
  IN UINT32           CompleteSize
  );

#endif // FCH_DMA_LIB_H__
