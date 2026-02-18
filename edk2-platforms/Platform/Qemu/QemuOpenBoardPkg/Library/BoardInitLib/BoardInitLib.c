/** @file
  Board initialization library

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BoardInitLib.h>
#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PciCf8Lib.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/I440FxPiix4.h>
#include <IndustryStandard/Q35MchIch9.h>
#include <Library/HobLib.h>

#define QEMU_IO_DEBUG_MAGIC  0xE9

/**
  This board service detects the board type.

  @retval EFI_SUCCESS   The board was detected successfully.
  @retval EFI_NOT_FOUND The board could not be detected.
**/
EFI_STATUS
EFIAPI
BoardDetect (
  VOID
  )
{
  UINT16  DeviceID, VendorID;

  DEBUG ((DEBUG_INFO, "BoardDetect()\n"));

  //
  // Retrieve chipset device ID and vendor ID
  //
  DeviceID = PciCf8Read16 (PCI_CF8_LIB_ADDRESS (0, 0, 0, PCI_DEVICE_ID_OFFSET));
  VendorID = PciCf8Read16 (PCI_CF8_LIB_ADDRESS (0, 0, 0, PCI_VENDOR_ID_OFFSET));

  //
  //  Qemu can emulate 2 chipsets:
  //  Intel 440FX with PIIX4 southbridge
  //  Intel Q35 memory host controller with ICH9 southbridge
  //

  switch (DeviceID) {
    case INTEL_82441_DEVICE_ID:
      DEBUG ((DEBUG_INFO, "Intel 440FX/PIIX4 platform detected!\n"));
      return EFI_SUCCESS;

    case INTEL_Q35_MCH_DEVICE_ID:
      DEBUG ((DEBUG_INFO, "Intel Q35 MCH/ICH9 platform detected!\n"));
      return EFI_SUCCESS;

    default:
      DEBUG ((DEBUG_ERROR, "Unable to detect board (Device id %u Vendor ID %u)\n", DeviceID, VendorID));
      return EFI_NOT_FOUND;
  }
}

/**
  This board service initializes board-specific debug devices.

  @retval EFI_SUCCESS   Board-specific debug initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardDebugInit (
  VOID
  )
{
  return EFI_SUCCESS;
}

EFI_BOOT_MODE
EFIAPI
BoardBootModeDetect (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardBootModeDetect()\n"));
  return BOOT_WITH_FULL_CONFIGURATION;
}

/**
  A hook for board-specific initialization prior to memory initialization.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitBeforeMemoryInit (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitBeforeMemoryInit()\n"));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific initialization after memory initialization.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitAfterMemoryInit (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitAfterMemoryInit()\n"));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific initialization prior to disabling temporary RAM.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitBeforeTempRamExit (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitBeforeTempRamExit()\n"));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific initialization after disabling temporary RAM.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitAfterTempRamExit (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitAfterTempRamExit()\n"));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific initialization prior to silicon initialization.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitBeforeSiliconInit (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitBeforeSiliconInit()\n"));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific initialization after silicon initialization.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitAfterSiliconInit (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitAfterSiliconInit()\n"));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific initialization after PCI enumeration.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitAfterPciEnumeration (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitAfterPciEnumeration()\n"));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific functionality for the ReadyToBoot event.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitReadyToBoot (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitReadyToBoot()\n"));
  return EFI_SUCCESS;
}

/**
  A hook for board-specific functionality for the ExitBootServices event.

  @retval EFI_SUCCESS   The board initialization was successful.
  @retval EFI_NOT_READY The board has not been detected yet.
**/
EFI_STATUS
EFIAPI
BoardInitEndOfFirmware (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "BoardInitEndOfFirmware()\n"));
  return EFI_SUCCESS;
}
