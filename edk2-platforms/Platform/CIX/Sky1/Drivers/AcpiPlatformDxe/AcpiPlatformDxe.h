/** @file

  Copyright 2023 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ACPI_PLATFORM_DXE_H_
#define _ACPI_PLATFORM_DXE_H_
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PciHostBridgeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AcpiLib.h>
#include <Library/ConfigParamsDataBlockLib.h>
#include <Protocol/AcpiTable.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <AcpiPlatform.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/PlatformAcpiConfigProtocol.h>
#include <Protocol/ConfigParamsManageProtocol.h>

EFI_STATUS
EFIAPI
GetAcpiTableBySignature (
  IN   PLATFORM_ACPI_CONFIG_PROTOCOL   *This,
  IN      UINT32                       Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER  **Table,
  IN OUT  UINTN                        *Handle
  );

EFI_STATUS
EFIAPI
UpdateNameAslCode (
  IN   PLATFORM_ACPI_CONFIG_PROTOCOL  *This,
  IN     UINT32                       AslSignature,
  IN     VOID                         *Buffer,
  IN     UINTN                        Length
  );

EFI_STATUS EFIAPI
InstallAcpiOnReadyToBoot (
  VOID
  );

EFI_STATUS
EFIAPI
UpdateAcpiDsdt (
  VOID
  );

EFI_STATUS
EFIAPI
UpdateAcpiDsdtTable (
  VOID
  );

EFI_STATUS
EFIAPI
SpcrDisable (
  VOID
  );

typedef EFI_STATUS (*ACPI_FUNCTION_ON_READ_TO_BOOT_HOOK)(
  void
  );
#endif /*_ACPI_PLATFORM_DXE_H_*/
