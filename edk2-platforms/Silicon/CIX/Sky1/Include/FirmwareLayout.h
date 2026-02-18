/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __FIRMWARE_LAYOUT_H__
#define __FIRMWARE_LAYOUT_H__

#include "MemoryMap.h"

#define FIRMWARE_LAYOUT_BASE_ADDRESS  XSPI_MMIO_BASE

#define EC_OFFSET                   0x10000
#define FIRMWARE_HEADER_OFFSET      0x100000
#define FIRMWARE_HEADER_OFFSET_ALT  0x200000
#define SECURE_DEBUG_OFFSET         0x107000
#define SECURE_DEBUG_OFFSET_ALT     0x207000
#define SECURE_DEBUG_SIZE           0x1000

#define FIRMWARE_HEADER_SIGNATURE  0x55AA55AA

#define CBFF_MAGIC       "CIXBTFF!"
#define CBFF_MAGIC_SIZE  8

#pragma pack(1)

typedef struct {
  UINT32    Type;
  UINT32    Base;
  UINT32    Length;
  UINT32    Reserved;
} FIRMWARE_ENTRY;

typedef struct {
  UINT32            Signature; // 0x55AA55AA
  UINT32            Version;
  UINT32            EntryCount;
  UINT32            ControlFlag;
  FIRMWARE_ENTRY    EntryNode[1];
} FIRMWARE_HEADER;

typedef struct _XSPI_CONFIG {
  UINT32    JedecId;
  UINT8     BusClock; // Item in XSPI_BUS_CLK list
  UINT8     ReadMode; // Item in XSPI_READ_MODE list
  UINT8     DummyCycle;
  UINT8     RxDelay;
  UINT8     ReadModeOpt;   // Item in XSPI_READ_MODE_OPT list but 0 means no-used
  UINT8     DummyCycleOpt; // Dummy cycle for read mode setting of ReadModeOpt but 0 means no-used
  UINT8     Reserved[3];
  UINT8     Padding[3];
} XSPI_CONFIG;

typedef struct _XSPI_CONFIG_TABLE {
  UINT32         Number;
  XSPI_CONFIG    Config[1];
} XSPI_CONFIG_TABLE;

typedef struct {
  UINT32    Name;
  UINT32    SerialNum;
  UINT64    Flags;
} FIP_TOC_HEADER;

typedef struct {
  UINT8     UUID[16];
  UINT64    Offset;
  UINT64    Size;
  UINT64    Flags;
} FIP_TOC_ENTRY;

typedef struct {
  FIP_TOC_HEADER    Header;
  FIP_TOC_ENTRY     Entries[1];
} FIP_TOC_FORMAT;

#pragma pack()

typedef enum {
  BOOT_LOADER_1  = 1,
  BOOT_LOADER_2  = 2,
  MEMORY_CONFIGS = 3,
  CSU_PM_CONFIGS = 4,
  SECURE_DEBUG   = 5,
  UEFI_NVRAM     = 100
} FIRMWARE_TYPE;

typedef enum {
  FIP_ENTRY_INDEX_EL3_FW = 0,
  FIP_ENTRY_INDEX_BL32_FW,
  FIP_ENTRY_INDEX_BL33_FW,
  FIP_ENTRY_INDEX_TRUSTED_KEY_CERT,
  FIP_ENTRY_INDEX_BL31_KEY_CERT,
  FIP_ENTRY_INDEX_BL32_KEY_CERT,
  FIP_ENTRY_INDEX_BL33_KEY_CERT,
  FIP_ENTRY_INDEX_BL31_FW_CERT,
  FIP_ENTRY_INDEX_BL32_FW_CERT,
  FIP_ENTRY_INDEX_BL33_FW_CERT,
} FIP_ENTRY_INDEX;

typedef enum {
  XSPI_25MHz = 0,
  XSPI_20MHz = 1,
  XSPI_40MHz = 2,
  XSPI_50MHz = 3,
  XSPI_10MHz = 4,
  XSPI_5MHz  = 5
} XSPI_BUS_CLK;

typedef enum {
  XSPI_1_1_1 = 0,
  XSPI_1_1_2 = 1,
  XSPI_1_1_4 = 2,
} XSPI_READ_MODE;

typedef enum {
  XSPI_1_2_2 = 1,
  XSPI_1_4_4 = 2
} XSPI_READ_MODE_OPT;

#endif
