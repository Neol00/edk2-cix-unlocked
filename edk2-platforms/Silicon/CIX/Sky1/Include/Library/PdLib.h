/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef _PD_LIB_H_
#define _PD_LIB_H_

#include <Base.h>
#include <Protocol/TypecPhy.h>

typedef struct {
  TYPEC_ORIENTATION    Orientation;
  TYPEC_MODE           Mode;
} TYPEC_PORT_STATE;

BOOLEAN
IsPdDeviceValid (
  UINT8  PdDevIdx
  );

EFI_STATUS
EFIAPI
PdGetPortState (
  IN     UINT8             PdDevIdx,
  IN OUT TYPEC_PORT_STATE  *PortState
  );

EFI_STATUS
EFIAPI
PdEnterAlertInterrupt (
  IN  UINT8  PdDevIdx
  );

EFI_STATUS
EFIAPI
PdExitAlertInterrupt (
  IN  UINT8  PdDevIdx
  );

#endif
