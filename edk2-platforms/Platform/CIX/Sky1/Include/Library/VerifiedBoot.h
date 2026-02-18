/** @VerifiedBoot.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __VERIFIEDBOOT_H__
#define __VERIFIEDBOOT_H__

#include <Uefi.h>

enum {
  NO_AVB = 0,
  AVB_1,
  AVB_2,
  AVB_LE
};

#define GUARD(code)                                                            \
  do {                                                                         \
    Status = (code);                                                           \
    if (Status != EFI_SUCCESS) {                                               \
      DEBUG ((EFI_D_ERROR, "Err: line:%d %a() status: %r\n", __LINE__,         \
              __FUNCTION__, Status));                                          \
      return Status;                                                           \
    }                                                                          \
  } while (0)

#define GUARD_OUT(code)                                                        \
  do {                                                                         \
    Status = (code);                                                           \
    if (Status != EFI_SUCCESS) {                                               \
      DEBUG ((EFI_D_ERROR, "Err: line:%d %a() status: %r\n", __LINE__,         \
              __FUNCTION__, Status));                                          \
      goto out;                                                                \
    }                                                                          \
  } while (0)

/* forward declare BootInfo */
typedef struct BootInfo BootInfo;

BOOLEAN
VerifiedBootEnbled (
  );

UINT32
GetAVBVersion (
  );

EFI_STATUS
LoadImageAndAuth (
  BootInfo  *Info
  );

#endif /* __VERIFIEDBOOT_H__ */
