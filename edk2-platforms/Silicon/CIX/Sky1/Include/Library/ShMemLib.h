/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SH_MEM_LIB_H__
#define __SH_MEM_LIB_H__

#include <Base.h>
#include <Uefi.h>

typedef enum {
  SMEM_FIRST = 0,
  SMEM_INFO_MEM,
  SMEM_INFO_SOC,
  SMEM_INFO_BOARD,

  SMEM_VER_SE,
  SMEM_VER_PM,
  SMEM_VER_PBL,
  SMEM_VER_ATF,
  SMEM_VER_UEFI,
  SMEM_VER_TEE,
  SMEM_VER_EC,
  SMEM_ADDR_UEFI_MEM_BOTTOM,
  SMEM_VER_PD,
  SMEM_FW_BOOT_PERF,

  SMEM_MAILBOX_AP_TO_ATF,//AP TO ATF

  SMEM_ERR_CRASH_LOG_SE,
  SMEM_ERR_CRASH_LOG_PM,
  SMEM_ERR_CRASH_LOG_PBL,
  SMEM_ERR_CRASH_LOG_ATF,
  SMEM_ERR_CRASH_LOG_UEFI,
  SMEM_ERR_CRASH_LOG_TEE,
  SMEM_DTS_PROPERTY
} SMEM_MEM_TYPE;

typedef struct _SHARED_MEM_INFO {
  SMEM_MEM_TYPE    SmemType;
  VOID             *SmemAddr;
  UINT32           SmemSize;
} SHARED_MEM_INFO;

void *
SmemGetAddr (
  SMEM_MEM_TYPE  SmemType,
  UINT32         *SmemSize
  );

#endif // __SH_MEM_LIB_H__
