/** @file
  This file describes the contents of the BDAT ACPI.

  @copyright
  Copyright 1999 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _BDAT_H_
#define _BDAT_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>

//
// Ensure proper structure formats
//
#pragma pack(1)

#define EFI_BDAT_TABLE_SIGNATURE          SIGNATURE_32('B','D','A','T')

#define BDAT_PRIMARY_VER    0x0004
#define BDAT_SECONDARY_VER  0x0000

typedef struct {
  UINT8   BiosDataSignature[8]; // "BDATHEAD"
  UINT32  BiosDataStructSize;   // sizeof BDAT_STRUCTURE + sizeof BDAT_MEMORY_DATA_STRUCTURE + sizeof BDAT_RMT_STRUCTURE
  UINT16  Crc16;                // 16-bit CRC of BDAT_STRUCTURE (calculated with 0 in this field)
  UINT16  Reserved;
  UINT16  PrimaryVersion;       // Primary version
  UINT16  SecondaryVersion;     // Secondary version
  UINT32  OemOffset;            // Optional offset to OEM-defined structure
  UINT32  Reserved1;
  UINT32  Reserved2;
} BDAT_HEADER_STRUCTURE;

typedef struct bdatSchemaList {
  UINT16                       SchemaListLength; //Number of Schemas present
  UINT16                       Reserved;
  UINT16                       Year;
  UINT8                        Month;
  UINT8                        Day;
  UINT8                        Hour;
  UINT8                        Minute;
  UINT8                        Second;
  UINT8                        Reserved1;
  //
  // This is a dynamic region, where Schema list address is filled out.
  // Each schema location is 32 bits long and complies with BDAT 4.0 version.
  //
} BDAT_SCHEMA_LIST_STRUCTURE;

//BDAT Header Struct which contains information all exisitng BDAT Schemas
typedef struct bdatStruct {
  BDAT_HEADER_STRUCTURE        BdatHeader;
  BDAT_SCHEMA_LIST_STRUCTURE   BdatSchemas;
} BDAT_STRUCTURE;

//
// BIOS Data ACPI structure
//
typedef struct {

  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE  BdatGas;

} EFI_BDAT_ACPI_DESCRIPTION_TABLE;

//
// BIOS Data Parameter Region Generic Address
// Information
//
#define EFI_BDAT_ACPI_POINTER             0x0

#define ___INTERNAL_CONVERT_TO_STRING___(a) #a
#define CONVERT_TO_STRING(a) ___INTERNAL_CONVERT_TO_STRING___(a)

//
// This is copied from Include\Acpi.h
//
#define CREATOR_ID_INTEL 0x4C544E49  //"LTNI""INTL"(Intel)
#define CREATOR_REV_INTEL 0x20090903

#define EFI_ACPI_TABLE_VERSION_ALL  (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_2_0|EFI_ACPI_TABLE_VERSION_3_0)

#pragma pack()

#endif
