/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/PlatformBootHookLib.h>

EFI_STATUS
EFIAPI
PlatformBootHookLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return EFI_SUCCESS;
}

VOID
EFIAPI
PlatformBootHook (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  return;
}
