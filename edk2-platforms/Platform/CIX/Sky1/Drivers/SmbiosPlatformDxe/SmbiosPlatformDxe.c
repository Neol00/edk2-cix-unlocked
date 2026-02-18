/** @file
  This driver installs SMBIOS information for CIX SKY1

  platforms Note SMBIOS 2.7.1 Required structures:
    BIOS Information (Type 0)
    System Information (Type 1)
    Board Information (Type 2)
    System Enclosure (Type 3)
    Processor Information (Type 4) - CPU Driver
    Cache Information (Type 7) - For cache that is external to processor
    System Slots (Type 9) - If system has slots
    Physical Memory Array (Type 16)
    Memory Device (Type 17) - For each socketed system-memory Device
    Memory Array Mapped Address (Type 19) - One per contiguous block per
Physical Memroy Array System Boot Information (Type 32)
  Copyright 2023 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.
  Copyright (c) Microsoft Corporation. All rights reserved.
  Copyright (c) 2012, Apple Inc. All rights reserved.
  Copyright (c) 2015, ARM Limited. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD
License which accompanies this distribution.  The full text of the license may
be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <IndustryStandard/SmBios.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/Smbios.h>
#include <Protocol/SocInfoProtocol.h>

#define TYPE0_STRINGS                                                          \
  "Cix Technology Corp.\0" /* Vendor */                                        \
  "1.0\0"                  /* BiosVersion */                                   \
  __DATE__ "\0"            /* BiosReleaseDate */

#define TYPE1_STRINGS                                                   \
  "Cix Technology Corp.\0" /* Manufacturer */                                \
  "CIX Reference Board\0"  /* Product Name */                                \
  "1.0\0"                  /* Version */                                     \
  "System Serial Number\0" /* Serial number */                               \
  "Default\0"              /* SKUNumber */                                   \
  "Default\0"              /* System Family */

#define TYPE2_STRINGS                                                         \
  "Cix Technology Corp.\0"     /* Manufacturer */                                \
  "CIX Reference Board\0"      /* Product Name */                                \
  "1.0\0"                      /* Version */                                     \
  "Base Board Serial Number\0" /* Serial */                                      \
  "Base Board Asset Tag\0"     /* Assert Tag */                                  \
  "Part Component\0"           /* board location */

#define TYPE3_STRINGS                                                          \
  "Cix Technology Corp.\0"  /* Manufacturer */                                    \
  "1.0\0"                   /* Version */                                         \
  "Chassis Serial Number\0" /* Serial  */                                         \
  "Chassis Asset Tag\0"     /* Assert Tag */

#define TYPE4_STRINGS                                                          \
  "FCBGA\0"                /* socket type */                                        \
  "Cix Technology Corp.\0" /* manufacturer */                                       \
  "SKY1\0"                 /* processor description */                            \
  "Part Number\0"          /* part number */

#define TYPE7_STRINGS                                                          \
  "L1 Instruction\0" /* L1I  */                                                \
  "L1 Data\0"        /* L1D  */                                                \
  "L2\0"             /* L2   */

#define TYPE9_STRINGS  "PCIE_M2\0" /* Slot0 */

#define TYPE16_STRINGS  "\0" /* nothing */

#define TYPE17_STRINGS                                                         \
  "OS Memory\0" /* location */                                                 \
  "BANK 0\0"    /* bank description */

#define TYPE19_STRINGS  "\0" /* nothing */

#define TYPE32_STRINGS  "\0" /* nothing */

//
// Type definition and contents of the default SMBIOS table.
// This table covers only the minimum structures required by
// the SMBIOS specification (section 6.2, version 3.0)
//
#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE0    Base;
  INT8                  Strings[sizeof (TYPE0_STRINGS)];
} ARM_TYPE0;

typedef struct {
  SMBIOS_TABLE_TYPE1    Base;
  UINT8                 Strings[sizeof (TYPE1_STRINGS)];
} ARM_TYPE1;

typedef struct {
  SMBIOS_TABLE_TYPE2    Base;
  UINT8                 Strings[sizeof (TYPE2_STRINGS)];
} ARM_TYPE2;

typedef struct {
  SMBIOS_TABLE_TYPE3    Base;
  UINT8                 Strings[sizeof (TYPE3_STRINGS)];
} ARM_TYPE3;

