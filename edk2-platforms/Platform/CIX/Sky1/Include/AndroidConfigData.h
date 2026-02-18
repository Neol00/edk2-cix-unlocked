/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ANDROID_CONFIG_DATA_H__
#define __ANDROID_CONFIG_DATA_H__

#define FORMID_MAIN_FORM  1
#define ANDROID_DXE_CONFIG_GUID \
  {0x6c73e2d3, 0x3f13, 0x4d93, {0xec, 0xa2, 0xed, 0xaa, 0x71, 0x78, 0x32, 0xac}}

#define PLATFORM_SETUP_VARIABLE_GUID \
  {0x5E5B2ABF, 0x599A, 0x4329, {0xBA, 0xA4, 0x6D, 0x6E, 0xAA, 0xAC, 0x8B, 0xC1}}

#define ANDROID_VAR_CFG           L"AndroidVarCfg"
#define MAX_EXTENDED_CMDLINE_LEN  128

#pragma pack(1)
typedef struct {
  CHAR16    Cmdline[MAX_EXTENDED_CMDLINE_LEN];
} ANDROID_CFG_DATA;
#pragma pack()

#endif
