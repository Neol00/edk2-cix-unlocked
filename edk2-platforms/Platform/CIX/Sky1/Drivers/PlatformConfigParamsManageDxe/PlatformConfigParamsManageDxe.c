/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PlatformConfigParamsManageDxe.h"

EFI_STATUS
EFIAPI
PlatformConfigDataGetEntry (
  IN     CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     UINT32                                      Index,
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN OUT VOID                                        *Buffer,
  IN OUT UINTN                                       *BufferSize
  )
{
  if (Index > This->EntryNum) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*BufferSize < This->Entry[Index].Size) {
    *BufferSize = This->Entry[Index].Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  if ((Buffer == NULL) || (Entry == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem ((VOID *)Buffer, (VOID *)(((UINT8 *)(This->Data)) + This->Entry[Index].Offset), This->Entry[Index].Size);
  CopyMem ((VOID *)Entry, (VOID *)(&This->Entry[Index]), sizeof (PLATFORM_CONFIG_PARAMS_DATA_ENTRY));

  *BufferSize = This->Entry[Index].Size;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PlatformConfigDataFindEntry (
  IN     CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     UINT32                                      Id,
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN OUT VOID                                        *Buffer,
  IN OUT UINTN                                       *BufferSize
  )
{
  UINT32  Index;

  if (BufferSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < This->EntryNum; Index++) {
    if (Id == This->Entry[Index].Id) {
      if (*BufferSize < This->Entry[Index].Size) {
        *BufferSize = This->Entry[Index].Size;
        return EFI_BUFFER_TOO_SMALL;
      }

      if ((Buffer == NULL) || (Entry == NULL)) {
        return EFI_INVALID_PARAMETER;
      }

      CopyMem ((VOID *)Buffer, (VOID *)(((UINT8 *)(This->Data)) + This->Entry[Index].Offset), This->Entry[Index].Size);
      CopyMem ((VOID *)Entry, (VOID *)(&This->Entry[Index]), sizeof (PLATFORM_CONFIG_PARAMS_DATA_ENTRY));
      *BufferSize = This->Entry[Index].Size;

      break;
    }
  }

  if (Index == This->EntryNum) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PlatformConfigDataModifyEntry (
  IN     CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     PLATFORM_CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN     VOID                                        *Buffer,
  IN     UINTN                                       BufferSize
  )
{
  EFI_STATUS                           Status = EFI_SUCCESS;
  UINT64                               Value;
  UINT32                               Num, Index;
  PLATFORM_CONFIG_PARAMS_DATA_OPTIONS  Options[MAX_PARAMS_OPTION_NUM];

  if ((Buffer == NULL) || (Entry == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize > Entry->Size) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Entry->Type) {
    case PARAMS_DATA_BOOLEAN_TYPE:
      Status = ParsePlatformConfigDataOption (Entry->Option, &Num, Options);
      if (!EFI_ERROR (Status)) {
        if (Num != 2) {
          Status = EFI_DEVICE_ERROR;
        } else {
          for (Index = 0; Index < Num; Index++) {
            if (Options[Index].Value == *(BOOLEAN *)Buffer) {
              CopyMem ((VOID *)(((UINT8 *)(This->Data)) + Entry->Offset), (VOID *)Buffer, BufferSize);
              break;
            }
          }

          if (Index == Num) {
            Status = EFI_NOT_FOUND;
          }
        }
      }

      break;
    case PARAMS_DATA_MULTI_OPTION_TYPE:
      Status = ParsePlatformConfigDataOption (Entry->Option, &Num, Options);
      if (!EFI_ERROR (Status)) {
        if (BufferSize == sizeof (UINT8)) {
          Value = *(UINT8 *)Buffer;
        } else if (BufferSize == sizeof (UINT16)) {
          Value = *(UINT16 *)Buffer;
        } else if (BufferSize == sizeof (UINT32)) {
          Value = *(UINT32 *)Buffer;
        } else if (BufferSize == sizeof (UINT64)) {
          Value = *(UINT64 *)Buffer;
        }

        for (Index = 0; Index < Num; Index++) {
          if (Options[Index].Value == Value) {
            CopyMem ((VOID *)(((UINT8 *)(This->Data)) + Entry->Offset), (VOID *)Buffer, BufferSize);
            break;
          }
        }

        if (Index == Num) {
          Status =  EFI_NOT_FOUND;
        }
      }

      break;
    case PARAMS_DATA_INTEGER_TYPE:
    case PARAMS_DATA_STRING_TYPE:
      CopyMem ((VOID *)(((UINT8 *)(This->Data)) + Entry->Offset), (VOID *)Buffer, BufferSize);
      break;
    default:
      Status = EFI_INVALID_PARAMETER;
      break;
  }

  return Status;
}

/**
  Entrypoint of Platform Config Params Manage Dxe Driver

  @param  ImageHandle[in]      The firmware allocated handle for the EFI image.
  @param  SystemTable[in]      A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The config params manage protocol has been registered.
  @retval Others               Unexpected error happened.

**/
EFI_STATUS
EFIAPI
PlatformConfigParamsManageDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                  Status           = EFI_SUCCESS;
  PLATFORM_CONFIG_PARAMS_DATA_BLOCK           *ConfigData      = NULL;
  PLATFORM_CONFIG_PARAMS_DATA_ENTRY           *ConfigDataEntry = NULL;
  CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage    = NULL;

  POST_CODE (PlatformConfigParamsManageDxeStart);

  ConfigData      = AllocateCopyPool (sizeof (PLATFORM_CONFIG_PARAMS_DATA_BLOCK), &mPlatformConfigParamsDataBlock);
  ConfigDataEntry = AllocateCopyPool (sizeof (PLATFORM_CONFIG_PARAMS_DATA_ENTRY) * mPlatformConfigDataEntryNum, &mPlatformConfigDataEntryTable);

  // platform hook routines for configuration parameters initialization
  PlatformConfigParamsHook (ConfigData);

  ConfigManage = AllocateZeroPool (sizeof (CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL));
  if (ConfigManage != NULL) {
    ConfigManage->Version     = CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL_VERSION;
    ConfigManage->Data        = ConfigData;
    ConfigManage->Entry       = ConfigDataEntry;
    ConfigManage->EntryNum    = mPlatformConfigDataEntryNum;
    ConfigManage->GetEntry    = PlatformConfigDataGetEntry;
    ConfigManage->FindEntry   = PlatformConfigDataFindEntry;
    ConfigManage->ModifyEntry = PlatformConfigDataModifyEntry;

    gBS->InstallProtocolInterface (
           &ImageHandle,
           &gCixPlatformConfigParamsManageProtocolGuid,
           EFI_NATIVE_INTERFACE,
           ConfigManage
           );
  }

  POST_CODE (PlatformConfigParamsManageDxeEnd);

  return Status;
}
