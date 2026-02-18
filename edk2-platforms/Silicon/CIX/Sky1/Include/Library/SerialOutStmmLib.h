/** @file  SerialOutStmmLib.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022-2024, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SERIAL_OUT_STMM_LIB_H__
#define __SERIAL_OUT_STMM_LIB_H__

EFI_STATUS
EFIAPI
SerialOutLogPrint (
  IN UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  );

#endif /* __SERIAL_OUT_STMM_LIB_H__ */
