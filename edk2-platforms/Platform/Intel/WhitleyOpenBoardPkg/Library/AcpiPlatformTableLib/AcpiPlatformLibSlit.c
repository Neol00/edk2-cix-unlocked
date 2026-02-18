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

extern SYSTEM_MEMORY_MAP_HOB        *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL         *mIioUds;
extern EFI_ACPI_TABLE_PROTOCOL      *mAcpiTable;
extern CPU_CSR_ACCESS_VAR           *mCpuCsrAccessVarPtr;

EFI_ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE_PMEM_INFO mSlitPmemInfo[EFI_ACPI_SLIT_PMEM_INFO_CNT];


/**
  Displays System Locality Distance Information Table (SLIT)

  @param None

  @retval None
**/
VOID
DisplayEntries (
  IN ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *SlitPtr
  )
{
  UINT16                                     EntryIdx = 0;
  UINT16                                     MaxTableEntries;
  UINT8                                      NodeCount;

  if (SlitPtr == NULL) {

    ASSERT (SlitPtr != NULL);
    return;
  }
  NodeCount = (UINT8)SlitPtr->Header.NumberOfSystemLocalities;
  MaxTableEntries = NodeCount * NodeCount;

  DEBUG ((DEBUG_INFO, "SLIT: Dump table (size %d):\n", NodeCount));

  while (EntryIdx < MaxTableEntries) {

    DEBUG ((DEBUG_INFO, "%02X ", SlitPtr->NumSlit[EntryIdx].Entry));
    if ((EntryIdx % NodeCount) == (NodeCount - 1)) {
      DEBUG ((DEBUG_INFO, "\n"));
    }
    EntryIdx++;
  }
}


/**
  Stores correlation between AEP PMEM NUMA node and socket

  @param[in] Pmem   AEP PMEM NUMA node number
  @param[in] Socket Socket number

  @retval None

**/
VOID
SavePmemInfo (
  IN UINT8 Pmem,
  IN UINT8 Socket,
  MEM_TYPE Type
  )
{
  if (Pmem < EFI_ACPI_SLIT_PMEM_INFO_CNT) {

    DEBUG ((DEBUG_INFO, "SLIT: Found PMem %d at socket %d (mem type 0x%X)\n", Pmem, Socket, Type));
    mSlitPmemInfo[Pmem].Pmem = Pmem;
    mSlitPmemInfo[Pmem].Socket = Socket;
    mSlitPmemInfo[Pmem].Valid = TRUE;
    return;
  }

  DEBUG ((DEBUG_ERROR, "SLIT: Error - Number of PMem nodes (%d) exceed PMem info data table size (%d)\n",
      Pmem, EFI_ACPI_SLIT_PMEM_INFO_CNT));
  ASSERT (FALSE);
}

/**
  Retrieves socket correlated with given AEP PMEM NUMA node

  @param[in] Pmem AEP PMEM Numa node

  @retval Socket correlated with given AEP PMEM NUMA node

**/
UINT8
GetSocketForPmem (
  IN UINT8 Pmem
  )
{
  UINT8 PmemInfoEntry;

  for (PmemInfoEntry = 0; PmemInfoEntry < EFI_ACPI_SLIT_PMEM_INFO_CNT; PmemInfoEntry++) {
    if (mSlitPmemInfo[PmemInfoEntry].Valid && mSlitPmemInfo[PmemInfoEntry].Pmem == Pmem) {
      return mSlitPmemInfo[PmemInfoEntry].Socket;
    }
  }

  DEBUG ((DEBUG_ERROR, "SLIT: Error - PMem node (%d) is not associated with any socket\n", Pmem));
  ASSERT (FALSE);
  return PMEM_INVALID_SOCKET;
}

/**
  This function gets the physical node index for given FPGA NUMA node.
  If the given FPGA NUMA node doesn't exist, an invalid physical node
  index 0xff will be returned.

  @param[in] FpgaNumaNodeId      FPGA NUMA node index (0 based).

  @retval Physical node index of given FPGA NUMA node (0 based).

**/
UINT8
GetPhyNodeIdForFpga (
  IN UINT8  FpgaNumaNodeId
  )
{
  UINT32    FpgaPresentBitMap;
  UINT8     PhyNodeId;

  PhyNodeId = 0;
  FpgaPresentBitMap = mCpuCsrAccessVarPtr->FpgaPresentBitMap;

  while ((FpgaNumaNodeId != (UINT8) -1) && (FpgaPresentBitMap != 0)) {
    if ((FpgaPresentBitMap & BIT0) != 0) {
      FpgaNumaNodeId--;
    }
    FpgaPresentBitMap >>= 1;
    PhyNodeId++;
  }

  if ((FpgaPresentBitMap == 0) && (FpgaNumaNodeId != (UINT8) -1)) {
    return (UINT8) -1;
  }

  return PhyNodeId - 1;
}

