/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __ACPI_COMMON_H__
#define __ACPI_COMMON_H__

#include <IndustryStandard/Acpi.h>

#define IC_DIST_VECTOR_BASE  0

// ACPI OSC Status bits
#define OSC_STS_BIT0_RES           (1U << 0)
#define OSC_STS_FAILURE            (1U << 1)
#define OSC_STS_UNRECOGNIZED_UUID  (1U << 2)
#define OSC_STS_UNRECOGNIZED_REV   (1U << 3)
#define OSC_STS_CAPABILITY_MASKED  (1U << 4)
#define OSC_STS_MASK               (OSC_STS_BIT0_RES          |    \
                                    OSC_STS_FAILURE           | \
                                    OSC_STS_UNRECOGNIZED_UUID | \
                                    OSC_STS_UNRECOGNIZED_REV  | \
                                    OSC_STS_CAPABILITY_MASKED)

// ACPI OSC for Platform-Wide Capability
#define OSC_CAP_CPPC_SUPPORT          (1U << 5)
#define OSC_CAP_CPPC2_SUPPORT         (1U << 6)
#define OSC_CAP_PLAT_COORDINATED_LPI  (1U << 7)
#define OSC_CAP_OS_INITIATED_LPI      (1U << 8)

// A macro to initialise the common header part of EFI ACPI tables as defined by
// EFI_ACPI_DESCRIPTION_HEADER structure.
#define ARM_ACPI_HEADER(Signature, Type, Revision)  {             \
    Signature,                  /* UINT32  Signature */       \
    sizeof (Type),              /* UINT32  Length */          \
    Revision,                   /* UINT8   Revision */        \
    0,                          /* UINT8   Checksum */        \
    EFI_ACPI_OEM_ID_PCI,        /* UINT8   OemId[6] */        \
    EFI_ACPI_OEM_TABLE_ID,      /* UINT64  OemTableId */      \
    EFI_ACPI_OEM_REVISION,      /* UINT32  OemRevision */     \
    EFI_ACPI_CREATOR_ID,        /* UINT32  CreatorId */       \
    EFI_ACPI_CREATOR_REVISION   /* UINT32  CreatorRevision */ \
  }

// EFI_ACPI_6_4_GIC_STRUCTURE
#define EFI_ACPI_6_4_GICC_STRUCTURE_INIT(GicId, AcpiCpuUid, Mpidr, Flags,      \
                                         PmuIrq, GicBase, GicVBase, GicHBase, GsivId, GicRBase, Efficiency,           \
                                         SpeOverflowInterrupt)                                                        \
  {                                                                            \
    EFI_ACPI_6_4_GIC,                     /* Type */                           \
    sizeof (EFI_ACPI_6_4_GIC_STRUCTURE),  /* Length */                         \
    EFI_ACPI_RESERVED_WORD,               /* Reserved */                       \
    GicId,                                /* CPUInterfaceNumber */             \
    AcpiCpuUid,                           /* AcpiProcessorUid */               \
    Flags,                                /* Flags */                          \
    0,                                    /* ParkingProtocolVersion */         \
    PmuIrq,                               /* PerformanceInterruptGsiv */       \
    0,                                    /* ParkedAddress */                  \
    GicBase,                              /* PhysicalBaseAddress */            \
    GicVBase,                             /* GICV */                           \
    GicHBase,                             /* GICH */                           \
    GsivId,                               /* VGICMaintenanceInterrupt */       \
    GicRBase,                             /* GICRBaseAddress */                \
    Mpidr,                                /* MPIDR */                          \
    Efficiency,                           /* ProcessorPowerEfficiencyClass */  \
    EFI_ACPI_RESERVED_BYTE,               /* Reserved2 */                      \
    SpeOverflowInterrupt                  /* SpeOverflowInterrupt */           \
  }

#define EFI_ACPI_6_0_GIC_REDISTRIBUTOR_INIT(RedisRegionAddr, RedisDiscLength) \
  {                                                                           \
    EFI_ACPI_6_0_GICR,                                                        \
    sizeof (EFI_ACPI_6_0_GICR_STRUCTURE),                                     \
    0,                                                                        \
    RedisRegionAddr,                                                          \
    RedisDiscLength                                                           \
  }
// EFI_ACPI_6_4_GIC_DISTRIBUTOR_STRUCTURE
#define EFI_ACPI_6_4_GIC_DISTRIBUTOR_INIT(GicDistHwId, GicDistBase,            \
                                          GicDistVector, GicVersion)                                                   \
  {                                                                            \
    EFI_ACPI_6_4_GICD,                    /* Type */                           \
    sizeof (EFI_ACPI_6_4_GIC_DISTRIBUTOR_STRUCTURE),                           \
    EFI_ACPI_RESERVED_WORD,               /* Reserved1 */                      \
    GicDistHwId,                          /* GicId */                          \
    GicDistBase,                          /* PhysicalBaseAddress */            \
    GicDistVector,                        /* SystemVectorBase */               \
    GicVersion,                           /* GicVersion */                     \
    {                                                                          \
      EFI_ACPI_RESERVED_BYTE,             /* Reserved2[0] */                   \
      EFI_ACPI_RESERVED_BYTE,             /* Reserved2[1] */                   \
      EFI_ACPI_RESERVED_BYTE              /* Reserved2[2] */                   \
    }                                                                          \
  }

