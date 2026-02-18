/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 1996 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"
#include <Library/CpuConfigLib.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiPlatformProtocol.h>
#include <Library/MemTypeLib.h>

extern BOOLEAN                      mX2ApicEnabled;
extern struct SystemMemoryMapHob    *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL         *mIioUds;
extern SOCKET_MEMORY_CONFIGURATION  mSocketMemoryConfiguration;
extern UINT32                       mNumOfBitShift;
extern UINT32                       mApicIdMap[MAX_SOCKET][MAX_CORE * MAX_THREAD];
extern UINTN                        mNumberOfCPUs;
extern CPU_ID_ORDER_MAP             mCpuApicIdOrderTable[];
extern EFI_ACPI_TABLE_PROTOCOL      *mAcpiTable;
extern CPU_CSR_ACCESS_VAR           *mCpuCsrAccessVarPtr;

UINT32 mPackageChaTotal[MAX_SOCKET];

CPU_LOGICAL_THREAD_ID_TABLE mCpuThreadIdMsrTable[MAX_CPU_NUM];

static ACPI_PLATFORM_PROTOCOL   mAcpiPlatformProtocol;

struct _ACPI_PLATFORM_UTILS_MEM_AFF_DATA {
  EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE AcpiMemAffData[MEMORY_AFFINITY_STRUCTURE_COUNT];
  BOOLEAN                                AcpiMemAffDataValid;
} mAcpiPlatformMemAffData;


VOID
CollectThreadIdMsrData (
  IN UINT8        SncEnabled,
  IN UINT8        SncNumOfCluster
  );

UINT32
GetProximityDomainForSNC (
  UINT32 ApicId
  );


/**
  This function counts the number of bits set in a 32-bit unsigned integer.

  @param[in] Value     The 32-bit unsigned integer to count.

  @retval The number of set bits.
**/
UINT8
BitCount32 (
  IN UINT32   Value
  )
{
  UINT8    Count;

  Count = 0;
  while (Value != 0) {
    Value &= Value - 1;
    Count++;
  }

  return Count;
}

/**
  Get the socket logical index.

  This function convert the socket physical index to logical index (0 based).
  If the specified physical socket is not enabled, an invalid logical index 0xff
  will be returned. The socket physical index and logical index will be the same
  if the indexes of enabled sockets are continuous.

  @param[in] SocketPhysicalId    Socket physical index.

  @retval Socket logical index.
**/
UINT8
GetSocketLogicalId (
  IN UINT8  SocketPhysicalId
  )
{
  UINT32    SocketBitMap;

  if ((mCpuCsrAccessVarPtr->socketPresentBitMap & (BIT0 << SocketPhysicalId)) == 0) {
    return (UINT8) -1;
  }

  SocketBitMap = mCpuCsrAccessVarPtr->socketPresentBitMap & ((BIT0 << SocketPhysicalId) - 1);
  return BitCount32 (SocketBitMap);
}

/**
  Get the socket physical index.

  This function convert the socket logical index to physical index (0 based).
  If the specified logical socket does not exist, an invalid physical index 0xff
  will be returned. The socket physical index and logical index will be the same
  if the indexes of enabled sockets are continuous.

  @param[in] SocketLogicalId    Socket logical index.

  @retval Socket physical index.
**/
UINT8
GetSocketPhysicalId (
  IN UINT8  SocketlogicId
  )
{
  UINT32    SocketBitMap;

  SocketBitMap = mCpuCsrAccessVarPtr->socketPresentBitMap;

  while (SocketlogicId != 0) {
    SocketBitMap &= SocketBitMap - 1;
    SocketlogicId--;
  }

  if (SocketBitMap == 0) {
    return (UINT8) -1;
  }

  return BitCount32 (SocketBitMap ^ (SocketBitMap - 1)) - 1;
}

