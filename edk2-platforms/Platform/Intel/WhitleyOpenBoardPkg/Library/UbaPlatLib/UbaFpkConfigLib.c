/** @file
  UBA FPK configuration library

  @copyright
  Copyright 2016 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/UbaCfgDb.h>
#include <Library/UbaFpkConfigLib.h>

/**
  Retrieves FPK config struct from UBA database

  @retval EFI_SUCCESS           Config struct is retrieved.
  @retval EFI_NOT_FOUND         UBA protocol, platform or data not found.
  @retval EFI_INVALID_PARAMETER If PlatformFpkConfigStruct is NULL.
**/
EFI_STATUS
FpkConfigGetConfigStruct (
  OUT PLATFORM_FPK_CONFIG_STRUCT *PlatformFpkConfigStruct
  )
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength = sizeof (*PlatformFpkConfigStruct);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformFpkConfigDataGuid,
                                    PlatformFpkConfigStruct,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (PlatformFpkConfigStruct->Signature == PLATFORM_FPK_CONFIG_SIGNATURE);
  ASSERT (PlatformFpkConfigStruct->Version   == PLATFORM_FPK_CONFIG_VERSION);

  return EFI_SUCCESS;
}
