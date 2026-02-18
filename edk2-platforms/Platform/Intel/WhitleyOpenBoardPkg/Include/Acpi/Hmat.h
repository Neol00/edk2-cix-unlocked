/** @file

  @copyright
  Copyright 2016 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _HMAT_HMAT_H_
#define _HMAT_HMAT_H_

#include <IndustryStandard/Acpi.h>
#include <UncoreCommonIncludes.h>
#include "Platform.h"
#include "Msars.h"
#include "Sllbis.h"
#include "Mscis.h"

#define EFI_ACPI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_REVISION 0x01
#define EFI_ACPI_OEM_HMAT_REVISION  0x00000001

#define EFI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_SIGNATURE  SIGNATURE_32('H', 'M', 'A', 'T')
#define MEMORY_SUBSYSTEM_ADDRESS_RANGE_STRUCTURE_TYPE 0
#define SYSTEM_LOCALITY_LATENCY_BANDWIDTH_INFORMATION_STRUCTURE_TYPE 1
#define MEMORY_SIDE_CACHE_INFORMATION_STRUCTURE_TYPE 2

#define EFI_ACPI_HMAT_MSARS_COUNT  (MC_MAX_NODE * MAX_CRS_ENTRIES_PER_NODE)
#define EFI_ACPI_HMAT_LBIS_COUNT   (EFI_ACPI_HMAT_MAX_SLLBIS_DATA_TYPES * MAX_HMAT_MEMORY_HIERACHY_LEVELS)
#define EFI_ACPI_HMAT_MSCIS_COUNT  (MC_MAX_NODE * MAX_CRS_ENTRIES_PER_NODE)


#define HMAT_BW_BASE_UNIT      1024 // 1024 MB/S
#define HMAT_LATENCY_BASE_UNIT 1    // 1ns

#define HBM_4_NODES_CASE  4
#define HBM_2_NODES_CASE  2
#define HBM_1_NODE_CASE   1

//
// In HBM as Cache mode, Cache Flags are only for Last Level of Cache (Flags = 1), and 1st Level of Cache (Flags = 2).
// So Flag = 0 should be skipped.
//
#define HMAT_HBM_CACHE_FLAG_OFFSET 1

#pragma pack(1)
typedef enum {
  TypeFlatMemoryMode        = 1,
  TypeCacheMemoryMode       = 2,
  TypeHybridMemoryMode      = 3,
} LBIS_MEMORY_FLAGS_TYPE;

typedef struct {
  UINT8   Valid;
  UINT32  ElementId;
  UINT64  MemMapIndexMap;
  UINT8   Cacheable;
  UINT64  MemorySideCacheSize;
  UINT16  NumSmbiosHandles;
  UINT16  SmbiosHandles[MAX_TYPE17_CACHE_DEVICES];
  UINT8   PhysicalSocketId;
} MEMORY_DOMAIN_LIST_INFO;

typedef struct {
  UINT32                   ProcessorDomainNumber;
  UINT32                   MemoryDomainNumber;
  UINT16                   ProcessorDomainList [EFI_ACPI_HMAT_NUMBER_OF_PROCESSOR_DOMAINS];
  MEMORY_DOMAIN_LIST_INFO  MemoryDomainList [EFI_ACPI_HMAT_NUMBER_OF_MEMORY_DOMAINS];
  UINT8                    SncEnabled;
  UINT8                    SncNumOfCluster;
  UINT8                    VirtualNumaEnabled;
  UINT8                    VirtualNumOfCluster;
  UINT8                    ProcessorDomainSocketIdList [EFI_ACPI_HMAT_NUMBER_OF_PROCESSOR_DOMAINS];
} HMAT_PROXIMITY_DOMAIN_DATA_STRUCTURE;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT32 Reserved; // To make the structures 8 byte aligned
} EFI_ACPI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_HEADER;

typedef struct {
  EFI_ACPI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_HEADER HmatHeader;
  MEMORY_SUBSYSTEM_ADDRESS_RANGE_STRUCTURE Msars[EFI_ACPI_HMAT_MSARS_COUNT];
  LATENCY_BANDWIDTH_INFO_STRUCTURE Lbis[EFI_ACPI_HMAT_LBIS_COUNT];
  MEMORY_SIDE_CACHE_INFORMATION_STRUCTURE MemSideCache[EFI_ACPI_HMAT_MSCIS_COUNT];
} EFI_ACPI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE;
#pragma pack()

#endif /* _HMAT_H_ */
