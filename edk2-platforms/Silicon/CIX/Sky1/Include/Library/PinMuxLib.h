/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef PIN_MUX_LIB_H__
#define PIN_MUX_LIB_H__

#include <Base.h>
#include "IoConfig.h"
#include "MemoryMap.h"

#pragma pack(1)

typedef struct _PINMUX_PAD_CFG {
  UINT32    DrvStr0    : 1;
  UINT32    DrvStr1    : 1;
  UINT32    DrvStr2    : 1;
  UINT32    DrvStr3    : 1;
  UINT32    SchTrigEn  : 1;
  UINT32    PullDownEn : 1;
  UINT32    PullUpEn   : 1;
  UINT32    FuncSel    : 2;
  UINT32    Reserved   : 23;
} PINMUX_PAD_CFG;

typedef struct _PINMUX_CFG {
  IO_DOMAIN           Domain;
  UINT32              Offset;
  UINT32              FuncSel;
  IO_PULL_UP_SEL      PullUp;
  IO_PULL_DOWN_SEL    PullDown;
  IO_SCH_TRIG_SEL     SchTrig;
  UINT8               DrvStren;
} PINMUX_CFG;

#pragma pack()

VOID
EFIAPI
PinMuxInit (
  IN PINMUX_CFG  *InitTable,
  IN UINT32      InitTableLength
  );

VOID
EFIAPI
PinMuxConfig (
  IN IO_DOMAIN         Domain,
  IN UINT32            Offset,
  IN UINT32            FuncSel,
  IN IO_PULL_UP_SEL    PullUp,
  IN IO_PULL_DOWN_SEL  PullDown,
  IN IO_SCH_TRIG_SEL   SchTrig,
  IN UINT8             DrvStren
  );

#endif // PIN_MUX_LIB_H__
