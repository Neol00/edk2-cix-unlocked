/** @file
  PCH Detection for the HDMI I2C Debug Port

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef HDMI_DEBUG_PCH_DETECTION_LIB_H_
#define HDMI_DEBUG_PCH_DETECTION_LIB_H_

#include <Uefi/UefiBaseType.h>

typedef enum {
  PchTypeUnknown = 0,
  PchTypeSptLp,
  PchTypeSptH,
  PchTypeKbpH,
  PchTypeCnlLp,
  PchTypeCnlH,
  PchTypeMax
} PCH_TYPE;

/**
  Returns the type of PCH on the system

  @retval   The PCH type.
**/
PCH_TYPE
GetPchTypeInternal (
  VOID
  );

#endif
