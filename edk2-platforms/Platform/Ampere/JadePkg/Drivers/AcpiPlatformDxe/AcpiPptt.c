/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <IndustryStandard/ArmCache.h>
#include <Library/ArmLib.h>
#include "AcpiPlatform.h"

EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR PPTTProcessorTemplate = {
  EFI_ACPI_6_3_PPTT_TYPE_PROCESSOR,
  sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR),
  { EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE },
  {0}, /* Flags */
  0,   /* Parent */
  0,   /* AcpiProcessorId */
  0    /* NumberOfPrivateResources */
};

EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE PPTTCacheTemplate = {
  EFI_ACPI_6_3_PPTT_TYPE_CACHE,
  sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE),
  { EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE },
  {0}, /* Flags */
  0,   /* NextLevelOfCache */
  0,   /* Size */
  0,   /* NumberOfSets */
  0,   /* Associativity */
  {0}, /* Attributes */
  0
};

EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER PPTTTableHeaderTemplate = {
  __ACPI_HEADER (
    EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_STRUCTURE_SIGNATURE,
    0, /* need fill in */
    EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_REVISION
    ),
};

STATIC EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER *PpttTablePointer;
STATIC UINT32                                                  PpttClusterOffset[PLATFORM_CPU_MAX_CPM * PLATFORM_CPU_MAX_SOCKET];
STATIC UINT32                                                  PpttSocketOffset[PLATFORM_CPU_MAX_SOCKET];
STATIC UINT32                                                  PpttRootOffset;
STATIC UINT32                                                  PpttL1DataCacheOffset[PLATFORM_CPU_MAX_NUM_CORES];
STATIC UINT32                                                  PpttL1InstructionCacheOffset[PLATFORM_CPU_MAX_NUM_CORES];
STATIC UINT32                                                  PpttL2CacheOffset[PLATFORM_CPU_MAX_NUM_CORES];

UINT32
AcpiPpttProcessorCoreNode (
  VOID   *EntryPointer,
  UINT32 CpuId
  )
{
  EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR *PpttProcessorEntryPointer = EntryPointer;
  UINT32                                *ResPointer;
  UINTN                                 ClusterIdPerSocket, CoreIdPerCpm, SocketId;

  CopyMem (
    PpttProcessorEntryPointer,
    &PPTTProcessorTemplate,
    sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR)
    );

  ClusterIdPerSocket = (CpuId / PLATFORM_CPU_NUM_CORES_PER_CPM) % PLATFORM_CPU_MAX_CPM;
  SocketId = (CpuId / PLATFORM_CPU_NUM_CORES_PER_CPM) / PLATFORM_CPU_MAX_CPM;
  CoreIdPerCpm = CpuId  % PLATFORM_CPU_NUM_CORES_PER_CPM;
  PpttProcessorEntryPointer->Flags.AcpiProcessorIdValid = 1;
  PpttProcessorEntryPointer->Flags.NodeIsALeaf = 1;
  PpttProcessorEntryPointer->Flags.IdenticalImplementation = 1;
  PpttProcessorEntryPointer->AcpiProcessorId = (SocketId << PLATFORM_SOCKET_UID_BIT_OFFSET) | (ClusterIdPerSocket << 8) | CoreIdPerCpm;
  PpttProcessorEntryPointer->Parent = (UINT32)PpttClusterOffset[CpuId / PLATFORM_CPU_NUM_CORES_PER_CPM];
  PpttProcessorEntryPointer->NumberOfPrivateResources = 2; /* L1I + L1D */

  ResPointer = (UINT32 *)((UINT64)EntryPointer +
                          sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR));
  ResPointer[0] = PpttL1InstructionCacheOffset[CpuId];
  ResPointer[1] = PpttL1DataCacheOffset[CpuId];

  PpttProcessorEntryPointer->Length = sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR) + 2 * sizeof (UINT32);

  return PpttProcessorEntryPointer->Length;
}

STATIC UINT32
AcpiPpttClusterNode (
  VOID   *EntryPointer,
  UINT32 ClusterId
  )
{
  EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR *PpttProcessorEntryPointer = EntryPointer;

  PpttClusterOffset[ClusterId] = (UINT64)EntryPointer - (UINT64)PpttTablePointer;

  CopyMem (
    PpttProcessorEntryPointer,
    &PPTTProcessorTemplate,
    sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR)
    );

  PpttProcessorEntryPointer->Parent = (UINT32)PpttSocketOffset[ClusterId / PLATFORM_CPU_MAX_CPM];
  PpttProcessorEntryPointer->Flags.IdenticalImplementation = 1;

  return PpttProcessorEntryPointer->Length;
}

