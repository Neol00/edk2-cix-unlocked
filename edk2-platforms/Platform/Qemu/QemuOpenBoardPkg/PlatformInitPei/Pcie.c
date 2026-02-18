/** @file Pcie.c
  PCI Express initialization for QEMU Q35

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PlatformInit.h"
#include <IndustryStandard/Pci.h>
#include <Library/PciCf8Lib.h>
#include <IndustryStandard/Q35MchIch9.h>
#include <Library/QemuOpenFwCfgLib.h>
#include <IndustryStandard/QemuFwCfg.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>

/**
  Initialize PCI Express support for QEMU Q35 system.
  It also publishes PCI MMIO and IO ranges PCDs for OVMF PciHostBridgeLib.

  @retval EFI_SUCCESS Initialization was successful
**/
EFI_STATUS
EFIAPI
InitializePcie (
  VOID
  )
{
  UINTN  PciBase;
  UINTN  PciSize;
  UINTN  PciIoBase;
  UINTN  PciIoSize;

  union {
    UINT64    Uint64;
    UINT32    Uint32[2];
  } PciExBarBase;

  PciExBarBase.Uint64 = FixedPcdGet64 (PcdPciExpressBaseAddress);

  //
  // Build a reserved memory space for PCIE MMIO
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_RESERVED,
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_TESTED,
    PciExBarBase.Uint64,
    SIZE_256MB
    );

  BuildMemoryAllocationHob (
    PciExBarBase.Uint64,
    SIZE_256MB,
    EfiReservedMemoryType
    );

  //
  // Clear lower 32 bits of register
  //
  PciWrite32 (DRAMC_REGISTER_Q35 (MCH_PCIEXBAR_LOW), 0);

  //
  // Program PCIE MMIO Base address in MCH PCIEXBAR register
  //
  PciWrite32 (DRAMC_REGISTER_Q35 (MCH_PCIEXBAR_HIGH), PciExBarBase.Uint32[1]);

  //
  // Enable 256Mb MMIO space
  //
  PciWrite32 (
    DRAMC_REGISTER_Q35 (MCH_PCIEXBAR_LOW),
    PciExBarBase.Uint32[0] | MCH_PCIEXBAR_BUS_FF | MCH_PCIEXBAR_EN
    );

  //
  // Disable PCI/PCIe MMIO above 4Gb
  //
  PcdSet64S (PcdPciMmio64Size, 0);

  //
  // Set Pci MMIO space below 4GB
  //
  PciBase = (UINTN)(PcdGet64 (PcdPciExpressBaseAddress) + SIZE_256MB);
  PciSize = PCI_MMIO_TOP_ADDRESS - PciBase;

  PcdSet64S (PcdPciMmio32Base, PciBase);
  PcdSet64S (PcdPciMmio32Size, PciSize);

  //
  // Set Pci IO port range
  //
  PciIoBase = Q35_PCI_IO_BASE;
  PciIoSize = Q35_PCI_IO_SIZE;

  PcdSet64S (PcdPciIoBase, PciIoBase);
  PcdSet64S (PcdPciIoSize, PciIoSize);

  return EFI_SUCCESS;
}
