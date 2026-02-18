/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PD_PLATFORM_DXE_H__
#define __PD_PLATFORM_DXE_H__

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PdLib.h>
#include <Protocol/TypecPhy.h>
#include <Protocol/PdProtocol.h>
#include <Library/TimerLib.h>
#include <Library/GpioLib.h>
#include <Library/CixPostCodeLib.h>

typedef struct {
  BOOLEAN             Enabled;
  UINT8               PhyControlIndex;
  UINT32              AlertPin;
  BOOLEAN             TypecPhyReadyForIRQ;
  TYPEC_PORT_STATE    TypecPortState;
} PD_DEV_INFO;

typedef struct {
  UINT8                 PhyControlIndex;
  TYPEC_PHY_PROTOCOL    *TypecPhyProtocol;
  TYPEC_PORT_STATE      TypecPortState;
} TYPEC_PHY_INFO;

typedef struct {
  UINT32              AlertPin;
  UINT8               ConnectCnt;
  UINT8*              ConnectIdxs;
} ALERT_INTR_INFO;

#endif
