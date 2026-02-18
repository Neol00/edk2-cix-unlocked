/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  Copyright 2025 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"

/*
Could use following PCDs in the future:
  gArmTokenSpaceGuid.PcdProcessorManufacturer|L"Cix Technology Group Co., Ltd."
  gCixTokenSpaceGuid.PcdCixProcessorVersion|L"CIX P1 CD8180"
*/

#define TYPE4_STRINGS                                                          \
  "FCBGA\0"                /* socket type */                                        \
  "Cix Technology Group Co., Ltd.\0" /* manufacturer */                                       \
  "SKY1\0"                 /* processor description */                            \
  "CIX P1 CD8180\0"          /* part number */

#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE4    Base;
  UINT8                 Strings[sizeof (TYPE4_STRINGS)];
} PLATFORM_SMBIOS_TYPE4;
#pragma pack()

// Enclosure
STATIC CONST PLATFORM_SMBIOS_TYPE4  mPlatformDefaultType4 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE4),                 // UINT8 Length
      SMBIOS_HANDLE_CLUSTER1,
    },
    1,                               // socket type
    CentralProcessor,                // processor type CPU
    ProcessorFamilyIndicatorFamily2, // processor family, acquire from
                                     // field2
    2,                               // manufacturer
    {{0,},{0.}},            // processor id - see section 7.5.3.3
    3,                      // processor description
    { 0, 0, 0, 0, 0, 1 },   // voltage
    0,                      // external clock
    4500,                   // max speed
    2600,                   // current speed (stock)
    0x41,                   // status CPU Enabled (0x01)
    ProcessorUpgradeOther,
    SMBIOS_HANDLE_L1I_CACHE,            // l1 cache handle
    SMBIOS_HANDLE_L2_CACHE,             // l2 cache handle
    SMBIOS_HANDLE_L3_CACHE,                       // l3 cache handle
    0,                            // serial not set
    0,                            // asset not set
    4,                            // part number
    FixedPcdGet32 (PcdCoreCount), // core count in socket
    FixedPcdGet32 (PcdCoreCount), // enabled core count in socket
    0,                            // threads per socket
    0xEC,                         // processor characteristics
    ProcessorFamilyARMv9,         // ARM core
  },
  TYPE4_STRINGS
};

STATIC CONST VOID  *DefaultCommonTables[] = {
  &mPlatformDefaultType4, NULL
};

EFI_STATUS
AddSmbiosType4 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  EFI_SMBIOS_HANDLE  SmbiosHandle;

  for (int TableEntry = 0; DefaultCommonTables[TableEntry] != NULL; TableEntry++) {
    SmbiosHandle =
      ((EFI_SMBIOS_TABLE_HEADER *)DefaultCommonTables[TableEntry])->Handle;
    Status = Smbios->Add (
                       Smbios,
                       NULL,
                       &SmbiosHandle,
                       (EFI_SMBIOS_TABLE_HEADER *)DefaultCommonTables[TableEntry]
                       );
    if (EFI_ERROR (Status)) {
      DEBUG (
        (
        DEBUG_ERROR,
        "[%a]:[%dL] Smbios Type4 Table Log Failed! %r \n",
        __FUNCTION__,
        DEBUG_LINE_NUMBER,
        Status
        )
        );
      break;
    }
  }

  return Status;
}
