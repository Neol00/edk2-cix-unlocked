/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Library/AcpiLib.h>
#include <Library/DebugLib.h>

EFI_GUID  pAcpiPlatformTableStorageGuid = {
  0xc1bb2ead, 0xc76a, 0x45dd, { 0x90, 0xb5, 0xd4, 0x02, 0x55, 0x17, 0x0a, 0x9c }
};

EFI_STATUS
EFIAPI
AcpiPlatformDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "Acpi Platform Dxe Entry\n"));

  Status = LocateAndInstallAcpiFromFv (&pAcpiPlatformTableStorageGuid);
  DEBUG ((DEBUG_INFO, "Install Acpi table status:%r\n", Status));

  DEBUG ((DEBUG_INFO, "Acpi Platform Dxe Exit\n"));

  return EFI_SUCCESS;
}
