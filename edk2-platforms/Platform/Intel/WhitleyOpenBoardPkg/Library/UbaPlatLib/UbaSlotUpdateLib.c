/** @file
  UbaSlotUpdateLib implementation.

  @copyright
  Copyright 2012 - 2016 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UbaSlotUpdateLib.h>
#include <Protocol/UbaCfgDb.h>

EFI_STATUS
PlatformGetSlotTableData (
  IN OUT IIO_BROADWAY_ADDRESS_DATA_ENTRY  **BroadwayTable,
  IN OUT UINT8                            *IOU2Setting,
  IN OUT UINT8                            *FlagValue
  )
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;
  PLATFORM_SLOT_UPDATE_TABLE        IioSlotUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength = sizeof(IioSlotUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                   UbaConfigProtocol,
                                   &gPlatformSlotDataDxeGuid,
                                   &IioSlotUpdateTable,
                                   &DataLength
                                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (IioSlotUpdateTable.Signature == PLATFORM_SLOT_UPDATE_SIGNATURE);
  ASSERT (IioSlotUpdateTable.Version   == PLATFORM_SLOT_UPDATE_VERSION);

  *BroadwayTable  = IioSlotUpdateTable.BroadwayTablePtr;
  *IOU2Setting    = IioSlotUpdateTable.GetIOU2Setting (*IOU2Setting);
  *FlagValue      = IioSlotUpdateTable.FlagValue;
  return Status;
}

EFI_STATUS
PlatformGetSlotTableData2 (
  IN OUT IIO_BROADWAY_ADDRESS_DATA_ENTRY  **BroadwayTable,
  IN OUT UINT8                            *IOU0Setting,
  IN OUT UINT8                            *FlagValue,
  IN OUT UINT8                            *IOU2Setting,
  IN     UINT8                            SkuPersonalityType
  )
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;
  PLATFORM_SLOT_UPDATE_TABLE2       IioSlotUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength = sizeof(IioSlotUpdateTable);
  if ((SkuPersonalityType == 1) || (SkuPersonalityType == 3)) {
    Status = UbaConfigProtocol->GetData (
                                     UbaConfigProtocol,
                                     &gPlatformSlotDataDxeGuid2_1,
                                     &IioSlotUpdateTable,
                                     &DataLength
                                     );
  } else {
    Status = UbaConfigProtocol->GetData (
                                     UbaConfigProtocol,
                                     &gPlatformSlotDataDxeGuid2,
                                     &IioSlotUpdateTable,
                                     &DataLength
                                     );
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (IioSlotUpdateTable.Signature == PLATFORM_SLOT_UPDATE_SIGNATURE);
  ASSERT (IioSlotUpdateTable.Version   == PLATFORM_SLOT_UPDATE_VERSION);

  *BroadwayTable = IioSlotUpdateTable.BroadwayTablePtr;
  *IOU0Setting   = IioSlotUpdateTable.GetIOU0Setting (*IOU0Setting);
  *FlagValue     = IioSlotUpdateTable.FlagValue;
  *IOU2Setting   = IioSlotUpdateTable.GetIOU2Setting (SkuPersonalityType, *IOU2Setting);

  return Status;
}
