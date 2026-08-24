/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
  See ACPI 6.1 Spec, 6.2.11, PCI Firmware Spec 3.0, 4.5
*/
#define PCI_OSC_SUPPORT() \
  Name(SUPP, Zero) /* PCI _OSC Support Field value */ \
  Name(CTRL, Zero) /* PCI _OSC Control Field value */ \
  Method(_OSC,4) { \
    If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))) { \
      /* Create DWord-adressable fields from the Capabilities Buffer */ \
      CreateDWordField(Arg3,0,CDW1) \
      CreateDWordField(Arg3,4,CDW2) \
      CreateDWordField(Arg3,8,CDW3) \
      /* Save Capabilities DWord2 & 3 */ \
      Store(CDW2,SUPP) \
      Store(CDW3,CTRL) \
      /* Only allow native hot plug control if OS supports: */ \
      /* ASPM */ \
      /* Clock PM */ \
      /* MSI/MSI-X */ \
      If(LNotEqual(And(SUPP, 0x16), 0x16)) { \
        And(CTRL,0x1E,CTRL) \
      }\
      \
      /* Grant all standard PCIe OS control capabilities: */ \
      /* PME, AER, PCIeCapability, Hotplug, LTR, DPC */ \
      /* Only mask SHPC (bit 1) — no SHPC controller in this system */ \
      And(CTRL,0xFD,CTRL) \
      If(LNotEqual(Arg1,One)) { /* Unknown revision */ \
        Or(CDW1,0x08,CDW1) \
      } \
      \
      If(LNotEqual(CDW3,CTRL)) { /* Capabilities bits were masked */ \
        Or(CDW1,0x10,CDW1) \
      } \
      \
      /* Update DWORD3 in the buffer */ \
      Store(CTRL,CDW3) \
      Return(Arg3) \
    } Else { \
      Or(CDW1,4,CDW1) /* Unrecognized UUID */ \
      Return(Arg3) \
    } \
  } // End _OSC

