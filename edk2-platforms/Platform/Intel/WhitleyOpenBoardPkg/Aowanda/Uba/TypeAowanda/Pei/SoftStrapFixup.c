/** @file
  Soft Strap update.

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021 - 2022, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PeiBoardInit.h"
#include <Library/UbaSoftStrapUpdateLib.h>
#include <GpioConfig.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>

PLATFORM_PCH_SOFTSTRAP_FIXUP_ENTRY  TypeAowandaSoftStrapTable[] =
{
  // SoftStrapNumber, LowBit, BitLength, Value
  { 0, 0, 0, 0 }
};

UINT32
TypeAowandaSystemBoardRevIdValue (
  VOID
  )
{
  EFI_HOB_GUID_TYPE  *GuidHob;
  EFI_PLATFORM_INFO  *PlatformInfo;

  GuidHob = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return 0xFF;
  }

  PlatformInfo = GET_GUID_HOB_DATA (GuidHob);
  return PlatformInfo->TypeRevisionId;
}

VOID
TypeAowandaPlatformSpecificUpdate (
  IN OUT  UINT8  *FlashDescriptorCopy
  )
{
}

PLATFORM_PCH_SOFTSTRAP_UPDATE  TypeAowandaSoftStrapUpdate =
{
  PLATFORM_SOFT_STRAP_UPDATE_SIGNATURE,
  PLATFORM_SOFT_STRAP_UPDATE_VERSION,
  TypeAowandaSoftStrapTable,
  TypeAowandaPlatformSpecificUpdate
};

EFI_STATUS
TypeAowandaInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  )
{
  EFI_STATUS  Status;

  Status = UbaConfigPpi->AddData (
                                  UbaConfigPpi,
                                  &gPlatformPchSoftStrapConfigDataGuid,
                                  &TypeAowandaSoftStrapUpdate,
                                  sizeof (TypeAowandaSoftStrapUpdate)
                                  );

  return Status;
}