STATIC UINT32
AcpiPpttSocketNode (
  VOID   *EntryPointer,
  UINT32 SocketId
  )
{
  EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR *PpttProcessorEntryPointer = EntryPointer;

  PpttSocketOffset[SocketId] = (UINT64)EntryPointer - (UINT64)PpttTablePointer;

  CopyMem (
    PpttProcessorEntryPointer,
    &PPTTProcessorTemplate,
    sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR)
    );

  PpttProcessorEntryPointer->Flags.PhysicalPackage = 1;
  PpttProcessorEntryPointer->Flags.IdenticalImplementation = 1;
  PpttProcessorEntryPointer->Parent = (UINT32)PpttRootOffset;

  return PpttProcessorEntryPointer->Length;
}

STATIC UINT32
AcpiPpttRootNode (
  VOID *EntryPointer
  )
{
  EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR *PpttProcessorEntryPointer = EntryPointer;

  PpttRootOffset = (UINT64)EntryPointer - (UINT64)PpttTablePointer;

  CopyMem (
    PpttProcessorEntryPointer,
    &PPTTProcessorTemplate,
    sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR)
    );

  PpttProcessorEntryPointer->Flags.IdenticalImplementation = 1;

  return PpttProcessorEntryPointer->Length;
}

STATIC VOID
AcpiPpttFillCacheSizeInfo (
  EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE *Node,
  UINT32                            Level,
  BOOLEAN                           DataCache,
  BOOLEAN                           UnifiedCache
  )
{
  CSSELR_DATA CsselrData;
  CCSIDR_DATA CcsidrData;

  CsselrData.Data = 0;
  CsselrData.Bits.Level = Level - 1;
  CsselrData.Bits.InD = (!DataCache && !UnifiedCache);

  CcsidrData.Data = ReadCCSIDR (CsselrData.Data);

  Node->Flags.LineSizeValid = 1;
  Node->Flags.NumberOfSetsValid = 1;
  Node->Flags.AssociativityValid = 1;
  Node->Flags.SizePropertyValid = 1;
  Node->Flags.CacheTypeValid = 1;
  Node->NumberOfSets = (UINT16)CcsidrData.BitsNonCcidx.NumSets + 1;
  Node->Associativity = (UINT16)CcsidrData.BitsNonCcidx.Associativity + 1;
  Node->LineSize = (UINT16)(1 << (CcsidrData.BitsNonCcidx.LineSize + 4));;
  Node->Size = Node->NumberOfSets *
               Node->Associativity *
               Node->LineSize;
}

STATIC UINT32
AcpiPpttL1DataCacheNode (
  VOID   *EntryPointer,
  UINT32 CpuId
  )
{
  EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE *PpttCacheEntryPointer = EntryPointer;

  PpttL1DataCacheOffset[CpuId] = (UINT64)EntryPointer - (UINT64)PpttTablePointer;
  CopyMem (
    PpttCacheEntryPointer,
    &PPTTCacheTemplate,
    sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE)
    );

  AcpiPpttFillCacheSizeInfo (PpttCacheEntryPointer, 1, TRUE, FALSE);
  PpttCacheEntryPointer->Attributes.CacheType = 0x0; /* Data Cache */
  PpttCacheEntryPointer->NextLevelOfCache = PpttL2CacheOffset[CpuId];

  return PpttCacheEntryPointer->Length;
}

STATIC UINT32
AcpiPpttL1InstructionCacheNode (
  VOID   *EntryPointer,
  UINT32 CpuId
  )
{
  EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE *PpttCacheEntryPointer = EntryPointer;

  PpttL1InstructionCacheOffset[CpuId] = (UINT64)EntryPointer - (UINT64)PpttTablePointer;
  CopyMem (
    PpttCacheEntryPointer,
    &PPTTCacheTemplate,
    sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE)
    );

  AcpiPpttFillCacheSizeInfo (PpttCacheEntryPointer, 1, FALSE, FALSE);
  PpttCacheEntryPointer->Attributes.CacheType = 0x1; /* Instruction Cache */
  PpttCacheEntryPointer->NextLevelOfCache = PpttL2CacheOffset[CpuId];

  return PpttCacheEntryPointer->Length;
}

