/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __ACPI_PLATFORM_H__
#define __ACPI_PLATFORM_H__

#include <Include/AcpiCommon.h>

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_OEM_TABLE_ID      SIGNATURE_64('S','K','Y','1','E','D','K','2')              // OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION      0x01000101
#define EFI_ACPI_CREATOR_ID        SIGNATURE_32('C','I','X',' ')
#define EFI_ACPI_CREATOR_REVISION  0x00000001
#define EFI_ACPI_OEM_ID_PCI        {'C','I','X','T','E','K'}              // OEMID: due to pci.sys driver

#define PLAT_CPU_COUNT     12
#define PLAT_CPU_L0_COUNT  4
#define PLAT_CPU_M0_COUNT  2
#define PLAT_CPU_M1_COUNT  2
#define PLAT_CPU_B0_COUNT  2
#define PLAT_CPU_B1_COUNT  2

// Gic
#define GIC_VERSION                   4
#define GICD_BASE                     0xE010000
#define GICI_BASE                     0xE050000
#define GICR_BASE                     0xE090000
#define GICR_LEN                      0x300000
#define GICC_BASE                     0xE010000
#define GICH_BASE                     0x0
#define GICV_BASE                     0x0
#define PLAT_GIC_REDISTRIBUTOR_COUNT  1
#define PLAT_GIC_ITS_COUNT            1

#define PLAT_GIC_VERSION  GIC_VERSION
#define PLAT_GICD_BASE    GICD_BASE
#define PLAT_GICI_BASE    GICI_BASE
#define PLAT_GICR_BASE    GICR_BASE
#define PLAT_GICR_LEN     GICR_LEN
#define PLAT_GICC_BASE    GICC_BASE
#define PLAT_GICH_BASE    GICH_BASE
#define PLAT_GICV_BASE    GICV_BASE

/* GIC CPU Interface information
   GIC_ENTRY (CPUInterfaceNumber, Mpidr, PmuIrq, VGicIrq, EnergyEfficiency)
 */
#define PLAT_GIC_CPU_INTERFACE  {                           \
             GICC_ENTRY (0,  GET_MPID (0, 0),  23, 0x19, 0), \
             GICC_ENTRY (1,  GET_MPID (1, 0),  23, 0x19, 0), \
             GICC_ENTRY (2,  GET_MPID (2, 0),  23, 0x19, 0), \
             GICC_ENTRY (3,  GET_MPID (3, 0),  23, 0x19, 0), \
             GICC_ENTRY (4,  GET_MPID (4, 0),  23, 0x19, 0), \
             GICC_ENTRY (5,  GET_MPID (5, 0),  23, 0x19, 0), \
             GICC_ENTRY (6,  GET_MPID (6, 0),  23, 0x19, 0), \
             GICC_ENTRY (7,  GET_MPID (7, 0),  23, 0x19, 0), \
             GICC_ENTRY (8,  GET_MPID (8, 0),  23, 0x19, 0), \
             GICC_ENTRY (9,  GET_MPID (9, 0),  23, 0x19, 0), \
             GICC_ENTRY (10,  GET_MPID (10, 0), 23, 0x19, 0), \
             GICC_ENTRY (11,  GET_MPID (11, 0), 23, 0x19, 0), \
}

#define PLAT_GIC_DISTRIBUTOR_INFO                                      \
  {                                                                    \
    PLAT_GICD_BASE,                  /* UINT64  PhysicalBaseAddress */ \
    0,                               /* UINT32  SystemVectorBase */    \
    PLAT_GIC_VERSION                 /* UINT8   GicVersion */          \
  }                                                                    \

#define PLAT_GIC_REDISTRIBUTOR_INFO                                    \
  {                                                                    \
    PLAT_GICR_BASE,      /* UINT64 DiscoveryRangeBaseAddress */        \
    PLAT_GICR_LEN        /* UINT32 DiscoveryRangeLength */             \
}                                                                      \

