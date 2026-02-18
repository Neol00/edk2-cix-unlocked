/** @file
  PeiBoardInit.

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021 - 2022, American Megatrends International LLC. <BR>

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
#include <MemCommon.h>
#include <Cpu/CpuIds.h>

// TypeAowanda
EFI_STATUS
TypeAowandaPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

EFI_STATUS
TypeAowandaPlatformUpdateAcpiTablePcds (
  VOID
  );

EFI_STATUS
TypeAowandaInstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

EFI_STATUS
TypeAowandaInstallPcdData (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

EFI_STATUS
TypeAowandaPchEarlyUpdate (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

EFI_STATUS
TypeAowandaIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

EFI_STATUS
TypeAowandaInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

EFI_STATUS
TypeAowandaInstallKtiEparamData (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

// TypeAowanda
EFI_STATUS
TypeAowandaInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

EFI_STATUS
TypeAowandaInstallSoftStrapData (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  );

#endif // _PEI_BOARD_INIT_PEIM_H_
