/** @file
  This file describes the contents of the ACPI Multiple APIC Description
  Table (MADT).  Some additional ACPI values are defined in Acpi1_0.h and
  Acpi2_0.h.
  To make changes to the MADT, it is necessary to update the count for the
  APIC structure being updated, and to modify table found in Madt.c.

  @copyright
  Copyright 1996 - 2014 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _MADT_H
#define _MADT_H

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include "Platform.h"

//
// MADT Definitions
//
#define EFI_ACPI_OEM_MADT_REVISION                      0x00000000
//
// Multiple APIC Flags are defined in AcpiX.0.h
//
#define EFI_ACPI_6_2_MULTIPLE_APIC_FLAGS  (EFI_ACPI_6_2_PCAT_COMPAT)

//
// Local APIC address
//
#define EFI_ACPI_LOCAL_APIC_ADDRESS 0xFEE00000
//
// Define the number of each table type.
// This is where the table layout is modified.
//
#define EFI_ACPI_PROCESSOR_LOCAL_APIC_COUNT           MAX_CPU_NUM
#define EFI_ACPI_LOCAL_APIC_NMI_COUNT                 MAX_CPU_NUM
#define EFI_ACPI_PROCESSOR_LOCAL_X2APIC_COUNT         MAX_CPU_NUM
#define EFI_ACPI_LOCAL_X2APIC_NMI_COUNT               MAX_CPU_NUM
#define EFI_ACPI_IO_APIC_COUNT                        32 + 1  // IIO I/O APIC (PCH) +  I/O APIC (PC00-PC31)
#define EFI_ACPI_INTERRUPT_SOURCE_OVERRIDE_COUNT      2
#define EFI_ACPI_NON_MASKABLE_INTERRUPT_SOURCE_COUNT  0
#define EFI_ACPI_LOCAL_APIC_ADDRESS_OVERRIDE_COUNT    0
#define EFI_ACPI_IO_SAPIC_COUNT                       0
#define EFI_ACPI_PROCESSOR_LOCAL_SAPIC_COUNT          0
#define EFI_ACPI_PLATFORM_INTERRUPT_SOURCES_COUNT     0

//
// MADT structure
//
//
// Ensure proper structure formats
//
#pragma pack(1)
//
// ACPI 4.0 Table structure
//
typedef struct {
  EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER   Header;

#if EFI_ACPI_PROCESSOR_LOCAL_APIC_COUNT > 0              // Type 0x00
  EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_STRUCTURE           LocalApic[EFI_ACPI_PROCESSOR_LOCAL_APIC_COUNT];
#endif

#if EFI_ACPI_IO_APIC_COUNT > 0                          // Type 0x01
  EFI_ACPI_6_2_IO_APIC_STRUCTURE                        IoApic[EFI_ACPI_IO_APIC_COUNT];
#endif

#if EFI_ACPI_INTERRUPT_SOURCE_OVERRIDE_COUNT > 0        // Type 0x02
  EFI_ACPI_6_2_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE      Iso[EFI_ACPI_INTERRUPT_SOURCE_OVERRIDE_COUNT];
#endif

#if EFI_ACPI_NON_MASKABLE_INTERRUPT_SOURCE_COUNT > 0    // Type 0x03
  EFI_ACPI_6_2_NON_MASKABLE_INTERRUPT_SOURCE_STRUCTURE  NmiSource[EFI_ACPI_NON_MASKABLE_INTERRUPT_SOURCE_COUNT];
#endif

#if EFI_ACPI_LOCAL_APIC_NMI_COUNT > 0                    // Type 0x04
  EFI_ACPI_6_2_LOCAL_APIC_NMI_STRUCTURE                 LocalApicNmi;
#endif

#if EFI_ACPI_LOCAL_APIC_ADDRESS_OVERRIDE_COUNT > 0      // Type 0x05
  EFI_ACPI_6_2_LOCAL_APIC_ADDRESS_OVERRIDE_STRUCTURE    LocalApicOverride[EFI_ACPI_LOCAL_APIC_OVERRIDE_COUNT];
#endif

#if EFI_ACPI_IO_SAPIC_COUNT > 0                          // Type 0x06
  EFI_ACPI_6_2_IO_SAPIC_STRUCTURE                       IoSapic[EFI_ACPI_IO_SAPIC_COUNT];
#endif

#if EFI_ACPI_PROCESSOR_LOCAL_SAPIC_COUNT > 0            // Type 0x07 : This table changes in madt 2.0
  EFI_ACPI_6_2_PROCESSOR_LOCAL_SAPIC_STRUCTURE          LocalSapic[EFI_ACPI_PROCESSOR_LOCAL_SAPIC_COUNT];
#endif

#if EFI_ACPI_PLATFORM_INTERRUPT_SOURCES_COUNT > 0        // Type 0x08
  EFI_ACPI_6_2_PLATFORM_INTERRUPT_SOURCES_STRUCTURE     PlatformInterruptSources[
    EFI_ACPI_PLATFORM_INTERRUPT_SOURCES_COUNT];
#endif

#if EFI_ACPI_PROCESSOR_LOCAL_X2APIC_COUNT  > 0          //Type 0x09
  EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_STRUCTURE LocalX2Apic[EFI_ACPI_PROCESSOR_LOCAL_X2APIC_COUNT];
#endif

#if EFI_ACPI_LOCAL_X2APIC_NMI_COUNT > 0                  //Type 0x0A
  EFI_ACPI_6_2_LOCAL_X2APIC_NMI_STRUCTURE X2ApicNmi;
#endif


} EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE;




#pragma pack()

#endif
