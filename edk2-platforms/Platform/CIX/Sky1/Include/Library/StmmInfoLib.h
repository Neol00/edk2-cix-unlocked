/** @file  StmmInfoLib.h

  Copyright 2024-2025 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022-2023, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __STMM_INFO_LIB_H__
#define __STMM_INFO_LIB_H__

EFI_STATUS
EFIAPI
StmmInfoGetVersion (
  IN OUT CHAR16  **Version,
  IN OUT UINT32  *Size
  );

#endif /* __STMM_INFO_LIB_H__ */
