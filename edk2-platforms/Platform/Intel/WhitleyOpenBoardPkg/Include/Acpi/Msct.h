/** @file
  This file describes the contents of the ACPI Maximum System Characteristics Table (MSCT).
  Some additional ACPI values are defined in Acpi1_0.h, Acpi2_0.h, and Acpi3_0.h.
  All changes to the MSCT contents should be done in this file.

  @copyright
  Copyright 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _MSCT_H_
#define _MSCT_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include <Platform.h>
#include <UncoreCommonIncludes.h>

//
// MSCT Definitions, see specification for details.
//
#ifndef EFI_ACPI_6_2_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE_SIGNATURE
#define EFI_ACPI_6_2_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE_SIGNATURE       0x5443534D
#endif
#define EFI_ACPI_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE_REVISION            0x01
#define EFI_ACPI_OEM_MSCT_REVISION                                        0x00000001
#define EFI_ACPI_MAXIMUM_PROXIMITY_DOMAIN_INFORMATION_STRUCTURE_REVISION  0x01


//
// MSCT Table definition
//
#pragma pack(1)

typedef struct {
  UINT8   Revision;
  UINT8   Length;
  UINT32  ProxDomRangeLow;
  UINT32  ProxDomRangeHigh;
  UINT32  MaxProcessorCapacity;
  UINT64  MaxMemoryCapacity;
} EFI_ACPI_MAXIMUM_PROXIMITY_DOMAIN_INFORMATION_STRUCTURE;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                              Header;
  UINT32                                                   OffsetProxDomInfo;
  UINT32                                                   MaxNumProxDom;
  UINT32                                                   MaxNumClockDom;
  UINT64                                                   MaxPhysicalAddress;
  EFI_ACPI_MAXIMUM_PROXIMITY_DOMAIN_INFORMATION_STRUCTURE  ProxDomInfoStructure[MAX_SOCKET];
} EFI_ACPI_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE;

#pragma pack()

#endif  //_MSCT_H_
