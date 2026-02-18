/** @file PlatformInit.h
  Headers for PlatformInitPei PEIM

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef QEMU_OPEN_BOARD_PKG_PLATFORM_INIT_H_
#define QEMU_OPEN_BOARD_PKG_PLATFORM_INIT_H_

#include <PiPei.h>
#include <Uefi.h>

#define PIIX4_PCI_IO_BASE  0xC000
#define PIIX4_PCI_IO_SIZE  0x4000

#define Q35_PCI_IO_BASE  0x6000
#define Q35_PCI_IO_SIZE  0xA000

#define PCI_MMIO_TOP_ADDRESS  0xFC000000

EFI_STATUS
EFIAPI
PlatformInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

UINT32
EFIAPI
GetMemoryBelow4Gb (
  VOID
  );

EFI_STATUS
EFIAPI
InstallMemory (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );

EFI_STATUS
EFIAPI
InitializePcie (
  VOID
  );

EFI_STATUS
EFIAPI
InitializePciPIIX4 (
  VOID
  );

EFI_STATUS
EFIAPI
MaxCpuInit (
  VOID
  );

#endif //QEMU_OPEN_BOARD_PKG_PLATFORM_INIT_H_
