/** @BootStats.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __BOOT_STATS_H
#define __BOOT_STATS_H

#define REG32(addr)        ((volatile UINT32 *)(addr))
#define READL(addr)        (*REG32 (addr))
#define WRITEL(addr, val)  (*REG32 (addr) = (val))

typedef enum {
  BS_BL_START = 0,
  BS_KERNEL_ENTRY,
  BS_KERNEL_RESERVED,
  BS_KERNEL_LOAD_TIME,
  BS_KERNEL_LOAD_START,
  BS_KERNEL_LOAD_DONE,
  BS_MAX,
} BS_ENTRY;

void
BootStatsSetTimeStamp (
  BS_ENTRY  BootStatId
  );

#endif
