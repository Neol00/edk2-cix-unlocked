/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"
#include <Library/UefiBootServicesTableLib.h>

ADD_PLATFORM_SMBIOS_TABLE  *PlatformSmbiosTableList[] = { PLATFORM_SMBIOS_TABLE_HOOK, NULL };
CHAR8                      *PlatformSmbiosTableName[] = { PLATFORM_SMBIOS_TABLE_NAME, NULL };

EFI_STATUS
EFIAPI
PlatformSmbiosEntryPoint (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS           Status;
  UINTN                Index;
  EFI_SMBIOS_PROTOCOL  *Smbios;

  //
  // Find the SMBIOS protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] Fail to locate gEfiSmbiosProtocolGuid!\n", __FUNCTION__));
    return Status;
  }

  for (Index = 0; PlatformSmbiosTableList[Index]; Index++) {
    Status = PlatformSmbiosTableList[Index](Smbios);
    DEBUG (
      (DEBUG_INFO,
       "[Platfrom Smbios] Execute Add: %a, Status: %r\n",
       PlatformSmbiosTableName[Index], Status)
      );
  }

  return EFI_SUCCESS;
}
