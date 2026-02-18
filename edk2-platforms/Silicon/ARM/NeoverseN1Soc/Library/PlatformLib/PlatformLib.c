/** @file

  Copyright (c) 2018-2021, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ArmPlatformLib.h>
#include <Library/BaseLib.h>
#include <Ppi/ArmMpCoreInfo.h>

STATIC ARM_CORE_INFO mCoreInfoTable[] = {
  { 0x0, 0x0 }, // Cluster 0, Core 0
  { 0x0, 0x1 }, // Cluster 0, Core 1
  { 0x1, 0x0 }, // Cluster 1, Core 0
  { 0x1, 0x1 }  // Cluster 1, Core 1
};

/**
  Return the current Boot Mode.

  @return    The boot reason on the platform.

**/
EFI_BOOT_MODE
ArmPlatformGetBootMode (
         VOID
  )
{
  return BOOT_WITH_FULL_CONFIGURATION;
}

/**
  Initialize controllers that must be setup in the normal world.

  This function is called by the ArmPlatformPkg/Pei or
  ArmPlatformPkg/Pei/PlatformPeim in the PEI phase.

  @param[in]    MpId          Processor ID
  @retval       RETURN_SUCCESS

**/
RETURN_STATUS
ArmPlatformInitialize (
  IN     UINTN                  MpId
  )
{
  return RETURN_SUCCESS;
}

/**
  Populate the Platform core information.

  This function populates the ARM_MP_CORE_INFO_PPI with information
  about the cores.

  @param[out]   CoreCount     Number of cores
  @param[out]   ArmCoreTable  Table containing information about the cores
  @retval       EFI_SUCCESS

**/
EFI_STATUS
PrePeiCoreGetMpCoreInfo (
     OUT UINTN                  *CoreCount,
     OUT ARM_CORE_INFO          **ArmCoreTable
  )
{
  *CoreCount = sizeof (mCoreInfoTable) / sizeof (ARM_CORE_INFO);
  *ArmCoreTable = mCoreInfoTable;
  return EFI_SUCCESS;
}

STATIC ARM_MP_CORE_INFO_PPI mMpCoreInfoPpi = {
  PrePeiCoreGetMpCoreInfo
};

EFI_PEI_PPI_DESCRIPTOR gPlatformPpiTable[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gArmMpCoreInfoPpiGuid,
    &mMpCoreInfoPpi
  }
};

/**
  Return the Platform specific PPIs

  This function exposes the N1Sdp Specific PPIs.

  @param[out]   PpiListSize   Size in Bytes of the Platform PPI List
  @param[out]   PpiList       Platform PPI List

**/
VOID
ArmPlatformGetPlatformPpiList (
     OUT UINTN                  *PpiListSize,
     OUT EFI_PEI_PPI_DESCRIPTOR **PpiList
  )
{
  *PpiListSize = sizeof (gPlatformPpiTable);
  *PpiList = gPlatformPpiTable;
}
