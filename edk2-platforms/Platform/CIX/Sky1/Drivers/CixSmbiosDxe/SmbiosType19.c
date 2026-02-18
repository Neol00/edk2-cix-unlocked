/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "CixSmbiosDxe.h"

#define TYPE19_STRINGS  "\0" /* nothing */

#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE19    Base;
  UINT8                  Strings[sizeof (TYPE19_STRINGS)];
} CIX_TYPE19;
#pragma pack()

// Memory array mapped address, this structure
// is overridden by InstallMemoryStructure
STATIC CIX_TYPE19  mCixDefaultType19 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE19),                      // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    0xFFFFFFFF,     // invalid, look at extended addr field
    0xFFFFFFFF,
    SMBIOS_HANDLE_MEMORY,     // handle
    1,
    0x080000000,     // starting addr of first 2GB
    0x100000000,     // ending addr of first 2GB
  },
  TYPE19_STRINGS
};

EFI_STATUS
AddSmbiosType19 (
  IN EFI_SMBIOS_PROTOCOL           *Smbios,
  IN const MEM_INIT_OUTPUT_BUFFER  *MemoryInfo
  )
{
  EFI_STATUS         Status;
  EFI_SMBIOS_HANDLE  SmbiosHandle;
  UINT64             StartingAddress;
  UINT64             RegionLength;

  StartingAddress = PcdGet64 (PcdSystemMemoryBase);
  RegionLength    = ((UINT64)(MemoryInfo->TotalSize))<<20;

  mCixDefaultType19.Base.ExtendedStartingAddress = StartingAddress;
  mCixDefaultType19.Base.ExtendedEndingAddress   = StartingAddress + RegionLength - 1;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status       = Smbios->Add (
                           Smbios,
                           NULL,
                           &SmbiosHandle,
                           (EFI_SMBIOS_TABLE_HEADER *)&mCixDefaultType19
                           );

  if (EFI_ERROR (Status)) {
    DEBUG (
      (
       DEBUG_ERROR,
       "[%a]:[%dL] Smbios Type19 Table Log Failed! %r \n",
       __FUNCTION__,
       DEBUG_LINE_NUMBER,
       Status
      )
      );
  }

  return EFI_SUCCESS;
}
