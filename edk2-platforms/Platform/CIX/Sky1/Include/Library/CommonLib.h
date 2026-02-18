/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_
#include <Uefi.h>

VOID
AsciiToUnicode (
  IN    CHAR8   *AsciiString,
  IN    CHAR16  *UnicodeString
  );

UINTN
UnicodeToAscii (
  IN CONST CHAR16  *UStr,
  IN CONST UINTN   Length,
  OUT CHAR8        *AStr
  );

VOID
PrintDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

BOOLEAN
IsRamdiskDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

VOID
TimeStampPrint();
#endif