/**
  Retrieves number of FPGA Presents

  @retval Number of FPGA Presents in the system

**/
UINT8
GetFpgaCount (
  VOID)
{
  UINT8   FpgaCount = 0;
  UINT32  FpgaPresentBitMap = mCpuCsrAccessVarPtr->FpgaPresentBitMap;

  while (FpgaPresentBitMap) {
    if (FpgaPresentBitMap & BIT0) {
      FpgaCount++;
    }
    FpgaPresentBitMap >>= 1;
  }
  return FpgaCount;
}

/**
  Retrieves number of AEP PMEM NUMA nodes

  @retval Number of AEP PMEM NUMA nodes

**/
UINT8
GetPmemNodeCount (
  VOID
  )
{
  UINT8 Socket;
  UINT8 SadRule;
  UINT8 PmemNodeCount = 0;

  EFI_STATUS                  Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL2 *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return 0;
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if ((mCpuCsrAccessVarPtr->socketPresentBitMap & (BIT0 << Socket)) == 0) {
      continue;
    }
    for (SadRule = 0; SadRule < MAX_SAD_RULES; SadRule++) {
      //
      // Only local SADs of PMEM type should be taken into consideration.
      // Skip any memory region marked reserved.
      //
      if (mSystemMemoryMap->Socket[Socket].SAD[SadRule].local &&
          DynamicSiLibraryProtocol2->IsMemTypeAppDirect (mSystemMemoryMap->Socket[Socket].SAD[SadRule].type) &&
          !DynamicSiLibraryProtocol2->IsMemTypeReserved (mSystemMemoryMap->Socket[Socket].SAD[SadRule].type)) {

        SavePmemInfo (PmemNodeCount++, Socket, mSystemMemoryMap->Socket[Socket].SAD[SadRule].type);
      }
    }
  }

  return PmemNodeCount;
}

/**
  Retrieves number of clusters in the system if system is in SNC mode. If system is not
  in SNC mode, the return 1.

  @return Number of clusters in the system in SNC mode or 1 if system is not in SNC mode.

**/
UINT8
GetNumClusters (
  VOID
  )
{
  UINT8 NumClusters;

  NumClusters = mIioUds->IioUdsPtr->SystemStatus.OutNumOfCluster;
  if ((NumClusters == 0) || (mIioUds->IioUdsPtr->SystemStatus.OutSncEn == 0)) {
    NumClusters = 1; // For non-SNC mode, we should also return 1.
  }
  if (mSystemMemoryMap->VirtualNumaEnable) {
    NumClusters = NumClusters * mSystemMemoryMap->VirtualNumOfCluster;
  }

  return NumClusters;
}

/**
  Retrieves number of enabled CPUs in the system

  @param None

  @retval Number of enabled CPUs in the system

**/
UINT8
GetNumCpus (
  VOID
  )
{
  return mIioUds->IioUdsPtr->SystemStatus.numCpus;
}

/**
  Calculates total number of nodes

  @param[in]      NumCpus       Number of CPUs
  @param[in]      NumClusters   Number of clusters
  @param[in]      PmemNodeCount Number of AEP PMEM NUMA nodes

  @retval Total number of nodes

**/
UINT8
GetNodeCount (
  IN UINT8  NumCpus,
  IN UINT8  NumClusters,
  IN UINT8  PmemNodeCount,
  IN UINT8 FpgaCount
  )
{
  UINT8 NodeCount;

  if (mSystemMemoryMap->volMemMode == VOL_MEM_MODE_MIX_1LM2LM) {
    NodeCount = ((NumCpus * NumClusters * EFI_ACPI_SLIT_DOMAINS_NODES_MAX_CNT) + PmemNodeCount + FpgaCount);
  } else {
    NodeCount = ((NumCpus * NumClusters) + PmemNodeCount + FpgaCount);
  }
  if ((NodeCount * NodeCount) < EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT) {
    return NodeCount;
  }

  DEBUG ((DEBUG_ERROR, "SLIT: Error - Nodes distances info data size (%d) exceed SLIT distances info table size (%d)\n",
    (NodeCount * NodeCount), EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT));
  ASSERT (FALSE);
  return 0;
}

/**
  Verifies whether sockets are linked

  @param[in]      SourceSocket  Source Socket ID
  @param[in]      TargetSocket  Targer Socket ID

  @retval TRUE link between source socket and target socket was found
          FALSE otherwise

**/
BOOLEAN
SocketsLinked (
  IN UINT32  SourceSocket,
  IN UINT32  TargetSocket
  )
{
  UINT8 PeerSocket;
  UINT8 LinkValid;
  UINT8 PeerSocId;

  EFI_STATUS                  Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL2 *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return FALSE;
  }

  //
  // Validate sockets ids
  //
  if ((SourceSocket < MAX_SOCKET) && (TargetSocket < MAX_SOCKET)) {
    //
    // Do not process when source socket is the same as target socket
    //
    if (SourceSocket != TargetSocket) {
      for (PeerSocket = 0; PeerSocket < (DynamicSiLibraryProtocol2->GetKtiPortCnt()); PeerSocket++) {
        LinkValid = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SourceSocket].PeerInfo[PeerSocket].Valid;
        PeerSocId = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SourceSocket].PeerInfo[PeerSocket].PeerSocId;
        if (LinkValid && (PeerSocId == TargetSocket)) {
          //
          // Link found
          //
          return TRUE;
        }
      }
    }
    //
    // Link not found
    //
    return FALSE;
  }

  DEBUG ((DEBUG_ERROR, "SLIT: Error when checking if sockets are linked (source socket id %d, target socket id %d)\n", SourceSocket, TargetSocket));
  return FALSE;
}

