/** @file
  DxeUbaIioConfigLib implementation.

  @copyright
  Copyright 2012 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/UbaCfgDb.h>
#include <Library/UbaIioConfigLib.h>

EFI_STATUS
PlatformIioConfigInit (
  IN OUT IIO_BIFURCATION_DATA_ENTRY       **BifurcationTable,
  IN OUT UINT8                            *BifurcationEntries,
  IN OUT IIO_SLOT_CONFIG_DATA_ENTRY       **SlotTable,
  IN OUT UINT8                            *SlotEntries
  )
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;
  PLATFORM_IIO_CONFIG_UPDATE_TABLE  IioUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength  = sizeof (IioUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformIioConfigDataDxeGuid,
                                    &IioUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (IioUpdateTable.Signature == PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE);
  ASSERT (IioUpdateTable.Version == PLATFORM_IIO_CONFIG_UPDATE_VERSION);

  *BifurcationTable = IioUpdateTable.IioBifurcationTablePtr;
  *BifurcationEntries = (UINT8) (IioUpdateTable.IioBifurcationTableSize / sizeof(IIO_BIFURCATION_DATA_ENTRY));

  *SlotTable = IioUpdateTable.IioSlotTablePtr;
  *SlotEntries = (UINT8)(IioUpdateTable.IioSlotTableSize / sizeof(IIO_SLOT_CONFIG_DATA_ENTRY));

  return EFI_SUCCESS;
}

EFI_STATUS
PlatformIioConfigInit2 (
  IN     UINT8                            SkuPersonalityType,
  IN OUT IIO_BIFURCATION_DATA_ENTRY       **BifurcationTable,
  IN OUT UINT8                            *BifurcationEntries,
  IN OUT IIO_SLOT_CONFIG_DATA_ENTRY       **SlotTable,
  IN OUT UINT8                            *SlotEntries
  )
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;
  PLATFORM_IIO_CONFIG_UPDATE_TABLE  IioUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength  = sizeof (IioUpdateTable);
  if (SkuPersonalityType == 1) {
    Status = UbaConfigProtocol->GetData (
                                      UbaConfigProtocol,
                                      &gPlatformIioConfigDataDxeGuid_1,
                                      &IioUpdateTable,
                                      &DataLength
                                      );
  } else if (SkuPersonalityType == 2) {
    Status = UbaConfigProtocol->GetData (
                                      UbaConfigProtocol,
                                      &gPlatformIioConfigDataDxeGuid_2,
                                      &IioUpdateTable,
                                      &DataLength
                                      );
  } else if (SkuPersonalityType == 3) {
    Status = UbaConfigProtocol->GetData (
                                      UbaConfigProtocol,
                                      &gPlatformIioConfigDataDxeGuid_3,
                                      &IioUpdateTable,
                                      &DataLength
                                      );
  } else {
    Status = UbaConfigProtocol->GetData (
                                      UbaConfigProtocol,
                                      &gPlatformIioConfigDataDxeGuid,
                                      &IioUpdateTable,
                                      &DataLength
                                      );
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (IioUpdateTable.Signature == PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE);
  ASSERT (IioUpdateTable.Version == PLATFORM_IIO_CONFIG_UPDATE_VERSION);

  *BifurcationTable = IioUpdateTable.IioBifurcationTablePtr;
  *BifurcationEntries = (UINT8) (IioUpdateTable.IioBifurcationTableSize / sizeof(IIO_BIFURCATION_DATA_ENTRY));

  *SlotTable = IioUpdateTable.IioSlotTablePtr;
  *SlotEntries = (UINT8)(IioUpdateTable.IioSlotTableSize / sizeof(IIO_SLOT_CONFIG_DATA_ENTRY));

  return EFI_SUCCESS;
}
