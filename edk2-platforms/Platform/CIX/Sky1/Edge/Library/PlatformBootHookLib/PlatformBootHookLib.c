/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/GpioLib.h>
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

EFI_STATUS
EFIAPI
ClearGmacIntrStatus (
  IN OUT BOOT_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  UINT32  IntStatus;

  // clear on read of GMAC interupt status when ready to boot
  // GMAC0_INT_L : GPIO045
  // GMAC1_INT_L : GPIO046
  GpioGetIntStatus (GPIO_BANK3, &IntStatus);

  return EFI_SUCCESS;
}

STATIC PLATFORM_BOOT_INIT_TABLE  mPlatformBootInitTable[] = {
  { NULL, NULL, ClearGmacIntrStatus },
  // add platform initialization routines on BOOT phase BEFORE this line, and they were invoked from top to down.
  { NULL, NULL, NULL                }
};

STATIC UINT32  mPlatformBootInitTableSize = ARRAY_SIZE (mPlatformBootInitTable);

VOID
EFIAPI
PlatformBootHook (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                   Status = EFI_SUCCESS;
  BOOT_HOOK_PARAMS_DATA_BLOCK  *ConfigData;
  UINT32                       Index = 0;
  VOID                         *Registration;

  if (Context == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: platform BOOT hook routine failed to get config data\n", __FUNCTION__));
    return;
  }

  ConfigData = (BOOT_HOOK_PARAMS_DATA_BLOCK *)Context;

  while (Index < mPlatformBootInitTableSize) {
    if (mPlatformBootInitTable[Index].Guid == NULL) {
      if (mPlatformBootInitTable[Index].Init != NULL) {
        Status = mPlatformBootInitTable[Index].Init (ConfigData);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: platform BOOT hook routine failed at index %d, status %r\n", __FUNCTION__, Index, Status));
        }
      }
    } else {
      EfiCreateProtocolNotifyEvent (
        mPlatformBootInitTable[Index].Guid,
        TPL_CALLBACK,
        mPlatformBootInitTable[Index].Notify,
        ConfigData,
        &Registration
        );
    }

    Index++;
  }

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
}
