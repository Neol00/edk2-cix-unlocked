/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PciSegmentLib.h>
#include <IndustryStandard/Vtd.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/IoMmu.h>
#include <Ppi/VtdInfo.h>

#include "IntelVTdDmarPei.h"

/**
  Dump DMAR DeviceScopeEntry.

  @param[in]  DmarDeviceScopeEntry      DMAR DeviceScopeEntry
**/
VOID
DumpDmarDeviceScopeEntry (
  IN EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER  *DmarDeviceScopeEntry
  )
{
  UINTN                                 PciPathNumber;
  UINTN                                 PciPathIndex;
  EFI_ACPI_DMAR_PCI_PATH                *PciPath;

  if (DmarDeviceScopeEntry == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "    *************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "    *       DMA-Remapping Device Scope Entry Structure                      *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "    *************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof (UINTN) == sizeof (UINT64)) ?
    "    DMAR Device Scope Entry address ...................... 0x%016lx\n" :
    "    DMAR Device Scope Entry address ...................... 0x%08x\n",
    DmarDeviceScopeEntry
    ));
  DEBUG ((DEBUG_INFO,
    "      Device Scope Entry Type ............................ 0x%02x\n",
    DmarDeviceScopeEntry->Type
    ));
  switch (DmarDeviceScopeEntry->Type) {
  case EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT:
    DEBUG ((DEBUG_INFO,
      "        PCI Endpoint Device\n"
      ));
    break;
  case EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE:
    DEBUG ((DEBUG_INFO,
      "        PCI Sub-hierachy\n"
      ));
    break;
  default:
    break;
  }
  DEBUG ((DEBUG_INFO,
    "      Length ............................................. 0x%02x\n",
    DmarDeviceScopeEntry->Length
    ));
  DEBUG ((DEBUG_INFO,
    "      Enumeration ID ..................................... 0x%02x\n",
    DmarDeviceScopeEntry->EnumerationId
    ));
  DEBUG ((DEBUG_INFO,
    "      Starting Bus Number ................................ 0x%02x\n",
    DmarDeviceScopeEntry->StartBusNumber
    ));

  PciPathNumber = (DmarDeviceScopeEntry->Length - sizeof (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER)) / sizeof (EFI_ACPI_DMAR_PCI_PATH);
  PciPath = (EFI_ACPI_DMAR_PCI_PATH *) (DmarDeviceScopeEntry + 1);
  for (PciPathIndex = 0; PciPathIndex < PciPathNumber; PciPathIndex++) {
    DEBUG ((DEBUG_INFO,
      "      Device ............................................. 0x%02x\n",
      PciPath[PciPathIndex].Device
      ));
    DEBUG ((DEBUG_INFO,
      "      Function ........................................... 0x%02x\n",
      PciPath[PciPathIndex].Function
      ));
  }

  DEBUG ((DEBUG_INFO,
    "    *************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR DRHD table.

  @param[in]  Drhd              DMAR DRHD table
**/
VOID
DumpDmarDrhd (
  IN EFI_ACPI_DMAR_DRHD_HEADER  *Drhd
  )
{
  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER     *DmarDeviceScopeEntry;
  INTN                                            DrhdLen;

  if (Drhd == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  *       DMA-Remapping Hardware Definition Structure                       *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof (UINTN) == sizeof (UINT64)) ?
    "  DRHD address ........................................... 0x%016lx\n" :
    "  DRHD address ........................................... 0x%08x\n",
    Drhd
    ));
  DEBUG ((DEBUG_INFO,
    "    Type ................................................. 0x%04x\n",
    Drhd->Header.Type
    ));
  DEBUG ((DEBUG_INFO,
    "    Length ............................................... 0x%04x\n",
    Drhd->Header.Length
    ));
  DEBUG ((DEBUG_INFO,
    "    Flags ................................................ 0x%02x\n",
    Drhd->Flags
    ));
  DEBUG ((DEBUG_INFO,
    "      INCLUDE_PCI_ALL .................................... 0x%02x\n",
    Drhd->Flags & EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_PCI_ALL
    ));
  DEBUG ((DEBUG_INFO,
    "    Segment Number ....................................... 0x%04x\n",
    Drhd->SegmentNumber
    ));
  DEBUG ((DEBUG_INFO,
    "    Register Base Address ................................ 0x%016lx\n",
    Drhd->RegisterBaseAddress
    ));

  DrhdLen  = Drhd->Header.Length - sizeof (EFI_ACPI_DMAR_DRHD_HEADER);
  DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *) (Drhd + 1);
  while (DrhdLen > 0) {
    DumpDmarDeviceScopeEntry (DmarDeviceScopeEntry);
    DrhdLen -= DmarDeviceScopeEntry->Length;
    DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *) ((UINTN) DmarDeviceScopeEntry + DmarDeviceScopeEntry->Length);
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR ACPI table.

  @param[in]  Dmar              DMAR ACPI table
