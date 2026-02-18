/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ACPI_TABLE_COMMON_DEFINE_H__
#define __ACPI_TABLE_COMMON_DEFINE_H__

#define GPIO_ACTIVE_HIGH 0
#define GPIO_ACTIVE_LOW  1

#define CPU_LPIB_STATE0_MIN_RES     0     // Min Residency (us)
#define CPU_LPIB_STATE0_WAKEUP_LAT  0     // Wakeup Latency (us)
#define CPU_LPIB_STATE1_MIN_RES     3000  // Min Residency (us)
#define CPU_LPIB_STATE1_WAKEUP_LAT  360   // Wakeup Latency (us)
#define CPU_LPIB_STATE2_MIN_RES     10000 // Min Residency (us)
#define CPU_LPIB_STATE2_WAKEUP_LAT  500   // Wakeup Latency (us)
#define CPU_LPIL_STATE0_MIN_RES     0     // Min Residency (us)
#define CPU_LPIL_STATE0_WAKEUP_LAT  0     // Wakeup Latency (us)
#define CPU_LPIL_STATE1_MIN_RES     3000  // Min Residency (us)
#define CPU_LPIL_STATE1_WAKEUP_LAT  360   // Wakeup Latency (us)
#define CPU_LPIL_STATE2_MIN_RES     10000 // Min Residency (us)
#define CPU_LPIL_STATE2_WAKEUP_LAT  500   // Wakeup Latency (us)

// DST
#define RDR_RES_BASE   0x83000000
#define RDR_RES_SIZE   0x400000
#define MNTNDUMP_ADDR  0x83de0000
#define MNTNDUMP_SIZE  0x20000

#define RAMOOPS_RES_BASE   0x83d00000
#define RAMOOPS_RES_SIZE   0x80000

// IORT
#define PCIE_SMMU_ENABLE        1
#define MM_HUB_SMMU_ENABLE      1
#define SUB_SYSTEM_SMMU_ENABLE  0

// MM Hub, the following defines are valid when MM_HUB_SMMU_ENABLE = 1.
#define DPU_SMMU_ENABLE  1
#define NPU_SMMU_ENABLE  1
#define ISP_SMMU_ENABLE  1
#define CSI_SMMU_ENABLE  1

// GTDT
#define GTDT_WATCHDOG_ENABLE  1

// SMMU identifier
#define PCIE_SMMU_ID        0
#define MMHUB_SMMU_ID       1
#define SUB_SYSTEM_SMMU_ID  2

// DEBUG
#define ASL_UART_DEBUG_ENABLE  1

// RMR
#define RMR_GOP_ENABLE  1

#define PMCG_ENABLE     0
#if PMCG_ENABLE
  #define PCIE_PMCG_ENABLE        PCIE_SMMU_ENABLE
  #define MM_HUB_PMCG_ENABLE      MM_HUB_SMMU_ENABLE
  #define SUB_SYSTEM_PMCG_ENABLE  SUB_SYSTEM_SMMU_ENABLE
#else
  #define PCIE_PMCG_ENABLE        0
  #define MM_HUB_PMCG_ENABLE      0
  #define SUB_SYSTEM_PMCG_ENABLE  0
#endif

#endif
