/** @file
  SocInitLib.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FW_VERSION_INFO_H__
#define __FW_VERSION_INFO_H__

#include <Uefi.h>

#define FW_HEADER_SIG        $UEFI_FIRMWARE$
#define FW_PROTOCOL_VERSION  0x01

#define ___INTERNAL_CONVERT_TO_STRING___(a)   #a
#define CONVERT_TO_STRING(a)                  ___INTERNAL_CONVERT_TO_STRING___(a)
#define ___INTERNAL_CONVERT_TO_WSTRING___(a)  L###a
#define CONVERT_TO_WSTRING(a)                 ___INTERNAL_CONVERT_TO_WSTRING___(a)
#define STR(a)                                CONVERT_TO_STRING(a)
#define WSTR(a)                               CONVERT_TO_WSTRING(a)

typedef enum {
  FwVerSE,
  FwVerPM,
  FwVerPBL,
  FwVerATF,
  FwVerTEE,
  FwVerEC,
  FwVerSTMM,
  FwVerUEFI,
  FwVerBoardId
} FW_VERSION_TYPE;

typedef struct _CIX_FW_VERSION_PROTOCOL CIX_FW_VERSION_PROTOCOL;

typedef
  EFI_STATUS
(EFIAPI *GET_FW_VERSION)(
  FW_VERSION_TYPE  FwVerType,
  IN OUT CHAR16  **FwVerBuff,
  IN OUT UINT32  *FwVerSize
  );

#pragma pack(push, 1)
typedef struct _FW_VER_HEADER {
  CHAR8    Signature[16];
  UINT8    HeaderVersion;
  CHAR8    FwVersion[64];
} FW_VER_HEADER;
#pragma pack(pop)

struct _CIX_FW_VERSION_PROTOCOL {
  FW_VER_HEADER     FwHeader;
  GET_FW_VERSION    GetFwVersion;
};

#endif
