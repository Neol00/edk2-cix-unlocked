/** @file Pci.c
  PCI Initialization for PIIX4 QEMU

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PlatformInit.h"
#include <IndustryStandard/Pci.h>
#include <Library/PciCf8Lib.h>
#include <Library/QemuOpenFwCfgLib.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>

/**
  Initialize PCI support for QEMU PIIX4 machine.

  It also publishes PCI MMIO and IO ranges PCDs for OVMF PciHostBridgeLib.

  @retval EFI_SUCCESS      Initialization was a success.
  @retval EFI_UNSUPPORTED  Initialization failed (Memory below 4Gb probing failed).
**/
EFI_STATUS
EFIAPI
InitializePciPIIX4 (
  VOID
  )
{
  UINTN  PciIoBase;
  UINTN  PciIoSize;
  UINTN  PciMmio32Base;
  UINTN  PciMmio32Size;

  //
  //  Setup PCI IO ranges for 440FX/PIIX4 platform
  //
  PciIoBase = PIIX4_PCI_IO_BASE;
  PciIoSize = PIIX4_PCI_IO_SIZE;

  PcdSet64S (PcdPciIoBase, PciIoBase);
  PcdSet64S (PcdPciIoSize, PciIoSize);

  //
  //  QEMU only allow a maximum of 2.8Gb of real memory below 4G
  //  PCI MMIO range below 4Gb starts at the end of real memory below 4G
  //
  PciMmio32Base = (UINTN)GetMemoryBelow4Gb ();

  if (PciMmio32Base == 0) {
    DEBUG ((DEBUG_ERROR, "Unable to detect memory below 4Gb\n"));
    ASSERT (PciMmio32Base != 0);
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_ERROR, "Memory below 4Gb: %x \n", PciMmio32Base));

  //
  // Maximum size being PCI_MMIO_TOP_ADDRESS - TopOfLowMem to avoid overlapping with IO-APIC and other hardware mmio ranges
  //
  PciMmio32Size = PCI_MMIO_TOP_ADDRESS - PciMmio32Base;

  PcdSet64S (PcdPciMmio32Base, PciMmio32Base);
  PcdSet64S (PcdPciMmio32Size, PciMmio32Size);

  return EFI_SUCCESS;
}
