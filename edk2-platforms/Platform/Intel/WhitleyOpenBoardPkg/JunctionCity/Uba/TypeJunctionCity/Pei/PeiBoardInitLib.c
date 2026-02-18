/** @file

 @copyright
  Copyright 2018 - 2021 Intel Corporation.
  Copyright (c) 2021, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


/**
  The constructor function for Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/

#include "PeiBoardInit.h"
#include <UncoreCommonIncludes.h>
#include <Library/PchMultiPchBase.h>
#include <Ppi/DynamicSiLibraryPpi.h>

EFI_STATUS
EFIAPI
TypeJunctionCityPeiBoardInitLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  UBA_CONFIG_DATABASE_PPI         *UbaConfigPpi;
  EFI_HOB_GUID_TYPE               *GuidHob;
  EFI_PLATFORM_INFO               *PlatformInfo;
  UINT8                           SocketIndex;
  UINT8                           ChannelIndex;

  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob);

  if (PlatformInfo->BoardId == TypeJunctionCity) {

    DEBUG ((DEBUG_INFO, "PEI UBA init BoardId 0x%X: JunctionCity\n", PlatformInfo->BoardId));

    // Socket 0 has SMT DIMM connector, Socket 1 has PTH DIMM connector
    for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
      for (ChannelIndex = 0; ChannelIndex < MAX_CH; ChannelIndex++) {
        switch (SocketIndex) {
          case 0:
            PlatformInfo->MemoryConnectorType[SocketIndex][ChannelIndex] = DimmConnectorSmt;
            break;
          case 1:
            // Fall through since socket 1 is PTH type
          default:
            // Use the more restrictive type as the default case
            PlatformInfo->MemoryConnectorType[SocketIndex][ChannelIndex] = DimmConnectorPth;
            break;
        }
      }
    }

    BuildGuidDataHob (
        &gEfiPlatformInfoGuid,
        &(PlatformInfo),
        sizeof (EFI_PLATFORM_INFO)
        );

    Status = PeiServicesLocatePpi (
              &gUbaConfigDatabasePpiGuid,
              0,
              NULL,
              (VOID **) &UbaConfigPpi
              );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = UbaConfigPpi->InitSku (
                       UbaConfigPpi,
                       PlatformInfo->BoardId,
                       NULL,
                       NULL
                       );
    ASSERT_EFI_ERROR (Status);

    Status = TypeJunctionCityInstallGpioData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeJunctionCityInstallPcdData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeJunctionCityInstallSoftStrapData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeJunctionCityPchEarlyUpdate (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeJunctionCityPlatformUpdateUsbOcMappings (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeJunctionCityInstallSlotTableData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = TypeJunctionCityInstallKtiEparamData (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {

      //
      // Set default memory type connector.
      // Socket 0: DimmConnectorSmt
      // Socket 1: DimmConnectorPth
      //
      if (SocketIndex % 2 == 0) {
        (*PeiServices)->SetMem (&PlatformInfo->MemoryConnectorType[SocketIndex], sizeof (PlatformInfo->MemoryConnectorType[SocketIndex]), DimmConnectorSmt);
      } else {
        (*PeiServices)->SetMem (&PlatformInfo->MemoryConnectorType[SocketIndex], sizeof (PlatformInfo->MemoryConnectorType[SocketIndex]), DimmConnectorPth);
      }
    }

    //
    // Initialize InterposerType to InterposerUnknown
    //
    for (SocketIndex = 0; SocketIndex < MAX_SOCKET; ++SocketIndex) {
      PlatformInfo->InterposerType[SocketIndex] = InterposerUnknown;
    }

    //
    //  TypeJunctionCityIioPortBifurcationInit will use PlatformInfo->InterposerType for PPO.
    //
    Status = TypeJunctionCityIioPortBifurcationInit (UbaConfigPpi);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }
  return Status;
}
