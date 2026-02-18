/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef CPU_CONFIG_NV_DATA_STRUC_H_
#define CPU_CONFIG_NV_DATA_STRUC_H_

#pragma pack(1)
typedef struct {
  UINT32 CpuSubNumaMode;
} CPU_VARSTORE_DATA;

#pragma pack()

#endif /* CPU_CONFIG_NV_DATA_STRUC_H_ */
