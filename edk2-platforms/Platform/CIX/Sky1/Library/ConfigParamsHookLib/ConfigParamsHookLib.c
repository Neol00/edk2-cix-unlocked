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

STATIC CONFIG_PARAMS_INIT_FUNC  mConfigParamsInitTable[] = {
  // add initializatrion routines for configuration parameters BEFORE this line, and they were invoked from top to down.
  NULL,
};

STATIC UINT32  mConfigParamsInitTableSize = ARRAY_SIZE (mConfigParamsInitTable);

VOID
EFIAPI
ConfigParamsHook (
  IN OUT CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      Index  = 0;

  if (ConfigData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: config params hook routine failed to get config data\n", __FUNCTION__));
    return;
  }

  while (Index < mConfigParamsInitTableSize) {
    if (mConfigParamsInitTable[Index] != NULL) {
      Status = mConfigParamsInitTable[Index](ConfigData);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: config params hook routine failed at index %d, status %r\n", __FUNCTION__, Index, Status));
      }
    }

    Index++;
  }
}
