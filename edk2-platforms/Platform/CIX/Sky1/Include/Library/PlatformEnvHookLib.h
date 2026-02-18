/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef _PLATFORM_ENV_LIB_H_
#define _PLATFORM_ENV_LIB_H_

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/ConfigParamsDataBlockLib.h>
#include <Library/PlatformConfigParamsDataBlockLib.h>
#include "MemoryMap.h"

typedef struct _ENV_HOOK_PARAMS_DATA_BLOCK {
  CONFIG_PARAMS_DATA_BLOCK             *SocConfig;
  PLATFORM_CONFIG_PARAMS_DATA_BLOCK    *PlatConfig;
} ENV_HOOK_PARAMS_DATA_BLOCK;

typedef
EFI_STATUS
(EFIAPI *PLATFORM_ENV_INIT_FUNC)(
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  );

typedef struct _PLATFORM_ENV_INIT_TABLE {
  EFI_GUID                  *Guid;
  EFI_EVENT_NOTIFY          Notify;
  PLATFORM_ENV_INIT_FUNC    Init;
} PLATFORM_ENV_INIT_TABLE;

VOID
EFIAPI
PlatformEnvHook (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  );

#endif
