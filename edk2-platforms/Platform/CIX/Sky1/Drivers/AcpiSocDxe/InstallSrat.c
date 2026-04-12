/** @file
  Install SRAT (System Resource Affinity Table) for the CIX Sky1 SoC.

  The Sky1 (CD8180) is a UMA design: the shared LPDDR5 memory controller
  services all five CPU clusters equally, so the entire platform lives in
  a single proximity domain (0).  This matters for NUMA-aware kernels that
  otherwise skip NUMA initialisation when no SRAT is present.

  Memory size varies across Orion O6 board variants (8 / 16 / 32 / 64 GB),
  so the table is built dynamically at ReadyToBoot time from the EFI memory
  map instead of using a fixed binary .aslc file.

  CPU topology (all domain 0, matches _UID order from mCoreIterOrder):
    _UID  0-1   Cortex-A720 Big G1  (boot cluster, MPIDR 0xA00-0xB00)
    _UID  2-3   Cortex-A720 Big G0  (MPIDR 0x800-0x900)
    _UID  4-5   Cortex-A720 Mid G0  (MPIDR 0x400-0x500)
    _UID  6-7   Cortex-A720 Mid G1  (MPIDR 0x600-0x700)
    _UID  8-11  Cortex-A520 Little   (MPIDR 0x000-0x300)

  Memory map topology (confirmed from MemoryMap.h):
    Low  DRAM bank : FCH_DDR_LOW_SYSHUB_OFFSET  = 0x0000_0000_8000_0000
    High DRAM bank : FCH_DDR_HIGH_SYSHUB_OFFSET = 0x0000_0080_0000_0000

  Copyright 2025 – author: ne
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "AcpiSocDxe.h"
#include <IndustryStandard/Acpi.h>  // redirects to Acpi64.h in this EDK2 tree

// Physical address below which everything is MMIO on this SoC (no DRAM).
// Mirrors FCH_DDR_LOW_SYSHUB_OFFSET from Silicon/CIX/Sky1/Include/MemoryMap.h
#define SRAT_DDR_LOW_BASE   0x80000000ULL

// ACPI 6.3 §5.2.16.6 GICC Affinity Flags, bit 0 = Enabled
// ACPI 6.3 §5.2.16.2 Memory Affinity Flags, bit 0 = Enabled
// These flag macros are absent from the MdePkg Acpi63.h in this toolchain.
#ifndef EFI_ACPI_6_4_GICC_AFFINITY_ENABLED
#define EFI_ACPI_6_4_GICC_AFFINITY_ENABLED    BIT0
#endif
#ifndef EFI_ACPI_6_4_MEMORY_AFFINITY_ENABLED
#define EFI_ACPI_6_4_MEMORY_AFFINITY_ENABLED  BIT0
#endif

// Number of CPU cores and the single proximity domain used for the UMA topology.
#define SRAT_CPU_COUNT      12
#define SRAT_PROXIMITY_DOM  0

/**
  Build and install a SRAT table derived from the current EFI memory map.

  Called via mAcpiFunctionOReadyToBootHook in AcpiSocDxe.c.

  @retval EFI_SUCCESS            Table installed.
  @retval EFI_OUT_OF_RESOURCES   Allocation failure.
  @retval other                  Error from GetMemoryMap or InstallAcpiTable.
**/
EFI_STATUS
EFIAPI
AcpiInstallSratTable (
  VOID
  )
{
  EFI_STATUS                                          Status;
  EFI_ACPI_TABLE_PROTOCOL                             *AcpiTable;
  UINTN                                               TableHandle;

  // EFI memory map variables
  UINTN                                               MapSize;
  UINTN                                               MapKey;
  UINTN                                               DescSize;
  UINT32                                              DescVer;
  EFI_MEMORY_DESCRIPTOR                               *MemMap;
  EFI_MEMORY_DESCRIPTOR                               *Desc;
  UINTN                                               NumDesc;
  UINTN                                               Idx;

  // SRAT output buffer
  UINTN                                               BufSize;
  UINT8                                               *Buf;
  UINT8                                               *Ptr;
  UINTN                                               FinalSize;
  UINTN                                               MemEntries;

  EFI_ACPI_6_4_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER  *Hdr;
  EFI_ACPI_6_4_GICC_AFFINITY_STRUCTURE                *GiccAff;
  EFI_ACPI_6_4_MEMORY_AFFINITY_STRUCTURE              *MemAff;

  // _UID values from Dsdt-CPU.asl, in order 0-11
  CONST UINT32  CpuUids[SRAT_CPU_COUNT] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

  // ----------------------------------------------------------------
  // 1. Retrieve the EFI memory map (two-call pattern per UEFI spec).
  // ----------------------------------------------------------------
  MapSize = 0;
  Status = gBS->GetMemoryMap (&MapSize, NULL, &MapKey, &DescSize, &DescVer);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    DEBUG ((DEBUG_ERROR, "SRAT: GetMemoryMap probe returned %r\n", Status));
    return Status;
  }

  // Add headroom: allocating memory can cause the map to grow by one entry.
  MapSize += 2 * DescSize;
  MemMap = AllocatePool (MapSize);
  if (MemMap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gBS->GetMemoryMap (&MapSize, MemMap, &MapKey, &DescSize, &DescVer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SRAT: GetMemoryMap failed: %r\n", Status));
    FreePool (MemMap);
    return Status;
  }
  NumDesc = MapSize / DescSize;

  // ----------------------------------------------------------------
  // 2. Allocate the SRAT output buffer.
  //    Worst case: every EFI descriptor becomes a memory affinity entry.
  // ----------------------------------------------------------------
  BufSize = sizeof (EFI_ACPI_6_4_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER)
          + (SRAT_CPU_COUNT * sizeof (EFI_ACPI_6_4_GICC_AFFINITY_STRUCTURE))
          + (NumDesc        * sizeof (EFI_ACPI_6_4_MEMORY_AFFINITY_STRUCTURE));

  Buf = AllocateZeroPool (BufSize);
  if (Buf == NULL) {
    FreePool (MemMap);
    return EFI_OUT_OF_RESOURCES;
  }
  Ptr = Buf;

  // ----------------------------------------------------------------
  // 3. Fill the SRAT header.
  // ----------------------------------------------------------------
  Hdr = (EFI_ACPI_6_4_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER *)Ptr;

  Hdr->Header.Signature        = EFI_ACPI_6_4_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE;
  Hdr->Header.Length           = 0;     // updated after all entries are written
  Hdr->Header.Revision         = EFI_ACPI_6_4_SYSTEM_RESOURCE_AFFINITY_TABLE_REVISION;
  Hdr->Header.Checksum         = 0;     // computed by InstallAcpiTable
  CopyMem (Hdr->Header.OemId, "CIXTEK", 6);
  Hdr->Header.OemTableId       = EFI_ACPI_OEM_TABLE_ID;
  Hdr->Header.OemRevision      = EFI_ACPI_OEM_REVISION;
  Hdr->Header.CreatorId        = EFI_ACPI_CREATOR_ID;
  Hdr->Header.CreatorRevision  = EFI_ACPI_CREATOR_REVISION;
  Hdr->Reserved1               = 1;    // ACPI spec: "Must be one"
  Hdr->Reserved2               = 0;

  Ptr += sizeof (EFI_ACPI_6_4_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER);

  // ----------------------------------------------------------------
  // 4. One GICC Affinity entry per CPU, all in proximity domain 0.
  // ----------------------------------------------------------------
  for (UINTN i = 0; i < SRAT_CPU_COUNT; i++) {
    GiccAff = (EFI_ACPI_6_4_GICC_AFFINITY_STRUCTURE *)Ptr;
    GiccAff->Type             = EFI_ACPI_6_4_GICC_AFFINITY;
    GiccAff->Length           = (UINT8)sizeof (EFI_ACPI_6_4_GICC_AFFINITY_STRUCTURE);
    GiccAff->ProximityDomain  = SRAT_PROXIMITY_DOM;
    GiccAff->AcpiProcessorUid = CpuUids[i];
    GiccAff->Flags            = EFI_ACPI_6_4_GICC_AFFINITY_ENABLED;
    GiccAff->ClockDomain      = 0;
    Ptr += sizeof (EFI_ACPI_6_4_GICC_AFFINITY_STRUCTURE);
  }

  // ----------------------------------------------------------------
  // 5. One Memory Affinity entry per EFI memory range that is DRAM.
  //
  //    Skip:
  //      - EfiMemoryMappedIO / EfiMemoryMappedIOPortSpace  (MMIO, not RAM)
  //      - EfiPalCode                                      (x86 only)
  //      - Regions below SRAT_DDR_LOW_BASE                 (SoC MMIO space)
  // ----------------------------------------------------------------
  MemEntries = 0;
  for (Idx = 0; Idx < NumDesc; Idx++) {
    Desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)MemMap + (Idx * DescSize));

    if ((Desc->Type == EfiMemoryMappedIO) ||
        (Desc->Type == EfiMemoryMappedIOPortSpace) ||
        (Desc->Type == EfiPalCode)) {
      continue;
    }

    if (Desc->PhysicalStart < SRAT_DDR_LOW_BASE) {
      continue;
    }

    MemAff = (EFI_ACPI_6_4_MEMORY_AFFINITY_STRUCTURE *)Ptr;
    MemAff->Type            = EFI_ACPI_6_4_MEMORY_AFFINITY;
    MemAff->Length          = (UINT8)sizeof (EFI_ACPI_6_4_MEMORY_AFFINITY_STRUCTURE);
    MemAff->ProximityDomain = SRAT_PROXIMITY_DOM;
    MemAff->Reserved1       = 0;
    MemAff->AddressBaseLow  = (UINT32)(Desc->PhysicalStart & 0xFFFFFFFFULL);
    MemAff->AddressBaseHigh = (UINT32)(Desc->PhysicalStart >> 32);
    MemAff->LengthLow       = (UINT32)((Desc->NumberOfPages * EFI_PAGE_SIZE) & 0xFFFFFFFFULL);
    MemAff->LengthHigh      = (UINT32)((Desc->NumberOfPages * EFI_PAGE_SIZE) >> 32);
    MemAff->Reserved2       = 0;
    MemAff->Flags           = EFI_ACPI_6_4_MEMORY_AFFINITY_ENABLED;
    MemAff->Reserved3       = 0;
    Ptr += sizeof (EFI_ACPI_6_4_MEMORY_AFFINITY_STRUCTURE);
    MemEntries++;
  }

  FreePool (MemMap);

  // ----------------------------------------------------------------
  // 6. Patch the total length and install.
  // ----------------------------------------------------------------
  FinalSize              = (UINTN)(Ptr - Buf);
  Hdr->Header.Length     = (UINT32)FinalSize;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SRAT: LocateProtocol(AcpiTable) failed: %r\n", Status));
    FreePool (Buf);
    return Status;
  }

  TableHandle = 0;
  Status = AcpiTable->InstallAcpiTable (AcpiTable, Buf, FinalSize, &TableHandle);
  FreePool (Buf);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SRAT: InstallAcpiTable failed: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO,
            "SRAT: installed — %u GICC + %u memory affinity entries "
            "(proximity domain 0, UMA)\n",
            (UINT32)SRAT_CPU_COUNT, (UINT32)MemEntries));
  }

  return Status;
}
