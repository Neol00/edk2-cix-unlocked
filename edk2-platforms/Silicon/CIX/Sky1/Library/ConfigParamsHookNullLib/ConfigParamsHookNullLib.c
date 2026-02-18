/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ConfigParamsHookLib.h>

EFI_STATUS
EFIAPI
ConfigParamsHookLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  return Status;
}

VOID
EFIAPI
ConfigParamsHook (
  IN OUT CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  return;
}
