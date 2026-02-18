/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "CixSmbiosDxe.h"

#define TYPE16_STRINGS  "\0" /* nothing */

#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE16    Base;
  UINT8                  Strings[sizeof (TYPE16_STRINGS)];
} CIX_TYPE16;
#pragma pack()

// Memory array
STATIC CIX_TYPE16  mCixDefaultType16 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE16),                // UINT8 Length
      SMBIOS_HANDLE_MEMORY,
    },
    MemoryArrayLocationSystemBoard,     // on motherboard
    MemoryArrayUseSystemMemory,         // system RAM
    MemoryErrorCorrectionNone,
    0x4000000,     // 64GB
    0xFFFE,        // No error information structure
    0x1,           // soldered memory
  },
  TYPE16_STRINGS
};

EFI_STATUS
AddSmbiosType16 (
  IN EFI_SMBIOS_PROTOCOL           *Smbios,
  IN const MEM_INIT_OUTPUT_BUFFER  *MemoryInfo
  )
{
  EFI_STATUS         Status;
  EFI_SMBIOS_HANDLE  SmbiosHandle;

  mCixDefaultType16.Base.MemoryErrorCorrection = MemoryInfo->EccType;
  mCixDefaultType16.Base.NumberOfMemoryDevices = COUNT_MEMORY_DEVICE_NUMBER (MemoryInfo->ChannelMask);

  SmbiosHandle = SMBIOS_HANDLE_MEMORY;
  Status       = Smbios->Add (
                           Smbios,
                           NULL,
                           &SmbiosHandle,
                           (EFI_SMBIOS_TABLE_HEADER *)&mCixDefaultType16
                           );

  if (EFI_ERROR (Status)) {
    DEBUG (
      (
       DEBUG_ERROR,
       "[%a]:[%dL] Smbios Type16 Table Log Failed! %r \n",
       __FUNCTION__,
       DEBUG_LINE_NUMBER,
       Status
      )
      );
  }

  return EFI_SUCCESS;
}
