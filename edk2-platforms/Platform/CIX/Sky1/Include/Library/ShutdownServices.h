/** @ShutdownServices.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2011-2012, ARM Limited. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __BDS_INTERNAL_H__
#define __BDS_INTERNAL_H__

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi.h>

// Reboot modes
typedef enum {
  NORMAL_MODE         = 0x0,
  RECOVERY_MODE       = 0x1,
  FASTBOOT_MODE       = 0x2,
  ALARM_BOOT          = 0x3,
  DM_VERITY_LOGGING   = 0x4,
  DM_VERITY_ENFORCING = 0x5,
  DM_VERITY_KEYSCLEAR = 0x6,
  OEM_RESET_MIN       = 0x20,
  OEM_RESET_MAX       = 0x3f,
  EMERGENCY_DLOAD     = 0xFF,
} RebootReasonType;

typedef struct {
  CHAR16  DataBuffer[12];
  UINT8   Bdata;
} __attribute ((__packed__)) ResetDataType;

// BdsHelper.c
EFI_STATUS
ShutdownUefiBootServices (
  VOID
  );

EFI_STATUS
PreparePlatformHardware (
  VOID
  );

VOID
RebootDevice (
  UINT8  RebootReason
  );

VOID
ShutdownDevice (
  VOID
  );

#endif
