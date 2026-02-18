/** @file
  Serial I/O Port library implementation for the HDMI I2C Debug Port
  Null implementation of Task Priority Level functions.
  This implementation is used by SEC, PEI, & SMM

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>

/**
  For boot phases that utilize task priority levels (TPLs), this function raises
  the TPL to the appriopriate level needed to execute I/O to the I2C Debug Port
**/
VOID
RaiseTplForI2cDebugPortAccess (
  VOID
  )
{
  return;
}

/**
  For boot phases that utilize task priority levels (TPLs), this function
  restores the TPL to the previous level after I/O to the I2C Debug Port is
  complete
**/
VOID
RestoreTplAfterI2cDebugPortAccess (
  VOID
  )
{
  return;
}
