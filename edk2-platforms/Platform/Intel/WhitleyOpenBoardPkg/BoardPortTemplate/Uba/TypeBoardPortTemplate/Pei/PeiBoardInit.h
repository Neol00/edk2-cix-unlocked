/** @file
  PeiBoardInit.

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PEI_BOARD_INIT_PEIM_H_
#define _PEI_BOARD_INIT_PEIM_H_

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/UbaCfgDb.h>
#include <Guid/PlatformInfo.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/GpioLib.h>
#include <GpioPinsSklH.h>
#include <Ppi/DynamicSiLibraryPpi.h>

// TypeBoardPortTemplate
EFI_STATUS
TypeBoardPortTemplatePlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBoardPortTemplatePlatformUpdateAcpiTablePcds (
  VOID
);

EFI_STATUS
TypeBoardPortTemplateInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBoardPortTemplateInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBoardPortTemplatePchEarlyUpdate (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBoardPortTemplateIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBoardPortTemplateInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

EFI_STATUS
TypeBoardPortTemplateInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);

// TypeBoardPortTemplate
EFI_STATUS
TypeBoardPortTemplateInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
) ;

EFI_STATUS
TypeBoardPortTemplateInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
);
#endif // _PEI_BOARD_INIT_PEIM_H_
