/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/EcLib.h>

extern I2C_HOST_DESCRIPTOR  *mHost;

RETURN_STATUS
EFIAPI
EcLibConstructor (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  mHost          = AllocateZeroPool (sizeof (I2C_HOST_DESCRIPTOR));
  mHost->Bus     = FixedPcdGet8 (PcdEcI2cBus);
  mHost->MemBase = I2cGetMemBase (mHost->Bus);
  mHost->BusClk  = I2cGetBusClk (mHost->Bus);

  if (PcdGetBool (PcdEcI2cHostInitialized) == FALSE) {
    I2cEnvInit (mHost->Bus);
    Status = I2cInitialize (mHost);
    if (Status == EFI_SUCCESS) {
      PcdSetBoolS (PcdEcI2cHostInitialized, TRUE);
    }
  }

  return Status;
}
