/** @UpdateCmdLine.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2009-2020, The Linux Foundation. All rights reserved.
 *  Copyright (c) 2009, Google Inc. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __UPDATECMDLINE_H__
#define __UPDATECMDLINE_H__

#include <Library/DebugLib.h>
#include <Library/DeviceInfo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi.h>

#define BOOT_BASE_BAND  " androidboot.baseband="
#define BATT_MIN_VOLT   3200

#define MAX_PATH_SIZE     72
#define SERIAL_NUM_SIZE   64
#define HW_REVISION_SIZE  64

typedef struct BootInfo BootInfo;

typedef struct UpdateCmdLineParamList {
  BOOLEAN        Recovery;
  BOOLEAN        MultiSlotBoot;
  BOOLEAN        AlarmBoot;
  UINT32         CmdLineLen;
  UINT32         HaveCmdLine;
  UINT32         HeaderVersion;
  CHAR8          *StrHardwareRev;
  CHAR8          *StrSerialNum;
  CHAR8          *SlotSuffixAscii;
  CONST CHAR8    *CmdLine;
  CHAR8          *BootDeviceRoot;
  CHAR8          *UsbSerialCmdLine;
  CHAR8          *AndroidSocType;
  CHAR8          *BtMacAddr;
  CHAR8          *AlarmBootCmdLine;
  CHAR8          *BootTime;
  CHAR8          *AndroidBootReason;
  CHAR8          *VerifiedBootState;
  CHAR8          *AndroidSlotSuffix;
  CHAR8          *FlashLock;
  CHAR8          *LcdDensity;
  CHAR8          *AndroidBootDtboIdx;
  CHAR8          *AndroidKeyStore;
  CHAR8          *AndroidBootForceNormalBoot;
  CHAR8          *AndroidBootConsole;
  CONST CHAR8    *VBCmdLine;
} UpdateCmdLineParamList;

EFI_STATUS
UpdateCmdLine (
  BootInfo     *Info,
  CONST CHAR8  *CmdLine,
  CHAR8        **FinalCmdLine
  );

BOOLEAN
TargetBatterySocOk (
  UINT32  *BatteryVoltage
  );

EFI_STATUS
TargetPauseForBatteryCharge (
  BOOLEAN  *BatteryStatus
  );

#endif
