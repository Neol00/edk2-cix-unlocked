#/** @file
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#  @par Glossary:
#    - Cm or CM   - Configuration Manager
#    - Obj or OBJ - Object
#**/

#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

  #include <AcpiPlatform.h>
  #include <AcpiNameSpaceObjects.h>

/** The configuration manager version
*/
#define CONFIGURATION_MANAGER_REVISION  CREATE_REVISION (0, 0)

/** The OEM ID
*/
#define CFG_MGR_OEM_ID  EFI_ACPI_OEM_ID_PCI

/** A helper macro for populating the GIC CPU information
 */
#define GICC_ENTRY(                                                   \
                                                                      CPUInterfaceNumber,                                               \
                                                                      Mpidr,                                                            \
                                                                      PmuIrq,                                                           \
                                                                      VGicIrq,                                                          \
                                                                      EnergyEfficiency                                                  \
                                                                      )  {\
  CPUInterfaceNumber,       /* UINT32  CPUInterfaceNumber         */  \
  CPUInterfaceNumber,       /* UINT32  AcpiProcessorUid           */  \
  EFI_ACPI_6_2_GIC_ENABLED, /* UINT32  Flags                      */  \
  0,                        /* UINT32  ParkingProtocolVersion     */  \
  PmuIrq,                   /* UINT32  PerformanceInterruptGsiv   */  \
  0,                        /* UINT64  ParkedAddress              */  \
  GICC_BASE,                /* UINT64  PhysicalBaseAddress        */  \
  GICV_BASE,                /* UINT64  GICV                       */  \
  GICH_BASE,                /* UINT64  GICH                       */  \
  VGicIrq,                  /* UINT32  VGICMaintenanceInterrupt   */  \
  0,                        /* UINT64  GICRBaseAddress            */  \
  Mpidr,                    /* UINT64  MPIDR                      */  \
  EnergyEfficiency          /* UINT8   ProcessorPowerEfficiency   */  \
}

///
/// ACPI 6.4 Generic Address Space definition
///
// typedef struct {
//   UINT8     AddressSpaceId;
//   UINT8     RegisterBitWidth;
//   UINT8     RegisterBitOffset;
//   UINT8     AccessSize;
//   UINT64    Address;
// } EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE;

#define CPC_NULL_BUFFER  {EFI_ACPI_6_4_SYSTEM_MEMORY,0,0,0,0}

/** A helper macro for populating the CPU _CPC information
 */
#define CPC_ENTRY(                                                   \
                                                                     DesiredPerfReg,                                               \
                                                                     PerfLimitedReg,                                               \
                                                                     GranularityMHz,                                               \
                                                                     HighestPerf,                                                  \
                                                                     NominalPerf,                                                  \
                                                                     LowestNonlinearPerf,                                          \
                                                                     LowestPerf,                                                   \
                                                                     RefPerf                                                       \
                                                                     )  { \
  3,                                                  /* UINT32                                   Revision                                  */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   HighestPerformanceBuffer                  */  \
  HighestPerf,                                        /* UINT32                                   HighestPerformanceInteger                 */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   NominalPerformanceBuffer                  */  \
  NominalPerf,                                        /* UINT32                                   NominalPerformanceInteger                 */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   LowestNonlinearPerformanceBuffer          */  \
  LowestNonlinearPerf,                                /* UINT32                                   LowestNonlinearPerformanceInteger         */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   LowestPerformanceBuffer                   */  \
  LowestPerf,                                         /* UINT32                                   LowestPerformanceInteger                  */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   GuaranteedPerformanceRegister             */  \
  {EFI_ACPI_6_4_SYSTEM_MEMORY,32,0,3,DesiredPerfReg}, /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   DesiredPerformanceRegister                */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   MinimumPerformanceRegister                */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   MaximumPerformanceRegister                */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   PerformanceReductionToleranceRegister     */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   TimeWindowRegister                        */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   CounterWraparoundTimeBuffer               */  \
  0,                                                  /* UINT32                                   CounterWraparoundTimeInteger              */  \
  {EFI_ACPI_6_4_FUNCTIONAL_FIXED_HARDWARE,64,0,4,1},  /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   ReferencePerformanceCounterRegister       */  \
  {EFI_ACPI_6_4_FUNCTIONAL_FIXED_HARDWARE,64,0,4,0},  /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   DeliveredPerformanceCounterRegister       */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   PerformanceLimitedRegister                */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   CPPCEnableRegister                        */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   AutonomousSelectionEnableBuffer           */  \
  0,                                                  /* UINT32                                   AutonomousSelectionEnableInteger          */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   AutonomousActivityWindowRegister          */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   EnergyPerformancePreferenceRegister       */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   ReferencePerformanceBuffer                */  \
  RefPerf,                                            /* UINT32                                   ReferencePerformanceInteger               */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   LowestFrequencyBuffer                     */  \
  LowestPerf * GranularityMHz,                        /* UINT32                                   LowestFrequencyInteger                    */  \
  CPC_NULL_BUFFER,                                    /* EFI_ACPI_6_4_GENERIC_ADDRESS_STRUCTURE   NominalFrequencyBuffer                    */  \
  NominalPerf * GranularityMHz                        /* UINT32                                   NominalFrequencyInteger                   */  \
}

/** The number of ACPI tables to install
*/
#define CM_MANDATORY_ACPI_TABLES  2
#define OEM_ACPI_TABLES           0
#define PLAT_ACPI_TABLE_COUNT     (CM_MANDATORY_ACPI_TABLES + OEM_ACPI_TABLES)

/** A structure describing the platform configuration
    manager repository information
*/
typedef struct PlatformRepositoryInfo {
  /// Configuration Manager Information
  CM_STD_OBJ_CONFIGURATION_MANAGER_INFO    CmInfo;

  /// List of ACPI tables
  CM_STD_OBJ_ACPI_TABLE_INFO               CmAcpiTableList[PLAT_ACPI_TABLE_COUNT];

  /// GIC CPU interface information
  CM_ARM_GICC_INFO                         *GicCInfo;

  /// GIC distributor information
  CM_ARM_GICD_INFO                         GicDInfo;

  /// GIC Redistributor information
  CM_ARM_GIC_REDIST_INFO                   GicRedistInfo;

  /// GIC ITS information
  CM_ARM_GIC_ITS_INFO                      GicItsInfo;

  /// CPU _CPC information
  CM_ARM_CPC_INFO                          CpuCpcInfo[PLAT_CPU_COUNT];

  /// Mapping table of CPU UID to core number
  UINTN                                    CpuUidtoCoreNumberMap[PLAT_CPU_COUNT];
} SKY1_PLATFORM_REPOSITORY_INFO;

#endif // CONFIGURATION_MANAGER_H

#define NUMBER_TO_BITMASK(n)  ((1U << (n)) - 1)
#define ROUND_DIVISION(a, b)  (((a) / (b)) + ((((a) % (b)) * 2 >= (b)) ? 1 : 0))
