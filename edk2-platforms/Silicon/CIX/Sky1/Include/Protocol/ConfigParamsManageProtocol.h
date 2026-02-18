/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef _CONFIG_PARAMS_MANAGE_PROTOCOL_H_
#define _CONFIG_PARAMS_MANAGE_PROTOCOL_H_

#include <Library/ConfigParamsDataBlockLib.h>

#define CIX_CONFIG_PARAMS_MANAGE_PROTOCOL_VERSION           0x43504D01    //   'C' 'P' 'M' '(version ID)'
#define CIX_CONFIG_PARAMS_MANAGE_PROTOCOL_VERSION_STR_MASK  0xFFFFFF00
#define CIX_CONFIG_PARAMS_MANAGE_PROTOCOL_VERSION_NUM_MASK  0x000000FF

#define CIX_CONFIG_PARAMS_MANAGE_PROTOCOL_GUID   \
    {0xC86EADA7, 0xD376, 0x474A, {0x9F, 0xAC, 0xCC, 0x4F, 0xC5, 0xFD, 0x41, 0x7B}}

typedef struct _CIX_CONFIG_PARAMS_MANAGE_PROTOCOL CIX_CONFIG_PARAMS_MANAGE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *CONFIG_PARAMS_DATA_GET_ENTRY)(
  IN     CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     UINT32                             Index,
  IN OUT CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN OUT VOID                               *Buffer,
  IN OUT UINTN                              *BufferSize
  );

typedef
EFI_STATUS
(EFIAPI *CONFIG_PARAMS_DATA_FIND_ENTRY)(
  IN     CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     UINT32                             Id,
  IN OUT CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN OUT VOID                               *Buffer,
  IN OUT UINTN                              *BufferSize
  );

typedef
EFI_STATUS
(EFIAPI *CONFIG_PARAMS_DATA_MODIFY_ENTRY)(
  IN     CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *This,
  IN     CONFIG_PARAMS_DATA_ENTRY           *Entry,
  IN     VOID                               *Buffer,
  IN     UINTN                              BufferSize
  );

struct _CIX_CONFIG_PARAMS_MANAGE_PROTOCOL {
  UINT32                             Version;
  CONFIG_PARAMS_DATA_BLOCK           *Data;
  CONFIG_PARAMS_DATA_ENTRY           *Entry;
  UINT32                             EntryNum;
  CONFIG_PARAMS_DATA_GET_ENTRY       GetEntry;
  CONFIG_PARAMS_DATA_FIND_ENTRY      FindEntry;
  CONFIG_PARAMS_DATA_MODIFY_ENTRY    ModifyEntry;
};

extern EFI_GUID  gCixConfigParamsManageProtocolGuid;

#endif
