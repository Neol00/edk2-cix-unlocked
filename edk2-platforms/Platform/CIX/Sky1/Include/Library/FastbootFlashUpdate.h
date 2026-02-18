/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FASTBOOT_FLASH_UPDATE_H_
#define _FASTBOOT_FLASH_UPDATE_H_

EFI_STATUS
GetBootloaderInfo (
  CHAR8  *BL1VersionStrNum,
  CHAR8  *BL2VersionStrNum
  );

EFI_STATUS
SystemFirmwareUpdate (
  IN UINT8  *pImage,
  UINTN     Length
  );

EFI_STATUS
SystemSingleFirmwareUpdate (
  IN UINT8   *pImage,
  IN UINT32  Type,
  UINTN      Length
  );

EFI_STATUS
GetSerialNum (
  CHAR8   *StrSerialNum,
  UINT32  Len
  );

#endif
