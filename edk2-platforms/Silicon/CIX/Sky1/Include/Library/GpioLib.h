/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef GPIO_LIB_H__
#define GPIO_LIB_H__

#include <Base.h>
#include <Uefi.h>
#include "IoConfig.h"
#include "MemoryMap.h"

#pragma pack(1)

typedef struct _GPIO_CFG {
  UINT32                     Num;
  IO_DIRECTION_SEL           Direction;
  IO_INOUT_VALUE_SEL         OutputVal;
  IO_INTERRUPT_ENABLE_SEL    IntrEn;
  IO_INTERRUPT_TYPE_SEL      IntrType;
} GPIO_CFG;

#pragma pack()

EFI_STATUS
EFIAPI
GpioInit (
  IN GPIO_CFG  *InitTable,
  IN UINT32    InitTableLength
  );

EFI_STATUS
EFIAPI
GpioConfig (
  IN UINT32                   Num,
  IN IO_DIRECTION_SEL         Direction,
  IN IO_INOUT_VALUE_SEL       OutputVal,
  IN IO_INTERRUPT_ENABLE_SEL  IntrEn,
  IN IO_INTERRUPT_TYPE_SEL    IntrType
  );

EFI_STATUS
EFIAPI
GpioSet (
  IN UINT32              Num,
  IN IO_INOUT_VALUE_SEL  Output
  );

EFI_STATUS
EFIAPI
GpioGet (
  IN UINT32               Num,
  OUT IO_INOUT_VALUE_SEL  *Input
  );

EFI_STATUS
EFIAPI
GpioGetIntStatus (
  IN     UINT32  Bank,
  IN OUT UINT32  *IntStatus
  );

EFI_STATUS
EFIAPI
GpioGetIntMask (
  IN     UINT32  Bank,
  IN OUT UINT32  *IntMask
  );

EFI_STATUS
EFIAPI
GpioGetIntStatusByPin (
  IN     UINT32   Num,
  IN OUT BOOLEAN  *Triggered
  );

EFI_STATUS
EFIAPI
GpioGetIntMaskByPin (
  IN     UINT32   Num,
  IN OUT BOOLEAN  *Masked
  );

EFI_STATUS
EFIAPI
GpioGetConfig (
  IN  UINT32                   Num,
  OUT IO_DIRECTION_SEL         *Direction,
  OUT IO_INOUT_VALUE_SEL       *OutputVal,
  OUT IO_INTERRUPT_ENABLE_SEL  *IntrEn,
  OUT IO_INTERRUPT_TYPE_SEL    *IntrType
  );

#endif // GPIO_LIB_H__
