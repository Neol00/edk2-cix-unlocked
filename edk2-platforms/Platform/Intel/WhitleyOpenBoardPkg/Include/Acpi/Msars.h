/** @file

  @copyright
  Copyright 2016 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _MSARS_H_
#define _MSARS_H_

#include <Uefi/UefiBaseType.h>

#pragma pack(1)
typedef struct {
  UINT16 ProcessorDomainValid:1;
  UINT16 MemoryDomainValid:1;
  UINT16 ReservationHint:1;
  UINT16 Reserved_15_3:13;
} MSARS_FLAGS_BITS;

typedef union {
  UINT16 Value;
  MSARS_FLAGS_BITS Bits;
} MSARS_FLAGS;

typedef struct {
  UINT16 Type;
  UINT16 Reserved_2_4;
  UINT32 Length;
  MSARS_FLAGS Flags;
  UINT16 Reserved_10_12;
  UINT32 ProcessorProximityDomain;
  UINT32 MemoryProximityDomain;
  UINT32 Reserved_20_24;
  UINT64 AddrBase; //System Physical Address Range Base
  UINT64 AddrLength; //System Physical Address Range Length
} MEMORY_SUBSYSTEM_ADDRESS_RANGE_STRUCTURE;
#pragma pack()

// MSARS_INIT Macro
// Used to initialize MEMORY_SUBSYSTEM_ADDRESS_RANGE_STRUCTURE
#define MSARS_INIT() {                                   \
  (UINT16) MEMORY_SUBSYSTEM_ADDRESS_RANGE_STRUCTURE_TYPE,     \
  (UINT16) 0,       \
  (UINT32) sizeof(MEMORY_SUBSYSTEM_ADDRESS_RANGE_STRUCTURE),  \
  {(UINT16) 0},     \
  (UINT16) 0,   \
  (UINT32) 0,       \
  (UINT32) 0,       \
  (UINT32) 0,       \
  (UINT64) 0,       \
  (UINT64) 0,       \
  },

#endif /* _MSARS_H_ */
