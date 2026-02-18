/** @file QemuOpenFwCfgLib.h
  QemuOpenFwCfgLib Headers

  Implements a minimal library to interact with Qemu FW CFG device

  QEMU FW CFG device allow the OS to retrieve files passed by QEMU or the user.
  Files can vary from E820 entries to ACPI tables.

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


#ifndef QEMU_OPEN_BOARD_PKG_QEMU_FW_CFG_LIB_H_
#define QEMU_OPEN_BOARD_PKG_QEMU_FW_CFG_LIB_H_

#include <PiPei.h>
#include <Library/IoLib.h>

// QEMU fw_cfg registers
#define FW_CFG_PORT_SEL   0x510
#define FW_CFG_PORT_DATA  0x511
#define FW_CFG_PORT_DMA   0x514

// QEMU Selectors
#define FW_CFG_SIGNATURE  0x0000
#define FW_CFG_ID         0x0001
#define FW_CFG_FILE_DIR   0x0019

#define FW_CFG_QEMU_SIGNATURE SIGNATURE_32('Q', 'E', 'M', 'U')

typedef struct {
  UINT32    Size;
  UINT16    Select;
  UINT16    Reserved;
  CHAR8     Name[56];
} QEMU_FW_CFG_FILE;

/**
  Checks for Qemu fw_cfg device by reading "QEMU" using the signature selector

  @return EFI_SUCCESS - The fw_cfg device is present
  @return EFI_UNSUPPORTED - The device is absent
 */
EFI_STATUS
EFIAPI
QemuFwCfgIsPresent (
  VOID
  );

/**
 Sets the selector register to the specified value

  @param[in] Selector

  @return EFI_SUCCESS
  @return EFI_UNSUPPORTED
 */
EFI_STATUS
EFIAPI
QemuFwCfgSelectItem (
  IN UINT16  Selector
  );

/**
 Reads 8 bits from the data register

  @return UINT8
 */
UINT8
EFIAPI
QemuFwCfgRead8 (
  VOID
  );

/**
  Reads N bytes from the data register

  @param Size
  @param Buffer
 */
VOID
EFIAPI
QemuFwCfgReadBytes (
  IN UINTN  Size,
  OUT VOID  *Buffer
  );

/**
  Finds a file in fw_cfg by its name

  @param[in]  String Pointer to an ASCII string to match in the database
  @param[out] FWConfigFile Buffer for the config file

  @return EFI_STATUS Entry was found, FWConfigFile is populated
  @return EFI_ERROR Entry was not found
 */
EFI_STATUS
EFIAPI
QemuFwCfgFindFile (
  IN  CHAR8              *String,
  OUT QEMU_FW_CFG_FILE   *FWConfigFile
  );

#endif // QEMU_OPEN_BOARD_PKG_QEMU_FW_CFG_LIB_H_
