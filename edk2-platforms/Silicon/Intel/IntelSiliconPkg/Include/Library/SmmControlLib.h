/** @file
  This is to publish the SMM Control Ppi instance.

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SMM_CONTROL_LIB_H_
#define _SMM_CONTROL_LIB_H_

/**
  This function is to install an SMM Control PPI
  - <b>Introduction</b> \n
    An API to install an instance of EFI_PEI_MM_CONTROL_PPI. This PPI provides a standard
    way for other modules to trigger software SMIs.

    @retval EFI_SUCCESS           - Ppi successfully started and installed.
    @retval EFI_NOT_FOUND         - Ppi can't be found.
    @retval EFI_OUT_OF_RESOURCES  - Ppi does not have enough resources to initialize the driver.
**/
EFI_STATUS
EFIAPI
PeiInstallSmmControlPpi (
  VOID
  );
#endif
