/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "AcpiPlatform.h"

EFI_ACPI_6_3_GIC_ITS_STRUCTURE GicItsTemplate = {
  EFI_ACPI_6_3_GIC_ITS,
  sizeof (EFI_ACPI_6_3_GIC_ITS_STRUCTURE),
  EFI_ACPI_RESERVED_WORD,
  0, /* GicItsId */
  0, /* PhysicalBaseAddress */
  0, /* Reserved2 */
};

EFI_ACPI_6_3_GICR_STRUCTURE GicRTemplate = {
  EFI_ACPI_6_3_GICR,
  sizeof (EFI_ACPI_6_3_GICR_STRUCTURE),
  EFI_ACPI_RESERVED_WORD,
  AC01_GICR_MASTER_BASE_ADDRESS, /* DiscoveryRangeBaseAddress */
  0x1000000,            /* DiscoveryRangeLength */
};

EFI_ACPI_6_3_GIC_DISTRIBUTOR_STRUCTURE GicDTemplate = {
  EFI_ACPI_6_3_GICD,
  sizeof (EFI_ACPI_6_3_GIC_DISTRIBUTOR_STRUCTURE),
  EFI_ACPI_RESERVED_WORD,
  0,             /* GicDistHwId */
  AC01_GICD_MASTER_BASE_ADDRESS, /* GicDistBase */
  0,             /* GicDistVector */
  0x3,           /* GicVersion */
  {EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE}
};

EFI_ACPI_6_3_GIC_STRUCTURE GiccTemplate = {
  EFI_ACPI_6_3_GIC,
  sizeof (EFI_ACPI_6_3_GIC_STRUCTURE),
  EFI_ACPI_RESERVED_WORD,
  0, /* GicId */
  0, /* AcpiCpuUid */
  0, /* Flags */
  0,
  23, /* PmuIrq */
  0,
  0,
  0,
  0,
  25, /* GsivId */
  0,  /* GicRBase */
  0,  /* Mpidr */
  0,  /* ProcessorPowerEfficiencyClass */
  0,  /* Reserved2 */
  21, /* SPE irq */
};

EFI_ACPI_6_3_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER MADTTableHeaderTemplate = {
  __ACPI_HEADER (
    EFI_ACPI_6_3_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
    0, /* need fill in */
    EFI_ACPI_6_3_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION
    ),
};

UINT32 Ac01CoreOrderMonolithic[PLATFORM_CPU_MAX_CPM * PLATFORM_CPU_NUM_CORES_PER_CPM] = {
  36, 52, 40, 56, 32, 48, 44, 60,
  20, 68, 24, 72, 16, 64, 28, 76,
  4, 8, 0, 12, 38, 54, 42, 58,
  34, 50, 46, 62, 22, 70, 26, 74,
  18, 66, 30, 78, 6, 10, 2, 14,
  37, 53, 41, 57, 33, 49, 45, 61,
  21, 69, 25, 73, 17, 65, 29, 77,
  5, 9, 1, 13, 39, 55, 43, 59,
  35, 51, 47, 63, 23, 71, 27, 75,
  19, 67, 31, 79, 7, 11, 3, 15,
};

UINT32 Ac01CoreOrderHemisphere[PLATFORM_CPU_MAX_CPM * PLATFORM_CPU_NUM_CORES_PER_CPM] = {
  32, 48, 16, 64, 36, 52, 0, 20,
  68, 4, 34, 50, 18, 66, 38, 54,
  2, 22, 70, 6, 33, 49, 17, 65,
  37, 53, 1, 21, 69, 5, 35, 51,
  19, 67, 39, 55, 3, 23, 71, 7,
  44, 60, 28, 76, 40, 56, 12, 24,
  72, 8, 46, 62, 30, 78, 42, 58,
  14, 26, 74, 10, 45, 61, 29, 77,
  41, 57, 13, 25, 73, 9, 47, 63,
  31, 79, 43, 59, 15, 27, 75, 11,
};

UINT32 Ac01CoreOrderQuadrant[PLATFORM_CPU_MAX_CPM * PLATFORM_CPU_NUM_CORES_PER_CPM] = {
  16, 32, 0, 20, 4, 18, 34, 2,
  22, 6, 17, 33, 1, 21, 5, 19,
  35, 3, 23, 7, 48, 64, 52, 68,
  36, 50, 66, 54, 70, 38, 49, 65,
  53, 69, 37, 51, 67, 55, 71, 39,
  28, 44, 12, 24, 8, 30, 46, 14,
  26, 10, 29, 45, 13, 25, 9, 31,
  47, 15, 27, 11, 60, 76, 56, 72,
  40, 62, 78, 58, 74, 42, 61, 77,
  57, 73, 41, 63, 79, 59, 75, 43,
};

