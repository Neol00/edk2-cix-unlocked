/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "GpioTableNullInternal.h"

GPIO_CFG  mGpioCfgTableDummy[] = {
  {
    0, DIRECTION_DEFAULT, INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT
  }
};

GPIO_CFG  *mGpioCfgTable      = mGpioCfgTableDummy;
UINT32    mGpioCfgTableLength = ARRAY_SIZE (mGpioCfgTableDummy);
