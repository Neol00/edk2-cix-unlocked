/** @file
*
*  Copyright 2024 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
*  Copyright 2023 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.
*  Copyright (c) 2017, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef __PLATFORM_CONFIG_HII_H__
#define __PLATFORM_CONFIG_HII_H__

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/NetworkStackSetup.h>
#include <PlatformSetupVar.h>

#define PLATFORM_CONFIG_FORMSET_GUID  \
  { 0x7dbfcdc4, 0xfb50, 0x452a, { 0xae, 0xd6, 0xfe, 0x21, 0x63, 0x70, 0x24, 0x61 } }

#define PLATFORM_SETUP_VARIABLE_GUID \
  {0x5E5B2ABF, 0x599A, 0x4329, {0xBA, 0xA4, 0x6D, 0x6E, 0xAA, 0xAC, 0x8B, 0xC1}}
#define CIX_GLOBAL_VARIABLE_GUID \
  {0x31d1bbc6, 0xc793, 0x48af, {0x8d, 0xcf, 0xf9, 0x18, 0x38, 0xc7, 0x3a, 0x2e}}

extern EFI_GUID  gPlatformConfigFormSetGuid;

#define PLATFORM_CONFIG_FORM_ID     0x2001
#define QUESTION_STATE_AFTER_G3_ID  0x2002

#endif