// EFI_ACPI_6_4_GICR_STRUCTURE
#define EFI_ACPI_6_4_GIC_REDISTRIBUTOR_INIT(RedisRegionAddr, RedisDiscLength)  \
  {                                                                            \
    EFI_ACPI_6_4_GICR,                    /* Type */                           \
    sizeof (EFI_ACPI_6_4_GICR_STRUCTURE), /* Length */                         \
    EFI_ACPI_RESERVED_WORD,               /* Reserved */                       \
    RedisRegionAddr,                      /* DiscoveryRangeBaseAddress */      \
    RedisDiscLength                       /* DiscoveryRangeLength */           \
  }

// EFI_ACPI_6_4_GIC_ITS_STRUCTURE
#define EFI_ACPI_6_4_GIC_ITS_INIT(GicItsId, GicItsBase)                        \
  {                                                                            \
    EFI_ACPI_6_4_GIC_ITS,                 /* Type */                           \
    sizeof (EFI_ACPI_6_4_GIC_ITS_STRUCTURE),                                   \
    EFI_ACPI_RESERVED_WORD,               /* Reserved */                       \
    GicItsId,                             /* GicItsId */                       \
    GicItsBase,                           /* PhysicalBaseAddress */            \
    EFI_ACPI_RESERVED_DWORD               /* DiscoveryRangeLength */           \
  }

// EFI_ACPI_6_4_MEMORY_AFFINITY_STRUCTURE
#define EFI_ACPI_6_4_MEMORY_AFFINITY_STRUCTURE_INIT(                           \
                                                                               ProximityDomain, Base, Length, Flags)                                \
  {                                                                            \
    1, sizeof (EFI_ACPI_6_4_MEMORY_AFFINITY_STRUCTURE), ProximityDomain,       \
    EFI_ACPI_RESERVED_WORD, (Base) & 0xffffffff,                               \
    (Base) >> 32, (Length) & 0xffffffff,                                       \
    (Length) >> 32, EFI_ACPI_RESERVED_DWORD, Flags,                            \
    EFI_ACPI_RESERVED_QWORD                                                    \
  }

// EFI_ACPI_6_4_GICC_AFFINITY_STRUCTURE
#define EFI_ACPI_6_4_GICC_AFFINITY_STRUCTURE_INIT(                             \
                                                                               ProximityDomain, ACPIProcessorUID, Flags, ClockDomain)               \
  {                                                                            \
    3, sizeof (EFI_ACPI_6_4_GICC_AFFINITY_STRUCTURE), ProximityDomain,         \
    ACPIProcessorUID,  Flags,  ClockDomain                                     \
  }

/** Helper macro for CPPC _CPC object initialization. Use of this macro is
    restricted to ASL file and not to TDL file.

    @param [in] DesiredPerfReg      Fastchannel address for desired performance
                                    register.
    @param [in] PerfLimitedReg      Fastchannel address for performance limited
                                    register.
    @param [in] GranularityMHz      Granularity of the performance scale.
    @param [in] HighestPerf         Highest performance in linear scale.
    @param [in] NominalPerf         Nominal performance in linear scale.
    @param [in] LowestNonlinearPerf Lowest non-linear performnce in linear
                                    scale.
    @param [in] LowestPerf          Lowest performance in linear scale.
    @param [in] RefPerf             Reference performance in linear scale.
**/
#define CPPC_PACKAGE_INIT(DesiredPerfReg, PerfLimitedReg, GranularityMHz,      \
                          HighestPerf, NominalPerf, LowestNonlinearPerf, LowestPerf, RefPerf)          \
  {                                                                            \
    23,                                 /* NumEntries */                       \
    3,                                  /* Revision */                         \
    HighestPerf,                        /* Highest Performance */              \
    NominalPerf,                        /* Nominal Performance */              \
    LowestNonlinearPerf,                /* Lowest Nonlinear Performance */     \
    LowestPerf,                         /* Lowest Performance */               \
  /* Guaranteed Performance Register */                                      \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Desired Performance Register */                                         \
    ResourceTemplate () { Register (SystemMemory, 32, 0, DesiredPerfReg, 3) }, \
  /* Minimum Performance Register */                                         \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Maximum Performance Register */                                         \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Performance Reduction Tolerance Register */                             \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Time Window Register */                                                 \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Counter Wraparound Time */                                              \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Reference Performance Counter Register */                               \
    ResourceTemplate () { Register (FFixedHW, 64, 0, 1, 4) },                  \
  /* Delivered Performance Counter Register */                               \
    ResourceTemplate () { Register (FFixedHW, 64, 0, 0, 4) },                  \
  /* Performance Limited Register */                                         \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) }, \
  /* CPPC Enable Register */                                                 \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Autonomous Selection Enable Register */                                 \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Autonomous Activity Window Register */                                  \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
  /* Energy Performance Preference Register */                               \
    ResourceTemplate () { Register (SystemMemory, 0, 0, 0, 0) },               \
    RefPerf,                            /* Reference Performance */            \
    (LowestPerf * GranularityMHz),      /* Lowest Frequency */                 \
    (NominalPerf * GranularityMHz),     /* Nominal Frequency */                \
  }

#endif
