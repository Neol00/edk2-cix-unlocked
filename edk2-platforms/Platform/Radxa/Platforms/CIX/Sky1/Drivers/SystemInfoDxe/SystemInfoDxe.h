/** @file
*
*  Copyright 2024 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
*  Copyright 2023 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.
*  Copyright (c) 2017, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef __SYSTEM_INFO_DXE_H__
#define __SYSTEM_INFO_DXE_H__

#include <Guid/HiiPlatformSetupFormset.h>

#include <Protocol/FwVersionProtocol.h>
#include <Protocol/EcPlatformProtocol.h>
#include <Protocol/SocInfoProtocol.h>
#include <Library/PrintLib.h>

#include "SystemInfoHii.h"

typedef union {
  struct {
    UINT16    PcbSku   : 4;   // PCB SKU
    UINT16    MemType  : 4;   // Memory Type
    UINT16    BoardRev : 2;   // Board Revision
    UINT16    Reserved : 6;   // Reserved
  } Bits;         ///< Bitfield definition of the register
  UINT16    Data; ///< The entire 16-bit value
} BOARD_ID_INFO;

#endif
