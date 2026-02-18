/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

**/

#ifndef __IMAGE_DESCRIPTOR_H__
#define __IMAGE_DESCRIPTOR_H__

#define PACKAGE_VERSION                     0xFFFFFFFF
#define PACKAGE_VERSION_STRING              L"Unknown"

#define CURRENT_FIRMWARE_VERSION            0x00000004
#define CURRENT_FIRMWARE_VERSION_STRING     L"0x00000004"
#define LOWEST_SUPPORTED_FIRMWARE_VERSION   0x00000003

#define IMAGE_ID                            SIGNATURE_64('C', 'I', 'X', ' ', 'S', 'K', 'Y', '1')
#define IMAGE_ID_STRING                     L"CIX SKY1"

// PcdSystemFmpCapsuleImageTypeIdGuid       351c9808-4b53-4363-9314-d339386e5685
#define IMAGE_TYPE_ID_GUID                  { 0x351c9808, 0x4b53, 0x4363, { 0x93, 0x14, 0xd3, 0x39, 0x38, 0x6e, 0x56, 0x85 } }

#pragma pack(1)
typedef struct {
  EDKII_SYSTEM_FIRMWARE_IMAGE_DESCRIPTOR  Descriptor;
  // real string data
  CHAR16                                  ImageIdNameStr[sizeof(IMAGE_ID_STRING)/sizeof(CHAR16)];
  CHAR16                                  VersionNameStr[sizeof(CURRENT_FIRMWARE_VERSION_STRING)/sizeof(CHAR16)];
  CHAR16                                  PackageVersionNameStr[sizeof(PACKAGE_VERSION_STRING)/sizeof(CHAR16)];
} IMAGE_DESCRIPTOR;
#pragma pack()

extern IMAGE_DESCRIPTOR mImageDescriptor_1;

#endif