typedef struct {
  SMBIOS_TABLE_TYPE4    Base;
  UINT8                 Strings[sizeof (TYPE4_STRINGS)];
} ARM_TYPE4;

typedef struct {
  SMBIOS_TABLE_TYPE7    Base;
  UINT8                 Strings[sizeof (TYPE7_STRINGS)];
} ARM_TYPE7;

typedef struct {
  SMBIOS_TABLE_TYPE9    Base;
  UINT8                 Strings[sizeof (TYPE9_STRINGS)];
} ARM_TYPE9;

typedef struct {
  SMBIOS_TABLE_TYPE16    Base;
  UINT8                  Strings[sizeof (TYPE16_STRINGS)];
} ARM_TYPE16;

typedef struct {
  SMBIOS_TABLE_TYPE17    Base;
  UINT8                  Strings[sizeof (TYPE17_STRINGS)];
} ARM_TYPE17;

typedef struct {
  SMBIOS_TABLE_TYPE19    Base;
  UINT8                  Strings[sizeof (TYPE19_STRINGS)];
} ARM_TYPE19;

typedef struct {
  SMBIOS_TABLE_TYPE32    Base;
  UINT8                  Strings[sizeof (TYPE32_STRINGS)];
} ARM_TYPE32;

// SMBIOS tables often reference each other using
// fixed constants, define a list of these constants
// for our hardcoded tables
enum SMBIOS_REFRENCE_HANDLES {
  SMBIOS_HANDLE_L1I = 0x1000,
  SMBIOS_HANDLE_L1D,
  SMBIOS_HANDLE_L2,
  SMBIOS_HANDLE_MOTHERBOARD,
  SMBIOS_HANDLE_CHASSIS,
  SMBIOS_HANDLE_CLUSTER,
  SMBIOS_HANDLE_MEMORY,
  SMBIOS_HANDLE_DRAM
};

#define SERIAL_LEN                                                             \
  10 // this must be less than the buffer len allocated in the type1 structure

#pragma pack()

// BIOS information (section 7.1)
STATIC ARM_TYPE0  mArmDefaultType0 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_BIOS_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE0),            // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    1,          // SMBIOS_TABLE_STRING       Vendor
    2,          // SMBIOS_TABLE_STRING       BiosVersion
    0xE800,     // UINT16                    BiosSegment
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
    0xFF,    // UINT8 EmbeddedControllerFirmwareMajorRelease
    0xFF     // UINT8                     EmbeddedControllerFirmwareMinorRelease
  },
  // Text strings (unformatted area)
  TYPE0_STRINGS
};

// System information (section 7.2)
STATIC CONST ARM_TYPE1  mArmDefaultType1 = {
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

// Baseboard (section 7.3)
STATIC ARM_TYPE2  mArmDefaultType2 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE2),                 // UINT8 Length
      SMBIOS_HANDLE_MOTHERBOARD,
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
    { SMBIOS_HANDLE_CLUSTER },   // ,SMBIOS_HANDLE_CLUSTER,SMBIOS_HANDLE_MEMORY},
  },
  TYPE2_STRINGS
};

// Enclosure
STATIC CONST ARM_TYPE3  mArmDefaultType3 = {
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

STATIC CONST ARM_TYPE4  mArmDefaultType4 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE4),                 // UINT8 Length
      SMBIOS_HANDLE_CLUSTER,
    },
    1,                               // socket type
    CentralProcessor,                // processor type CPU
    ProcessorFamilyIndicatorFamily2, // processor family, acquire from
                                     // field2
    2,                               // manufacturer
    {
      {
        0,
      },
      { 0. }
    },                      // processor id - see section 7.5.3.3
    3,                      // processor description
    { 0, 0, 0, 0, 0, 1 },   // voltage
    0,                      // external clock
    2800,                   // max speed
    2600,                   // current speed
    0x41,                   // status CPU Enabled (0x01)
    ProcessorUpgradeOther,
    SMBIOS_HANDLE_L1I,            // l1 cache handle
    SMBIOS_HANDLE_L2,             // l2 cache handle
    0xFFFF,                       // l3 cache handle
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

