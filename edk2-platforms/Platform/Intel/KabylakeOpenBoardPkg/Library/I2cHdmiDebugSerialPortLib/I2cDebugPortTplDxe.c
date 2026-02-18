/** @file
  Serial I/O Port library implementation for the HDMI I2C Debug Port
  DXE Library implementation

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

STATIC EFI_TPL  mPreviousTpl  = 0;

/**
  For boot phases that utilize task priority levels (TPLs), this function raises
  the TPL to the appriopriate level needed to execute I/O to the I2C Debug Port
**/
VOID
RaiseTplForI2cDebugPortAccess (
  VOID
  )
{
  if (EfiGetCurrentTpl () < TPL_NOTIFY) {
    mPreviousTpl = gBS->RaiseTPL (TPL_NOTIFY);
  }
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
  if (mPreviousTpl > 0) {
    gBS->RestoreTPL (mPreviousTpl);
    mPreviousTpl = 0;
  }
}
