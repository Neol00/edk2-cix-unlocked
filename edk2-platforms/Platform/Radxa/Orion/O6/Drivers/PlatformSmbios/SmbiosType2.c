/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"
#include <Protocol/FwVersionProtocol.h>
#include <Protocol/EcPlatformProtocol.h>

#define TYPE2_STRINGS                                                                    \
  "Radxa Computer (Shenzhen) Co., Ltd.\0"   /* Manufacturer */                           \
  "Radxa Orion O6\0"                        /* Product Name */                           \
  "1.0\0"                              /* Version */                                     \
  "Radxa Board Serial Number\0"         /* Serial */                                      \
  "Radxa Board Asset Tag\0"             /* Assert Tag */                                  \
  "Part Component\0"                   /* board location */
#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE2    Base;
  UINT8                 Strings[sizeof (TYPE2_STRINGS)];
} PLATFORM_SMBIOS_TYPE2;
#pragma pack()

// Baseboard (section 7.3)
STATIC PLATFORM_SMBIOS_TYPE2  mPlatformDefaultType2 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE2),                 // UINT8 Length
      SMBIOS_HANDLE_BASE_BOARD,
    },
    1,     // Manufacturer
    2,     // Product Name
    3,     // Version
    4,     // Serial
    5,     // Asset tag
    {
      1,
    },     // motherboard, not replaceable
    6,     // location of board
    SMBIOS_HANDLE_CHASSIS,
    BaseBoardTypeMotherBoard,
    1,
    { SMBIOS_HANDLE_CLUSTER1 },   // ,SMBIOS_HANDLE_CLUSTER,SMBIOS_HANDLE_MEMORY},
  },
  TYPE2_STRINGS
};

EFI_STATUS
AddSmbiosType2 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS               Status;
  EFI_SMBIOS_HANDLE        SmbiosHandle, SmbiosHandleType4;
  EFI_SMBIOS_TABLE_HEADER  *Record;
  EFI_SMBIOS_TYPE          Type4 = SMBIOS_TYPE_PROCESSOR_INFORMATION;

  SmbiosHandleType4 = SMBIOS_HANDLE_PI_RESERVED;
  Status            = Smbios->GetNext (Smbios, &SmbiosHandleType4, &Type4, &Record, NULL);
  if (!EFI_ERROR (Status) && (SmbiosHandleType4 != SMBIOS_HANDLE_PI_RESERVED)) {
    mPlatformDefaultType2.Base.ContainedObjectHandles[0] = SmbiosHandleType4;
  } else {
    DEBUG ((DEBUG_ERROR, "Fail to locate the handle of Smbios Type4 entry!\n"));
  }

  SmbiosHandle = SMBIOS_HANDLE_BASE_BOARD;
  Status       = Smbios->Add (
                           Smbios,
                           NULL,
                           &SmbiosHandle,
                           (EFI_SMBIOS_TABLE_HEADER *)&mPlatformDefaultType2
                           );

  if (EFI_ERROR (Status)) {
    DEBUG (
      (
       DEBUG_ERROR,
       "[%a]:[%dL] Smbios Type2 Table Log Failed! %r \n",
       __FUNCTION__,
       DEBUG_LINE_NUMBER,
       Status
      )
      );
  }

  return EFI_SUCCESS;
}
