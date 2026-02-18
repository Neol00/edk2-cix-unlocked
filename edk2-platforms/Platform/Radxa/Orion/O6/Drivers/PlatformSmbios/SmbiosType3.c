/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"

#define TYPE3_STRINGS                                                                      \
  "Radxa Computer (Shenzhen) Co., Ltd.\0" /* Manufacturer */                               \
  "1.0\0"                            /* Version */                                         \
  "Radxa Serial Number\0"            /* Serial  */                                         \
  "Radxa Asset Tag\0"              /* Assert Tag */

#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE3    Base;
  UINT8                 Strings[sizeof (TYPE3_STRINGS)];
} PLATFORM_SMBIOS_TYPE3;
#pragma pack()

// Enclosure
STATIC CONST PLATFORM_SMBIOS_TYPE3  mPlatformDefaultType3 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE3),            // UINT8 Length
      SMBIOS_HANDLE_CHASSIS,
    },
    1,                             // Manufacturer
    MiscChassisTypeUnknown,        // enclosure type
    2,                             // version
    3,                             // serial
    4,                             // asset tag
    ChassisStateUnknown,           // boot chassis state
    ChassisStateSafe,              // power supply state
    ChassisStateSafe,              // thermal state
    ChassisSecurityStatusNone,     // security state
    {
      0,
      0,
      0,
      0,
    },     // OEM defined
    0,     // unknown height
    0,     // unkonwn number of power cords
    0,     // no contained elements
  },
  TYPE3_STRINGS
};

EFI_STATUS
AddSmbiosType3 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS         Status;
  EFI_SMBIOS_HANDLE  SmbiosHandle;

  SmbiosHandle = SMBIOS_HANDLE_CHASSIS;
  Status       = Smbios->Add (
                           Smbios,
                           NULL,
                           &SmbiosHandle,
                           (EFI_SMBIOS_TABLE_HEADER *)&mPlatformDefaultType3
                           );

  if (EFI_ERROR (Status)) {
    DEBUG (
      (
       DEBUG_ERROR,
       "[%a]:[%dL] Smbios Type3 Table Log Failed! %r \n",
       __FUNCTION__,
       DEBUG_LINE_NUMBER,
       Status
      )
      );
  }

  return EFI_SUCCESS;
}
