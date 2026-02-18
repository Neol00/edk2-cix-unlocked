/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ABL_AUTHENTICATE_LIB_H_
#define _ABL_AUTHENTICATE_LIB_H_

#include <Uefi.h>

#define EFI_ABL_INFO_PROTOCOL_GUID  { 0x314188ee, 0x4a65, 0xc3cf, {0x95, 0xd4, 0xc5, 0x81, 0x3f, 0xdb, 0x09, 0xca }}

#define FMP_PAYLOAD_HEADER_SIGNATURE  SIGNATURE_32 ('M', 'S', 'S', '1')

EFI_STATUS
AuthenticateAbl (
  IN OUT  VOID   **CapsuleBuffer,
  IN OUT  UINTN  *CapsuleBufferSize,
  OUT UINT32     *Version,
  OUT UINT32     *LowestSupportedVersion
  );

EFI_STATUS
StartAblImage (
  IN VOID   *ImageBuffer,
  IN UINTN  ImageSize
  );

//
// Protocol definition
//
typedef struct {
  UINT8     *AuthenticatedImage;
  UINTN     AuthenticatedImageSize;
} EFI_ABL_INFO_PROTOCOL;

#pragma pack(1)

typedef struct {
  UINT32    Signature;
  UINT32    HeaderSize;
  UINT32    FwVersion;
  UINT32    LowestSupportedVersion;
} FMP_PAYLOAD_HEADER;

#pragma pack()

#endif
