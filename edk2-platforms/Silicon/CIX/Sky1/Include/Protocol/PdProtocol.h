/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PD_PROTOCOL_H_
#define _PD_PROTOCOL_H_

#include <Uefi.h>
#include <Library/PdLib.h>

typedef struct _CIX_FD_PROTOCOL CIX_FD_PROTOCOL;

typedef
  UINT8
(EFIAPI *GET_PD_DEVICE_COUNT)(
  VOID
  );

typedef
  EFI_STATUS
(EFIAPI *SET_PHY_READY_FOR_ALERT_IRQ)(
  IN UINT8 PdDevIndex
  );

typedef
  EFI_STATUS
(EFIAPI *GET_PORT_STATE)(
  IN      UINT8             PdDevIndex,
  IN OUT  TYPEC_PORT_STATE  *PortState
  );

struct _CIX_FD_PROTOCOL {
  GET_PD_DEVICE_COUNT            GetPdDeviceCount;
  SET_PHY_READY_FOR_ALERT_IRQ    SetPhyReadyForAlertIRQ;
  GET_PORT_STATE                 GetPortState;
};

#endif
