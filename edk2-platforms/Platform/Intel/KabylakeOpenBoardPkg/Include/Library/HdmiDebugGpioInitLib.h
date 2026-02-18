/** @file
  GPIO initialization for the HDMI I2C Debug Port

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef HDMI_DEBUG_GPIO_INIT_LIB_H_
#define HDMI_DEBUG_GPIO_INIT_LIB_H_

#include <Uefi.h>

/**
  Configures GPIOs to enable usage of the HDMI DDC I2C Bus

  @retval EFI_SUCCESS        The function completed successfully
  @retval EFI_UNSUPPORTED    The platform is using a PCH that is not supported yet.

**/
EFI_STATUS
HdmiDebugGpioInit (
  VOID
  );

#endif
