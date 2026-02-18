/** @file
  SocInfoProtocol.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SOC_INFO_PROTOCOL_H__
#define __SOC_INFO_PROTOCOL_H__

#include <Uefi.h>

typedef UINT8   uint8_t;
typedef UINT16  uint16_t;
typedef UINT32  uint32_t;
typedef UINT64  uint64_t;

#include "MemOutputBuffer.h"

/* csec_km_meta_t.config index */
#define KM_CFG_VERSION        0
#define KM_CFG_LC             1
#define KM_CFG_ASYM_ALG_TYPE  2
#define KM_CFG_SEC_ENABLE     3
#define KM_CFG_ENC_ENABLE     4

/* 64 bits serial num + 64 bits chip ip in fuse, 4 word*/
#define KM_CFG_SERIAL_NUM  5
#define KM_CFG_MAX         16

/* max size 4k */
// typedef struct _km_meta_tag {
//         uint8_t hpubk[32];
//         uint8_t rng[64];
//         uint32_t config[KM_CFG_MAX];
// } csec_km_meta_t;

typedef union {
  struct {
    UINT8     Hpubk[32];
    UINT8     Rng[64];
    UINT32    Reserved1[5];  // config[0]~config[4]
    UINT32    SerialNum[2];  // 64 bits serial num
    UINT32    ChipId[2];     // 64 bits chip id
    UINT32    Reserved2[7];
  } SocInfo;
  UINT8    Data[SIZE_4KB];
} SE_HANDOFF_DATA;

typedef union {
  struct {
    UINT8    BootStrap; // boot strap pin status 1:normal linux ; 0:fastboot
  } Sts;
  UINT8    Data[4];
} BOOT_MODE;

typedef enum {
  SerialNum,
  ChipId
} SOC_INFO_TYPE;

typedef struct _CIX_SOC_INFO_PROTOCOL CIX_SOC_INFO_PROTOCOL;

typedef
  EFI_STATUS
(EFIAPI *GET_SOC_INFO)(
  SOC_INFO_TYPE  Type,
  IN OUT UINT32  **SocInfo,
  IN OUT UINT32  *SocInfoSize
  );

struct _CIX_SOC_INFO_PROTOCOL {
  SE_HANDOFF_DATA           *SeHandoffData;
  BOOT_MODE                 *BootMode;
  MEM_INIT_OUTPUT_BUFFER    *MemInfo;
  GET_SOC_INFO              GetSocInfo;
};

#endif
