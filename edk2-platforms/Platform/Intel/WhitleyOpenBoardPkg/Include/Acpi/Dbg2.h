/** @file

  @copyright
  Copyright 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _DBG2_H_
#define _DBG2_H_

//
// Statements that include other files
//
#include <IndustryStandard/DebugPort2Table.h>

#pragma pack(1)
typedef
struct {
  EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE       Table;
  EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT Entry;
  EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE        BaseAdressRegister;
  UINT32                                        AddressSize;
  CHAR8                                         NamespaceString[2];

} DBG2_DEBUG_TABLE;
#pragma pack()


#endif  //_DBG2_H_
