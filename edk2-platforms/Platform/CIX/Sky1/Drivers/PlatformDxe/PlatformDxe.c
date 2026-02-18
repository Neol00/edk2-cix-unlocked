/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  Copyright (c) 2021, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PlatformDxe.h"

/**
  Entrypoint of Platform Dxe Driver

  @param  ImageHandle[in]      The firmware allocated handle for the EFI image.
  @param  SystemTable[in]      A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The RAM disk has been registered.
  @retval EFI_NOT_FOUND        No RAM disk protocol instances were found.
  @retval EFI_UNSUPPORTED      The ImageType is not supported.
  @retval Others               Unexpected error happened.

**/
EFI_STATUS
EFIAPI
PlatformDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                  Status;
  EFI_EVENT                                   Event;
  EFI_HANDLE                                  Handle;
  ENV_HOOK_PARAMS_DATA_BLOCK                  *EnvConfigData  = NULL;
  BOOT_HOOK_PARAMS_DATA_BLOCK                 *BootConfigData = NULL;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL           *ConfigManage;
  CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *PlatformConfigManage;

  POST_CODE (PlatformDxeStart);

  EnvConfigData  = AllocateZeroPool (sizeof (ENV_HOOK_PARAMS_DATA_BLOCK));
  BootConfigData = AllocateZeroPool (sizeof (BOOT_HOOK_PARAMS_DATA_BLOCK));

  Status = gBS->LocateProtocol (
                  &gCixConfigParamsManageProtocolGuid,
                  NULL,
                  (VOID **)&ConfigManage
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: config parameters invalid %r\n", __FUNCTION__, Status));
  } else {
    if (EnvConfigData != NULL) {
      EnvConfigData->SocConfig = ConfigManage->Data;
    }

    if (BootConfigData != NULL) {
      BootConfigData->SocConfig = ConfigManage->Data;
    }
  }

  Status = gBS->LocateProtocol (
                  &gCixPlatformConfigParamsManageProtocolGuid,
                  NULL,
                  (VOID **)&PlatformConfigManage
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: platform config parameters invalid %r\n", __FUNCTION__, Status));
  } else {
    if (EnvConfigData != NULL) {
      EnvConfigData->PlatConfig = PlatformConfigManage->Data;
    }

    if (BootConfigData != NULL) {
      BootConfigData->PlatConfig = PlatformConfigManage->Data;
    }
  }

  // Platform ENV phase routines
  PlatformEnvHook (EnvConfigData);

  // Platform BOOT phase routines register
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             PlatformBootHook,
             BootConfigData,
             &Event
             );

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gCixPlatformInitCompleteProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  POST_CODE (PlatformDxeEnd);
  return Status;
}
