/** @file
  This driver manages the initial phase of Multi PCH

  @copyright
  Copyright 2019 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PchMultiPch.h>

///
/// The default PCH PCI segment number
///
#define DEFAULT_PCI_SEGMENT_NUMBER_PCH  0

/**
 @brief
   Multi PCH entry point.

 @param[in] FileHandle  PEIM file handle
 @param[in] PeiServices General purpose services available to every PEIM

 @retval EFI_SUCCESS    The function completed successfully.
**/
EFI_STATUS
MultiPchPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                    Status;
  PCH_IP_INFO                   *PchInfo;
  EFI_PEI_PPI_DESCRIPTOR        *PchIpInfoPpiDesc;

  DEBUG ((DEBUG_INFO, "[PCH] MultiPchPeiEntryPoint called.\n"));

  //
  // Create PchIpInfo
  //
  PchInfo = (PCH_IP_INFO *) AllocateZeroPool (sizeof (PCH_IP_INFO));
  if (PchInfo == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  PchInfo->Revision = PCH_IP_INFO_REVISION;
  PchInfo->Valid[PCH_LEGACY_ID] = TRUE;
  PchInfo->Segment[PCH_LEGACY_ID] = DEFAULT_PCI_SEGMENT_NUMBER_PCH;
  PchInfo->Bus[PCH_LEGACY_ID] = DEFAULT_PCI_BUS_NUMBER_PCH;
  PchInfo->P2sbBar[PCH_LEGACY_ID] = PCH_PCR_BASE_ADDRESS;
  PchInfo->PmcBar[PCH_LEGACY_ID] = PCH_PWRM_BASE_ADDRESS;
  PchInfo->SpiBar[PCH_LEGACY_ID] = PCH_SPI_BASE_ADDRESS;
  PchInfo->TempBar[PCH_LEGACY_ID] = PCH_TEMP_BASE_ADDRESS;

  //
  // Install PchIpInfoPpi
  //
  PchIpInfoPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (PchIpInfoPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PchIpInfoPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PchIpInfoPpiDesc->Guid  = &gPchIpInfoPpiGuid;
  PchIpInfoPpiDesc->Ppi   = PchInfo;

  Status = PeiServicesInstallPpi (PchIpInfoPpiDesc);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
