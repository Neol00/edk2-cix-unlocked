/** @file

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021 - 2022, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PeiBoardInit.h"
#include <KtiSetupDefinitions.h>
#include <UbaKti.h>
#include <UncoreCommonIncludes.h>

extern EFI_GUID  gPlatformKtiEparamUpdateDataGuid;

ALL_LANES_EPARAM_LINK_INFO  KtiAowandaAllLanesEparamTable[] = {
  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  // Please propagate changes to WilsonCitySMT and WilsonCityModular UBA KtiEparam tables
  //
  //
  // Socket 0
  //
  { 0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT) | (1 << SPEED_REC_112GT), (1 << KTI_LINK0), 0x2B33373F, ADAPTIVE_CTLE },
  { 0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT) | (1 << SPEED_REC_112GT), (1 << KTI_LINK1), 0x2A33363F, ADAPTIVE_CTLE },
  { 0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT) | (1 << SPEED_REC_112GT), (1 << KTI_LINK2), 0x2B34363F, ADAPTIVE_CTLE }
};

PLATFORM_KTI_EPARAM_UPDATE_TABLE  TypeAowandaKtiEparamUpdate =
{
  PLATFORM_KTIEP_UPDATE_SIGNATURE,
  PLATFORM_KTIEP_UPDATE_VERSION,
  KtiAowandaAllLanesEparamTable,
  sizeof (KtiAowandaAllLanesEparamTable),
  NULL,
  0
};

ALL_LANES_EPARAM_LINK_INFO  KtiAowandaCpxAllLanesEparamTable[] = {
  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  //
  //
  // Socket 0
  //
  { 0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT) | (1 << SPEED_REC_112GT), (1 << KTI_LINK0), 0x2E39343F, ADAPTIVE_CTLE },
  { 0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT) | (1 << SPEED_REC_112GT), (1 << KTI_LINK5), 0x2F39353F, ADAPTIVE_CTLE }
};

PLATFORM_KTI_EPARAM_UPDATE_TABLE  TypeAowandaCpxKtiEparamUpdate =
{
  PLATFORM_KTIEP_UPDATE_SIGNATURE,
  PLATFORM_KTIEP_UPDATE_VERSION,
  KtiAowandaCpxAllLanesEparamTable,
  sizeof (KtiAowandaCpxAllLanesEparamTable),
  NULL,
  0
};

EFI_STATUS
TypeAowandaInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  )
{
  EFI_STATUS         Status;
  EFI_HOB_GUID_TYPE  *GuidHob;

  GuidHob = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = UbaConfigPpi->AddData (
                                  UbaConfigPpi,
                                  &gPlatformKtiEparamUpdateDataGuid,
                                  &TypeAowandaKtiEparamUpdate,
                                  sizeof (TypeAowandaKtiEparamUpdate)
                                  );

  return Status;
}
