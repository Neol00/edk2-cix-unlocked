/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef _BOOT_SCRIPT_INTERNAL_FORMAT_H_
#define _BOOT_SCRIPT_INTERNAL_FORMAT_H_

#pragma pack(1)

// *******************************************
// CIX Boot Script Opcode definitions
// *******************************************
#define CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE       0x00
#define CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE  0x01
#define CIX_S3_BOOT_SCRIPT_TABLE_OPCODE           0xAA
#define CIX_S3_BOOT_SCRIPT_TERMINATE_OPCODE       0xFF
//
// Boot Script Opcode Header Structure Definitions
//

typedef struct {
  UINT8    OpCode;
  UINT8    Length;
} CIX_BOOT_SCRIPT_GENERIC_HEADER;

typedef struct {
  UINT8     OpCode;
  UINT8     Length;
  UINT16    Version;
  UINT32    TableLength;
  UINT16    Reserved[2];
} CIX_BOOT_SCRIPT_TABLE_HEADER;

typedef struct {
  UINT8     OpCode;
  UINT8     Length;
  UINT32    Width;
} CIX_BOOT_SCRIPT_COMMON_HEADER;

typedef struct {
  UINT8     OpCode;
  UINT8     Length;
  UINT8     Width;
  UINT8     Count;
  UINT64    Address;
} CIX_BOOT_SCRIPT_MEM_WRITE;

typedef struct {
  UINT8     OpCode;
  UINT8     Length;
  UINT8     Width;
  UINT64    Address;
} CIX_BOOT_SCRIPT_MEM_READ_WRITE;

typedef struct {
  UINT8    OpCode;
  UINT8    Length;
} CIX_BOOT_SCRIPT_TERMINATE;

#pragma pack()

#define BOOT_SCRIPT_TABLE_VERSION  0x0001

#endif
