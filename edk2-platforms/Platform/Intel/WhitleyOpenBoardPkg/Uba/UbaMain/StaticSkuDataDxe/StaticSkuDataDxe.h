/** @file
  UBA static sku data update dxe driver.

  @copyright
  Copyright 2013 - 2022 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _STATIC_SKU_DATA_DXE_H_
#define _STATIC_SKU_DATA_DXE_H_

#include <Base.h>
#include <Uefi.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <UncoreCommonIncludes.h>
#include <Protocol/UbaCfgDb.h>

EFI_STATUS
InstallMpTableData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
);

EFI_STATUS
InstallPirqData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
);

EFI_STATUS
InstallAcpiFixupTableData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
);

#endif // _STATIC_SKU_DATA_DXE_H_
