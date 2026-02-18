/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _LAUNCH_EFI_LIB_H_
#define _LAUNCH_EFI_LIB_H_
#include <Uefi.h>

EFI_STATUS
EFIAPI
LaunchRamBufferEfi (
  VOID   *ImageBuffer,
  UINTN  ImageSize
  );

VOID
EFIAPI
LaunchGrubFromRam (
  );

VOID
EFIAPI
UninstallGrubRamdisk (
  );

#endif