/**

    Update the SRAT APIC IDs.

    @param *SRAAcpiTable   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchSratAllApicIds(
   IN OUT   STATIC_RESOURCE_AFFINITY_TABLE  *SRAAcpiTable
   )
{
  UINT16                      ThreadIndex;   // Support more than 256 threads (8S case)
  UINT8                       *ApicTablePtr;
  UINT8                       socket;
  UINT8                       Index;

  ThreadIndex = 0;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    ApicTablePtr = (UINT8*)mApicIdMap[socket];
    //
    // Even IDs must be list first
    //
    for (Index = 0; Index < MAX_CORE * MAX_THREAD; Index += 2) {
      SRAAcpiTable->Apic[ThreadIndex].ApicId = (UINT8)ApicTablePtr[Index] + (socket << mNumOfBitShift);
      SRAAcpiTable->Apic[ThreadIndex].ProximityDomain7To0 = socket; //as ProxDomain are all 0, or we can come up some algorithm if there is any dependency
      if ((UINT8)ApicTablePtr[Index] != 0xff) {
        SRAAcpiTable->Apic[ThreadIndex].Flags = EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED;
      }
      ThreadIndex++;
    }  //end of for coreThreadIndex
  } //end for socket

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // for odd APICID and must be after all even APICIDs
    //
    ApicTablePtr = (UINT8*)mApicIdMap[socket];
    for (Index = 1; Index < MAX_CORE * MAX_THREAD; Index += 2) {
      SRAAcpiTable->Apic[ThreadIndex].ApicId = (UINT8)ApicTablePtr[Index] + (socket << mNumOfBitShift);
      SRAAcpiTable->Apic[ThreadIndex].ProximityDomain7To0 = socket; //as ProxDomain are all 0, or we can come up some algorithm if there is any dependency
      if ((UINT8)ApicTablePtr[Index] != 0xff) {
        SRAAcpiTable->Apic[ThreadIndex].Flags = EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED;
      }
      ThreadIndex++;
    }
  }  //end of for coreThreadIndex
  ASSERT (ThreadIndex == MAX_CPU_NUM);
  return EFI_SUCCESS;
}

UINT32
ProximityDomainOf (
  UINT8   SocketId,
  UINT16  MemType,
  UINT8   MaxEnabledImc,
  UINT8   SncEnabled,
  UINT8   SncNumOfCluster,
  UINT8   ImcInterBitmap,
  UINT8   MemMode,
  UINT32  LastDomainId
  )
{
  UINT32  DomainId = (UINT16)~0;
  INTN    FirstImc;
  UINT8   ImcPerCluster;
  UINT8   NumSockets = mIioUds->IioUdsPtr->SystemStatus.numCpus;
  UINT8   SocketLogicalId;

  EFI_STATUS                      Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL2     *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
  }

  if (SncEnabled == 0) {
    SncNumOfCluster = 1;
  }

  if (!DynamicSiLibraryProtocol2->IsMemTypeVolatile (MemType)) {
    //
    // Persistent memory nodes and FPGA nodes are just pending behinds volatile memories
    //
    DomainId = LastDomainId + 1;
  } else {
    SocketLogicalId = GetSocketLogicalId (SocketId);
    //
    // Process volatile memory nodes
    //
    if (SncEnabled == 0) {
      DomainId = SocketLogicalId;
    } else {
      //
      // Find the cluster ID using ImcInterBitmap
      //
      ImcPerCluster = MAX_IMC / SncNumOfCluster;
      FirstImc = LowBitSet32 (ImcInterBitmap);
      if (FirstImc == -1) {
        FirstImc = 0;
      }
      if (MaxEnabledImc <= SncNumOfCluster) {
        DomainId = (SocketLogicalId * SncNumOfCluster) + (UINT32) (FirstImc / ImcPerCluster);
      } else {
        DomainId = (SocketLogicalId * SncNumOfCluster) + ((UINT32) FirstImc) / ImcPerCluster;
      }
    }
    if (MemMode == VOL_MEM_MODE_MIX_1LM2LM) {
      //
      // 2LM nodes follow the completion of iteration of all 1LM nodes
      //
      DomainId += (UINT32)((DynamicSiLibraryProtocol2->IsMemType2lm (MemType) ? 1 : 0) * NumSockets * SncNumOfCluster);
    }
  }

  DEBUG ((DEBUG_INFO, "SocketId = %d, SncEnabled = %d, SncNumOfCluster = %d, ImcInterBitmap = 0x%x, ",
           SocketId, SncEnabled, SncNumOfCluster, ImcInterBitmap));

  DEBUG ((DEBUG_INFO, "MemType = %d, MemMode = %d, Physical DomainId = %d\n",
           MemType, MemMode, DomainId));

  return DomainId;
}

VOID
PatchMemorySratEntries (
  IN OUT STATIC_RESOURCE_AFFINITY_TABLE       *SratTable,
  UINT8                                       SncEnabled,
  UINT8                                       SncNumOfCluster,
  UINT8                                       *LastIndexUsed
  )
{
  UINT8   LegacyNodeId;
  UINT8   NodeId;
  UINT8   Index;
  UINT8   TableIndex = 0;
  UINT8   Socket;
  UINT32  LastDomainId = 0;
  UINT64  MemoryAddress;
  UINT64  MemorySize;
  UINT8   Pass;
  UINT8   PrevIndex;
  BOOLEAN SkipEntry;
  UINT8   MaxEnabledImc = 0;
  UINTN   ImcIndex;
  UINT8   MemSocketBitmap = 0;
  UINT8   NoMemSocketBitmap;
  UINT32  ProximityDomain;
  UINT8   PhysicalClusters;
  UINT8   VirtualClusters;

  EFI_STATUS                  Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL2 *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return;
  }

  LegacyNodeId = 0xFF;
  DEBUG ((DEBUG_INFO, "\nSRAT: Updating SRAT memory information!\n"));
  DEBUG ((DEBUG_INFO, "Idx  Base              Length           Proximity Flags\n"));

  if (mSystemMemoryMap->VirtualNumaEnable) {
    PhysicalClusters = SncNumOfCluster / mSystemMemoryMap->VirtualNumOfCluster;
    VirtualClusters = mSystemMemoryMap->VirtualNumOfCluster;
  } else {
    PhysicalClusters = SncNumOfCluster;
    VirtualClusters = 1;
  }

  //
  // Looping the System Memory Map elements twice
  // Fist loop creates domains for all volatile regions based on socket
  // The second loop creates domains for all persistent memory ranges
  //
  for (Pass = 1; Pass < 3; Pass++ ) {
    TableIndex = 0;
    for (Index = 0; Index < mSystemMemoryMap->numberEntries; Index++ ) {
      SkipEntry = FALSE;
      NodeId = mSystemMemoryMap->Element[Index].NodeId;
      ASSERT (NodeId < MC_MAX_NODE);

      if (TableIndex >= MEMORY_AFFINITY_STRUCTURE_COUNT) {
        ASSERT (0);
        break;
      }

      //
      // Skip any memory region marked reserved
      //
      if (DynamicSiLibraryProtocol2->IsMemTypeReserved (mSystemMemoryMap->Element[Index].Type)) {
        continue;
      }

      //
      // As the HOB has base addr in 64 MB chunks
      //
      MemoryAddress = ((UINT64)mSystemMemoryMap->Element[Index].BaseAddress << MEM_ADDR_SHFT_VAL);

      //
      // Skip duplicate entries if applicable
      //
      if (TableIndex) {
        for (PrevIndex = 0; PrevIndex < TableIndex; PrevIndex++) {
          if (MemoryAddress == ((UINT64)SratTable->Memory[PrevIndex].AddressBaseHigh << 32) + SratTable->Memory[PrevIndex].AddressBaseLow) {
            SkipEntry = TRUE;
            break;
          }
        }
      }
      if (SkipEntry) {
        continue;
      }

      //
      // Update bitmap for sockets with memory populated
      //
      if (DynamicSiLibraryProtocol2->IsMemTypeVolatile (mSystemMemoryMap->Element[Index].Type)) {
        MemSocketBitmap |= BIT0 << mSystemMemoryMap->Element[Index].SocketId;
      }

      SratTable->Memory[TableIndex].AddressBaseLow = (UINT32)(MemoryAddress & 0xFFFFFFFF);
      SratTable->Memory[TableIndex].AddressBaseHigh  = (UINT32)((UINTN)MemoryAddress >> 32);

      //
      // As the HOB has Length in 64 MB chunks
      //
      MemorySize  = ((UINT64)mSystemMemoryMap->Element[Index].ElementSize << MEM_ADDR_SHFT_VAL);
      SratTable->Memory[TableIndex].LengthLow = (UINT32)(MemorySize & 0xFFFFFFFF);
      SratTable->Memory[TableIndex].LengthHigh = (UINT32)((UINTN)MemorySize >> 32);

      if ((Pass == 2) || DynamicSiLibraryProtocol2->IsMemTypeVolatile (mSystemMemoryMap->Element[Index].Type)) {

        //
        // Get max enabled IMC for this socket
        //
        for (ImcIndex = 0, MaxEnabledImc = 0; ImcIndex < MAX_IMC; ImcIndex++) {
          if (mSystemMemoryMap->Socket[mSystemMemoryMap->Element[Index].SocketId].imcEnabled[ImcIndex] != 0) {
            MaxEnabledImc ++;
          }
        }

        ProximityDomain = ProximityDomainOf (
                           mSystemMemoryMap->Element[Index].SocketId,
                           mSystemMemoryMap->Element[Index].Type,
                           MaxEnabledImc,
                           SncEnabled,
                           PhysicalClusters,
                           mSystemMemoryMap->Element[Index].ImcInterBitmap,
                           mSystemMemoryMap->volMemMode,
                           LastDomainId
                           );
        SratTable->Memory[TableIndex].ProximityDomain = (ProximityDomain * VirtualClusters) + (Index % VirtualClusters);
        if (LastDomainId < SratTable->Memory[TableIndex].ProximityDomain) {
          LastDomainId = SratTable->Memory[TableIndex].ProximityDomain;
        }
        if ((MemoryAddress == 0) && (MemorySize > 0)) {
          LegacyNodeId = NodeId;
        }

        //
        // Enable the Memory structure
        //
        if ((LegacyNodeId == NodeId) || (!mSocketMemoryConfiguration.SratMemoryHotPlug) ) {
          SratTable->Memory[TableIndex].Flags = EFI_ACPI_6_2_MEMORY_ENABLED;
        } else {
          SratTable->Memory[TableIndex].Flags = EFI_ACPI_6_2_MEMORY_ENABLED | EFI_ACPI_6_2_MEMORY_HOT_PLUGGABLE;
        }
        if (!DynamicSiLibraryProtocol2->IsMemTypeVolatile (mSystemMemoryMap->Element[Index].Type) &&
            !DynamicSiLibraryProtocol2->IsMemTypeFpga (mSystemMemoryMap->Element[Index].Type)) {
          SratTable->Memory[TableIndex].Flags |= EFI_ACPI_6_2_MEMORY_NONVOLATILE;
        }

        if (Pass == 2) {
          DEBUG ((DEBUG_INFO, "%3d  %08x%08x, %08x%08x %2x         %x\n",
                 TableIndex,
                 SratTable->Memory[TableIndex].AddressBaseHigh,
                 SratTable->Memory[TableIndex].AddressBaseLow,
                 SratTable->Memory[TableIndex].LengthHigh,
                 SratTable->Memory[TableIndex].LengthLow,
                 SratTable->Memory[TableIndex].ProximityDomain,
                 SratTable->Memory[TableIndex].Flags));
        }
      }
      TableIndex++;
    }
    //
    // Update LastDomainId for enabled sockets with no memory
    //
    NoMemSocketBitmap = mCpuCsrAccessVarPtr->socketPresentBitMap & ~MemSocketBitmap;
    if (Pass == 1) {
      for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if ((BIT0 << Socket) & NoMemSocketBitmap) {
          LastDomainId += SncNumOfCluster;
        }
      }
    }
  }

  *LastIndexUsed = TableIndex;
}

EFI_STATUS
PatchSratTable (
  IN OUT STATIC_RESOURCE_AFFINITY_TABLE  *SratTable
  )
{
  UINT8                                     Index;
  UINT8                                     NewIndex;
  UINT16                                    ThreadIndex;   // Support more than 256 threads (8S case)
  UINT8                                     NodeId;
  UINTN                                     HighTopMemory;
  UINTN                                     HotPlugBase;
  UINTN                                     HotPlugLen;
  UINT8                                     SncEnabled;
  UINT8                                     SncNumOfCluster;

  SncEnabled      = mIioUds->IioUdsPtr->SystemStatus.OutSncEn;
  SncNumOfCluster = mIioUds->IioUdsPtr->SystemStatus.OutNumOfCluster;

  if (mSystemMemoryMap != NULL) {
    if (mSystemMemoryMap->VirtualNumaEnable) {
     if (SncEnabled) {
      SncNumOfCluster = SncNumOfCluster * mSystemMemoryMap->VirtualNumOfCluster;
     } else {
       //
       // If Virtual NUMA enabled without SNC, use the number of Virtual NUMA clusters.
       // Do not multiply by "OutNumOfCluster" if the system is in a UMA Based Clustering mode (e.g. Hemisphere).
       //
       SncNumOfCluster = mSystemMemoryMap->VirtualNumOfCluster;
       SncEnabled = 1;
     }
    }
  }

  if (SncNumOfCluster == 0) {
    SncNumOfCluster = 1;
  }

  CollectThreadIdMsrData (SncEnabled, SncNumOfCluster);

  if (mSocketMemoryConfiguration.SratCpuHotPlug) {
    PatchSratAllApicIds (SratTable);
  } else {
    DEBUG (( DEBUG_INFO, "-------- SRAT TABLE ---------- %x\n",  PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT));

    for (ThreadIndex = 0; ThreadIndex < PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT; ThreadIndex++) {
      if (ThreadIndex < mNumberOfCPUs) {

        //
        // Use mCpuThreadIdMsrTable.ApicId by sorting SRAT sequentially to fix proximity domain grouping of threads in OSes
        //
        if (mX2ApicEnabled) {
          SratTable->x2Apic[ThreadIndex].ProximityDomain = GetProximityDomainForSNC (mCpuThreadIdMsrTable[ThreadIndex].ApicId);
          SratTable->x2Apic[ThreadIndex].X2ApicId = mCpuThreadIdMsrTable[ThreadIndex].ApicId;
          SratTable->x2Apic[ThreadIndex].Flags = EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED;
        } else {
          //
          // If SNC is enabled and 2 clusters, there is 1 extra Proximity Domain per socket
          // SNC cannot exist unless all HA's have memory
          //
          SratTable->Apic[ThreadIndex].ProximityDomain7To0 = (UINT8)GetProximityDomainForSNC (mCpuThreadIdMsrTable[ThreadIndex].ApicId);
          SratTable->Apic[ThreadIndex].ApicId = (UINT8)mCpuThreadIdMsrTable[ThreadIndex].ApicId;
          SratTable->Apic[ThreadIndex].Flags = EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED;
        }
      } else {
        if (mX2ApicEnabled) {
          SratTable->x2Apic[ThreadIndex].X2ApicId = (UINT32)-1;
        } else {
          SratTable->Apic[ThreadIndex].ApicId = 0xFF;
        }
      }
      if (mX2ApicEnabled) {
          DEBUG (( DEBUG_INFO, "\nSRAT: CpuThreadIndex\t%x, ApicId\t%x,  Flags\t%x!\n",
                ThreadIndex, SratTable->x2Apic[ThreadIndex].X2ApicId, SratTable->x2Apic[ThreadIndex].Flags));
      } else {
          DEBUG (( DEBUG_INFO, "\nSRAT: CpuThreadIndex\t%x, ApicId\t%x,  Flags\t%x, ProximityDomain\t%x!\n",
                ThreadIndex, SratTable->Apic[ThreadIndex].ApicId, SratTable->Apic[ThreadIndex].Flags,
                SratTable->Apic[ThreadIndex].ProximityDomain7To0));
      }
    }
  }

  if (mSystemMemoryMap != NULL) {

    PatchMemorySratEntries (SratTable, SncEnabled, SncNumOfCluster, &Index);

    HotPlugBase = 0x100;
    if (mSocketMemoryConfiguration.MemoryHotPlugBase == 0) { // Auto
      //
      // Read the actual TOHM and set it as the hot memory base
      //
      HighTopMemory  = (UINT64)mIioUds->IioUdsPtr->SystemStatus.tohmLimit << 26;
      HotPlugBase = (HighTopMemory >> 32);
      if ((UINT32)HighTopMemory > 0) {
        HotPlugBase++;
      }
    } else if (mSocketMemoryConfiguration.MemoryHotPlugBase) { // Number
      HotPlugBase = mSocketMemoryConfiguration.MemoryHotPlugBase * 0x80;
    }
    HotPlugLen = (mSocketMemoryConfiguration.MemoryHotPlugLen + 1) * 0x10;

    if (mSocketMemoryConfiguration.SratMemoryHotPlug) {
      DEBUG (( DEBUG_INFO, "SRAT: Updating SRAT hotplug memory information!\n" ));
      for (NodeId = 0; NodeId < MC_MAX_NODE; NodeId++) {
        NewIndex = Index + NodeId;
        if (NewIndex >= MEMORY_AFFINITY_STRUCTURE_COUNT) {
          ASSERT (0);
          break;
        }
        //
        // As the HOB has base addr in 1 GB chunks
        //
        SratTable->Memory[NewIndex].ProximityDomain = (NodeId >> 1);
        SratTable->Memory[NewIndex].AddressBaseLow  = 0;
        SratTable->Memory[NewIndex].AddressBaseHigh = (UINT32)(HotPlugBase +  NodeId * HotPlugLen);
        SratTable->Memory[NewIndex].LengthLow  = 0;
        SratTable->Memory[NewIndex].LengthHigh = (UINT32)HotPlugLen;
        SratTable->Memory[NewIndex].Flags = EFI_ACPI_6_2_MEMORY_ENABLED | EFI_ACPI_6_2_MEMORY_HOT_PLUGGABLE;

        DEBUG ((DEBUG_INFO, "%3d  %08x%08x  %08x%08x %x %x\n", NewIndex,
                SratTable->Memory[Index].AddressBaseHigh,
                SratTable->Memory[Index].AddressBaseLow,
                SratTable->Memory[Index].LengthHigh,
                SratTable->Memory[Index].LengthLow,
                SratTable->Memory[Index].ProximityDomain,
                SratTable->Memory[NewIndex].Flags));
      }
    }
    //
    // Copy SRAT memory affinity data to ACPI platform protocol
    //
    DEBUG ((DEBUG_INFO, "ACPI Platform Protocol - Memory Affinity Data updated\n"));
    ZeroMem (&mAcpiPlatformMemAffData, sizeof (mAcpiPlatformMemAffData));
    CopyMem (&mAcpiPlatformMemAffData.AcpiMemAffData[0], &SratTable->Memory[0], sizeof (mAcpiPlatformMemAffData.AcpiMemAffData));
    mAcpiPlatformMemAffData.AcpiMemAffDataValid = TRUE;
  }
  return EFI_SUCCESS;
}


/**
  Routine Description:
    Collect ThreadIdMsr Value for all the AP's and sorts it by the ThreadIdMsr Value.
    The Sorted Table, for every socket the first half of the threads will be Mapped to 1st HA
    The second half f threads in every socket will mapped to 2nd HA.

  Arguments:
    NONE

  Returns:
    NONE
**/
VOID
CollectThreadIdMsrData (
  IN UINT8        SncEnabled,
  IN UINT8        SncNumOfCluster
  )
{
  UINT32                      SocketIndex;
  UINT32                      ThreadIndex;
  UINTN                       NumOfChaPerCluster;
  BOOLEAN                     CollocatedChaIdPresent;
  UINT32                      NumberOfThreads;
  UINT8                       SocketLogicalId;

  NumOfChaPerCluster = 0;
  CollocatedChaIdPresent = FALSE;
  AsmCpuidEx (CPUID_EXTENDED_TOPOLOGY, 0, NULL, &NumberOfThreads, NULL, NULL);

  //
  // Get total number of CHAs per socket
  //
  for(SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    mPackageChaTotal[SocketIndex] = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].TotCha;
  }

  //
  // Determine whether collocated CHA ID is encoded in upper 32 bits of MSR 0x53 LOGICAL_THREAD_ID
  //
  if (SncEnabled) {
    for(ThreadIndex = 0; ThreadIndex < (MAX_CORE * MAX_THREAD); ThreadIndex++) {
      if (mCpuThreadIdMsrTable[ThreadIndex].CollocatedChaId != 0) {
        if (mCpuThreadIdMsrTable[ThreadIndex].CollocatedChaId != 0xFF) {
          CollocatedChaIdPresent = TRUE;
        }
        break;
      }
    }
    if (!CollocatedChaIdPresent) {
      DEBUG ((DEBUG_WARN, "ERROR: Collocated CHA ID is not found! Correct SNC Proximity Domain programming not guaranteed.\n"));
    }
  }

  //
  // Set SNC Proximity Domain variables for each thread
  //
  for(ThreadIndex = 0; ThreadIndex < MAX_CPU_NUM; ThreadIndex++) {
    if (mCpuThreadIdMsrTable[ThreadIndex].ThreadIdValue == 0xff) {
      continue;
    }

    //
    // We divide cores in group by number of clusters.
    // CHA number is used instead of active threads to account for when cores or
    // threads are disabled.
    //
    SocketIndex = mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[ThreadIndex].ProcessorLocation.Package;
    SocketLogicalId = GetSocketLogicalId ((UINT8) SocketIndex);
    if (!SncEnabled) {
      mCpuThreadIdMsrTable[ThreadIndex].SNCProximityDomain = SocketLogicalId;
    } else {
      NumOfChaPerCluster = mPackageChaTotal[SocketIndex]/SncNumOfCluster;
      if (CollocatedChaIdPresent) {
        mCpuThreadIdMsrTable[ThreadIndex].SNCProximityDomain = (UINT32)((SocketLogicalId * SncNumOfCluster) + (mCpuThreadIdMsrTable[ThreadIndex].CollocatedChaId / (NumOfChaPerCluster)));
      } else {
        mCpuThreadIdMsrTable[ThreadIndex].SNCProximityDomain = (UINT32)((SocketLogicalId * SncNumOfCluster) + (mCpuThreadIdMsrTable[ThreadIndex].ThreadIdValue / (NumOfChaPerCluster*NumberOfThreads)));
      }
      DEBUG ((DEBUG_INFO, "ThreadIndex=%x, SncNumOfCluster=%x, NumOfChaPerCluster=%x, SNCProximityDomain=%x\n", ThreadIndex, SncNumOfCluster, NumOfChaPerCluster, mCpuThreadIdMsrTable[ThreadIndex].SNCProximityDomain));
      DEBUG ((DEBUG_INFO, "ThreadIdValue=%x\n", mCpuThreadIdMsrTable[ThreadIndex].ThreadIdValue));
    }
  }
}