EFI_ACPI_6_3_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *MadtTablePointer;

UINT32 *
CpuGetCoreOrder (
  VOID
  )
{
  UINT8              SubNumaMode;

  SubNumaMode = CpuGetSubNumaMode ();
  switch (SubNumaMode) {
  case SUBNUMA_MODE_MONOLITHIC:
    return (UINT32 *)&Ac01CoreOrderMonolithic;

  case SUBNUMA_MODE_HEMISPHERE:
    return (UINT32 *)&Ac01CoreOrderHemisphere;

  case SUBNUMA_MODE_QUADRANT:
    return (UINT32 *)&Ac01CoreOrderQuadrant;

  default:
    // Should never reach here
    ASSERT (FALSE);
    return NULL;
  }

  return NULL;
}

UINT32
AcpiInstallMadtProcessorNode (
  VOID   *EntryPointer,
  UINT32 CpuId
  )
{
  EFI_ACPI_6_3_GIC_STRUCTURE *MadtProcessorEntryPointer = EntryPointer;
  UINT32                     SocketId;
  UINT32                     ClusterId;
  UINTN                      Size;

  Size = sizeof (GiccTemplate);
  CopyMem (MadtProcessorEntryPointer, &GiccTemplate, Size);

  SocketId = SOCKET_ID (CpuId);
  ClusterId = CLUSTER_ID (CpuId);

  //
  // GICv2 compatibility mode is not supported.
  // Hence, set GIC's CPU Interface Number to 0.
  //
  MadtProcessorEntryPointer->CPUInterfaceNumber = 0;
  MadtProcessorEntryPointer->AcpiProcessorUid =
    (SocketId << PLATFORM_SOCKET_UID_BIT_OFFSET) +
    (ClusterId << 8) + (CpuId  % PLATFORM_CPU_NUM_CORES_PER_CPM);
  MadtProcessorEntryPointer->Flags = 1;
  MadtProcessorEntryPointer->MPIDR =
    (((ClusterId << 8) + (CpuId  % PLATFORM_CPU_NUM_CORES_PER_CPM)) << 8);
  MadtProcessorEntryPointer->MPIDR += (((UINT64)SocketId) << 32);

  return Size;
}

UINT32
AcpiInstallMadtGicD (
  VOID *EntryPointer
  )
{
  EFI_ACPI_6_3_GIC_DISTRIBUTOR_STRUCTURE *GicDEntryPointer = EntryPointer;
  UINTN                                  Size;

  Size = sizeof (GicDTemplate);
  CopyMem (GicDEntryPointer, &GicDTemplate, Size);

  return Size;
}

UINT32
AcpiInstallMadtGicR (
  VOID   *EntryPointer,
  UINT32 SocketId
  )
{
  EFI_ACPI_6_3_GICR_STRUCTURE *GicREntryPointer = EntryPointer;
  UINTN                       Size;

  /*
   * If the Slave socket is not present, discard the Slave socket
   * GIC redistributor region
   */
  if (SocketId == 1 && !IsSlaveSocketActive ()) {
    return 0;
  }

  Size = sizeof (GicRTemplate);
  CopyMem (GicREntryPointer, &GicRTemplate, Size);

  if (SocketId == 1) {
    GicREntryPointer->DiscoveryRangeBaseAddress = AC01_GICR_SLAVE_BASE_ADDRESS;
  }

  return Size;
}

UINT32
AcpiInstallMadtGicIts (
  VOID   *EntryPointer,
  UINT32 Index
  )
{
  EFI_ACPI_6_3_GIC_ITS_STRUCTURE *GicItsEntryPointer = EntryPointer;
  UINTN                          Size, Offset;
  UINT64                         GicBase = AC01_GICD_MASTER_BASE_ADDRESS;
  UINT32                         ItsId = Index;

  if (Index > SOCKET0_LAST_RC) { /* Socket 1, Index: 8-15 */
    GicBase = AC01_GICD_SLAVE_BASE_ADDRESS;
    Index -= (SOCKET0_LAST_RC + 1); /* Socket 1, Index:8 -> RCA0 */
  }
  Size = sizeof (GicItsTemplate);
  CopyMem (GicItsEntryPointer, &GicItsTemplate, Size);
  Offset = 0x40000 + Index * 0x20000;
  GicItsEntryPointer->GicItsId = ItsId;
  GicItsEntryPointer->PhysicalBaseAddress = Offset + GicBase;

  return Size;
}

/*
 *  Install MADT table.
 */
