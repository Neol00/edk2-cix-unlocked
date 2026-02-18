/** @file
  This file describes the contents of the ACPI Serial Port Console Redirection
  Table (SPCR).  Some additional ACPI 2.0 values are defined in Acpi2_0.h.
  All changes to the Spcr contents should be done in this file.

  @copyright
  Copyright 1999 - 2002 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SPCR_H_
#define _SPCR_H_

//
// SPCR Definitions, see TBD specification for details.
//

#define EFI_ACPI_OEM_SPCR_REVISION 0x00000000 // TBD

#define EFI_ACPI_INTERFACE_TYPE                     0x00 // full 16550 interface.

//
// Base Address
//
#define EFI_ACPI_BASE_ADDRESS_ADDRESS_SPACE_ID      EFI_ACPI_6_2_SYSTEM_IO
#define EFI_ACPI_BASE_ADDRESS_BIT_WIDTH             0x08
#define EFI_ACPI_BASE_ADDRESS_BIT_OFFSET            0x00
#define EFI_ACPI_BASE_ADDRESS_ADDRESS               0x0000000000000000 // Updated at run time

#define EFI_ACPI_INTERRUPT_TYPE                     0x03 // Dual-8259 and IOAPIC
#define EFI_ACPI_IRQ                                0x04 // Updated at run time
#define EFI_ACPI_GLOBAL_SYSTEM_INTERRUPT            0x04 // Updated at run time
#define EFI_ACPI_BAUD_RATE                          0x07 // 115200
#define EFI_ACPI_PARITY                             0x00 // No parity
#define EFI_ACPI_STOP_BITS                          0x00 // 0 stop bit
#define EFI_ACPI_FLOW_CONTROL                       0x00 // No flow control
#define EFI_ACPI_TERMINAL_TYPE                      0x03 // ANSI
#define EFI_ACPI_LANGUAGE                           0x00 // Reserved
#define EFI_ACPI_PCI_DEVICE_ID                      0xFFFF
#define EFI_ACPI_PCI_VENDOR_ID                      0xFFFF
#define EFI_ACPI_PCI_BUS_NUMBER                     0x00
#define EFI_ACPI_PCI_DEVICE_NUMBER                  0x00
#define EFI_ACPI_PCI_FUNCTION_NUMBER                0x00
#define EFI_ACPI_PCI_FLAGS                          0x00000000
#define EFI_ACPI_PCI_SEGMENT                        0x00

#endif