STATIC CONST ARM_TYPE7  mArmDefaultType7_l1i = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),             // UINT8 Length
      SMBIOS_HANDLE_L1I,
    },
    1,
    0x380,                      // L1 enabled, unknown WB
    64,                         // 32k i cache max
    64,                         // 32k installed
    { 0, 1 },                   // SRAM type
    { 0, 1 },                   // SRAM type
    0,                          // unkown speed
    CacheErrorParity,           // parity checking
    CacheTypeInstruction,       // instruction cache
    CacheAssociativity2Way,     // two way
  },
  TYPE7_STRINGS
};

STATIC CONST ARM_TYPE7  mArmDefaultType7_l1d = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),             // UINT8 Length
      SMBIOS_HANDLE_L1D,
    },
    2,
    0x180,                      // L1 enabled, WB
    64,                         // 32k d cache max
    64,                         // 32k installed
    { 0, 1 },                   // SRAM type
    { 0, 1 },                   // SRAM type
    0,                          // unkown speed
    CacheErrorSingleBit,        // ECC checking
    CacheTypeData,              // instruction cache
    CacheAssociativity4Way,     // four way associative
  },
  TYPE7_STRINGS
};

STATIC CONST ARM_TYPE7  mArmDefaultType7_l2 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_CACHE_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE7),             // UINT8 Length
      SMBIOS_HANDLE_L2,
    },
    3,
    0x181,                      // L2 enabled, WB
    512,                        // cache max
    512,                        // installed
    { 0, 1 },                   // SRAM type
    { 0, 1 },                   // SRAM type
    0,                          // unkown speed
    CacheErrorSingleBit,        // ECC checking
    CacheTypeUnified,           // instruction cache
    CacheAssociativity16Way,    // 16 way associative
  },
  TYPE7_STRINGS
};

// Slots
STATIC CONST ARM_TYPE9  mArmDefaultType9_0 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_SLOTS,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE9),        // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    1,                   // slot 0
    SlotTypeUnknown,     // Key E, PCIe x2, USB, I2C, SDIO, UART, PCM
    SlotDataBusWidthUnknown,
    SlotUsageUnknown,
    SlotLengthUnknown,
    0,
    {
      0,
      0,
      1,
    },     // Slot Characteristics 1 - 3.3v
    {
      0,
    },     // Slot Characteristics 2
    0,
    0,
    0,
  },
  TYPE9_STRINGS
};

// Memory array
STATIC CONST ARM_TYPE16  mArmDefaultType16 = {
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
    0x4000000,     // 16GB
    0xFFFE,        // No error information structure
    0x1,           // soldered memory
  },
  TYPE16_STRINGS
};

// Memory device
STATIC ARM_TYPE17  mArmDefaultType17 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_MEMORY_DEVICE,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE17),        // UINT8 Length
      SMBIOS_HANDLE_DRAM,
    },
    SMBIOS_HANDLE_MEMORY,                            // array to which this module belongs
    0xFFFE,                                          // no errors
    0xFFFF,                                          // unknown total width
    0xFFFF,                                          // unknown data width
    0x4000,                                          // 16GB
    0x05,                                            // Chip
    0,                                               // not part of a set
    1,                                               // Device locator
    2,                                               // bank 0
    MemoryTypeLpddr5,                                // LP DDR4
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, // unbuffered
    3200,                                            // 3200MT/S
    0,                                               // varies between diffrent production runs
    0,                                               // serial
    0,                                               // asset tag
    0,                                               // part number
    0,                                               // rank
  },
  TYPE17_STRINGS
};

// Memory array mapped address, this structure
// is overridden by InstallMemoryStructure
STATIC CONST ARM_TYPE19  mArmDefaultType19 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE19),                      // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    0xFFFFFFFF,     // invalid, look at extended addr field
    0xFFFFFFFF,
    SMBIOS_HANDLE_DRAM,     // handle
    1,
    0x080000000,     // starting addr of first 2GB
    0x100000000,     // ending addr of first 2GB
  },
  TYPE19_STRINGS
};

// System boot info
STATIC CONST ARM_TYPE32  mArmDefaultType32 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE32),                  // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
    },
    { 0, 0, 0, 0, 0, 0 },   // reserved
    BootInformationStatusNoError,
  },
  TYPE32_STRINGS
};

