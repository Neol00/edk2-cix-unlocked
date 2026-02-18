/** @UpdateDeviceTree.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2015-2017, 2020, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __UPDATEDEVICETREE_H__
#define __UPDATEDEVICETREE_H__

#include "libfdt.h"
#include <Library/DebugLib.h>
#include <Library/Partition.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Uefi.h>

#define DTB_OFFSET_LOCATION_IN_ARCH32_KERNEL_HDR  0x2C

/* Return True if integer overflow will occur */
#define CHECK_ADD64(a, b)  ((MAX_UINT64 - b < a) ? TRUE : FALSE)

EFI_STATUS
UpdateDeviceTree (
  VOID         *DeviceTreeLoadAddr,
  CONST CHAR8  *CmdLine,
  VOID         *RamDiskLoadAddr,
  UINT32       RamDiskSize,
  BOOLEAN      BootingWith32BitKernel
  );

#endif
