/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef _PLATFORM_CONFIG_PARAMS_HOOK_LIB_H_
#define _PLATFORM_CONFIG_PARAMS_HOOK_LIB_H_

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PlatformConfigParamsDataBlockLib.h>

typedef
EFI_STATUS
(EFIAPI *PLATFORM_CONFIG_PARAMS_INIT_FUNC)(
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  );

VOID
EFIAPI
PlatformConfigParamsHook (
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  );

#endif
