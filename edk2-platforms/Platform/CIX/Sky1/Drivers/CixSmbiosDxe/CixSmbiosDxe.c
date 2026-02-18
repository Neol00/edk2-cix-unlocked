/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "CixSmbiosDxe.h"
#include <Library/CpuInfoLib.h>

EFI_STATUS
UpdateSmbiosType4 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS               Status;
  EFI_SMBIOS_HANDLE        SmbiosHandleType4;
  EFI_SMBIOS_TABLE_HEADER  *Record;
  EFI_SMBIOS_TYPE          Type4 = SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SMBIOS_TABLE_TYPE4       *Type4Record;

  SmbiosHandleType4 = SMBIOS_HANDLE_PI_RESERVED;
  Status            = Smbios->GetNext (Smbios, &SmbiosHandleType4, &Type4, &Record, NULL);
  if (!EFI_ERROR (Status) && (SmbiosHandleType4 != SMBIOS_HANDLE_PI_RESERVED)) {
    Type4Record                   = (SMBIOS_TABLE_TYPE4 *)Record;
    Type4Record->ProcessorFamily2 = ProcessorFamilyARMv9;
    Type4Record->CurrentSpeed     = (UINT16)(ROUND_DIVISION (GetCurrentCpuFreq (), 1000000));
  } else {
    DEBUG ((DEBUG_ERROR, "Fail to locate the handle of Smbios Type4 entry!\n"));
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CixSmbiosDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS             Status;
  EFI_SMBIOS_PROTOCOL    *Smbios;
  CIX_SOC_INFO_PROTOCOL  *pSocInfoProtocol;

  //
  // Find the SMBIOS protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Update processor (type4) table
  Status = UpdateSmbiosType4 (Smbios);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to update smbios type4!\n"));
  }

  Status = gBS->LocateProtocol (
                  &gCixSocInfoProtocolGuid,
                  NULL,
                  (VOID **)&pSocInfoProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: soc info protocol not found\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  if (pSocInfoProtocol->MemInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "MemInfo not find!\n"));
    return EFI_NOT_FOUND;
  }

  Status = AddSmbiosType16 (Smbios, pSocInfoProtocol->MemInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to add smbios type16!\n"));
  }

  Status = AddSmbiosType17 (Smbios, pSocInfoProtocol->MemInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to add smbios type17!\n"));
  }

  Status = AddSmbiosType19 (Smbios, pSocInfoProtocol->MemInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to add smbios type19!\n"));
  }

  return Status;
}
