/** @file PcieWindowLib.h

  Runtime-selectable sizing of the PCIe 64-bit (above-4G) MMIO apertures.

  The stock Sky1 layout gives every root bridge exactly 16 GB, 16 GB aligned.
  That is too tight for a GPU whose Resizable BAR wants the full 16 GB: the BAR
  can only land on the window's own base address, leaving nothing for the
  device's remaining BARs or for the bridge window itself, so the resize fails.
  This library picks an alternative layout so that such a device fits.

  Copyright (c) 2025, Radxa Orion O6 custom firmware.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCIE_WINDOW_LIB_H_
#define _PCIE_WINDOW_LIB_H_

#include <Uefi.h>
#include <Library/PlatformPcieLib.h>

//
// Values of PLATFORM_SETUP_DATA.PcieBarWindow, and of the
// ARV_PCIE_MEM64_MODE_OFFSET byte handed to ACPI.
//
#define PCIE_WINDOW_MODE_AUTO   0
#define PCIE_WINDOW_MODE_STOCK  1
#define PCIE_WINDOW_MODE_LARGE  2
#define PCIE_WINDOW_MODE_HUGE   3
#define PCIE_WINDOW_MODE_MAX    4
#define PCIE_WINDOW_MODE_COUNT  5

//
// Every base and size this library produces is a whole multiple of 4 GB, which
// is what lets the layout be handed to ACPI as one byte per value.
//
#define PCIE_WINDOW_UNIT  SIZE_4GB

//
// A window must keep at least this much room beyond the largest BAR it has to
// hold, for the device's other BARs and for the root port's bridge window.
//
#define PCIE_WINDOW_HEADROOM  SIZE_512MB

typedef struct {
  UINT64    Base;
  UINT64    Size;
} PCIE_MEM64_WINDOW;

/**
  Return the 64-bit MMIO window for every root bridge.

  The result is derived from the BIOS setup selection and, in Auto mode, from
  the Resizable BAR capability advertised by the device behind each root port.
  It is deterministic: every caller in the same boot gets the same answer, which
  is what keeps the UEFI host bridge and the ACPI _CRS in agreement without
  needing a protocol between them.

  @param[out]  Windows  Receives PCIE_MAX_ROOTBRIDGE window descriptors, indexed
                        by root bridge number (0 is the x8 slot).

  @return  The mode that was actually applied, never PCIE_WINDOW_MODE_AUTO.

**/
UINT8
EFIAPI
PcieGetMem64Windows (
  OUT PCIE_MEM64_WINDOW  *Windows
  );

#endif
