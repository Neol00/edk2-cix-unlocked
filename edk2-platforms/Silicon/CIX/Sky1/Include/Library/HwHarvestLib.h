/*
 * Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HARVEST_H__
#define __HARVEST_H__

#include <Base.h>
#include <Uefi.h>

typedef enum {
  Core0,
  Core1,
  Core2,
  Core3,
  Core4,
  Core5,
  Core6,
  Core7,
  Core8,
  Core9,
  Core10,
  Core11,
  NpuCore0,
  NpuCore1_2,
  DpuCore0,
  DpuCore1,
  DpuCore2,
  DpuCore3,
  DpuCore4,
  PcieX8,
  Audio,
  Isp
} HARVEST_TYPE;


typedef union {
  UINT32    Data;
  struct {
    UINT32    Core0      : 1; // bit 0
    UINT32    Core1      : 1; // bit 1
    UINT32    Core2      : 1; // bit 2
    UINT32    Core3      : 1; // bit 3
    UINT32    Core4      : 1; // bit 4
    UINT32    Core5      : 1; // bit 5
    UINT32    Core6      : 1; // bit 6
    UINT32    Core7      : 1; // bit 7
    UINT32    Core8      : 1; // bit 8
    UINT32    Core9      : 1; // bit 9
    UINT32    Core10     : 1; // bit 10
    UINT32    Core11     : 1; // bit 11
    UINT32    BootCore   : 4; // bit 12-15
    UINT32    NpuCore1_2 : 1; // bit 16
    UINT32    NpuCore0   : 1; // bit 17
    UINT32    DpuCore0   : 1; // bit 18
    UINT32    DpuCore1   : 1; // bit 19
    UINT32    DpuCore2   : 1; // bit 20
    UINT32    DpuCore3   : 1; // bit 21
    UINT32    DpuCore4   : 1; // bit 22
    UINT32    PcieX8     : 1; // bit 23
    UINT32    Audio      : 1; // bit 24
    UINT32    Isp        : 1; // bit 25
    UINT32    Reserved   : 6; // bit 26-31
  } Bits;
} SOC_HW_HARVEST;


UINT32
IsIpHarvested (
  IN HARVEST_TYPE  IpType
  );

BOOLEAN
IsApWatchdogEnable (
  );
#endif /* __HARVEST_H__ */
