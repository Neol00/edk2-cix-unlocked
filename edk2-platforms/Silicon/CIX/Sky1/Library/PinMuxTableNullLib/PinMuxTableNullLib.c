/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PinMuxTableNullInternal.h"

PINMUX_CFG  mPinMuxCfgTableDummy[] = {
  { UNDEFINE_DOMAIN, IO_S0_S5_UNDEFINED, IO_UNDEFINE, PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }
};

PINMUX_CFG  *mPinMuxCfgTable      = mPinMuxCfgTableDummy;
UINT32      mPinMuxCfgTableLength = ARRAY_SIZE (mPinMuxCfgTableDummy);
