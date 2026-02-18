/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  Copyright 2025 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"

#define TYPE7_STRINGS                                   \
  "L1 Instruction\0"            /* L1I */               \
  "L1 Data\0"                   /* L1D */               \
  "L2\0"                        /* L2  */               \
  "L3\0"                        /* L3  */

typedef enum {
  L1Instruction = 1,
  L1Data,
  L2,
  L3,
} TYPE7_STRING_ELEMENTS;

#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE7    Base;
  UINT8                 Strings[sizeof (TYPE7_STRINGS)];
} PLATFORM_SMBIOS_TYPE7;
#pragma pack()

// Enclosure
STATIC CONST PLATFORM_SMBIOS_TYPE7  mArmDefaultType7_l1i = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),             // UINT8 Length
      SMBIOS_HANDLE_L1I_CACHE,
    },
    L1Instruction,
    0x380,                      // L1 enabled, unknown WB
    64,                         // 64k i cache max
    64,                         // 64k installed
    { 0, 1 },                   // SRAM type
    { 0, 1 },                   // SRAM type
    0,                          // unkown speed
    CacheErrorParity,           // parity checking
    CacheTypeInstruction,       // instruction cache
    CacheAssociativity2Way,     // two way
    64,                         // 64k i cache max
    64,                         // 64k installed
  },
  TYPE7_STRINGS
};

STATIC CONST PLATFORM_SMBIOS_TYPE7  mArmDefaultType7_l1d = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),             // UINT8 Length
      SMBIOS_HANDLE_L1D_CACHE,
    },
    L1Data,
    0x180,                      // L1 enabled, WB
    64,                         // 64k d cache max
    64,                         // 64k installed
    { 0, 1 },                   // SRAM type
    { 0, 1 },                   // SRAM type
    0,                          // unkown speed
    CacheErrorSingleBit,        // ECC checking
    CacheTypeData,              // instruction cache
    CacheAssociativity4Way,     // four way associative
    64,                         // 64k d cache max
    64,                         // 64k installed
  },
  TYPE7_STRINGS
};

STATIC CONST PLATFORM_SMBIOS_TYPE7  mArmDefaultType7_l2 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),             // UINT8 Length
      SMBIOS_HANDLE_L2_CACHE,
    },
    L2,
    0x181,                      // L2 enabled, WB
    512,                        // cache max
    512,                        // installed
    { 0, 1 },                   // SRAM type
    { 0, 1 },                   // SRAM type
    0,                          // unkown speed
    CacheErrorSingleBit,        // ECC checking
    CacheTypeUnified,           // instruction cache
    CacheAssociativity16Way,    // 16 way associative
    512,                        // cache max
    512,                        // installed
  },
  TYPE7_STRINGS
};

STATIC CONST PLATFORM_SMBIOS_TYPE7  mArmDefaultType7_l3 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),             // UINT8 Length
      SMBIOS_HANDLE_L3_CACHE,
    },
    L3,
    0x182,                      // L3 enabled, WB
    12288,                      // cache max
    12288,                      // installed
    { 0, 1 },                   // SRAM type
    { 0, 1 },                   // SRAM type
    0,                          // unkown speed
    CacheErrorUnknown,          // ECC checking
    CacheTypeUnknown,           // instruction cache
    CacheAssociativityUnknown,  // unknown associative
    12288,                      // cache max
    12288,                      // installed
  },
  TYPE7_STRINGS
};

STATIC CONST VOID  *DefaultCommonTables[] = {
  &mArmDefaultType7_l1i, &mArmDefaultType7_l1d,
  &mArmDefaultType7_l2, &mArmDefaultType7_l3, NULL
};

EFI_STATUS
AddSmbiosType7 (
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
        "[%a]:[%dL] Smbios Type7 Table Log Failed! %r \n",
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