**/
VOID
DumpAcpiDMAR (
  IN EFI_ACPI_DMAR_HEADER       *Dmar
  )
{
  EFI_ACPI_DMAR_STRUCTURE_HEADER        *DmarHeader;
  INTN                                  DmarLen;

  if (Dmar == NULL) {
    return;
  }

  //
  // Dump Dmar table
  //
  DEBUG ((DEBUG_INFO,
    "*****************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "*         DMAR Table                                                        *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "*****************************************************************************\n"
    ));

  DEBUG ((DEBUG_INFO,
    (sizeof (UINTN) == sizeof (UINT64)) ?
    "DMAR address ............................................. 0x%016lx\n" :
    "DMAR address ............................................. 0x%08x\n",
    Dmar
    ));

  DEBUG ((DEBUG_INFO,
    "  Table Contents:\n"
    ));
  DEBUG ((DEBUG_INFO,
    "    Host Address Width ................................... 0x%02x\n",
    Dmar->HostAddressWidth
    ));
  DEBUG ((DEBUG_INFO,
    "    Flags ................................................ 0x%02x\n",
    Dmar->Flags
    ));
  DEBUG ((DEBUG_INFO,
    "      INTR_REMAP ......................................... 0x%02x\n",
    Dmar->Flags & EFI_ACPI_DMAR_FLAGS_INTR_REMAP
    ));
  DEBUG ((DEBUG_INFO,
    "      X2APIC_OPT_OUT_SET ................................. 0x%02x\n",
    Dmar->Flags & EFI_ACPI_DMAR_FLAGS_X2APIC_OPT_OUT
    ));
  DEBUG ((DEBUG_INFO,
    "      DMA_CTRL_PLATFORM_OPT_IN_FLAG ...................... 0x%02x\n",
    Dmar->Flags & EFI_ACPI_DMAR_FLAGS_DMA_CTRL_PLATFORM_OPT_IN_FLAG
    ));

  DmarLen  = Dmar->Header.Length - sizeof (EFI_ACPI_DMAR_HEADER);
  DmarHeader = (EFI_ACPI_DMAR_STRUCTURE_HEADER *) (Dmar + 1);
  while (DmarLen > 0) {
    switch (DmarHeader->Type) {
    case EFI_ACPI_DMAR_TYPE_DRHD:
      DumpDmarDrhd ((EFI_ACPI_DMAR_DRHD_HEADER *) DmarHeader);
      break;
    default:
      break;
    }
    DmarLen -= DmarHeader->Length;
    DmarHeader = (EFI_ACPI_DMAR_STRUCTURE_HEADER *) ((UINTN) DmarHeader + DmarHeader->Length);
  }

  DEBUG ((DEBUG_INFO,
    "*****************************************************************************\n\n"
    ));

  return;
}

/**
  Parse DMAR DRHD table.

  @param[in]  AcpiDmarTable     DMAR ACPI table
  @param[in]  Callback          Callback function for handle DRHD
  @param[in]  Context           Callback function Context

  @return the VTd engine number.

**/
UINTN
ParseDmarAcpiTableDrhd (
  IN EFI_ACPI_DMAR_HEADER               *AcpiDmarTable,
  IN PROCESS_DRHD_CALLBACK_FUNC         Callback,
  IN VOID                               *Context
  )
{
  EFI_ACPI_DMAR_STRUCTURE_HEADER        *DmarHeader;
  UINTN                                 VtdIndex;

  VtdIndex = 0;
  DmarHeader = (EFI_ACPI_DMAR_STRUCTURE_HEADER *) ((UINTN) (AcpiDmarTable + 1));

  while ((UINTN) DmarHeader < (UINTN) AcpiDmarTable + AcpiDmarTable->Header.Length) {
    switch (DmarHeader->Type) {
    case EFI_ACPI_DMAR_TYPE_DRHD:
      if (Callback != NULL) {
        Callback (Context, VtdIndex, (EFI_ACPI_DMAR_DRHD_HEADER *) DmarHeader);
      }
      VtdIndex++;
      break;
    default:
      break;
    }
    DmarHeader = (EFI_ACPI_DMAR_STRUCTURE_HEADER *) ((UINTN) DmarHeader + DmarHeader->Length);
  }

  return VtdIndex;
}

