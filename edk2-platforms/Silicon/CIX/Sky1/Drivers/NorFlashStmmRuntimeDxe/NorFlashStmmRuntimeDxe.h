/** @file  NorFlashStmmRuntimeDxe.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022-2024, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __NOR_FLASH_STMM_RUNTIME_DXE_H__
#define __NOR_FLASH_STMM_RUNTIME_DXE_H__

#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/NorFlashPlatformLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/CixPostCodeLib.h>
#include "NorFlashLib.h"
#include "NorFlashBlockIoDxe.h"
#include "NorFlashDiskIoDxe.h"
#include "NorFlashFvb.h"

#pragma pack (1)
typedef struct {
  UINTN     DeviceBaseAddress;      // Start address of the Device Base Address
  UINT32    DeviceSize;             // Size of Device
  UINTN     RegionBaseAddress;      // Start address of one single region
  UINT32    RegionSize;             // Size of Region
  UINT32    BlockSize;              // Size of Block
} NOR_FLASH_REGION_DESCRIPTION;
#pragma pack ()

#endif /* __NOR_FLASH_STMM_RUNTIME_DXE_H__ */