EFI_STATUS
AcpiInstallMadtTable (
  VOID
  )
{
  EFI_ACPI_6_3_GIC_STRUCTURE *GiccEntryPointer = NULL;
  EFI_ACPI_TABLE_PROTOCOL    *AcpiTableProtocol;
  UINTN                      MadtTableKey  = 0;
  INTN                       Index;
  EFI_STATUS                 Status;
  UINTN                      Size;
  UINT32                     *CoreOrder;
  UINT32                     SktMaxCoreNum;

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID **)&AcpiTableProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Size = sizeof (MADTTableHeaderTemplate) +
          (PLATFORM_CPU_MAX_NUM_CORES * sizeof (GiccTemplate)) +
          sizeof (GicDTemplate) +
          (PLATFORM_CPU_MAX_SOCKET * sizeof (GicRTemplate)) +
          ((SOCKET0_LAST_RC - SOCKET0_FIRST_RC +  1) * sizeof (GicItsTemplate));
  if (IsSlaveSocketActive ()) {
    Size += ((SOCKET1_LAST_RC - SOCKET1_FIRST_RC +  1) * sizeof (GicItsTemplate));
  } else if (!IsSlaveSocketAvailable ()) {
    Size += 2 * sizeof (GicItsTemplate); /* RCA0/1 */
  }

  MadtTablePointer =
    (EFI_ACPI_6_3_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *)AllocateZeroPool (Size);
  if (MadtTablePointer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  GiccEntryPointer =
    (EFI_ACPI_6_3_GIC_STRUCTURE *)((UINT64)MadtTablePointer +
                                    sizeof (MADTTableHeaderTemplate));

  /* Install Gic interface for each processor */
  Size = 0;
  CoreOrder = CpuGetCoreOrder ();
  ASSERT (CoreOrder != NULL);
  SktMaxCoreNum = PLATFORM_CPU_MAX_CPM * PLATFORM_CPU_NUM_CORES_PER_CPM;
  for (Index = 0; Index < SktMaxCoreNum; Index++) {
    if (IsCpuEnabled (CoreOrder[Index])) {
      Size += AcpiInstallMadtProcessorNode ((VOID *)((UINT64)GiccEntryPointer + Size), CoreOrder[Index]);
    }
  }

  for (Index = 0; Index < SktMaxCoreNum; Index++) {
    if (IsCpuEnabled (CoreOrder[Index] + SktMaxCoreNum)) {
      Size += AcpiInstallMadtProcessorNode ((VOID *)((UINT64)GiccEntryPointer + Size), CoreOrder[Index] + SktMaxCoreNum);
    }
  }

  /* Install Gic Distributor */
  Size += AcpiInstallMadtGicD ((VOID *)((UINT64)GiccEntryPointer + Size));

  /* Install Gic Redistributor */
  for (Index = 0; Index < PLATFORM_CPU_MAX_SOCKET; Index++) {
    Size += AcpiInstallMadtGicR ((VOID *)((UINT64)GiccEntryPointer + Size), Index);
  }

  /* Install Gic ITS */
  if (!IsSlaveSocketAvailable ()) {
    for (Index = 0; Index <= 1; Index++) { /* RCA0/1 */
      Size += AcpiInstallMadtGicIts ((VOID *)((UINT64)GiccEntryPointer + Size), Index);
    }
  }
  for (Index = SOCKET0_FIRST_RC; Index <= SOCKET0_LAST_RC; Index++) {
    Size += AcpiInstallMadtGicIts ((VOID *)((UINT64)GiccEntryPointer + Size), Index);
  }
  if (IsSlaveSocketActive ()) {
    for (Index = SOCKET1_FIRST_RC; Index <= SOCKET1_LAST_RC; Index++) {
      Size += AcpiInstallMadtGicIts ((VOID *)((UINT64)GiccEntryPointer + Size), Index);
    }
  }
  CopyMem (
    MadtTablePointer,
    &MADTTableHeaderTemplate,
    sizeof (MADTTableHeaderTemplate)
    );

  Size += sizeof (MADTTableHeaderTemplate);
  MadtTablePointer->Header.Length = Size;
  CopyMem (
    MadtTablePointer->Header.OemId,
    PcdGetPtr (PcdAcpiDefaultOemId),
    sizeof (MadtTablePointer->Header.OemId)
    );

  AcpiUpdateChecksum ((UINT8 *)MadtTablePointer, MadtTablePointer->Header.Length);

  Status = AcpiTableProtocol->InstallAcpiTable (
                                AcpiTableProtocol,
                                (VOID *)MadtTablePointer,
                                MadtTablePointer->Header.Length,
                                &MadtTableKey
                                );
  FreePool ((VOID *)MadtTablePointer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