/**
  Return the Domain Flag value of the specific APICID
  Proximity Domain Flag
    0 Denotes upper half of the sorted thread needs to be mapped to 1st HA.
    1 Denotes lower half and mapped to 2nd HA

  @retval      Proximity Domain Value for the thread within a Socket.
**/
UINT32
GetProximityDomainForSNC (
  IN  UINT32 ApicId
  )
{
  UINT32 ThreadIndex     = 0;
  static UINT32 SncIndex = 0;

  //
  // The ApicIds are in order. Saving the index will reduce loop iterations
  //
  for (ThreadIndex = SncIndex; ThreadIndex < MAX_CPU_NUM; ThreadIndex++) {
    if (mCpuThreadIdMsrTable[ThreadIndex].ApicId == ApicId){
      SncIndex = ThreadIndex + 1;
      return mCpuThreadIdMsrTable[ThreadIndex].SNCProximityDomain;
    } else if (mCpuThreadIdMsrTable[ThreadIndex].ApicId == (UINT32) -1) {
      //
      // We have reached the end of the populated threads
      //
      break;
    }
  }

  //
  // Start again from the beginning if we made it to the end of the array
  //
  for (ThreadIndex = 0; ThreadIndex < MAX_CPU_NUM; ThreadIndex++) {
    if (mCpuThreadIdMsrTable[ThreadIndex].ApicId == ApicId){
      SncIndex = ThreadIndex + 1;
      return mCpuThreadIdMsrTable[ThreadIndex].SNCProximityDomain;
    }
  }

  DEBUG ((DEBUG_ERROR, "APICID not found in CpuThreadIdMsrValueTable\n"));
  ASSERT (FALSE);
  ThreadIndex--;
  return mCpuThreadIdMsrTable[ThreadIndex].SNCProximityDomain;
}

