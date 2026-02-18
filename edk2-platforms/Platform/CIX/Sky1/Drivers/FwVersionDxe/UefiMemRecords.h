/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

**/
#include <Uefi.h>


#define MEMORY_RECORD_STRUCT_SIGNATURE   0x55AA55AA

#pragma pack(push)
#pragma pack(1)

typedef struct {
  volatile UINT64    StartAddr;
  volatile UINT64    EndAddr;
} RECORD_ENTRY;

typedef struct {
  volatile UINT32          Signature;   // 0x55AA55AA
  volatile UINT32          EntryCount;
  RECORD_ENTRY    Entry[16];
} MEMORY_RECORD_STRUCT;

#pragma pack(pop)

