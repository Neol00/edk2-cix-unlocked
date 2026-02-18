/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "HdaVerbTableInternal.h"

UINT32  AudioVerbTable[] =
{
  //
  //  Codec Vendor:
  //
  // ===== HDA Codec Subsystem ID Verb-table =====
  // HDA Codec Subsystem ID  : 0xFFFFFFFF
  0x001720FF,
  0x001721FF,
  0x001722FF,
  0x001723FF,
};

#define OEM_HDA_VERB_TABLE  {{0xFFFF, 0xFFFF, 0xFF, 0xFF, ARRAY_SIZE(AudioVerbTable)}, AudioVerbTable}

HDA_VERB_TABLE  mPlatformHdaVerbTable[] = { OEM_HDA_VERB_TABLE };

UINT32  mPlatformHdaVerbTableLength = ARRAY_SIZE (mPlatformHdaVerbTable);