#define PLAT_GIC_ITS_INFO                                                    \
  {                                                                          \
    0,                   /* UINT32 GIC ITS ID */                             \
    PLAT_GICI_BASE,      /* UINT64 The 64-bit physical address for ITS */    \
    0                    /* UINT32 Populate the GIC ITS affinity in SRAT. */ \
  }                                                                          \

#define PLAT_LPI_MAP_INFO  {                \
  "LPIL",                                   \
  "LPIL",                                   \
  "LPIL",                                   \
  "LPIL",                                   \
  "LPIB",                                   \
  "LPIB",                                   \
  "LPIB",                                   \
  "LPIB",                                   \
  "LPIB",                                   \
  "LPIB",                                   \
  "LPIB",                                   \
  "LPIB",                                   \
}                                           \


#define CORE_0_TO_3_DESIRED_PERF_REG        0x06590094
#define CORE_0_TO_3_HIGHEST_PERF            4500
#define CORE_0_TO_3_NOMINAL_PERF            4500
#define CORE_0_TO_3_LOWEST_NON_LINEAR_PERF  800
#define CORE_0_TO_3_LOWEST_PERF             800

#define CORE_4_5_DESIRED_PERF_REG        0x065900A0
#define CORE_4_5_HIGHEST_PERF            4500
#define CORE_4_5_NOMINAL_PERF            4500
#define CORE_4_5_LOWEST_NON_LINEAR_PERF  800
#define CORE_4_5_LOWEST_PERF             800

#define CORE_6_7_DESIRED_PERF_REG        0x065900A4
#define CORE_6_7_HIGHEST_PERF            4500
#define CORE_6_7_NOMINAL_PERF            4500
#define CORE_6_7_LOWEST_NON_LINEAR_PERF  800
#define CORE_6_7_LOWEST_PERF             800

#define CORE_8_9_DESIRED_PERF_REG        0x06590098
#define CORE_8_9_HIGHEST_PERF            4500
#define CORE_8_9_NOMINAL_PERF            4500
#define CORE_8_9_LOWEST_NON_LINEAR_PERF  800
#define CORE_8_9_LOWEST_PERF             800

#define CORE_10_11_DESIRED_PERF_REG        0x0659009C
#define CORE_10_11_HIGHEST_PERF            4500
#define CORE_10_11_NOMINAL_PERF            4500
#define CORE_10_11_LOWEST_NON_LINEAR_PERF  800
#define CORE_10_11_LOWEST_PERF             800

#define REF_PERF  1000

#define CPC_GRANULARITYMHZ  1

#define PLAT_CPC_INFO  {\
            CPC_ENTRY(CORE_0_TO_3_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_0_TO_3_HIGHEST_PERF, CORE_0_TO_3_NOMINAL_PERF, CORE_0_TO_3_LOWEST_NON_LINEAR_PERF, CORE_0_TO_3_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_0_TO_3_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_0_TO_3_HIGHEST_PERF, CORE_0_TO_3_NOMINAL_PERF, CORE_0_TO_3_LOWEST_NON_LINEAR_PERF, CORE_0_TO_3_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_0_TO_3_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_0_TO_3_HIGHEST_PERF, CORE_0_TO_3_NOMINAL_PERF, CORE_0_TO_3_LOWEST_NON_LINEAR_PERF, CORE_0_TO_3_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_0_TO_3_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_0_TO_3_HIGHEST_PERF, CORE_0_TO_3_NOMINAL_PERF, CORE_0_TO_3_LOWEST_NON_LINEAR_PERF, CORE_0_TO_3_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_4_5_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_4_5_HIGHEST_PERF, CORE_4_5_NOMINAL_PERF, CORE_4_5_LOWEST_NON_LINEAR_PERF, CORE_4_5_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_4_5_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_4_5_HIGHEST_PERF, CORE_4_5_NOMINAL_PERF, CORE_4_5_LOWEST_NON_LINEAR_PERF, CORE_4_5_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_6_7_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_6_7_HIGHEST_PERF, CORE_6_7_NOMINAL_PERF, CORE_6_7_LOWEST_NON_LINEAR_PERF, CORE_6_7_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_6_7_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_6_7_HIGHEST_PERF, CORE_6_7_NOMINAL_PERF, CORE_6_7_LOWEST_NON_LINEAR_PERF, CORE_6_7_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_8_9_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_8_9_HIGHEST_PERF, CORE_8_9_NOMINAL_PERF, CORE_8_9_LOWEST_NON_LINEAR_PERF, CORE_8_9_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_8_9_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_8_9_HIGHEST_PERF, CORE_8_9_NOMINAL_PERF, CORE_8_9_LOWEST_NON_LINEAR_PERF, CORE_8_9_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_10_11_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_10_11_HIGHEST_PERF, CORE_10_11_NOMINAL_PERF, CORE_10_11_LOWEST_NON_LINEAR_PERF, CORE_10_11_LOWEST_PERF, REF_PERF),\
            CPC_ENTRY(CORE_10_11_DESIRED_PERF_REG, 0, CPC_GRANULARITYMHZ, CORE_10_11_HIGHEST_PERF, CORE_10_11_NOMINAL_PERF, CORE_10_11_LOWEST_NON_LINEAR_PERF, CORE_10_11_LOWEST_PERF, REF_PERF),\
}

