/** @file
  Secondary System Description Table (SSDT)

  Copyright (c) 2021, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - ACPI for CoreSight 1.1, Platform Design Document
  - ACPI for Arm Components  1.0, Platform Design Document

**/

#include "N1SdpAcpiHeader.h"

/*
  See ACPI 6.4 Section 6.2.13

  There are two ways that _PRT can be used.

  In the first model, a PCI Link device is used to provide additional
  configuration information such as whether the interrupt is Level or
  Edge triggered, it is active High or Low, Shared or Exclusive, etc.

  In the second model, the PCI interrupts are hardwired to specific
  interrupt inputs on the interrupt controller and are not
  configurable. In this case, the Source field in _PRT does not
  reference a device, but instead contains the value zero, and the
  Source Index field contains the global system interrupt to which the
  PCI interrupt is hardwired.

  We use the first model with link indirection to set the correct
  interrupt type as PCI defaults (Level Triggered, Active Low) are not
  compatible with GICv2.
*/
#define LNK_DEVICE(Unique_Id, Link_Name, irq)                           \
  Device(Link_Name) {                                                   \
  Name(_HID, EISAID("PNP0C0F"))                                         \
  Name(_UID, Unique_Id)                                                 \
  Name(_PRS, ResourceTemplate() {                                       \
    Interrupt(ResourceProducer, Level, ActiveHigh, Exclusive) { irq }   \
    })                                                                  \
  Method (_CRS, 0) { Return (_PRS) }                                    \
  Method (_SRS, 1) { }                                                  \
  Method (_DIS) { }                                                     \
}

#define PRT_ENTRY(Address, Pin, Link)                                                   \
  Package (4) {                                                                         \
    Address,  /* uses the same format as _ADR */                                        \
    Pin,      /* The PCI pin number of the device (0-INTA, 1-INTB, 2-INTC, 3-INTD)  */  \
    Link,     /* Interrupt allocated via Link device  */                                \
    Zero      /* global system interrupt number (no used) */                            \
}

/*
  See Reference [1] 6.1.1
  "High word-Device #, Low word-Function #. (for example, device 3,
  function 2 is 0x00030002). To refer to all the functions on a device #,
  use a function number of FFFF)."
*/
#define ROOT_PRT_ENTRY(Pin, Link)   PRT_ENTRY(0x0000FFFF, Pin, Link)  // Device 0 for Bridge.

DefinitionBlock("SsdtRemotePci.aml", "SSDT", 1, "ARMLTD", "N1Sdp",
                EFI_ACPI_ARM_OEM_REVISION)
{
  Scope (_SB) {

    // Remote PCI Root Complex
    LNK_DEVICE(9, LNKI, 681)
    LNK_DEVICE(10, LNKJ, 682)
    LNK_DEVICE(11, LNKK, 683)
    LNK_DEVICE(12, LNKL, 684)

    //Remote PCIe root complex
      Device(PCI2) {
      Name (_HID, EISAID("PNP0A08")) // PCI Express Root Bridge
      Name (_CID, EISAID("PNP0A03")) // Compatible PCI Root Bridge
      Name (_SEG, FixedPcdGet32 (PcdRemotePcieSegmentNumber)) // Segment Number
      Name (_BBN, FixedPcdGet32 (PcdRemotePcieBusBaseNumber)) // BusBase Number
      Name (_CCA, 1)                 // Cache Coherency Attribute

      // Remote Root Complex 0
      Device (RP0) {
        Name(_ADR, 0xF0000000)       // Dev 0, Func 0
      }

      // PCI Routing Table
      Name(_PRT, Package() {
        ROOT_PRT_ENTRY(0, LNKI),     // INTA
        ROOT_PRT_ENTRY(1, LNKJ),     // INTB
        ROOT_PRT_ENTRY(2, LNKK),     // INTC
        ROOT_PRT_ENTRY(3, LNKL),     // INTD
      })

      // Root complex resources
      Method (_CRS, 0, Serialized) {
        Name (RBUF, ResourceTemplate () {
          WordBusNumber (                             // Bus numbers assigned to this root
            ResourceProducer,
            MinFixed,
            MaxFixed,
            PosDecode,
            0,                                        // AddressGranularity
            FixedPcdGet32 (PcdPcieBusMin),            // AddressMinimum - Minimum Bus Number
            FixedPcdGet32 (PcdPcieBusMax),            // AddressMaximum - Maximum Bus Number
            0,                                        // AddressTranslation - Set to 0
            FixedPcdGet32 (PcdPcieBusCount)           // RangeLength - Number of Busses
          )

          QWordMemory (                               // 32-bit BAR Windows
            ResourceProducer,
            PosDecode,
            MinFixed,
            MaxFixed,
            Cacheable,
            ReadWrite,
            0x00000000,                               // Granularity
            FixedPcdGet32 (PcdPcieMmio32Base),        // Min Base Address
            FixedPcdGet32 (PcdPcieMmio32MaxBase),     // Max Base Address
            FixedPcdGet32 (PcdRemotePcieMmio32Translation), // Translate
            FixedPcdGet32 (PcdPcieMmio32Size)         // Length
          )

          QWordMemory (                               // 64-bit BAR Windows
            ResourceProducer,
            PosDecode,
            MinFixed,
            MaxFixed,
            Cacheable,
            ReadWrite,
            0x00000000,                               // Granularity
            FixedPcdGet64 (PcdPcieMmio64Base),        // Min Base Address
            FixedPcdGet64 (PcdPcieMmio64MaxBase),     // Max Base Address
            FixedPcdGet64 (PcdRemotePcieMmio64Translation), // Translate
            FixedPcdGet64 (PcdPcieMmio64Size)         // Length
          )

          QWordIo (                                   // IO window
            ResourceProducer,
            MinFixed,
            MaxFixed,
            PosDecode,
            EntireRange,
            0x00000000,                               // Granularity
            FixedPcdGet32 (PcdPcieIoBase),            // Min Base Address
            FixedPcdGet32 (PcdPcieIoMaxBase),         // Max Base Address
            FixedPcdGet64 (PcdRemotePcieIoTranslation), // Translate
            FixedPcdGet32 (PcdPcieIoSize),            // Length
            ,
            ,
            ,
            TypeTranslation
          )
        }) // Name(RBUF)

        Return (RBUF)
      } // Method (_CRS)
    }
  }
}
