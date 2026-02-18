/** @file
  Public include file for the CPU Configuration Library

  @copyright
  Copyright 2006 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _CPU_CONFIG_LIB_H_
#define _CPU_CONFIG_LIB_H_

#include <Protocol/MpService.h>
#include <AcpiCpuData.h>
#include <CpuDataStruct.h>


// CPU C State Settings
#define C0_ENABLE                           0x00
#define C6_ENABLE                           0x03

//
// Structure for collected CPUID data.
//
typedef struct {
  EFI_CPUID_REGISTER         *CpuIdLeaf;
  UINTN                      NumberOfBasicCpuidLeafs;
  UINTN                      NumberOfExtendedCpuidLeafs;
  UINTN                      NumberOfCacheAndTlbCpuidLeafs;
  UINTN                      NumberOfDeterministicCacheParametersCpuidLeafs;
  UINTN                      NumberOfExtendedTopologyEnumerationLeafs;
} CPU_CPUID_DATA;

typedef struct {
  UINTN    Ratio;
  UINTN    Vid;
  UINTN    Power;
  UINTN    TransitionLatency;
  UINTN    BusMasterLatency;
} FVID_ENTRY;

typedef struct {
  VOID     *MicrocodeData;
  UINTN    MicrocodeSize;
  UINT32   ProcessorId;
} MICROCODE_INFO;

//
// Miscellaneous processor data
//
typedef struct {
  //
  // Local Apic Data
  //
  UINT32                     InitialApicID;  ///< Initial APIC ID
  UINT32                     ApicID;         ///< Current APIC ID
  EFI_PHYSICAL_ADDRESS       ApicBase;
  UINT32                     ApicVersion;
  //
  // Frequency data
  //
  UINTN                      IntendedFsbFrequency;
  UINTN                      ActualFsbFrequency;
  UINTN                      MaxCoreToBusRatio;
  UINTN                      MinCoreToBusRatio;
  UINTN                      MaxTurboRatio;
  UINTN                      PackageTdp;
  UINTN                      NumberOfPStates;
  FVID_ENTRY                 *FvidTable;
  UINTN                      GreaterNumberOfPStates;  // Greater Than 16 p-state support
  FVID_ENTRY                 *GreaterFvidTable;       // Greater Than 16 p-state support
  //
  // Other data
  //
  UINT32                     MicrocodeRevision;
  UINT64                     EnabledThreadCountMsr;
  MICROCODE_INFO             MicrocodeInfo;
  UINT64                     MiscEnablesMsr;
} CPU_MISC_DATA;

//
// Structure for all collected processor data
//
typedef struct {
  CPU_CPUID_DATA             CpuidData;
  EFI_CPU_PHYSICAL_LOCATION  ProcessorLocation;
  CPU_MISC_DATA              CpuMiscData;
  UINT8                      PackageIdBitOffset;
  BOOLEAN                    PackageBsp;
} CPU_COLLECTED_DATA;

//
// Definition of Processor Configuration Context Buffer
//
typedef struct {
  UINTN                    NumberOfProcessors;
  UINTN                    BspNumber;
  CPU_COLLECTED_DATA       *CollectedDataBuffer;
  CPU_REGISTER_TABLE       *PreSmmInitRegisterTable;
  CPU_REGISTER_TABLE       *RegisterTable;
  BOOLEAN                  RegisterTableSaved;
} CPU_CONFIG_CONTEXT_BUFFER;

//
// Structure conveying socket ID configuration information.
//
typedef struct {
  UINT32                    DefaultSocketId;
  UINT32                    NewSocketId;
} CPU_SOCKET_ID_INFO;

#endif
