/** @file

  @copyright
  Copyright 2017 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Library/UbaSystemConfigUpdateLib.h>
#include <Protocol/UbaCfgDb.h>

EFI_STATUS
UpdateIioDefaultConfig (
  IN  SYSTEM_CONFIGURATION       *Default
  )
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;
  SYSTEM_CONFIG_UPDATE_DATA         SystemConfigUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR," [UpdateIioDefaultConfig] Locate UbaConfigProtocol fail!\n"));
    return Status;
  }

  DataLength = sizeof(SystemConfigUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                   UbaConfigProtocol,
                                   &gSystemConfigUpdateDataGuid,
                                   &SystemConfigUpdateTable,
                                   &DataLength
                                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR," [UpdateIioDefaultConfig] Get Data fail!\n"));
    return Status;
  }

  ASSERT (SystemConfigUpdateTable.Signature == SYSTEM_CONFIG_UPDATE_SIGNATURE);
  ASSERT (SystemConfigUpdateTable.Version   == SYSTEM_CONFIG_UPDATE_VERSION);

  SystemConfigUpdateTable.CallUpdateIioConfig (Default);

  return Status;
}
