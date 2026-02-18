/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _FLASH_UPDATE_H_
#define _FLASH_UPDATE_H_

#include <Uefi.h>
#include <string.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>

#define   EC_OFFSET                   0x100000
#define   FIRMWARE_HEADER_OFFSET      0x100000
#define   FIRMWARE_HEADER_OFFSET_ALT  0x200000
#define   FIRMWARE_HEADER_SIZE        0x1000
#define   FIRMWARE_HEADER_SIGNATURE   0x55AA55AA
#define   FLASH_AHB_MMIO_ADDR         0x08000000
#define   FLASH_BLOCK_SIZE            0x1000
#define   FLASH_SIZE                  0x1000000 // 16MB

#define   UpdateEntireFirmware  0x00000001
#define   UpdateEcFirmware      0x00000002
#define   UpdateSingleFirmware  0x00000003
#define   ONE_SECOND            10000000

#define   DBARLEN  60
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

typedef struct {
  CHAR8     Magic[8];
  UINT32    Cbff_version;
  UINT32    Encrypt_type;
  UINT32    Keys_number;
} CBFF_IMAGE_HEADER;

typedef struct {
  UINT32    name;
  UINT32    serial_number;
  UINT64    flags;
} FIP_TOC_HEADER;

typedef struct {
  UINT32    NewBL1Version;
  UINT32    NewBL2Version;
  UINT32    OldBL1Version;
  UINT32    OldBL2Version;
} IMAGE_VERSION_INFO;

typedef struct {
  UINT8    Status;
  UINT8    Type;
} UPDATE_RESULT;

typedef union {
  struct {
    UINT32    revision : 16;
    UINT32    minor    : 8;
    UINT32    major    : 8;
  } Bits;         ///< Bitfield definition of the register
  UINT32    info; ///< The entire 32-bit value
} VERSION_INFO;
#pragma pack()

#endif