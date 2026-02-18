/** @DeviceInfo.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2011, 2014 - 2015, 2017, 2021 The Linux Foundation. All rights
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef _DEVINFO_H_
#define _DEVINFO_H_
#include <Uefi.h>

#define DEVICE_MAGIC             "ANDROID-BOOT!"
#define DEVICE_MAGIC_SIZE        13
#define MAX_VERSION_LEN          64
#define MAX_VB_PARTITIONS        32
#define MAX_USER_KEY_SIZE        2048
#define MAX_DISPLAY_CMDLINE_LEN  128

enum unlock_type {
  UNLOCK = 0,
  UNLOCK_CRITICAL,
};

typedef struct device_info {
  CHAR8      magic[DEVICE_MAGIC_SIZE];
  BOOLEAN    is_unlocked;
  BOOLEAN    is_unlock_critical;
  BOOLEAN    is_charger_screen_enabled;
  CHAR8      bootloader_version[MAX_VERSION_LEN];
  CHAR8      radio_version[MAX_VERSION_LEN];
  BOOLEAN    verity_mode; // TRUE = enforcing, FALSE = logging
  UINT32     user_public_key_length;
  CHAR8      user_public_key[MAX_USER_KEY_SIZE];
  UINT64     rollback_index[MAX_VB_PARTITIONS];
  CHAR8      Display_Cmdline[MAX_DISPLAY_CMDLINE_LEN];
} DeviceInfo;

BOOLEAN
IsUnlocked (
  VOID
  );

EFI_STATUS
UpdateDevUnlockState (
  BOOLEAN State
  );

VOID
GetBootloaderVersion (
  CHAR8   *BootloaderVersion,
  UINT32  Len
  );

EFI_STATUS
ReadRollbackIndex (
  UINT32  Loc,
  UINT64  *RollbackIndex
  );

EFI_STATUS
GetUserKey (
  CHAR8   **UserKey,
  UINT32  *UserKeySize
  );

#endif
