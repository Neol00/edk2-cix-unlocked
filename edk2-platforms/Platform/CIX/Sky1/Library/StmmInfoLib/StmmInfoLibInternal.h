/** @file  StmmInfoLibInternal.h

  Copyright 2024 - 2025 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022-2023, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __STMM_INFO_LIB_INTERNAL_H__
#define __STMM_INFO_LIB_INTERNAL_H__

#include <Base.h>
#include <Include/Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/MmCommunication2.h>

typedef struct {
  UINTN         Function;
  EFI_STATUS    ReturnStatus;
  UINT8         Data[1];
} MM_STMM_INFO_COMMUNICATE_HEADER;

typedef struct {
  UINT8    Data[1];
} MM_STMM_INFO_COMMUNICATE_GET_VERSION;

typedef struct {
  UINTN    PayloadSize;
} MM_STMM_INFO_COMMUNICATE_GET_PAYLOAD_SIZE;

//
// The payload for this function is MM_STMM_INFO_COMMUNICATE_GET_VERSION.
//
#define MM_STMM_INFO_FUNCTION_GET_VERSION  1

//
// The payload for this function is MM_STMM_INFO_COMMUNICATE_GET_PAYLOAD_SIZE.
//
#define MM_STMM_INFO_FUNCTION_GET_PAYLOAD_SIZE  2

///
/// Size of MM communicate header, without including the payload.
///
#define MM_COMMUNICATE_HEADER_SIZE  (OFFSET_OF (EFI_MM_COMMUNICATE_HEADER, Data))

///
/// Size of MM NOR flash communicate header, without including the payload.
///
#define MM_STMM_INFO_COMMUNICATE_HEADER_SIZE  (OFFSET_OF (MM_STMM_INFO_COMMUNICATE_HEADER, Data))

#endif /* __STMM_INFO_LIB_INTERNAL_H__ */
