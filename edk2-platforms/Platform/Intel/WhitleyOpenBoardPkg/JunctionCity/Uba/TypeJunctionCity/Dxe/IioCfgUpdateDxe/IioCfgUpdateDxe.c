/** @file
  IIO Config Update.

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "IioCfgUpdateDxe.h"

EFI_STATUS
UpdateJunctionCityIioConfig (
  IN  IIO_GLOBALS             *IioGlobalData
  )
{
  return EFI_SUCCESS;
}

PLATFORM_IIO_CONFIG_UPDATE_TABLE  TypeJunctionCityIioConfigTable =
{
  PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE,
  PLATFORM_IIO_CONFIG_UPDATE_VERSION,

  IioBifurcationTable,
  sizeof(IioBifurcationTable),
  UpdateJunctionCityIioConfig,
  IioSlotTable,
  sizeof(IioSlotTable)

};

/**
  The Driver Entry Point.

  The function is the driver Entry point.

  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES      Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
IioCfgUpdateEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                               Status;
  UBA_CONFIG_DATABASE_PROTOCOL             *UbaConfigProtocol = NULL;

  DEBUG((DEBUG_INFO, "UBA:IioCfgUpdate-TypeJunctionCity\n"));
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformIioConfigDataDxeGuid,
                                     &TypeJunctionCityIioConfigTable,
                                     sizeof(TypeJunctionCityIioConfigTable)
                                     );

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformIioConfigDataDxeGuid_1,
                                     &TypeJunctionCityIioConfigTable,
                                     sizeof(TypeJunctionCityIioConfigTable)
                                     );

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformIioConfigDataDxeGuid_2,
                                     &TypeJunctionCityIioConfigTable,
                                     sizeof(TypeJunctionCityIioConfigTable)
                                     );

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformIioConfigDataDxeGuid_3,
                                     &TypeJunctionCityIioConfigTable,
                                     sizeof(TypeJunctionCityIioConfigTable)
                                     );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}
