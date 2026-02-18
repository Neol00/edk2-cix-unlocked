/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"

#define STR(x) XSTR(x)
#define XSTR(x) #x

#define TYPE0_STRINGS                                                                    \
  "Radxa Computer (Shenzhen) Co., Ltd.\0" /* Vendor */                                   \
  STR (UEFI_FW_VERSION) "\0"                   /* BiosVersion */                              \
  STR (COMPILE_BUILD_DATE) "\0"                /* BiosReleaseDate */

#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE0    Base;
  INT8                  Strings[sizeof (TYPE0_STRINGS)];
} PLATFORM_SMBIOS_TYPE0;
#pragma pack()

// BIOS information (section 7.1)
STATIC PLATFORM_SMBIOS_TYPE0  mPlatformDefaultType0 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_BIOS_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE0),            // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    1,          // SMBIOS_TABLE_STRING       Vendor
    2,          // SMBIOS_TABLE_STRING       BiosVersion
    0,          // UINT16                    BiosSegment
    3,          // SMBIOS_TABLE_STRING       BiosReleaseDate
    0x7F,       // UINT8                     BiosSize
    {
      0, 0, 0, 0, 0, 0,
      1,       // PCI supported
      0,
      1,       // PNP supported
      0,
      1,       // BIOS upgradable
      0, 0, 0,
      0,       // Boot from CD
      1,       // selectable boot
    },         // MISC_BIOS_CHARACTERISTICS BiosCharacteristics
    {
      // BIOSCharacteristicsExtensionBytes[2]
      0x1,       // Acpi supported
      0xC,       // TargetContentDistribution, UEFI
    },
    0,       // UINT8                     SystemBiosMajorRelease
    1,       // UINT8                     SystemBiosMinorRelease
    0xFF,    // UINT8                     EmbeddedControllerFirmwareMajorRelease
    0xFF     // UINT8                     EmbeddedControllerFirmwareMinorRelease
  },
  // Text strings (unformatted area)
  TYPE0_STRINGS
};

EFI_STATUS
AddSmbiosType0 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS         Status;
  EFI_SMBIOS_HANDLE  SmbiosHandle;

  mPlatformDefaultType0.Base.SystemBiosMajorRelease =
    (PcdGet32 (PcdFirmwareRevision) >> 16) & 0xFF;
  mPlatformDefaultType0.Base.SystemBiosMinorRelease =
    PcdGet32 (PcdFirmwareRevision) & 0xFF;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status       = Smbios->Add (
                           Smbios,
                           NULL,
                           &SmbiosHandle,
                           (EFI_SMBIOS_TABLE_HEADER *)&mPlatformDefaultType0
                           );

  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "[%a]:[%dL] Smbios Type0 Table Log Failed! %r \n",
      __FUNCTION__,
      DEBUG_LINE_NUMBER,
      Status
      ));
  }

  return EFI_SUCCESS;
}
