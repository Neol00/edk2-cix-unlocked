/** @file
  This file describes the contents of the MIGT ACPI table.

  @copyright
  Copyright 1999 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _MIGT_H_
#define _MIGT_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>

//
// MIGT ACPI structure
//
typedef struct {

  EFI_ACPI_DESCRIPTION_HEADER             Header;
  // MIGT Specific Entries
  EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE  ControlRegister;
  UINT32                                  ControlRegisterValue;
  EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE  ActionRegion;

} EFI_MIGT_ACPI_DESCRIPTION_TABLE;


//
// MIGT ACPI Definitions
//
#define MIGT_SMI_SERVICE_ID                       0xFD        // Door Bell

#define EFI_MIGT_ACPI_TABLE_SIGNATURE             SIGNATURE_32('M','I','G','T')
#define EFI_MIGT_ACPI_DESCRIPTION_TABLE_REVISION  0x01
#define EFI_MIGT_ACPI_OEM_REVISION                0x00000000

//
// MIGT Control Register Generic Address Information
//
#define EFI_MIGT_CR_ACPI_ADDRESS_SPACE_ID         EFI_ACPI_6_2_SYSTEM_IO
#define EFI_MIGT_CR_ACPI_REGISTER_BIT_WIDTH       0x8
#define EFI_MIGT_CR_ACPI_REGISTER_BIT_OFFSET      0x0
#define EFI_MIGT_CR_ACPI_SMI_ADDRESS              0xB2

//
// MIGT Action Region Generic Address Information
//
#define EFI_MIGT_AR_ACPI_ADDRESS_SPACE_ID         EFI_ACPI_6_2_SYSTEM_MEMORY
#define EFI_MIGT_AR_ACPI_REGISTER_BIT_WIDTH       64
#define EFI_MIGT_AR_ACPI_REGISTER_BIT_OFFSET      0x0
#define EFI_MIGT_AR_ACPI_MEMORY_ADDRESS           0x0

#endif
