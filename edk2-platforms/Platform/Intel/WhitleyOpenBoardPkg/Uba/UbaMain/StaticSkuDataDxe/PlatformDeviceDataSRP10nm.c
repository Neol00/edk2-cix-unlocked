/** @file

  @copyright
  Copyright 2017 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PlatPirqData.h>
#include <PlatDevData.h>
#include <IndustryStandard/LegacyBiosMpTable.h>

#ifndef V_INTEL_VID
#define V_INTEL_VID               0x8086
#endif // #ifndef V_INTEL_VID

//
// Describes Local APICs' connections.
//
STATIC DEVICE_DATA_HW_LOCAL_INT      DeviceDataHwLocalInt1[] = {
  {
    {{0},{{0xFF,0},{0xFF,0},{0xFF,0}}},
    0x00,
    0xff,
    0x00,
    EfiLegacyMpTableEntryLocalIntTypeExtInt,
    EfiLegacyMpTableEntryLocalIntFlagsPolaritySpec,
    EfiLegacyMpTableEntryLocalIntFlagsTriggerSpec
  },
  {
    {{0},{{0xFF,0},{0xFF,0},{0xFF,0}}},
    0x00,
    0xff,
    0x01,
    EfiLegacyMpTableEntryLocalIntTypeInt,
    EfiLegacyMpTableEntryLocalIntFlagsPolaritySpec,
    EfiLegacyMpTableEntryLocalIntFlagsTriggerSpec
  },
};

//
// Describes system's address space mapping, specific to the system.
//
STATIC DEVICE_DATA_HW_ADDR_SPACE_MAPPING DeviceDataHwAddrSpace1[] = {
  //
  // Legacy IO addresses.
  //
  { {0}, EfiLegacyMpTableEntryExtSysAddrSpaceMappingIo,       0x0000,     0x1000    },
};

//
// IRQ priority
//
STATIC EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY  IrqPriorityTable1[] = {
  {11, 0},
  {10, 0},
  {9,  0},
  {5,  0},
  {0,  0},
  {0,  0},
  {0,  0}
};

//
// Note : UpdateBusNumbers updates the bus numeber
//
STATIC EFI_LEGACY_PIRQ_TABLE  PirqTableHead1 [] = {
  {
    {
      EFI_PIRQ_TABLE_SIGNATURE,
      00,
      01,
      0000,
      00,
      00,
      0000,
      V_INTEL_VID,
      30,
      00000000,
      {00,
      00,
      00,
      00,
      00,
      00,
      00,
      00,
      00,
      00,
      00},
      00
    }
  }
};

//
// Instantiation of the system device data.
//
DEVICE_DATA           mDeviceDataPlatformSRP10nm = {
  DeviceDataHwLocalInt1,   sizeof (DeviceDataHwLocalInt1) / sizeof (DeviceDataHwLocalInt1[0]),
  DeviceDataHwAddrSpace1,  sizeof (DeviceDataHwAddrSpace1)/ sizeof (DeviceDataHwAddrSpace1[0])
};

//
// Instantiation of platform PIRQ data.
//
PLATFORM_PIRQ_DATA    mPlatformPirqDataPlatformSRP10nm = {
  IrqPriorityTable1,    sizeof(IrqPriorityTable1) / sizeof(IrqPriorityTable1[0]),
  PirqTableHead1,       sizeof(PirqTableHead1) / sizeof(PirqTableHead1[0])
};
