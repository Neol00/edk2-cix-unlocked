/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 1996 - 2015 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"


extern BIOS_ACPI_PARAM              *mAcpiParameter;
extern EFI_IIO_UDS_PROTOCOL         *mIioUds;
extern CPU_CSR_ACCESS_VAR           *mCpuCsrAccessVarPtr;
extern SYSTEM_CONFIGURATION         mSystemConfiguration;

EFI_STATUS
PatchMcfgAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  )
{
  UINT8     NodeId;
  UINT8     NodeCount;
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *McfgTable;

  McfgTable = (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *)Table;

  //
  // mAcpiParameter memory buffer has been zero'ed out, so mAcpiParameter->PcieSegNum[] are 0's
  // Patch \_SB.PSYS.SGEN with User Setup Option data
  //
  //
  // dynamically allow multi-seg support
  //
  mAcpiParameter->PcieMultiSegSupport = 0;
  for (NodeId = 0; NodeId < MAX_SOCKET; NodeId++) {
    if ((UINT16) (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment) > 0) {
      mAcpiParameter->PcieMultiSegSupport = 1;
      break;
    }
  }

  //
  // Update MCFG table entries (segment number, base addr and start/end bus numbers)
  //
  if (mAcpiParameter->PcieMultiSegSupport == 0) {

    //
    // Original code for single  PCIe segment start
    //
    McfgTable->Segment[0].BaseAddress = mIioUds->IioUdsPtr->PlatformData.PciExpressBase;
    McfgTable->Segment[0].EndBusNumber = (UINT8)RShiftU64 (mIioUds->IioUdsPtr->PlatformData.PciExpressSize, 20) - 1;
    //
    // Original code for single  PCIe segment end
    //

    //
    // Single segment with segment number as 0
    //
    McfgTable->Segment[0].PciSegmentGroupNumber = 0;
    NodeCount = 1;

  } else {
    //
    // PCIe Multi-Segment handling - Assume each CPU socket as a segment, and copy Segement info from IioUds HOB to MCFG table entries
    //

    //
    // Segment count = 0
    //
    NodeCount = 0;

    for (NodeId = 0; NodeId < MAX_SOCKET; NodeId++) {

      //
      // Skip a socket if it does not exist or does not contain valid bus range data
      //
      if ( (UINT8)(mCpuCsrAccessVarPtr->SocketLastBus[NodeId]) ==
            (UINT8)(mCpuCsrAccessVarPtr->SocketFirstBus[NodeId]) ) {
        continue;
      }

      //
      // Copy PCIe Segement info from IioUds HOB to MCFG table entries
      //
      McfgTable->Segment[NodeCount].PciSegmentGroupNumber = (UINT16)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment);

      McfgTable->Segment[NodeCount].BaseAddress = \
        LShiftU64 (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SegMmcfgBase.hi, 32) + \
        (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SegMmcfgBase.lo);

      McfgTable->Segment[NodeCount].StartBusNumber = (UINT8)(mCpuCsrAccessVarPtr->SocketFirstBus[NodeId]);

      McfgTable->Segment[NodeCount].EndBusNumber = (UINT8)(mCpuCsrAccessVarPtr->SocketLastBus[NodeId]);

      //
      // Update segment number returned by AML  _SEG() .  It resides in mAcpiParameter region now.
      //
      mAcpiParameter->PcieSegNum[NodeId] = (UINT8)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment);

      //
      // Update count of valid segments
      //
      NodeCount++;
    }
  }

  //
  // Set MCFG table "Length" field based on the number of PCIe segments enumerated so far
  //
  McfgTable->Header.Header.Length = \
    sizeof (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER) + \
    sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE) * NodeCount;

  //
  // Debug dump of MCFG table
  //
  DEBUG ((DEBUG_ERROR, "ACPI MCFG table @ address 0x%x\n", Table ));
  DEBUG ((DEBUG_ERROR, "  Multi-Seg Support = %x\n", mAcpiParameter->PcieMultiSegSupport));
  DEBUG ((DEBUG_ERROR, "  Number of Segments (sockets): %2d\n", NodeCount ));
  DEBUG ((DEBUG_ERROR, "  Table Length = 0x%x\n\n", McfgTable->Header.Header.Length ));
  for (NodeId = 0; NodeId < NodeCount; NodeId ++) {
    DEBUG ((DEBUG_ERROR, "   Segment[%2d].BaseAddress = %x\n",  NodeId, McfgTable->Segment[NodeId].BaseAddress));
    DEBUG ((DEBUG_ERROR, "   Segment[%2d].PciSegmentGroupNumber = %x\n", NodeId, McfgTable->Segment[NodeId].PciSegmentGroupNumber));
    DEBUG ((DEBUG_ERROR, "   Segment[%2d].StartBusNumber = %x\n", NodeId, McfgTable->Segment[NodeId].StartBusNumber));
    DEBUG ((DEBUG_ERROR, "   Segment[%2d].EndBusNumber = %x\n\n", NodeId, McfgTable->Segment[NodeId].EndBusNumber));
  }

  return EFI_SUCCESS;
}
