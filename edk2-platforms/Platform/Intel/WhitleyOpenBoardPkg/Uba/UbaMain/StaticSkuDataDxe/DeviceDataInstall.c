/** @file
  Device data installation.

  @copyright
  Copyright 2014 - 2022 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "StaticSkuDataDxe.h"

#include <Library/UbaPirqUpdateLib.h>
#include <Library/UbaMpTableUpdateLib.h>

#include <PlatPirqData.h>
#include <PlatDevData.h>

#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>

extern PLATFORM_PIRQ_DATA    mPlatformPirqDataPlatformSRP10nm;

PLATFORM_PIRQ_UPDATE_TABLE  PirqUpdate10nm =
{
  PLATFORM_PIRQ_UPDATE_SIGNATURE,
  PLATFORM_PIRQ_UPDATE_VERSION,
  &mPlatformPirqDataPlatformSRP10nm
};

EFI_STATUS
InstallPirqData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  PLATFORM_PIRQ_UPDATE_TABLE            *PtrTable;
  UINT32                                TableSize;
  EFI_HOB_GUID_TYPE                     *GuidHob;

  GuidHob = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  PtrTable = &PirqUpdate10nm;
  TableSize = sizeof(PirqUpdate10nm);
  DEBUG ((DEBUG_ERROR, "UBA: Loading Pirqupdate table for ICX\n"));

  Status = UbaConfigProtocol->AddData (
                                UbaConfigProtocol,
                                &gPlatformPirqConfigDataGuid,
                                PtrTable,
                                TableSize
                                );
  return Status;
}

extern DEVICE_DATA           mDeviceDataPlatformSRP10nm;

PLATFORM_MP_UPDATE_TABLE  MpTableUpdate10nm =
{
  PLATFORM_MP_TABLE_UPDATE_SIGNATURE,
  PLATFORM_MP_TABLE_UPDATE_VERSION,
  &mDeviceDataPlatformSRP10nm
};

EFI_STATUS
InstallMpTableData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  PLATFORM_MP_UPDATE_TABLE              *PtrTable;
  UINT32                                TableSize;
  EFI_HOB_GUID_TYPE                     *GuidHob;

  GuidHob = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  PtrTable = &MpTableUpdate10nm;
  TableSize = sizeof(MpTableUpdate10nm);
  DEBUG ((DEBUG_ERROR, "UBA: Loading MpTableupdate table for ICX\n"));

  Status = UbaConfigProtocol->AddData (
                                UbaConfigProtocol,
                                &gPlatformMpTableConfigDataGuid,
                                PtrTable,
                                TableSize
                                );

  return Status;
}