Device (PCI0)
{
  Name (_HID, "PNP0A08")
  Name (_CID, "PNP0A03")
  Name (_UID, 0)
  Name (_STR, Unicode ("PCIe 0 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0xC0)
  Name (_CCA, 1)

  // PCIe is only available if PCIe link is up
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_PCIE_RP_00_LINK_STS_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  // Declare the resources assigned to this root complex.
  Method (_CRS, 0, Serialized) {           // Root complex resources, _CRS: current resource setting
    Name (RBUF, ResourceTemplate () {
      WordBusNumber (
        ResourceProducer, // Specify bus ranged is passed to child devices
        MinFixed,         // Specify min address is fixed
        MaxFixed,         // Specify max address is fixed
        PosDecode,        // Positive decode of bus number
        0,                // AddressGranularity 2 power of 0
        0xC0,                // AddressMinimum - Minimum Bus Number
        0xFF,               // AddressMaximum - Maximum Bus Number
        0,                // AddressTranslation - Set to 0
        64)               // RangeLength - Number of Bus

      // PCI memory space
      //  Memory32Fixed (ReadWrite, 0x60000000, 0x20000000, )
      DWordMemory ( // 32-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x60000000,               // Min Base Address
        0x7FFFFFFF,               // Max Base Address
        0x00000000,               // Translate
        0x20000000                // Length 512M
      )

      QWordMemory ( // 64-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x1800000000,             // Min Base Address
        0x1BFFFFFFFF,             // Max Base Address
        0x00000000,               // Translate
        0x400000000,              // Length 16G, resized below
        ,,
        PM64
      )
    })                            // Name(RBUF)

    //
    // The 16 GB above declares the stock window only.  Replace it with the
    // window the firmware actually programmed for this root bridge, which the
    // BIOS "PCIe 64-bit Window" setting can enlarge.  Linux resizes device
    // BARs against what _CRS reports, so this has to agree with what UEFI
    // handed out or a large-BAR GPU will fail to get its BAR.
    //
    CreateQWordField (RBUF, PM64._MIN, M64B)
    CreateQWordField (RBUF, PM64._MAX, M64E)
    CreateQWordField (RBUF, PM64._LEN, M64L)

    Local0 = \_SB.GETV(ARV_PCIE_RP_00_MEM64_BASE_OFFSET)
    Local1 = \_SB.GETV(ARV_PCIE_RP_00_MEM64_SIZE_OFFSET)
    If (LNotEqual (Local1, Zero)) {
      Local0 = ShiftLeft (Local0, ARV_PCIE_MEM64_UNIT_SHIFT)
      Local1 = ShiftLeft (Local1, ARV_PCIE_MEM64_UNIT_SHIFT)
      M64B = Local0
      M64L = Local1
      M64E = Local0 + Local1 - One
    }

    Return (RBUF)
  }                               // Method(_CRS), this method return RBUF!

  // Declare the PCI Routing Table.
  Name (_PRT, Package() {
    // Routing for device 0, all functions.
    // Note: ARM doesn't support LNK nodes, so the third param
    // is 0 and the fourth param is the SPI number of the interrupt
    // line.
    Package() {0x0000FFFF, 0, 0, PCIE_SUBSYS_X8_INTX_OUT0_INTERRUPT_ID}, // INTA
    Package() {0x0000FFFF, 1, 0, PCIE_SUBSYS_X8_INTX_OUT1_INTERRUPT_ID}, // INTB
    Package() {0x0000FFFF, 2, 0, PCIE_SUBSYS_X8_INTX_OUT2_INTERRUPT_ID}, // INTC
    Package() {0x0000FFFF, 3, 0, PCIE_SUBSYS_X8_INTX_OUT3_INTERRUPT_ID}, // INTD

  })

  PCI_OSC_SUPPORT()

}

Device (PCI1)
{
  Name (_HID, "PNP0A08")
  Name (_CID, "PNP0A03")
  Name (_UID, 1)
  Name (_STR, Unicode ("PCIe 1 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0x90)
  Name (_CCA, 1)

  // PCIe is only available if PCIe link is up
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_PCIE_RP_01_LINK_STS_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  // Declare the resources assigned to this root complex.
  Method (_CRS, 0, Serialized) {           // Root complex resources, _CRS: current resource setting
    Name (RBUF, ResourceTemplate () {
      WordBusNumber (
        ResourceProducer, // Specify bus ranged is passed to child devices
        MinFixed,         // Specify min address is fixed
        MaxFixed,         // Specify max address is fixed
        PosDecode,        // Positive decode of bus number
        0,                // AddressGranularity 2 power of 0
        0x90,               // AddressMinimum - Minimum Bus Number
        0xAF,              // AddressMaximum - Maximum Bus Number
        0,                // AddressTranslation - Set to 0
        32)               // RangeLength - Number of Bus

      // PCI memory space
      DWordMemory ( // 32-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x50000000,               // Min Base Address
        0x5FFFFFFF,               // Max Base Address
        0x00000000,               // Translate
        0x10000000                // Length 256M
      )

      QWordMemory ( // 64-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x1400000000,             // Min Base Address
        0x17FFFFFFFF,             // Max Base Address
        0x00000000,               // Translate
        0x400000000,              // Length 16G, resized below
        ,,
        PM64
      )
    })                            // Name(RBUF)

    //
    // The 16 GB above declares the stock window only.  Replace it with the
    // window the firmware actually programmed for this root bridge, which the
    // BIOS "PCIe 64-bit Window" setting can enlarge.  Linux resizes device
    // BARs against what _CRS reports, so this has to agree with what UEFI
    // handed out or a large-BAR GPU will fail to get its BAR.
    //
    CreateQWordField (RBUF, PM64._MIN, M64B)
    CreateQWordField (RBUF, PM64._MAX, M64E)
    CreateQWordField (RBUF, PM64._LEN, M64L)

    Local0 = \_SB.GETV(ARV_PCIE_RP_01_MEM64_BASE_OFFSET)
    Local1 = \_SB.GETV(ARV_PCIE_RP_01_MEM64_SIZE_OFFSET)
    If (LNotEqual (Local1, Zero)) {
      Local0 = ShiftLeft (Local0, ARV_PCIE_MEM64_UNIT_SHIFT)
      Local1 = ShiftLeft (Local1, ARV_PCIE_MEM64_UNIT_SHIFT)
      M64B = Local0
      M64L = Local1
      M64E = Local0 + Local1 - One
    }

    Return (RBUF)
  }                               // Method(_CRS), this method return RBUF!
  // Declare the PCI Routing Table.
  Name (_PRT, Package() {
    // Routing for device 0, all functions.
    // Note: ARM doesn't support LNK nodes, so the third param
    // is 0 and the fourth param is the SPI number of the interrupt
    // line.
    Package() {0x0000FFFF, 0, 0, PCIE_SUBSYS_X4_INTX_OUT0_INTERRUPT_ID}, // INTA
    Package() {0x0000FFFF, 1, 0, PCIE_SUBSYS_X4_INTX_OUT1_INTERRUPT_ID}, // INTB
    Package() {0x0000FFFF, 2, 0, PCIE_SUBSYS_X4_INTX_OUT2_INTERRUPT_ID}, // INTC
    Package() {0x0000FFFF, 3, 0, PCIE_SUBSYS_X4_INTX_OUT3_INTERRUPT_ID}, // INTD

  })

  PCI_OSC_SUPPORT()

}

Device (PCI2)
{
  Name (_HID, "PNP0A08")
  Name (_CID, "PNP0A03")
  Name (_UID, 2)
  Name (_STR, Unicode ("PCIe 2 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0x60)
  Name (_CCA, 1)

  // PCIe is only available if PCIe link is up
  // PCIe is only available if PCIe link is up
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_PCIE_RP_02_LINK_STS_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  // Declare the resources assigned to this root complex.
  Method (_CRS, 0, Serialized) {           // Root complex resources, _CRS: current resource setting
    Name (RBUF, ResourceTemplate () {
      WordBusNumber (
        ResourceProducer, // Specify bus ranged is passed to child devices
        MinFixed,         // Specify min address is fixed
        MaxFixed,         // Specify max address is fixed
        PosDecode,        // Positive decode of bus number
        0,                // AddressGranularity 2 power of 0
        0x60,              // AddressMinimum - Minimum Bus Number
        0X7F,              // AddressMaximum - Maximum Bus Number
        0,                // AddressTranslation - Set to 0
        32)               // RangeLength - Number of Bus

      // PCI memory space
      //  Memory32Fixed (ReadWrite, 0x40000000, 0x10000000, )
      DWordMemory ( // 32-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x40000000,               // Min Base Address
        0x4FFFFFFF,               // Max Base Address
        0x00000000,               // Translate
        0x10000000                // Length 256M
      )

      QWordMemory ( // 64-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x1000000000,             // Min Base Address
        0x13FFFFFFFF,             // Max Base Address
        0x00000000,               // Translate
        0x400000000,              // Length 16G, resized below
        ,,
        PM64
      )
    })                            // Name(RBUF)

    //
    // The 16 GB above declares the stock window only.  Replace it with the
    // window the firmware actually programmed for this root bridge, which the
    // BIOS "PCIe 64-bit Window" setting can enlarge.  Linux resizes device
    // BARs against what _CRS reports, so this has to agree with what UEFI
    // handed out or a large-BAR GPU will fail to get its BAR.
    //
    CreateQWordField (RBUF, PM64._MIN, M64B)
    CreateQWordField (RBUF, PM64._MAX, M64E)
    CreateQWordField (RBUF, PM64._LEN, M64L)

    Local0 = \_SB.GETV(ARV_PCIE_RP_02_MEM64_BASE_OFFSET)
    Local1 = \_SB.GETV(ARV_PCIE_RP_02_MEM64_SIZE_OFFSET)
    If (LNotEqual (Local1, Zero)) {
      Local0 = ShiftLeft (Local0, ARV_PCIE_MEM64_UNIT_SHIFT)
      Local1 = ShiftLeft (Local1, ARV_PCIE_MEM64_UNIT_SHIFT)
      M64B = Local0
      M64L = Local1
      M64E = Local0 + Local1 - One
    }

    Return (RBUF)
  }                               // Method(_CRS), this method return RBUF!

  // Declare the PCI Routing Table.
  Name (_PRT, Package() {
    // Routing for device 0, all functions.
    // Note: ARM doesn't support LNK nodes, so the third param
    // is 0 and the fourth param is the SPI number of the interrupt
    // line.
    Package() {0x0000FFFF, 0, 0, PCIE_SUBSYS_X2_INTX_OUT0_INTERRUPT_ID}, // INTA
    Package() {0x0000FFFF, 1, 0, PCIE_SUBSYS_X2_INTX_OUT1_INTERRUPT_ID}, // INTB
    Package() {0x0000FFFF, 2, 0, PCIE_SUBSYS_X2_INTX_OUT2_INTERRUPT_ID}, // INTC
    Package() {0x0000FFFF, 3, 0, PCIE_SUBSYS_X2_INTX_OUT3_INTERRUPT_ID}, // INTD

  })

  PCI_OSC_SUPPORT()

}

Device (PCI3)
{
  Name (_HID, "PNP0A08")
  Name (_CID, "PNP0A03")
  Name (_UID, 3)
  Name (_STR, Unicode ("PCIe 3 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0x30)
  Name (_CCA, 1)

  // PCIe is only available if PCIe link is up
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_PCIE_RP_03_LINK_STS_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  // Declare the resources assigned to this root complex.
  Method (_CRS, 0, Serialized) {           // Root complex resources, _CRS: current resource setting
    Name (RBUF, ResourceTemplate () {
      WordBusNumber (
        ResourceProducer, // Specify bus ranged is passed to child devices
        MinFixed,         // Specify min address is fixed
        MaxFixed,         // Specify max address is fixed
        PosDecode,        // Positive decode of bus number
        0,                // AddressGranularity 2 power of 0
        0x30,              // AddressMinimum - Minimum Bus Number
        0x4F,              // AddressMaximum - Maximum Bus Number
        0,                // AddressTranslation - Set to 0
        32)               // RangeLength - Number of Bus

      // PCI memory space
      //  Memory32Fixed (ReadWrite, 0x38000000, 0x08000000, )
      DWordMemory ( // 32-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x38000000,               // Min Base Address
        0x3FFFFFFF,               // Max Base Address
        0x00000000,               // Translate
        0x08000000                // Length 128M
      )

      QWordMemory ( // 64-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x0C00000000,             // Min Base Address
        0x0FFFFFFFFF,             // Max Base Address
        0x00000000,               // Translate
        0x400000000,              // Length 16G, resized below
        ,,
        PM64
      )
    })                            // Name(RBUF)

    //
    // The 16 GB above declares the stock window only.  Replace it with the
    // window the firmware actually programmed for this root bridge, which the
    // BIOS "PCIe 64-bit Window" setting can enlarge.  Linux resizes device
    // BARs against what _CRS reports, so this has to agree with what UEFI
    // handed out or a large-BAR GPU will fail to get its BAR.
    //
    CreateQWordField (RBUF, PM64._MIN, M64B)
    CreateQWordField (RBUF, PM64._MAX, M64E)
    CreateQWordField (RBUF, PM64._LEN, M64L)

    Local0 = \_SB.GETV(ARV_PCIE_RP_03_MEM64_BASE_OFFSET)
    Local1 = \_SB.GETV(ARV_PCIE_RP_03_MEM64_SIZE_OFFSET)
    If (LNotEqual (Local1, Zero)) {
      Local0 = ShiftLeft (Local0, ARV_PCIE_MEM64_UNIT_SHIFT)
      Local1 = ShiftLeft (Local1, ARV_PCIE_MEM64_UNIT_SHIFT)
      M64B = Local0
      M64L = Local1
      M64E = Local0 + Local1 - One
    }

    Return (RBUF)
  }                               // Method(_CRS), this method return RBUF!

  // Declare the PCI Routing Table.
  Name (_PRT, Package() {
    // Routing for device 0, all functions.
    // Note: ARM doesn't support LNK nodes, so the third param
    // is 0 and the fourth param is the SPI number of the interrupt
    // line.
    Package() {0x0000FFFF, 0, 0, PCIE_SUBSYS_X1_1_INTX_OUT0_INTERRUPT_ID}, // INTA
    Package() {0x0000FFFF, 1, 0, PCIE_SUBSYS_X1_1_INTX_OUT1_INTERRUPT_ID}, // INTB
    Package() {0x0000FFFF, 2, 0, PCIE_SUBSYS_X1_1_INTX_OUT2_INTERRUPT_ID}, // INTC
    Package() {0x0000FFFF, 3, 0, PCIE_SUBSYS_X1_1_INTX_OUT3_INTERRUPT_ID}, // INTD

  })

  PCI_OSC_SUPPORT()

}

Device (PCI4)
{
  Name (_HID, "PNP0A08")
  Name (_CID, "PNP0A03")
  Name (_UID, 4)
  Name (_STR, Unicode ("PCIe 4 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0x00)
  Name (_CCA, 1)

  // PCIe is only available if PCIe link is up
  // PCIe is only available if PCIe link is up
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_PCIE_RP_04_LINK_STS_OFFSET)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  // Declare the resources assigned to this root complex.
  Method (_CRS, 0, Serialized) {           // Root complex resources, _CRS: current resource setting
    Name (RBUF, ResourceTemplate () {
      WordBusNumber (
        ResourceProducer, // Specify bus ranged is passed to child devices
        MinFixed,         // Specify min address is fixed
        MaxFixed,         // Specify max address is fixed
        PosDecode,        // Positive decode of bus number
        0,                // AddressGranularity 2 power of 0
        0x00,              // AddressMinimum - Minimum Bus Number
        0x1F,              // AddressMaximum - Maximum Bus Number
        0,                // AddressTranslation - Set to 0
        32)               // RangeLength - Number of Bus

      // PCI memory space
      DWordMemory ( // 32-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x30000000,               // Min Base Address
        0x37FFFFFF,               // Max Base Address
        0x00000000,               // Translate
        0x08000000                // Length 128M
      )

      QWordMemory ( // 64-bit BAR Windows
        ResourceProducer, PosDecode,
        MinFixed, MaxFixed,
        Cacheable, ReadWrite,
        0x00000000,               // Granularity
        0x0800000000,             // Min Base Address
        0x0BFFFFFFFF,             // Max Base Address
        0x00000000,               // Translate
        0x400000000,              // Length 16G, resized below
        ,,
        PM64
      )
    })                            // Name(RBUF)

    //
    // The 16 GB above declares the stock window only.  Replace it with the
    // window the firmware actually programmed for this root bridge, which the
    // BIOS "PCIe 64-bit Window" setting can enlarge.  Linux resizes device
    // BARs against what _CRS reports, so this has to agree with what UEFI
    // handed out or a large-BAR GPU will fail to get its BAR.
    //
    CreateQWordField (RBUF, PM64._MIN, M64B)
    CreateQWordField (RBUF, PM64._MAX, M64E)
    CreateQWordField (RBUF, PM64._LEN, M64L)

    Local0 = \_SB.GETV(ARV_PCIE_RP_04_MEM64_BASE_OFFSET)
    Local1 = \_SB.GETV(ARV_PCIE_RP_04_MEM64_SIZE_OFFSET)
    If (LNotEqual (Local1, Zero)) {
      Local0 = ShiftLeft (Local0, ARV_PCIE_MEM64_UNIT_SHIFT)
      Local1 = ShiftLeft (Local1, ARV_PCIE_MEM64_UNIT_SHIFT)
      M64B = Local0
      M64L = Local1
      M64E = Local0 + Local1 - One
    }

    Return (RBUF)
  }                               // Method(_CRS), this method return RBUF!

  // Declare the PCI Routing Table.
  Name (_PRT, Package() {
    // Routing for device 0, all functions.
    // Note: ARM doesn't support LNK nodes, so the third param
    // is 0 and the fourth param is the SPI number of the interrupt
    // line.
    Package() {0x0000FFFF, 0, 0, PCIE_SUBSYS_X1_0_INTX_OUT0_INTERRUPT_ID}, // INTA
    Package() {0x0000FFFF, 1, 0, PCIE_SUBSYS_X1_0_INTX_OUT1_INTERRUPT_ID}, // INTB
    Package() {0x0000FFFF, 2, 0, PCIE_SUBSYS_X1_0_INTX_OUT2_INTERRUPT_ID}, // INTC
    Package() {0x0000FFFF, 3, 0, PCIE_SUBSYS_X1_0_INTX_OUT3_INTERRUPT_ID}, // INTD

  })

  PCI_OSC_SUPPORT()

}

// RES0 (PNP0C02) was removed: its sole QWordMemory claim (0x20000000–0x2FFFFFFF)
// is entirely covered by the Memory32Fixed entries in PRC0–PRC4 _CRS, so the
// kernel rejected it with "could not be reserved".  The MCFG table already
// advertises the ECAM range; no PNP0C02 reservation is needed.