/**
  Function retrieves selected data of ACPI SRAT Memory Affinity Structures
  (please note that data will not be available until SRAT table installation)

  @param[out] *MemAffData         ACPI Memory Affinity Data
  @param[out] *MemAffDataLength   ACPI Memory Affinity Data Length

  @retval EFI_SUCCESS             ACPI Memory Affinity Data retrieved successfully
  @retval EFI_NOT_FOUND           ACPI Memory Affinity Data not found (SRAT ACPI table was not published)
  @retval EFI_INVALID_PARAMETER   One or more of input arguments is NULL
**/
EFI_STATUS
GetAcpiMemoryAffinityData (
  OUT ACPI_MEMORY_AFFINITY_DATA **MemAffData,
  OUT UINTN                     *MemAffDataLength
  )
{
  if (MemAffData == NULL || MemAffDataLength == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!mAcpiPlatformMemAffData.AcpiMemAffDataValid) {
    return EFI_NOT_FOUND;
  }

  *MemAffData = mAcpiPlatformMemAffData.AcpiMemAffData;
  *MemAffDataLength = (UINT8)MEMORY_AFFINITY_STRUCTURE_COUNT;

  return EFI_SUCCESS;
}

/**
  Function initialize and install ACPI Platform Protocol

  @param None

  @retval EFI_SUCCESS Operation completed successfully
**/
EFI_STATUS
InstallAcpiPlatformProtocol (
  VOID
  )
{
  EFI_HANDLE Handle = NULL;

  ZeroMem (&mAcpiPlatformProtocol, sizeof(mAcpiPlatformProtocol));
  mAcpiPlatformProtocol.GetAcpiMemoryAffinityData = GetAcpiMemoryAffinityData;

  return gBS->InstallProtocolInterface (&Handle, &gAcpiPlatformProtocolGuid, EFI_NATIVE_INTERFACE, &mAcpiPlatformProtocol);
}

