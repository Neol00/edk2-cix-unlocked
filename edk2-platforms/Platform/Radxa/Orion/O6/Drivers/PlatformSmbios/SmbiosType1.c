/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"
#include <Protocol/FwVersionProtocol.h>
#include <Protocol/EcPlatformProtocol.h>

#define TYPE1_STRINGS                                                                         \
  "Radxa Computer (Shenzhen) Co., Ltd.\0"   /* Manufacturer */                                \
  "Radxa Orion O6\0"                        /* Product Name */                                \
  "1.0\0"                                   /* Version */                                     \
  "Radxa System Serial Number\0"            /* Serial number */                               \
  "Default\0"                               /* SKUNumber */                                   \
  "Orion O6\0"                              /* System Family */

#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE1    Base;
  UINT8                 Strings[sizeof (TYPE1_STRINGS)];
} PLATFORM_SMBIOS_TYPE1;
#pragma pack()

// System information (section 7.2)
STATIC PLATFORM_SMBIOS_TYPE1  mPlatformDefaultType1 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_INFORMATION,
      sizeof (SMBIOS_TABLE_TYPE1),
      SMBIOS_HANDLE_PI_RESERVED,
    },
    1,     // Manufacturer
    2,     // Product Name
    3,     // Version
    4,     // Serial
    { 0xFFFFFFFF,
        0xFFFF,
        0xFFFF,
        { 0xFF,      0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    },                                                      // UUID
    SystemWakeupTypePowerSwitch,                            // Wakeup type
    5,                                                      // SKU
    6,                                                      // Family
  },
  // Text strings (unformatted)
  TYPE1_STRINGS
};

EFI_STATUS
AddSmbiosType1 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS               Status;
  EFI_SMBIOS_HANDLE        SmbiosHandle;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status       = Smbios->Add (
                           Smbios,
                           NULL,
                           &SmbiosHandle,
                           (EFI_SMBIOS_TABLE_HEADER *)&mPlatformDefaultType1
                           );

  if (EFI_ERROR (Status)) {
    DEBUG (
      (
       DEBUG_ERROR,
       "[%a]:[%dL] Smbios Type1 Table Log Failed! %r \n",
       __FUNCTION__,
       DEBUG_LINE_NUMBER,
       Status
      )
      );
  }

  return EFI_SUCCESS;
}
