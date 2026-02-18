/** @file

  @copyright
  Copyright 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SPMI_H_
#define _SPMI_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>

//
// Spmi Definitions, see specification for details.
//
#ifndef EFI_ACPI_6_2_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_SIGNATURE
#define EFI_ACPI_6_2_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_SIGNATURE         0x494D5053
#endif

#define EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_REVISION             0x05
#define EFI_ACPI_OEM_SPMI_REVISION                                                0x00000001
#define EFI_ACPI_SPMI_SPECIFICATION_REVISION                                      0x0200


//
// SPMI Table definition
//
#pragma pack(1)

#define KEYBOARD_CONTROLLER_STYLE          0x01
#define SYSTEM_IO                          0x01
#define IPMI_DEFAULT_IO_BASE               0xCA2

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                              Header;
  UINT8                                                    InterfaceType;                      // 36
  UINT8                                                    Reserved37;                         // 37
  UINT16                                                   SpecificationRevision;              // 38 - 39
  UINT8                                                    InterruptType;                      // 40
  UINT8                                                    GPE;                                // 41
  UINT8                                                    Reserved42;                         // 42
  UINT8                                                    PciDeviceFlag;                      // 43
  UINT32                                                   GlobalSystemInterrupt;              // 44 - 47
  EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE                   BaseAddress;                        // 48 - 59
  UINT8                                                    UidByte1;                           // 60 LSB
  UINT8                                                    UidByte2;                           // 61
  UINT8                                                    UidByte3;                           // 62
  UINT8                                                    UidByte4;                           // 63 MSB
  UINT8                                                    Reserved64;                         // 64
} EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_TABLE;


#pragma pack()

#endif  //_SPMI_H_
