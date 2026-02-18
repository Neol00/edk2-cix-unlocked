/** @file
  This file describes the contents of the ACPI High Precision Event Timer Description Table
  HPET).  Some additional ACPI values are defined in Acpi1_0.h, Acpi2_0.h, and Acpi3_0.h
  All changes to the HPET contents should be done in this file.

  @copyright
  Copyright 1999 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _HPET_H_
#define _HPET_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>

//
// HPET Definitions
//
#define EFI_ACPI_OEM_HPET_REVISION    0x00000001

#define EFI_ACPI_EVENT_TIMER_BLOCK_ID 0x8086A701

//
// Event Timer Block Base Address Information
//
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS_SPACE_ID EFI_ACPI_6_2_SYSTEM_MEMORY
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_WIDTH        0x00
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_OFFSET       0x00
#define EFI_ACPI_EVENT_TIMER_ACCESS_SIZE            0x00
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS          0x00000000FED00000

#define EFI_ACPI_HPET_NUMBER                        0x00

#define EFI_ACPI_MIN_CLOCK_TICK                     0x0080

#define EFI_ACPI_HPET_ATTRIBUTES                    0x00

#endif