/**
  Initializes SLIT Table entries

  @param[in,out] Table Pointer to SLIT ACPI tables

  @retval EFI_SUCCESS  operation completed successfully

**/
EFI_STATUS
InitEntries (
  IN OUT ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *SlitPtr
  )
{
  if (SlitPtr == NULL) {
    ASSERT (SlitPtr != NULL);
    return EFI_INVALID_PARAMETER;
  }
  SetMem (&SlitPtr->NumSlit[0].Entry, EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT, 0xFF);

  return EFI_SUCCESS;
}

/**
  Processes socket nodes

  @param[in,out]  Table         Pointer to SLIT ACPI tables
  @param[in]      NumCpus       Number of CPUs
  @param[in]      NumClusters   Number of clusters
  @param[in]      PmemNodeCount Number of AEP PMEM NUMA nodes
  @param[in]      FpgaCount     Number of FPGA NUMA nodes
  @param[in]      NodeCount     Number of all nodes

  @retval EFI_SUCCESS  operation completed successfully

**/
EFI_STATUS
ProcessSockets (
  IN OUT EFI_ACPI_COMMON_HEADER *Table,
  IN UINT8                      NumCpus,
  IN UINT8                      NumClusters,
  IN UINT8                      PmemNodeCount,
  IN UINT8                      FpgaCount,
  IN UINT8                      NodeCount
  )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  UINT16                                  EntryIdx;
  UINT8                                   SourceNode;
  UINT8                                   SourceSocket;
  UINT8                                   SourceCluster;
  UINT8                                   TargetSocket;
  UINT8                                   TargetCluster;
  UINT8                                   VirtualNumaFactor;

  if (NULL == Table) {
    DEBUG ((DEBUG_ERROR, "SLIT: Error in SLIT update with data about nodes on same socket\n"));
    return EFI_INVALID_PARAMETER;
  }
  VirtualNumaFactor = 1;
  if (mSystemMemoryMap->VirtualNumaEnable) {
    VirtualNumaFactor = mSystemMemoryMap->VirtualNumOfCluster;
  }

  DEBUG ((DEBUG_INFO, "SLIT: Update with data about nodes on same socket\n"));

  SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

  for (SourceNode = 0; SourceNode < (NodeCount - PmemNodeCount - FpgaCount) ; SourceNode++) {
    SourceSocket = SourceNode / NumClusters;
    SourceCluster = SourceNode % NumClusters;
    for (TargetSocket = 0; TargetSocket < NumCpus; TargetSocket++) {
      for (TargetCluster = 0; TargetCluster < NumClusters; TargetCluster++) {
        if (SourceSocket == TargetSocket) {
          EntryIdx = (SourceNode * (NodeCount)) + (TargetSocket * NumClusters) + TargetCluster;
          //
          // Source and target are nodes on same socket
          //
          if ((SourceCluster / VirtualNumaFactor) == (TargetCluster / VirtualNumaFactor)) {
            //
            // a) Same socket same physical cluster
            //
            SlitAcpiTable->NumSlit[EntryIdx].Entry = ZERO_HOP;
          } else {
            //
            // b) Same socket different cluster
            //
            SlitAcpiTable->NumSlit[EntryIdx].Entry = ZERO_ONE;
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Processes socket nodes

  @param[in,out]  Table         Pointer to SLIT ACPI tables
  @param[in]      NumCpus       Number of CPUs
  @param[in]      NumClusters   Number of clusters
  @param[in]      PmemNodeCount Number of AEP PMEM NUMA nodes
  @param[in]      FpgaCount     Number of FPGA NUMA nodes
  @param[in]      NodeCount     Number of all nodes

  @retval EFI_SUCCESS  operation completed successfully

**/
EFI_STATUS
ProcessMixedModeSockets (
  IN OUT EFI_ACPI_COMMON_HEADER *Table,
  IN UINT8                      NumCpus,
  IN UINT8                      NumClusters,
  IN UINT8                      PmemNodeCount,
  IN UINT8                      FpgaCount,
  IN UINT8                      NodeCount
  )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  UINT16                                  EntryIdx1LM;
  UINT16                                  EntryIdx2LM;
  UINT8                                   SourceNode;
  UINT8                                   SourceSocket;
  UINT8                                   SourceCluster;
  UINT8                                   TargetSocket;
  UINT8                                   TargetCluster;
  BOOLEAN                                 Is2LM;

  if (NULL == Table) {
    DEBUG ((DEBUG_ERROR, "SLIT: Error in SLIT update with data about nodes on same socket for Mixed Mode\n"));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "SLIT: Update with data about nodes on same socket for Mixed Mode\n"));

  SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

  for (SourceNode = 0; SourceNode < (NodeCount - PmemNodeCount - FpgaCount) ; SourceNode++) {
    Is2LM = (SourceNode >= NumCpus * NumClusters) ? TRUE : FALSE;
    SourceSocket = (SourceNode - NumCpus * NumClusters * (Is2LM ? 1 : 0)) / NumClusters;
    SourceCluster = (SourceNode - NumCpus * NumClusters * (Is2LM ? 1 : 0)) % NumClusters;

    TargetSocket = SourceSocket;
    for (TargetCluster = 0; TargetCluster < NumClusters; TargetCluster++) {
      EntryIdx1LM = (UINT16)((SourceNode * NodeCount) + (TargetSocket * NumClusters) + TargetCluster);
      EntryIdx2LM = EntryIdx1LM + NumCpus * NumClusters;

      if ((SourceCluster == TargetCluster) && (Is2LM == FALSE)) {
        //
        // CPU -> 1LM at the same socket, the same cluster
        //
        SlitAcpiTable->NumSlit[EntryIdx1LM].Entry = ZERO_HOP;
        //
        // CPU -> 2LM at the same socket, the same cluster
        //
        SlitAcpiTable->NumSlit[EntryIdx2LM].Entry = ZERO_ONE;
      } else if ((SourceCluster == TargetCluster) && (Is2LM == TRUE)) {
        //
        // CPU -> 2LM at the same socket, the same cluster
        //
        SlitAcpiTable->NumSlit[EntryIdx1LM].Entry = ZERO_ONE;
        //
        // not effective
        //
        SlitAcpiTable->NumSlit[EntryIdx2LM].Entry = ZERO_HOP;
      } else if ((SourceCluster != TargetCluster) && (Is2LM == FALSE)) {
        //
        // CPU -> 1LM at the same socket, different cluster
        //
        SlitAcpiTable->NumSlit[EntryIdx1LM].Entry = ZERO_ONE;
        //
        // CPU -> 2LM at the same socket, different cluster
        //
        SlitAcpiTable->NumSlit[EntryIdx2LM].Entry = ZERO_TWO;
      } else {
        //
        // branch condition: ((SourceCluster != TargetCluster) && (Is2LM == TRUE))
        // CPU -> 2LM at the same socket, different cluster
        //
        SlitAcpiTable->NumSlit[EntryIdx1LM].Entry = ZERO_TWO;
        //
        // not effective
        //
        SlitAcpiTable->NumSlit[EntryIdx2LM].Entry = ZERO_TWO;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Processes connections between sockets to retrieve valid distances

  @param[in,out]  Table         Pointer to SLIT ACPI tables
  @param[in]      NumCpus       Number of CPUs
  @param[in]      NumClusters   Number of clusters
  @param[in]      PmemNodeCount Number of AEP PMEM NUMA nodes
  @param[in]      FpgaCount     Number of FPGA NUMA nodes
  @param[in]      NodeCount     Number of all nodes

  @retval EFI_SUCCESS  operation completed successfully

**/
EFI_STATUS
ProcessSocketsLinks (
  IN OUT EFI_ACPI_COMMON_HEADER *Table,
  IN UINT8                      NumCpus,
  IN UINT8                      NumClusters,
  IN UINT8                      PmemNodeCount,
  IN UINT8                      FpgaCount,
  IN UINT8                      NodeCount
  )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  UINT8                                   SourceNode;
  UINT8                                   SourceSocket;
  UINT8                                   TargetSocket;

  if (NULL == Table) {
    DEBUG ((DEBUG_ERROR, "SLIT: Error in processing links between sockets\n"));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "SLIT: Update table with links between sockets\n"));

  SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

  for (SourceNode = 0; SourceNode < (NodeCount - PmemNodeCount - FpgaCount); SourceNode++) {
    SourceSocket = SourceNode / NumClusters;
    for (TargetSocket = 0; TargetSocket < NumCpus; TargetSocket++) {
      if (SocketsLinked (GetSocketPhysicalId (SourceSocket), GetSocketPhysicalId (TargetSocket))) {
        SetMem (&SlitAcpiTable->NumSlit[(SourceNode * NodeCount) + (TargetSocket * NumClusters)].Entry,
                NumClusters, ONE_HOP);
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Processes connections between sockets to retrieve valid distances

  @param[in,out]  Table         Pointer to SLIT ACPI tables
  @param[in]      NumCpus       Number of CPUs
  @param[in]      NumClusters   Number of clusters
  @param[in]      PmemNodeCount Number of AEP PMEM NUMA nodes
  @param[in]      FpgaCount     Number of FPGA NUMA nodes
  @param[in]      NodeCount     Number of all nodes

  @retval EFI_SUCCESS  operation completed successfully

**/
EFI_STATUS
ProcessMixedModeSocketsLinks (
  IN OUT EFI_ACPI_COMMON_HEADER *Table,
  IN UINT8                      NumCpus,
  IN UINT8                      NumClusters,
  IN UINT8                      PmemNodeCount,
  IN UINT8                      FpgaCount,
  IN UINT8                      NodeCount
  )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  UINT16                                  EntryIdx1LM;
  UINT16                                  EntryIdx2LM;
  UINT8                                   SourceNode;
  UINT8                                   SourceSocket;
  UINT8                                   TargetSocket;
  UINT8                                   TargetCluster;
  BOOLEAN                                 Is2LM;

  if (NULL == Table) {
    DEBUG ((DEBUG_ERROR, "SLIT: Error in processing links between sockets in Mixed Mode\n"));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "SLIT: Update table with links between sockets in Mixed Mode\n"));

  SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

  for (SourceNode = 0; SourceNode < (NodeCount - PmemNodeCount - FpgaCount); SourceNode++) {
    Is2LM = (SourceNode >= NumCpus * NumClusters) ? TRUE : FALSE;
    SourceSocket = (SourceNode - NumCpus * NumClusters * (Is2LM ? 1 : 0)) / NumClusters;

    for (TargetSocket = 0; TargetSocket < NumCpus; TargetSocket++) {
      if (SocketsLinked (GetSocketPhysicalId (SourceSocket), GetSocketPhysicalId (TargetSocket))) {
        //
        // If both sockets are linked by KTI and not the same socket.
        //
        for (TargetCluster = 0; TargetCluster < NumClusters; TargetCluster++) {
          EntryIdx1LM = (UINT16)((SourceNode * NodeCount) + (TargetSocket * NumClusters) + TargetCluster);
          EntryIdx2LM = EntryIdx1LM + NumCpus * NumClusters;

          if (Is2LM == FALSE) {
            //
            // CPU -> 1LM at different socket
            //
            SlitAcpiTable->NumSlit[EntryIdx1LM].Entry = ONE_HOP;
            //
            // CPU -> 2LM at different socket
            //
            SlitAcpiTable->NumSlit[EntryIdx2LM].Entry = ONE_ONE;
          } else {
            //
            // branch condition: (Is2LM == TRUE)
            // CPU -> 2LM at different socket
            //
            SlitAcpiTable->NumSlit[EntryIdx1LM].Entry = ONE_ONE;
            //
            // not effective
            //
            SlitAcpiTable->NumSlit[EntryIdx2LM].Entry = ONE_TWO;
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Processes all AEP PMEM NUMA nodes

  @param[in,out]  Table         Pointer to SLIT ACPI tables
  @param[in]      NumCpus       Number of CPUs
  @param[in]      NumClusters   Number of clusters
  @param[in]      PmemNodeCount Number of AEP PMEM NUMA nodes
  @param[in]      FpgaCount     Number of FPGA NUMA nodes
  @param[in]      NodeCount     Number of all nodes

  @retval EFI_SUCCESS  operation completed successfully

**/
EFI_STATUS
ProcessPmems (
  IN OUT EFI_ACPI_COMMON_HEADER *Table,
  IN UINT8                      NumCpus,
  IN UINT8                      NumClusters,
  IN UINT8                      PmemNodeCount,
  IN UINT8                      FpgaCount,
  IN UINT8                      NodeCount
  )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  UINT16                                  EntryIdx;
  UINT8                                   SourceNode;
  UINT8                                   TargetNode;
  UINT8                                   SourceSocket;
  UINT8                                   TargetCluster;
  UINT8                                   TargetSocket;
  UINT8                                   SourcePmem;
  UINT8                                   TargetPmem;
  UINT8                                   SourcePmemSocket;
  UINT8                                   TargetPmemSocket;
  UINT8                                   TotalVolMemNodes;

  if (NULL == Table) {
    DEBUG ((DEBUG_ERROR, "SLIT: Error in processing PMems\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (mSystemMemoryMap->volMemMode == VOL_MEM_MODE_MIX_1LM2LM) {
    TotalVolMemNodes = NumCpus * NumClusters * EFI_ACPI_SLIT_DOMAINS_NODES_MAX_CNT;
  } else {
    TotalVolMemNodes = NumCpus * NumClusters;
  }

  DEBUG ((DEBUG_INFO, "SLIT: Include PMem NUMA nodes\n"));

  if (PmemNodeCount > 0) {

    SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

    //
    // 1) AEP PMEM nodes to AEP PMEM nodes distances
    //
    for (SourceNode = (NodeCount - PmemNodeCount - FpgaCount); SourceNode < (NodeCount - FpgaCount); SourceNode++) {
      SourcePmem = SourceNode - TotalVolMemNodes;
      for (TargetPmem = 0; TargetPmem < PmemNodeCount; TargetPmem++) {
        TargetNode = TargetPmem + TotalVolMemNodes;
        EntryIdx = (SourceNode * NodeCount) + TargetNode;
        if (SourcePmem == TargetPmem) {
          SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_ZERO_HOP;
        } else {
          //
          // Retrieve sockets associated with PMEMs
          //
          SourcePmemSocket = GetSocketForPmem (SourcePmem);
          TargetPmemSocket = GetSocketForPmem (TargetPmem);

          if (SourcePmemSocket == TargetPmemSocket) {
            //
            // PMEMs are on the same socket
            //
            SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_ONE_ONE;
          } else {
            //
            // Assign 2 hop and process with PeerInfo checking
            //
            SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_TWO_HOP;

            //
            // Examine PeerInfo to look for link between AEP PMEM source socket and AEP PMEM target socket
            //
            if (SocketsLinked (SourcePmemSocket, TargetPmemSocket)) {
              //
              // Link found assign 1 hop
              //
              SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_ONE_HOP;
            }
          }
        }
      }
    }

    //
    // 2) Sockets to AEP PMEM nodes distances
    //
    for (SourceNode = 0; SourceNode < (NodeCount - PmemNodeCount - FpgaCount); SourceNode++) {
      SourceSocket = GetSocketPhysicalId (SourceNode / NumClusters);
      for (TargetPmem = 0; TargetPmem < PmemNodeCount; TargetPmem++) {
        TargetPmemSocket = GetSocketForPmem (TargetPmem);
        TargetNode = TargetPmem + TotalVolMemNodes;
        EntryIdx = (SourceNode * NodeCount) + TargetNode;
        if (SourceSocket == TargetPmemSocket) {
          SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_ONE_ONE;
        } else {
          //
          // Assign 2 hop and process with PeerInfo checking
          //
          SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_TWO_HOP;

          //
          // Examine PeerInfo to look for link between source socket and AEP PMEM socket
          //
          if (SocketsLinked (SourceSocket, TargetPmemSocket)) {
            //
            // Link found assign 1 hop
            //
            SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_ONE_HOP;
          }
        }
      }
    }

    //
    // 3) AEP PMEM nodes to sockets distances
    //
    for (SourceNode = (NodeCount - PmemNodeCount - FpgaCount); SourceNode < (NodeCount - FpgaCount); SourceNode++) {
      SourcePmem = SourceNode - TotalVolMemNodes;
      SourcePmemSocket = GetSocketForPmem (SourcePmem);
      for (TargetSocket = 0; TargetSocket < NumCpus; TargetSocket++) {
        for (TargetCluster = 0; TargetCluster < NumClusters; TargetCluster++) {
          EntryIdx = (SourceNode * NodeCount) + (TargetSocket * NumClusters) + TargetCluster;
          if(SourcePmemSocket == GetSocketPhysicalId (TargetSocket)) {
            SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_ONE_ONE;
          } else {
            //
            // Assign 2 hop and process with PeerInfo checking
            //
            SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_TWO_HOP;

            //
            // Examine PeerInfo to look for link between source socket and AEP PMEM socket
            //
            if (SocketsLinked (SourcePmemSocket, GetSocketPhysicalId (TargetSocket))) {
              //
              // Link found assign 1 hop
              //
              SlitAcpiTable->NumSlit[EntryIdx].Entry = PMEM_ONE_HOP;
            }
          }
        }
      }
    }
  } else {
    DEBUG ((DEBUG_INFO, "SLIT: PMem NUMA nodes not present\n"));
  }

  return EFI_SUCCESS;
}

/**
  This function processes all FPGA NUMA nodes.

  @param[in,out] Table             Pointer to SLIT ACPI tables.
  @param[in] NumClusters           Number of clusters.
  @param[in] PmemNodeCount         Number of AEP PMEM NUMA nodes.
  @param[in] FpgaCount             Number of FPGA NUMA nodes.
  @param[in] NodeCount             Number of all nodes.

  @retval EFI_SUCCESS              This function is executed successfully.
  @retval EFI_INVALID_PARAMETER    Some of input parameters are invalid.
**/
EFI_STATUS
ProcessFpgaNodes (
  IN OUT EFI_ACPI_COMMON_HEADER *Table,
  IN UINT8                      NumClusters,
  IN UINT8                      PmemNodeCount,
  IN UINT8                      FpgaCount,
  IN UINT8                      NodeCount
  )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  UINT8                                   SourceNode;
  UINT8                                   TargetNode;
  UINT8                                   SourceSocket;
  UINT8                                   TargetSocket;

  if (Table == NULL) {
    DEBUG ((DEBUG_ERROR, "SLIT: Error in processing FPGA nodes\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (FpgaCount == 0) {
    return EFI_SUCCESS;
  }

  SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

  for (SourceNode = (NodeCount - FpgaCount); SourceNode < NodeCount; SourceNode++) {
    SourceSocket = GetPhyNodeIdForFpga (SourceNode - (NodeCount - FpgaCount));

    for (TargetNode = 0; TargetNode < NodeCount; TargetNode++) {
      if (TargetNode < (NodeCount- PmemNodeCount - FpgaCount)) {
        TargetSocket = GetSocketPhysicalId (TargetNode / NumClusters);             // Normal nodes
      } else if (TargetNode < (NodeCount - FpgaCount)) {
        TargetSocket = GetSocketForPmem (TargetNode - (NodeCount - PmemNodeCount - FpgaCount));  // PMEM nodes
      } else {
        TargetSocket = GetPhyNodeIdForFpga (TargetNode - (NodeCount - FpgaCount)); // FPGA nodes
      }

      if (SourceSocket == TargetSocket) {
        SlitAcpiTable->NumSlit[SourceNode * NodeCount + TargetNode].Entry = ZERO_HOP;
      } else if (SocketsLinked (SourceSocket, TargetSocket)) {
        SlitAcpiTable->NumSlit[SourceNode * NodeCount + TargetNode].Entry = ONE_HOP;
        SlitAcpiTable->NumSlit[TargetNode * NodeCount + SourceNode].Entry = ONE_HOP;
      } else {
        SlitAcpiTable->NumSlit[SourceNode * NodeCount + TargetNode].Entry = TWO_HOP;
        SlitAcpiTable->NumSlit[TargetNode * NodeCount + SourceNode].Entry = TWO_HOP;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Processes all remaining valid SLIT nodes

  @param[in,out] Table  pointer to SLIT ACPI tables

  @retval EFI_SUCCESS  operation completed successfully

**/
EFI_STATUS
ProcessRemainingNodes (
  IN OUT EFI_ACPI_COMMON_HEADER *Table
  )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  UINT16                                  EntryIdx = 0;
  UINT16                                  MaxTableEntries;
  UINT8                                   NodeCount;

  if (NULL == Table) {
    DEBUG ((DEBUG_ERROR, "SLIT: Error while processing remaining valid nodes\n"));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "SLIT: Fill in the rest of the SLIT table\n"));

  SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

  NodeCount = (UINT8)SlitAcpiTable->Header.NumberOfSystemLocalities;
  MaxTableEntries = NodeCount * NodeCount;

  while (EntryIdx < MaxTableEntries) {
    if (SlitAcpiTable->NumSlit[EntryIdx].Entry == 0xFF) {
      //
      // This entry has not been filled yet, assign 2 hop to this table entry
      //
      SlitAcpiTable->NumSlit[EntryIdx].Entry = TWO_HOP;
    }

    if ((EntryIdx % NodeCount) == 0) {
      DEBUG ((DEBUG_INFO, "[%2d - %2d] ", EntryIdx/NodeCount, EntryIdx%NodeCount));
    }
    EntryIdx++;
  }

  return EFI_SUCCESS;
}

/**
  Processes unused SLIT nodes

  @param[in,out] Table  pointer to SLIT ACPI tables

  @retval EFI_SUCCESS  operation completed successfully

**/
EFI_STATUS
ProcessUnusedNodes (
  IN OUT EFI_ACPI_COMMON_HEADER *Table
  )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  UINT16                                  MaxTableEntries;
  UINT8                                   NodeCount;

  if (NULL == Table) {
    DEBUG ((DEBUG_ERROR, "SLIT: Error while processing unused nodes\n"));
    return EFI_INVALID_PARAMETER;

  }

  DEBUG ((DEBUG_INFO, "SLIT: Zero out the unused nodes\n"));

  SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

  NodeCount = (UINT8)SlitAcpiTable->Header.NumberOfSystemLocalities;
  MaxTableEntries = NodeCount * NodeCount;

  SetMem (&SlitAcpiTable->NumSlit[MaxTableEntries],
         (UINTN)&SlitAcpiTable->NumSlit[EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT] - (UINTN)&SlitAcpiTable->NumSlit[MaxTableEntries], 0);

  return EFI_SUCCESS;
}

/**
  Updates System Locality Distance Information Table (SLIT)

  @param[in,out] Table  pointer to SLIT ACPI tables

  @retval EFI_SUCCESS  operation completed successfully
  @retval EFI_ABORTED  operation not completed due to processing error

**/
EFI_STATUS
PatchSLitTable (
   IN OUT EFI_ACPI_COMMON_HEADER  *Table
   )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SlitAcpiTable;
  EFI_STATUS                              Status;
  UINT8                                   NodeCount;
  UINT8                                   NumCpus;
  UINT8                                   NumClusters;
  UINT8                                   PmemNodeCount;
  UINT8                                   FpgaCount;

  NumCpus = GetNumCpus ();
  NumClusters = GetNumClusters ();
  PmemNodeCount = GetPmemNodeCount ();
  FpgaCount = GetFpgaCount();
  NodeCount = GetNodeCount (NumCpus, NumClusters, PmemNodeCount, FpgaCount);

  DEBUG ((DEBUG_INFO, "SLIT: NumCpus %d, NumClusters %d, PmemNodeCount %d -> NodeCount %d FpgaCount %d\n",
                             NumCpus, NumClusters, PmemNodeCount, NodeCount,FpgaCount));

  SlitAcpiTable = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;
  SlitAcpiTable->Header.NumberOfSystemLocalities = NodeCount;

  //
  // 1) Initialize all entries to 0xFF
  //
  Status = InitEntries (SlitAcpiTable);

  //
  // 2) Update SLIT table with data about nodes on same socket
  //
  if (!EFI_ERROR(Status)) {
    if (mSystemMemoryMap->volMemMode == VOL_MEM_MODE_MIX_1LM2LM) {
      Status = ProcessMixedModeSockets (Table, NumCpus, NumClusters, PmemNodeCount, FpgaCount, NodeCount);
    } else {
      Status = ProcessSockets (Table, NumCpus, NumClusters, PmemNodeCount, FpgaCount, NodeCount);
    }
  }

  //
  // 3) Update table with links between sockets by examining PeerInfo structure
  //
  if (!EFI_ERROR (Status)) {
    if (mSystemMemoryMap->volMemMode == VOL_MEM_MODE_MIX_1LM2LM) {
      Status = ProcessMixedModeSocketsLinks (Table, NumCpus, NumClusters, PmemNodeCount, FpgaCount, NodeCount);
    } else {
      Status = ProcessSocketsLinks (Table, NumCpus, NumClusters, PmemNodeCount, FpgaCount, NodeCount);
    }
  }

  //
  // 4) Update table with PMEMs
  //
  if (!EFI_ERROR (Status)) {
    Status = ProcessPmems (Table, NumCpus, NumClusters, PmemNodeCount, FpgaCount, NodeCount);
  }

  //
  // 5 Update table with FPGA
  //
  if (!EFI_ERROR (Status)) {
    Status = ProcessFpgaNodes (Table, NumClusters, PmemNodeCount, FpgaCount, NodeCount);
  }

  //
  // 6) Fill in the rest of the Slit table, 2 hops between any remaining valid nodes
  //
  if (!EFI_ERROR (Status)) {
    Status = ProcessRemainingNodes (Table);
  }

  //
  // 7) Zero out the unused nodes
  //
  if (!EFI_ERROR (Status)) {
    Status = ProcessUnusedNodes (Table);
  }

  //
  // 8) Print the entire SLIT table
  //
  if (!EFI_ERROR (Status)) {
    DisplayEntries (SlitAcpiTable);
  }

  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Allocate memory and fill SLIT information to this buffer, then
  install this table to ACPI table.

  @retval EFI_SUCCESS           Install table success.
  @retval EFI_OUT_OF_RESOURCES  Out of memory.

**/
EFI_STATUS
InstallSlitTable (
   VOID
   )
{
  ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *Table;
  UINTN                                  TableSize;
  UINTN                                  TableHandle = 0;
  EFI_STATUS                             Status;

  TableSize = sizeof (EFI_ACPI_6_2_SYSTEM_LOCALITY_DISTANCE_INFORMATION_TABLE_HEADER) +
              sizeof (ACPI_SYSTEM_LOCALITIES_STRUCTURE) * EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT;

  Table = (ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *) AllocateZeroPool (TableSize);
  if (Table == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Table->Header.Header.Signature = EFI_ACPI_6_2_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE;
  Table->Header.Header.Length = (UINT32) TableSize;
  Table->Header.Header.Revision = EFI_ACPI_6_2_SYSTEM_LOCALITY_DISTANCE_INFORMATION_TABLE_REVISION;
  Table->Header.Header.OemRevision = EFI_ACPI_OEM_SLIT_REVISION;
  CopyMem (Table->Header.Header.OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof (Table->Header.Header.OemId));
  Table->Header.Header.OemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
  Table->Header.Header.CreatorId = PcdGet32 (PcdAcpiDefaultCreatorId);
  Table->Header.Header.CreatorRevision = PcdGet32 (PcdAcpiDefaultCreatorRevision);

  //
  // All node init with 0 before pass to patcher
  //
  PatchSLitTable ((EFI_ACPI_COMMON_HEADER *)&Table->Header.Header);

  //
  // Publish SLIT Structure to ACPI
  //

  Status = mAcpiTable->InstallAcpiTable (
                          mAcpiTable,
                          Table,
                          Table->Header.Header.Length,
                          &TableHandle
                          );

  //
  // Free memory
  //
  if (Table != NULL) {
    FreePool (Table);
  }

  return Status;
}
