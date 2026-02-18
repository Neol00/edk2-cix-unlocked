/** @file
*
*  Copyright 2024 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
*  Copyright 2023 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.
*  Copyright (c) 2017, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef __SYSTEM_INFO_HII_H__
#define __SYSTEM_INFO_HII_H__

#include <Guid/HiiPlatformSetupFormset.h>

#define SYSTEM_INFO_FORMSET_GUID  \
  { 0xdbb34835, 0x1a17, 0x4f04, { 0xb0, 0x0f, 0x57, 0xdb, 0xa4, 0xd1, 0x3c, 0x98 } }

extern EFI_GUID  gSystemInfoFormSetGuid;

#define SYSTEM_INFO_FORM_ID      0x2000

#endif
