/** @file
  Pch Early update.

  @copyright
  Copyright 2019 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021 - 2022, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PeiBoardInit.h"

#include <Library/UbaPchEarlyUpdateLib.h>

#include <PchAccess.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <Ppi/DynamicSiLibraryPpi.h>


EFI_STATUS
TypeAowandaPchLanConfig (
  IN SYSTEM_CONFIGURATION         *SystemConfig
)
{
  return EFI_SUCCESS;
}

EFI_STATUS
TypeAowandaOemInitLateHook (
  IN SYSTEM_CONFIGURATION         *SystemConfig
)
{
  return EFI_SUCCESS;
}


PLATFORM_PCH_EARLY_UPDATE_TABLE  TypeAowandaPchEarlyUpdateTable =
{
  PLATFORM_PCH_EARLY_UPDATE_SIGNATURE,
  PLATFORM_PCH_EARLY_UPDATE_VERSION,
  TypeAowandaPchLanConfig,
  TypeAowandaOemInitLateHook
};


/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
EFIAPI
TypeAowandaPchEarlyUpdate(
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
  )
{
  EFI_STATUS                            Status;

  Status = PeiServicesLocatePpi (
             &gUbaConfigDatabasePpiGuid,
             0,
             NULL,
             (VOID **) &UbaConfigPpi
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                               UbaConfigPpi,
                               &gPlatformPchEarlyConfigDataGuid,
                               &TypeAowandaPchEarlyUpdateTable,
                               sizeof(TypeAowandaPchEarlyUpdateTable)
                               );

  return Status;
}
