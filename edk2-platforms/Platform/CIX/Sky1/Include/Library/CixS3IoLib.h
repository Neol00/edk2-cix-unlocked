/** @file
  I/O and MMIO Library Services that do I/O and also enable the I/O operation
  to be replayed during an S3 resume. This library class maps directly on top
  of the IoLib class.

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CIX_S3_IO_LIB_H__
#define __CIX_S3_IO_LIB_H__

#include <Uefi/UefiBaseType.h>

VOID
EFIAPI
MmioWrite8S3 (
  IN UINTN  Address,
  IN UINT8  Value
  );

VOID
EFIAPI
MmioWrite16S3 (
  IN UINTN   Address,
  IN UINT16  Value
  );

VOID
EFIAPI
MmioWrite32S3 (
  IN UINTN   Address,
  IN UINT32  Value
  );

VOID
EFIAPI
MmioWrite64S3 (
  IN UINTN   Address,
  IN UINT64  Value
  );

VOID
EFIAPI
RwMem8S3 (
  IN UINTN  Address,
  IN UINT8  SetBit8,
  IN UINT8  ResetBit8
  );

VOID
EFIAPI
RwMem16S3 (
  IN UINTN   Address,
  IN UINT16  SetBit16,
  IN UINT16  ResetBit16
  );

VOID
EFIAPI
RwMem32S3 (
  IN UINTN   Address,
  IN UINT32  SetBit32,
  IN UINT32  ResetBit32
  );

VOID
EFIAPI
RwMem64S3 (
  IN UINTN   Address,
  IN UINT64  SetBit64,
  IN UINT64  ResetBit64
  );

#endif
