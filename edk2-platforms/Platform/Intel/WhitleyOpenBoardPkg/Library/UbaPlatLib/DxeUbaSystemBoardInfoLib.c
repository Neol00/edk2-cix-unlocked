/** @file

  @copyright
  Copyright 2017 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Library/UbaSystemBoardInfoLib.h>
#include <Protocol/UbaCfgDb.h>


EFI_STATUS
GetSystemBoardInfo (
  IN OUT   DXE_SYSTEM_BOARD_INFO       **SystemboardInfoTableBuffer
  )
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;
  SYSTEM_BOARD_INFO_DATA            SystemBoardInfoData;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR," [GetSystemBoardInfo] Locate UbaConfigProtocol fail!\n"));
    return Status;
  }

  DataLength = sizeof(SystemBoardInfoData);
  Status = UbaConfigProtocol->GetData (
                                   UbaConfigProtocol,
                                   &gSystemBoardInfoConfigDataGuid,
                                   &SystemBoardInfoData,
                                   &DataLength
                                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR," [GetSystemBoardInfo] Get Data fail!\n"));
    return Status;
  }

  ASSERT (SystemBoardInfoData.Signature == SYSTEM_SYSTEM_BOARD_INFO_SIGNATURE);
  ASSERT (SystemBoardInfoData.Version   == SYSTEM_SYSTEM_BOARD_INFO_VERSION);

  *SystemboardInfoTableBuffer = SystemBoardInfoData.CallUpdate ();

  return Status;
}