#define PLAT_PSD_INFO  {\
            {5, 0, 2, 0xFD, 4},\
            {5, 0, 2, 0xFD, 4},\
            {5, 0, 2, 0xFD, 4},\
            {5, 0, 2, 0xFD, 4},\
            {5, 0, 5, 0xFD, 2},\
            {5, 0, 5, 0xFD, 2},\
            {5, 0, 6, 0xFD, 2},\
            {5, 0, 6, 0xFD, 2},\
            {5, 0, 3, 0xFD, 2},\
            {5, 0, 3, 0xFD, 2},\
            {5, 0, 4, 0xFD, 2},\
            {5, 0, 4, 0xFD, 2},\
}

// PCC for CPPC
#define PCC_CPPC_BASE              0x83BF1280
#define PCC_CPPC_BASE_LENGTH       0x1000
#define PCC_CPPC_DOORBELL_REG      0x06590090
#define PCC_CPPC_DOORBELL_ACK_REG  0x0

#define PCC_CPPC_CORE0_BASE   0x00
#define PCC_CPPC_CORE1_BASE   0x40
#define PCC_CPPC_CORE2_BASE   0x80
#define PCC_CPPC_CORE3_BASE   0xC0
#define PCC_CPPC_CORE4_BASE   0x100
#define PCC_CPPC_CORE5_BASE   0x140
#define PCC_CPPC_CORE6_BASE   0x180
#define PCC_CPPC_CORE7_BASE   0x1C0
#define PCC_CPPC_CORE8_BASE   0x200
#define PCC_CPPC_CORE9_BASE   0x240
#define PCC_CPPC_CORE10_BASE  0x280
#define PCC_CPPC_CORE11_BASE  0x2C0

#define PLAT_CPC_INFO_PCC  {                       \
            CPC_ENTRY_PCC(PCC_CPPC_CORE0_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE1_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE2_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE3_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE4_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE5_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE6_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE7_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE8_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE9_BASE),     \
            CPC_ENTRY_PCC(PCC_CPPC_CORE10_BASE),    \
            CPC_ENTRY_PCC(PCC_CPPC_CORE11_BASE),    \
}

typedef struct {
  UINT64    ReferencePerfCounter;
  UINT64    DeliveredPerfCounter;
  UINT32    DesiredPerfLevel;
  UINT32    HighestPerfLevel;
  UINT32    NominalPerfLevel;
  UINT32    LowestNonlinearPerfLevel;
  UINT32    LowestPerfLevel;
  UINT32    GuaranteedPerfLevel;
  UINT32    PerfLevelLimited;
  UINT32    ReferencePerf;
  UINT32    LowestFreq;
  UINT32    NominalFreq;
  UINT32    DeliveredPerfLevel;
  UINT32    Reserved[1];
} PCC_CORE_REGS;

#endif
