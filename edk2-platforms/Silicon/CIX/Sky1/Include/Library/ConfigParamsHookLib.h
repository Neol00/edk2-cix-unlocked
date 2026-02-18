/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef _CONFIG_PARAMS_HOOK_LIB_H_
#define _CONFIG_PARAMS_HOOK_LIB_H_

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ConfigParamsDataBlockLib.h>

typedef
EFI_STATUS
(EFIAPI *CONFIG_PARAMS_INIT_FUNC)(
  IN OUT CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  );

VOID
EFIAPI
ConfigParamsHook (
  IN OUT CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  );

#endif
