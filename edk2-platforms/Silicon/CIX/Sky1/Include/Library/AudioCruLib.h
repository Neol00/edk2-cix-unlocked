/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef AUDIO_CRU_LIB_H__
#define AUDIO_CRU_LIB_H__

#include <Base.h>
#include <Uefi.h>
#include "IoConfig.h"
#include "MemoryMap.h"

typedef enum _AUDIO_BLOCK {
  AUDIO_DMAC     = 0x00,
  AUDIO_I2S0     = 0x01,
  AUDIO_I2S1     = 0x02,
  AUDIO_I2S2     = 0x03,
  AUDIO_I2S3     = 0x04,
  AUDIO_I2S4     = 0x05,
  AUDIO_I2S5     = 0x06,
  AUDIO_I2S6     = 0x07,
  AUDIO_I2S7     = 0x08,
  AUDIO_I2S8     = 0x09,
  AUDIO_I2S9     = 0x0A,
  AUDIO_HDA      = 0x0B,
  AUDIO_WDT      = 0x0C,
  AUDIO_TIMER    = 0x0D,
  AUDIO_MAILBOX0 = 0x0E,
  AUDIO_MAILBOX1 = 0x0F,
  AUDIO_HIFI5    = 0x10,
  AUDIO_SRAM     = 0x11,
} AUDIO_BLOCK;

EFI_STATUS
AudioCruClkCtrl (
  IN AUDIO_BLOCK  Block,
  IN BOOLEAN      ClkEn
  );

EFI_STATUS
AudioCruRstCtrl (
  IN AUDIO_BLOCK  Block,
  IN BOOLEAN      Assert
  );

#endif // AUDIO_CRU_LIB_H__