STATIC CONST VOID  *DefaultCommonTables[] = {
  &mArmDefaultType0,     &mArmDefaultType1,     &mArmDefaultType2,   &mArmDefaultType3,
  &mArmDefaultType7_l1i, &mArmDefaultType7_l1d,
  &mArmDefaultType7_l2,  &mArmDefaultType4,     &mArmDefaultType9_0,
  &mArmDefaultType16,    &mArmDefaultType17,
  //    &mArmDefaultType19, //memory range type 19 dynamically generated
  &mArmDefaultType32,    NULL
};

/**
   Installs a memory descriptor (type19) for the given address range

   @param  Smbios               SMBIOS protocol

**/
EFI_STATUS
InstallMemoryStructure (
  IN EFI_SMBIOS_PROTOCOL  *Smbios,
  IN UINT64               StartingAddress,
  IN UINT64               RegionLength
  )
{
  EFI_SMBIOS_HANDLE  SmbiosHandle;
  ARM_TYPE19         MemoryDescriptor;
  EFI_STATUS         Status = EFI_SUCCESS;

  CopyMem (&MemoryDescriptor, &mArmDefaultType19, sizeof (ARM_TYPE19));

  MemoryDescriptor.Base.ExtendedStartingAddress = StartingAddress;
  MemoryDescriptor.Base.ExtendedEndingAddress   = StartingAddress + RegionLength;
  SmbiosHandle                                  = MemoryDescriptor.Base.Hdr.Handle;

  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER *)&MemoryDescriptor
                     );
  return Status;
}

/**
   Update memory device (type17) table

**/
EFI_STATUS
UpdateSmbiosType17 (
  VOID
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  CIX_SOC_INFO_PROTOCOL  *pSocInfoProtocol;

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

  mArmDefaultType17.Base.Size = pSocInfoProtocol->MemInfo->TotalSize;
  return Status;
}

/**
   Install a whole table worth of structructures

   @parm
**/
EFI_STATUS
InstallStructures (
  IN EFI_SMBIOS_PROTOCOL  *Smbios,
  IN CONST VOID           *DefaultTables[]
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  EFI_SMBIOS_HANDLE  SmbiosHandle;

  int  TableEntry;

  for (TableEntry = 0; DefaultTables[TableEntry] != NULL; TableEntry++) {
    SmbiosHandle =
      ((EFI_SMBIOS_TABLE_HEADER *)DefaultTables[TableEntry])->Handle;
    Status = Smbios->Add (
                       Smbios,
                       NULL,
                       &SmbiosHandle,
                       (EFI_SMBIOS_TABLE_HEADER *)DefaultTables[TableEntry]
                       );
    if (EFI_ERROR (Status)) {
      break;
    }
  }

  return Status;
}

/**
   Install all structures from the DefaultTables structure

   @param  Smbios               SMBIOS protocol

**/
EFI_STATUS
InstallAllStructures (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  // Fixup some table values
  mArmDefaultType0.Base.SystemBiosMajorRelease =
    (PcdGet32 (PcdFirmwareRevision) >> 16) & 0xFF;
  mArmDefaultType0.Base.SystemBiosMinorRelease =
    PcdGet32 (PcdFirmwareRevision) & 0xFF;

  // Update memory device (type17) table
  Status = UpdateSmbiosType17 ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to update smbios type17!\n"));
  }

  //
  // Add all table entries
  //
  Status = InstallStructures (Smbios, DefaultCommonTables);
  ASSERT_EFI_ERROR (Status);

  // Generate memory descriptors for the two memory ranges we know about
  Status = InstallMemoryStructure (
             Smbios,
             PcdGet64 (PcdSystemMemoryBase),
             PcdGet64 (PcdSystemMemorySize)
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
   Installs SMBIOS information for ARM platforms

   @param ImageHandle     Module's image handle
   @param SystemTable     Pointer of EFI_SYSTEM_TABLE

   @retval EFI_SUCCESS    Smbios data successfully installed
   @retval Other          Smbios data was not installed

**/
EFI_STATUS
EFIAPI
SmbiosTablePublishEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS           Status;
  EFI_SMBIOS_PROTOCOL  *Smbios;

  //
  // Find the SMBIOS protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = InstallAllStructures (Smbios);

  return Status;
}
