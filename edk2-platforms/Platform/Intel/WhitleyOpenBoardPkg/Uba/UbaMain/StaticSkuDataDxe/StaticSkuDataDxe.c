/** @file
  UBA static sku data update dxe driver.

  @copyright
  Copyright 2013 - 2014 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "StaticSkuDataDxe.h"

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
StaticSkuConfigDataDxeEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                              Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = InstallMpTableData (UbaConfigProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = InstallPirqData (UbaConfigProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = InstallAcpiFixupTableData (UbaConfigProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}