VOID
PrintSratTable (
  IN EFI_ACPI_DESCRIPTION_HEADER *Header
  )
{
  EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER          *Table;
  EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE  *SubType;
  UINTN                                                       TotalLength;
  UINT8                                                       *TempPtr;
  EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE  *ApicType;
  EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE                      *MemType;
  EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY_STRUCTURE      *x2ApicType;
  UINTN                                                       TempLength;

  Table = (EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER *) Header;
  if (Table->Header.Signature != EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE) {
    DEBUG ((DEBUG_ERROR, "[SRAT] Not SRAT table, skip!\n"));
    return;
  }

  TotalLength = Table->Header.Length;

  DEBUG ((DEBUG_INFO, "----------------------------[SRAT Table] --------------------\n"));
  DEBUG ((DEBUG_INFO, "----Header-----\n"));
  DEBUG ((DEBUG_INFO, "Length          :  %d\n", Table->Header.Length));
  DEBUG ((DEBUG_INFO, "Revision        :  %d\n", Table->Header.Revision));
  DEBUG ((DEBUG_INFO, "Checksum        :  %2X\n", Table->Header.Checksum));
  DEBUG ((DEBUG_INFO, "OemTableId      :  %X\n", Table->Header.OemTableId));
  DEBUG ((DEBUG_INFO, "OemRevision     :  %d\n", Table->Header.OemRevision));
  DEBUG ((DEBUG_INFO, "CreatorId       :  %X\n", Table->Header.CreatorId));
  DEBUG ((DEBUG_INFO, "CreatorRevision :  %X\n", Table->Header.CreatorRevision));
  DEBUG ((DEBUG_INFO, "\n"));

  TempPtr = (UINT8 *)Table;
  TempPtr += sizeof(EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER);
  TempLength = ((EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE *) TempPtr)->Length;
  while (TempPtr < ((UINT8 *)Table + TotalLength)){
    SubType = (EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE *) TempPtr;
    if (SubType->Type == EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY) {
      ApicType = SubType;
      DEBUG ((DEBUG_INFO, "APIC Type            :  %d\n", ApicType->Type));
      DEBUG ((DEBUG_INFO, "Length               :  %d\n", ApicType->Length));
      DEBUG ((DEBUG_INFO, "ProximityDomain7To0  :  %d\n", ApicType->ProximityDomain7To0));
      DEBUG ((DEBUG_INFO, "ApicId               :  %X\n", ApicType->ApicId));
      DEBUG ((DEBUG_INFO, "Flags                :  %X\n", ApicType->Flags));
      DEBUG ((DEBUG_INFO, "LocalSapicEid        :  %d\n", ApicType->LocalSapicEid));
      DEBUG ((DEBUG_INFO, "ProximityDomain31To8 :  %d\n", (*(UINT32 *)ApicType->ProximityDomain31To8) & 0xFFFFFF));
      DEBUG ((DEBUG_INFO, "ClockDomain          :  %d\n", ApicType->ClockDomain));

    } else if (SubType->Type == EFI_ACPI_6_2_MEMORY_AFFINITY) {
      MemType = (EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE *)SubType;
      DEBUG ((DEBUG_INFO, "Mem Type        :  %d\n", MemType->Type));
      DEBUG ((DEBUG_INFO, "Length          :  %d\n", MemType->Length));
      DEBUG ((DEBUG_INFO, "ProximityDomain :  %d\n", MemType->ProximityDomain));
      DEBUG ((DEBUG_INFO, "AddressBaseLow  :  %X\n", MemType->AddressBaseLow));
      DEBUG ((DEBUG_INFO, "AddressBaseHigh :  %X\n", MemType->AddressBaseHigh));
      DEBUG ((DEBUG_INFO, "LengthLow       :  %X\n", MemType->LengthLow));
      DEBUG ((DEBUG_INFO, "LengthHigh      :  %X\n", MemType->LengthHigh));
      DEBUG ((DEBUG_INFO, "Flags           :  %X\n", MemType->Flags));

    } else if (SubType->Type == EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY) {
      x2ApicType = (EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY_STRUCTURE *) SubType;
      DEBUG ((DEBUG_INFO, "x2APIC Type     :  %d\n", x2ApicType->Type));
      DEBUG ((DEBUG_INFO, "Length          :  %d\n", x2ApicType->Length));
      DEBUG ((DEBUG_INFO, "ProximityDomain :  %d\n", x2ApicType->ProximityDomain));
      DEBUG ((DEBUG_INFO, "X2ApicId        :  %X\n", x2ApicType->X2ApicId));
      DEBUG ((DEBUG_INFO, "Flags           :  %X\n", x2ApicType->Flags));
      DEBUG ((DEBUG_INFO, "ClockDomain     :  %d\n", x2ApicType->ClockDomain));

    } else {
      DEBUG ((DEBUG_INFO, "Unknown Type : %d\n", SubType->Type));
      DEBUG ((DEBUG_INFO, "Length : %d\n", SubType->Length));
    }
    if (SubType->Length < 10) {
      DEBUG ((DEBUG_ERROR, "Error in Length %d, try previous length.\n", SubType->Length));
      TempPtr += TempLength;
    } else {
      TempPtr += SubType->Length;
      TempLength = SubType->Length;
    }
    DEBUG ((DEBUG_INFO, "\n"));
  }
}

