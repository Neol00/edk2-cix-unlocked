/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef _PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL_H_
#define _PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL_H_

#include <Library/PlatformConfigParamsDataBlockLib.h>

#define CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL_VERSION           0x50504D01    //   'P' 'P' 'M' '(version ID)'
#define CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL_VERSION_STR_MASK  0xFFFFFF00
#define CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL_VERSION_NUM_MASK  0x000000FF

#define CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL_GUID   \
    {0xC8723B5F, 0x2550, 0x498E, {0xB1, 0x1F, 0x4D, 0x05, 0x25, 0x8C, 0x31, 0x57}}

typedef struct _CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *PLATFORM_CONFIG_PARAMS_DATA_GET_ENTRY)(
  IN     CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     UINT32                                      Index,
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN OUT VOID                                        *Buffer,
  IN OUT UINTN                                       *BufferSize
  );

typedef
EFI_STATUS
(EFIAPI *PLATFORM_CONFIG_PARAMS_DATA_FIND_ENTRY)(
  IN     CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     UINT32                                      Id,
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN OUT VOID                                        *Buffer,
  IN OUT UINTN                                       *BufferSize
  );

typedef
EFI_STATUS
(EFIAPI *PLATFORM_CONFIG_PARAMS_DATA_MODIFY_ENTRY)(
  IN     CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     PLATFORM_CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN     VOID                                        *Buffer,
  IN     UINTN                                       BufferSize
  );

struct _CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL {
  UINT32                                      Version;
  PLATFORM_CONFIG_PARAMS_DATA_BLOCK           *Data;
  PLATFORM_CONFIG_PARAMS_DATA_ENTRY           *Entry;
  UINT32                                      EntryNum;
  PLATFORM_CONFIG_PARAMS_DATA_GET_ENTRY       GetEntry;
  PLATFORM_CONFIG_PARAMS_DATA_FIND_ENTRY      FindEntry;
  PLATFORM_CONFIG_PARAMS_DATA_MODIFY_ENTRY    ModifyEntry;
};

extern EFI_GUID  gCixPlatformConfigParamsManageProtocolGuid;

#endif
