/** @file
  Kaby Lake board SEC initialization.

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/BoardInitLib.h>
#include <Library/PcdLib.h>
#include <Library/SiliconInitLib.h>
#include <Library/HdmiDebugGpioInitLib.h>

EFI_STATUS
EFIAPI
BoardAfterTempRamInit (
  VOID
  )
{
  ///
  /// Do Early PCH init
  ///
  EarlySiliconInit ();

  ///
  /// Initialize HDMI DDC GPIOs if HDMI I2C Debug Port is Enabled
  ///
  if (PcdGetBool (PcdI2cHdmiDebugPortEnable) ||
      PcdGetBool (PcdI2cHdmiDebugPortSerialTerminalEnable)) {
    HdmiDebugGpioInit ();
  }

  return EFI_SUCCESS;
}