STATIC UINT32
AcpiPpttL2CacheNode (
  VOID   *EntryPointer,
  UINT32 CpuId
  )
{
  EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE *PpttCacheEntryPointer = EntryPointer;

  PpttL2CacheOffset[CpuId] = (UINT64)EntryPointer - (UINT64)PpttTablePointer;
  CopyMem (
    PpttCacheEntryPointer,
    &PPTTCacheTemplate,
    sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE)
    );

  AcpiPpttFillCacheSizeInfo (PpttCacheEntryPointer, 2, FALSE, TRUE);
  PpttCacheEntryPointer->Attributes.CacheType = 0x3; /* Unified Cache */
  PpttCacheEntryPointer->NextLevelOfCache = 0;

  return PpttCacheEntryPointer->Length;
}

/*
 *  Install PPTT table.
 */
EFI_STATUS
AcpiInstallPpttTable (
  VOID
  )
{
  EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR *PpttProcessorEntryPointer = NULL;
  EFI_ACPI_TABLE_PROTOCOL               *AcpiTableProtocol;
  UINTN                                 PpttTableKey  = 0;
  INTN                                  Count;
  EFI_STATUS                            Status;
  UINTN                                 Size;

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID **)&AcpiTableProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Size = sizeof (EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER) +
          sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR) +                                                        /* Root node */
          (PLATFORM_CPU_MAX_SOCKET * sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR)) +                            /* Socket node */
          (PLATFORM_CPU_MAX_CPM * PLATFORM_CPU_MAX_SOCKET * sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR)) +     /* Cluster node */
          (PLATFORM_CPU_MAX_NUM_CORES * (sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR) + 2 * sizeof (UINT32))) + /* Core node */
          (PLATFORM_CPU_MAX_NUM_CORES * sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE)) +                             /* L1I node */
          (PLATFORM_CPU_MAX_NUM_CORES * sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE)) +                             /* L1D node */
          (PLATFORM_CPU_MAX_NUM_CORES * sizeof (EFI_ACPI_6_3_PPTT_STRUCTURE_CACHE));                              /* L2 node */

  PpttTablePointer =
    (EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER *)AllocateZeroPool (Size);
  if (PpttTablePointer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PpttProcessorEntryPointer =
    (EFI_ACPI_6_3_PPTT_STRUCTURE_PROCESSOR *)((UINT64)PpttTablePointer +
                                              sizeof (EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER));

  Size = 0;
  Size += AcpiPpttRootNode ((VOID *)((UINT64)PpttProcessorEntryPointer + Size));

  for (Count = 0; Count < PLATFORM_CPU_MAX_SOCKET; Count++) {
    Size += AcpiPpttSocketNode ((VOID *)((UINT64)PpttProcessorEntryPointer + Size), Count);
  }

  for (Count = 0; Count < PLATFORM_CPU_MAX_CPM * PLATFORM_CPU_MAX_SOCKET; Count++) {
    Size += AcpiPpttClusterNode ((VOID *)((UINT64)PpttProcessorEntryPointer + Size), Count);
  }

  for (Count = 0; Count < PLATFORM_CPU_MAX_NUM_CORES; Count++) {
    Size += AcpiPpttL2CacheNode ((VOID *)((UINT64)PpttProcessorEntryPointer + Size), Count);
    Size += AcpiPpttL1InstructionCacheNode ((VOID *)((UINT64)PpttProcessorEntryPointer + Size), Count);
    Size += AcpiPpttL1DataCacheNode ((VOID *)((UINT64)PpttProcessorEntryPointer + Size), Count);
    Size += AcpiPpttProcessorCoreNode ((VOID *)((UINT64)PpttProcessorEntryPointer + Size), Count);
  }

  CopyMem (
    PpttTablePointer,
    &PPTTTableHeaderTemplate,
    sizeof (EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER)
    );

  Size += sizeof (EFI_ACPI_6_3_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER);
  PpttTablePointer->Header.Length = Size;

  AcpiUpdateChecksum ((UINT8 *)PpttTablePointer, PpttTablePointer->Header.Length);

  Status = AcpiTableProtocol->InstallAcpiTable (
                                AcpiTableProtocol,
                                (VOID *)PpttTablePointer,
                                PpttTablePointer->Header.Length,
                                &PpttTableKey
                                );
  FreePool ((VOID *)PpttTablePointer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