/**
  Build from scratch and install the SRAT.

  @retval EFI_SUCCESS           The SRAT was installed successfully.
  @retval EFI_OUT_OF_RESOURCES  Could not allocate required structures.
**/
EFI_STATUS
InstallSratTable (
  VOID
  )
{
  EFI_STATUS                                          Status;
  UINTN                                               Index;
  UINTN                                               TableHandle;
  UINTN                                               TableSize;
  STATIC_RESOURCE_AFFINITY_TABLE                      *SratTable;

  Status = EFI_SUCCESS;
  TableHandle = 0;

  if (mSocketMemoryConfiguration.Srat == 0)  {
    return EFI_SUCCESS;
  }

  if  (mSystemMemoryMap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SratTable = (STATIC_RESOURCE_AFFINITY_TABLE *) AllocateZeroPool (sizeof(STATIC_RESOURCE_AFFINITY_TABLE));
  if (SratTable == NULL) {
    DEBUG ((DEBUG_ERROR, "InstallSratTable: allocate SRAT table failed \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  TableSize = sizeof (EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER) +
              sizeof (EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE) * PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT +
              sizeof (EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE) * MEMORY_AFFINITY_STRUCTURE_COUNT +
              sizeof (EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY_STRUCTURE) * X2APIC_AFFINITY_STRUCTURE_COUNT ;

  SratTable->SratHeader = (EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER *) AllocateZeroPool (TableSize);
  if (SratTable->SratHeader == NULL) {
    DEBUG ((DEBUG_ERROR, "InstallSratTable: Create SratHeader has failed \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  SratTable->Apic = (EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE *) (UINTN)((UINTN)SratTable->SratHeader +
                      (UINTN)sizeof(EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER));

  SratTable->Memory = (EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE *) (UINTN)((UINTN)SratTable->Apic +
                      (UINTN)sizeof(EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE) * PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT);

  SratTable->x2Apic = (EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY_STRUCTURE *) (UINTN)((UINTN)SratTable->Memory +
                      (UINTN)sizeof(EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE)* MEMORY_AFFINITY_STRUCTURE_COUNT);


  SratTable->SratHeader->Header.Signature = EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE;
  SratTable->SratHeader->Header.Length = (UINT32) TableSize;
  SratTable->SratHeader->Header.Revision = EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_REVISION;
  CopyMem (SratTable->SratHeader->Header.OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (SratTable->SratHeader->Header.OemId));
  SratTable->SratHeader->Header.OemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
  SratTable->SratHeader->Header.OemRevision = EFI_ACPI_OEM_SRAT_REVISION;
  SratTable->SratHeader->Header.CreatorId = PcdGet32 (PcdAcpiDefaultCreatorId);
  SratTable->SratHeader->Header.CreatorRevision = PcdGet32 (PcdAcpiDefaultCreatorRevision);

  SratTable->SratHeader->Reserved1 = EFI_ACPI_SRAT_RESERVED_FOR_BACKWARD_COMPATIBILITY;
  SratTable->SratHeader->Reserved2 = EFI_ACPI_RESERVED_QWORD;

  for (Index = 0; Index < PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT; Index++) {
    SratTable->Apic[Index].Type = EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY;
    SratTable->Apic[Index].Length = sizeof(EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE);
    SratTable->Apic[Index].ApicId = 0xFF;
  }

  for (Index = 0; Index < MEMORY_AFFINITY_STRUCTURE_COUNT; Index++) {
    SratTable->Memory[Index].Type = EFI_ACPI_6_2_MEMORY_AFFINITY;
    SratTable->Memory[Index].Length = sizeof(EFI_ACPI_6_2_MEMORY_AFFINITY_STRUCTURE);
  }

  for (Index = 0; Index < X2APIC_AFFINITY_STRUCTURE_COUNT; Index++) {
    SratTable->x2Apic[Index].Type = EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY;
    SratTable->x2Apic[Index].Length = sizeof(EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_AFFINITY_STRUCTURE);
    SratTable->x2Apic[Index].X2ApicId = 0xFFFFFFFF;
  }

  PatchSratTable (SratTable);
  PrintSratTable (&SratTable->SratHeader->Header);

  //
  // Publish SRAT Structure to ACPI
  //
  Status = mAcpiTable->InstallAcpiTable (
                          mAcpiTable,
                          SratTable->SratHeader,
                          SratTable->SratHeader->Header.Length,
                          &TableHandle
                          );


  FreePool (SratTable->SratHeader);
  FreePool (SratTable);

  InstallAcpiPlatformProtocol ();

  return Status;
}
