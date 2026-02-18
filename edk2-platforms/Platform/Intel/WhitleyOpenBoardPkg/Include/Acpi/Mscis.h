/** @file

  @copyright
  Copyright 2016 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _MSCIS_H_
#define _MSCIS_H_

#include <Uefi/UefiBaseType.h>

#define EFI_ACPI_HMAT_CACHE_LEVEL_NONE  0
#define EFI_ACPI_HMAT_ONE_LEVEL_CACHE   1
#define EFI_ACPI_HMAT_TWO_LEVEL_CACHE   2
#define EFI_ACPI_HMAT_THREE_LEVEL_CACHE 3

#define EFI_ACPI_HMAT_CACHE_ASSOCIATIVITY_NONE                    0
#define EFI_ACPI_HMAT_CACHE_ASSOCIATIVITY_DIRECT_MAPPED           1
#define EFI_ACPI_HMAT_CACHE_ASSOCIATIVITY_COIMPLEX_CACHE_INDEXING 2

#define EFI_ACPI_HMAT_WRITE_POLICY_NONE 0
#define EFI_ACPI_HMAT_WRITE_POLICY_WB   1
#define EFI_ACPI_HMAT_WRITE_POLICY_WT   2

#define HBM_CACHE_LINE_SIZE       72
#define DDR4_CACHE_LINE_SIZE      64

//
// MAX_CH    = MAX_IMC * MAX_MC_CH
// MAX_IMC   = Maximum memory controllers per socket
// MAX_MC_CH = Max number of channels per MC
// For each channel, 1 DDR can act as 2LM cache. This is the maximum number of cache devices per memory domain
//
#define MAX_TYPE17_CACHE_DEVICES  MAX_CH

#pragma pack(1)
typedef struct {
  UINT32 TotalCacheLevels:4;
  UINT32 CacheLevel:4;
  UINT32 CacheAssociativity:4;
  UINT32 WritePolicy:4;
  UINT32 CacheLineSize:16;
} CACHE_ATTRIBUTES_BITS;

typedef union {
  UINT32 Data;
  CACHE_ATTRIBUTES_BITS Bits;
} CACHE_ATTRIBUTES;

typedef struct {
  UINT16 Type;
  UINT16 Reserved_2_4;
  UINT32 Length;
  UINT32 MemoryProximityDomain;
  UINT32 Reserved_12_16;
  UINT64 MemorySideCacheSize;
  CACHE_ATTRIBUTES CacheAttributes;
  UINT16 Reserved_28_30;
  UINT16 NumSmbiosHandles;
  UINT16 SmbiosHandles [MAX_TYPE17_CACHE_DEVICES];
} MEMORY_SIDE_CACHE_INFORMATION_STRUCTURE;
#pragma pack()

// MSCIS_INIT Macro
// Used to initialize MEMORY_SIDE_CACHE_INFORMATION_STRUCTURE
#define MSCIS_INIT() {                                       \
  (UINT16) MEMORY_SIDE_CACHE_INFORMATION_STRUCTURE_TYPE,     \
  (UINT16) 0,   \
  (UINT32) sizeof(MEMORY_SIDE_CACHE_INFORMATION_STRUCTURE),  \
  (UINT32) 0,   \
  (UINT32) 0,   \
  (UINT64) 0,   \
  {(UINT32) 0},   \
  (UINT16) 0,   \
  (UINT16) 0,   \
  },
/*
 SMBIOS handles not used for 2LM.  Will be added for HBM
  {(UINT16) 0}, \
  },
*/
#endif /* _MSCIS_H_ */
