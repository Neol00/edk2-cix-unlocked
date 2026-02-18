/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifdef PCIE_X8_VCC_REGULATOR
External (PCIE_X8_VCC_REGULATOR, DeviceObj)
#endif

#ifdef PCIE_X4_VCC_REGULATOR
External (PCIE_X4_VCC_REGULATOR, DeviceObj)
#endif

#ifdef PCIE_X2_VCC_REGULATOR
External (PCIE_X2_VCC_REGULATOR, DeviceObj)
#endif

Device (PRC0) { /* PCIE0 X8 */
  Name (_HID, "CIXH2020")
  Name (_UID, 0x0)
  Name (_STR, Unicode ("PCIe 0 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0xC0)
  Name (_CCA, 1)

  Name (MLKS, 4)
  Name (NUML, 8)
  Name (MPAL, 512)
  Name (MAPM, 3)
  Name (ASPM, 0)

  Method(_INI, 0) {
    Local0 = GETV(ARV_PCIE_RP_00_BANDWITCH_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
        case (1) { Local1=2 }
        case (2) { Local1=4 }
        case (3) { Local1=8 }
      }
      if (Local1 != 0) { NUML = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_00_MAX_SPEED_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
        case (1) { Local1=2 }
        case (2) { Local1=3 }
        case (3) { Local1=4 }
      }
      if (Local1 != 0) { MLKS = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_00_MAX_PAYLOAD_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=128 }
        case (1) { Local1=256 }
        case (2) { Local1=512 }
      }
      if (Local1 != 0) { MPAL = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_00_MAX_ASPM_OFFSET)
    if (Local0 != 0) { MAPM = Local0 }
    Local0 = GETV(ARV_PCIE_RP_00_ASPM_OFFSET)
    if (Local0 != 0) { ASPM = Local0 }
  }

  Method (_STA, 0x0, Serialized) {
    Return (0xF)
  }
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0a010000, 0x10000)
    Memory32Fixed (ReadWrite, 0x0a000000, 0x10000)
    Memory32Fixed (ReadWrite, 0x2c000000, 0x4000000)
    Memory32Fixed (ReadWrite, 0x60000000, 0x100000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X8_CORRECTABLE_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X8_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X8_NON_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X8_LOCAL_INTERRUPT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X8_PHY_INTERRUPT_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X8_PHY_STATE_CHG_STS_P_0_INTERRUPT_ID }

    WordBusNumber (
      ResourceProducer, // Specify bus ranged is passed to child devices
      MinFixed,         // Specify min address is fixed
      MaxFixed,         // Specify max address is fixed
      PosDecode,        // Positive decode of bus number
      0,                // AddressGranularity 2 power of 0
      0xC0,               // AddressMinimum - Minimum Bus Number
      0xFF,              // AddressMaximum - Maximum Bus Number
      0,                // AddressTranslation - Set to 0
      64)               // RangeLength - Number of Bus

    // PCI IO space
    DWordIo ( // 32-bit BAR Windows
      ResourceProducer,
      MinFixed, MaxFixed,
      , ,
      0x00000000,               // Granularity
      0x60100000,               // Min Base Address
      0x601FFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x00100000                // Length 1M
    )

    // PCI memory space
    DWordMemory ( // 32-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x60200000,               // Min Base Address
      0x6FFFFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x0fe00000                // Length 254M
    )

    QWordMemory ( // 64-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x1800000000,             // Min Base Address
      0x1BFFFFFFFF,             // Max Base Address
      0x00000000,               // Translate
      0x400000000               // Length 16G
    )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_pcie_x8_rc", ResourceConsumer,)
#if PCIE_X8_PERST
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
                PCIE_X8_PERST_GPIO_CTR, 0, ResourceConsumer) { PCIE_X8_PERST_GPIO }
#endif
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "device_type", "pci" },
          Package () { "vendor-id", 0x1f6c },
          Package () { "device-id", 0x0001 },
          Package () { "bus-range", Package () { 0xc0, 0xff } },
          Package () { "max-link-speed", MLKS },
          Package () { "num-lanes", NUML },
          Package () { "cdns,no-inbound-bar", 0 },
          Package () { "sky1,pcie-ctrl-id", 0x0 },
          Package () { "sky1,aer-uncor-panic", 0 },
          Package () { "cdns,pcie-phy", \_SB.PCP0.PX8P },
          Package () { "max-payload", MPAL },
          Package () { "max-aspm-support", MAPM },
          Package () { "aspm", ASPM },
#if PCIE_X8_PERST
          Package () { "reset-gpios", Package () { ^PRC0, 0, 0, PERST_GPIO_ACTIVE_LEVEL } },
#endif
#if PCIE_X8_VCC_SUPPLY
          Package () { "vcc-pcie-supply", PCIE_X8_VCC_REGULATOR },
#endif
#if PCIE_X8_STR_PWRON
          Package () { "sky1,str-pwron", 1 },
#endif
#if PCIE_X8_STD_PWRON
          Package () { "sky1,std-pwron", 1 },
#endif
        },
  })

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

  Name (CLKT, Package() {
    Package() {CLK_TREE_PCIE_CTRL0_CLK, "axi_clk", \_SB.PRC0},
    Package() {CLK_TREE_PCIE_X8CTRL_APB, "apb_clk", \_SB.PRC0},
    Package() {CLK_TREE_PCIE_REF_B0, "refclk_b", \_SB.PRC0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_PCIE0_RESET_N, \_SB.PRC0, "pcie_reset"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.PCP0 , \_SB.PRC0, 0},
#if PCIE_X8_VCC_SUPPLY
    Package() {PCIE_X8_VCC_REGULATOR , \_SB.PRC0, 0},
#endif
  })
  Name (RSNL, Package() {
    Package() {\_SB.PRC0 , RESOURCE_MEM, 0, "reg"},
    Package() {\_SB.PRC0 , RESOURCE_MEM, 1, "rcsu"},
    Package() {\_SB.PRC0 , RESOURCE_MEM, 2, "cfg"},
    Package() {\_SB.PRC0 , RESOURCE_MEM, 3, "msg"},
    Package() {\_SB.PRC0 , RESOURCE_IRQ, 0, "aer_c"},
    Package() {\_SB.PRC0 , RESOURCE_IRQ, 1, "aer_f"},
    Package() {\_SB.PRC0 , RESOURCE_IRQ, 2, "aer_nf"},
    Package() {\_SB.PRC0 , RESOURCE_IRQ, 3, "local"},
    Package() {\_SB.PRC0 , RESOURCE_IRQ, 4, "phy_int"},
    Package() {\_SB.PRC0 , RESOURCE_IRQ, 5, "phy_sta"},
  })

  OperationRegion(OPR0,SystemMemory,PCIE_X8_RCSU_PD_REG, 0x04)
  Field (OPR0, DWordAcc, NoLock, Preserve) {
    MSK0, 32,
  }
  Method(PWON, 0, Serialized){
    Local0 = MSK0
    Local0 = Local0 | PGFSM_REG_CTRL | TIME_CYCLE_CNT
    MSK0 = Local0
    \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_PCIE, PCIE_X8_RCSU_BASE_REG, BIT0)
  }
}

Device (PCP0) //PCIE PHY1
{
  Name (_HID, "CIXH2023")     // _HID: Hardware ID
  Name (_UID, 0x00)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute

  Method (_STA, 0x0, Serialized) {
    Return (0xF)
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0a020000, 0x40000)
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_PCIE_X8_PHY_APB, "pclk", \_SB.PCP0},
    Package() {CLK_TREE_PCIE_REF_PHY_X8, "refclk", \_SB.PCP0},
  })

  Device(PX8P)
  {
    Name (_ADR, 0x00)

    Name (NUML, 8)
    Method(_INI, 0) {
      Local0 = GETV(ARV_PCIE_RP_00_BANDWITCH_OFFSET)
      if (Local0 != 0) {
        Local1 = 0
        Switch(Local0) {
          case (0) { Local1=1 }
          case (1) { Local1=2 }
          case (2) { Local1=4 }
          case (3) { Local1=8 }
        }
        if (Local1 != 0) { NUML = Local1 }
      }
    }
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "reg", 0 },
            Package () { "num-lanes", NUML },
      },
    })
  }
}

Device (PRC1) { /* PCIE2 X4 */
  Name (_HID, "CIXH2020")
  Name (_UID, 0x1)
  Name (_STR, Unicode ("PCIe 1 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0x90)
  Name (_CCA, 1)

  Name (MLKS, 4)
  Name (NUML, 4)
  Name (MPAL, 512)
  Name (MAPM, 3)
  Name (ASPM, 0)

  Method(_INI, 0) {
    Local0 = GETV(ARV_PCIE_RP_01_BANDWITCH_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
        case (1) { Local1=2 }
        case (2) { Local1=4 }
      }
      if (Local1 != 0) { NUML = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_01_MAX_SPEED_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
        case (1) { Local1=2 }
        case (2) { Local1=3 }
        case (3) { Local1=4 }
      }
      if (Local1 != 0) { MLKS = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_01_MAX_PAYLOAD_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=128 }
        case (1) { Local1=256 }
        case (2) { Local1=512 }
      }
      if (Local1 != 0) { MPAL = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_01_MAX_ASPM_OFFSET)
    if (Local0 != 0) { MAPM = Local0 }
    Local0 = GETV(ARV_PCIE_RP_01_ASPM_OFFSET)
    if (Local0 != 0) { ASPM = Local0 }
  }

  Method (_STA, 0x0, Serialized) {
    Return (0xF)
  }
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0a070000, 0x10000)
    Memory32Fixed (ReadWrite, 0x0a060000, 0x10000)
    Memory32Fixed (ReadWrite, 0x29000000, 0x3000000)
    Memory32Fixed (ReadWrite, 0x50000000, 0x100000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X4_CORRECTABLE_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X4_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X4_NON_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X4_LOCAL_INTERRUPT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X4_PHY_INTERRUPT_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X4_PHY_STATE_CHG_STS_P_0_INTERRUPT_ID }

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

    // PCI IO space
    DWordIo ( // 32-bit BAR Windows
      ResourceProducer,
      MinFixed, MaxFixed,
      , ,
      0x00000000,               // Granularity
      0x50100000,               // Min Base Address
      0x501FFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x00100000                // Length 1M
    )

    // PCI memory space
    DWordMemory ( // 32-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x50200000,               // Min Base Address
      0x5FFFFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x0fe00000                // Length 254M
    )

    QWordMemory ( // 64-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x1400000000,             // Min Base Address
      0x17FFFFFFFF,             // Max Base Address
      0x00000000,               // Translate
      0x400000000               // Length 16G
    )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_pcie_x4_rc", ResourceConsumer,)
#if PCIE_X4_PERST
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
                PCIE_X4_PERST_GPIO_CTR, 0, ResourceConsumer) { PCIE_X4_PERST_GPIO }
#endif
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "device_type", "pci" },
          Package () { "vendor-id", 0x1f6c },
          Package () { "device-id", 0x0001 },
          Package () { "bus-range", Package () { 0x90, 0xbf } },
          Package () { "max-link-speed", MLKS },
          Package () { "num-lanes", NUML },
          Package () { "cdns,no-inbound-bar", 0 },
          Package () { "sky1,pcie-ctrl-id", 0x1 },
          Package () { "sky1,aer-uncor-panic", 0 },
          Package () { "cdns,pcie-phy", \_SB.PCP1.PX4P },
          Package () { "max-payload", MPAL },
          Package () { "max-aspm-support", MAPM },
          Package () { "aspm", ASPM },
#if PCIE_X4_PERST
          Package () { "reset-gpios", Package () { ^PRC1, 0, 0, PERST_GPIO_ACTIVE_LEVEL } },
#endif
#if PCIE_X4_VCC_SUPPLY
          Package () { "vcc-pcie-supply", PCIE_X4_VCC_REGULATOR },
#endif
#if PCIE_X4_STR_PWRON
          Package () { "sky1,str-pwron", 1 },
#endif
#if PCIE_X4_STD_PWRON
          Package () { "sky1,std-pwron", 1 },
#endif
        },
  })

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

  Name (CLKT, Package() {
    Package() {CLK_TREE_PCIE_CTRL1_CLK, "axi_clk", \_SB.PRC1},
    Package() {CLK_TREE_PCIE_X4CTRL_APB, "apb_clk", \_SB.PRC1},
    Package() {CLK_TREE_PCIE_REF_B1, "refclk_b", \_SB.PRC1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_PCIE1_RESET_N, \_SB.PRC1, "pcie_reset"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.PCP1 , \_SB.PRC1, 0},
#if PCIE_X4_VCC_SUPPLY
    Package() {PCIE_X4_VCC_REGULATOR , \_SB.PRC1, 0},
#endif
  })
  Name (RSNL, Package() {
    Package() {\_SB.PRC1 , RESOURCE_MEM, 0, "reg"},
    Package() {\_SB.PRC1 , RESOURCE_MEM, 1, "rcsu"},
    Package() {\_SB.PRC1 , RESOURCE_MEM, 2, "cfg"},
    Package() {\_SB.PRC1 , RESOURCE_MEM, 3, "msg"},
    Package() {\_SB.PRC1 , RESOURCE_IRQ, 0, "aer_c"},
    Package() {\_SB.PRC1 , RESOURCE_IRQ, 1, "aer_f"},
    Package() {\_SB.PRC1 , RESOURCE_IRQ, 2, "aer_nf"},
    Package() {\_SB.PRC1 , RESOURCE_IRQ, 3, "local"},
    Package() {\_SB.PRC1 , RESOURCE_IRQ, 4, "phy_int"},
    Package() {\_SB.PRC1 , RESOURCE_IRQ, 5, "phy_sta"},
  })
}

Device (PCP1) //PCIE PHY1
{
  Name (_HID, "CIXH2023")     // _HID: Hardware ID
  Name (_UID, 0x01)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute

  Method (_STA, 0x0, Serialized) {
    Return (0xF)
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0a080000, 0x40000)
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_PCIE_X4_PHY_APB, "pclk", \_SB.PCP1},
    Package() {CLK_TREE_PCIE_REF_PHY_X4, "refclk", \_SB.PCP1},
  })

  Device(PX4P)
  {
    Name (_ADR, 0x00)

    Name (NUML, 4)
    Method(_INI, 0) {
      Local0 = GETV(ARV_PCIE_RP_01_BANDWITCH_OFFSET)
      if (Local0 != 0) {
        Local1 = 0
        Switch(Local0) {
          case (0) { Local1=1 }
          case (1) { Local1=2 }
          case (2) { Local1=4 }
        }
        if (Local1 != 0) { NUML = Local1 }
      }
    }
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "reg", 0 },
            Package () { "num-lanes", NUML },
      },
    })
  }
}

Device (PRC2) { /* PCIE3 X2 */
  Name (_HID, "CIXH2020")
  Name (_UID, 0x2)
  Name (_STR, Unicode ("PCIe 2 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0x60)
  Name (_CCA, 1)

  Name (MLKS, 4)
  Name (NUML, 2)
  Name (MPAL, 512)
  Name (MAPM, 3)
  Name (ASPM, 0)

  Method(_INI, 0) {
    Local0 = GETV(ARV_PCIE_RP_02_BANDWITCH_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
        case (1) { Local1=2 }
      }
      if (Local1 != 0) { NUML = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_02_MAX_SPEED_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
        case (1) { Local1=2 }
        case (2) { Local1=3 }
        case (3) { Local1=4 }
      }
      if (Local1 != 0) { MLKS = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_02_MAX_PAYLOAD_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=128 }
        case (1) { Local1=256 }
        case (2) { Local1=512 }
      }
      if (Local1 != 0) { MPAL = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_02_MAX_ASPM_OFFSET)
    if (Local0 != 0) { MAPM = Local0 }
    Local0 = GETV(ARV_PCIE_RP_02_ASPM_OFFSET)
    if (Local0 != 0) { ASPM = Local0 }
  }

  Method (_STA, 0x0, Serialized) {
    Return (0xF)
  }
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0a0c0000, 0x10000)
    Memory32Fixed (ReadWrite, 0x0a060000, 0x10000)
    Memory32Fixed (ReadWrite, 0x26000000, 0x3000000)
    Memory32Fixed (ReadWrite, 0x40000000, 0x100000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X2_CORRECTABLE_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X2_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X2_NON_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X2_LOCAL_INTERRUPT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X2_PHY_INTERRUPT_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X2_1_1_PHY_STATE_CHG_STS_P_2_INTERRUPT_ID }

    WordBusNumber (
      ResourceProducer, // Specify bus ranged is passed to child devices
      MinFixed,         // Specify min address is fixed
      MaxFixed,         // Specify max address is fixed
      PosDecode,        // Positive decode of bus number
      0,                // AddressGranularity 2 power of 0
      0x60,               // AddressMinimum - Minimum Bus Number
      0x8F,              // AddressMaximum - Maximum Bus Number
      0,                // AddressTranslation - Set to 0
      48)               // RangeLength - Number of Bus

    // PCI IO space
    DWordIo ( // 32-bit BAR Windows
      ResourceProducer,
      MinFixed, MaxFixed,
      , ,
      0x00000000,               // Granularity
      0x40100000,               // Min Base Address
      0x401FFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x00100000                // Length 1M
    )

    // PCI memory space
    DWordMemory ( // 32-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x40200000,               // Min Base Address
      0x4FFFFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x0fe00000                // Length 254M
    )

    QWordMemory ( // 64-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x1000000000,             // Min Base Address
      0x13FFFFFFFF,             // Max Base Address
      0x00000000,               // Translate
      0x400000000               // Length 16G
    )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_pcie_x2_rc", ResourceConsumer,)
#if PCIE_X2_PERST
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
                PCIE_X2_PERST_GPIO_CTR, 0, ResourceConsumer) { PCIE_X2_PERST_GPIO }
#endif
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "device_type", "pci" },
          Package () { "vendor-id", 0x1f6c },
          Package () { "device-id", 0x0001 },
          Package () { "bus-range", Package () { 0x60, 0x8f } },
          Package () { "max-link-speed", MLKS },
          Package () { "num-lanes", NUML },
          Package () { "cdns,no-inbound-bar", 0 },
          Package () { "sky1,pcie-ctrl-id", 0x2 },
          Package () { "sky1,aer-uncor-panic", 0 },
          Package () { "cdns,pcie-phy", \_SB.PCP2.PX2P },
          Package () { "max-payload", MPAL },
          Package () { "max-aspm-support", MAPM },
          Package () { "aspm", ASPM },
#if PCIE_X2_PERST
          Package () { "reset-gpios", Package () { ^PRC2, 0, 0, PERST_GPIO_ACTIVE_LEVEL } },
#endif
#if PCIE_X2_VCC_SUPPLY
          Package () { "vcc-pcie-supply", PCIE_X2_VCC_REGULATOR },
#endif
#if PCIE_X2_STR_PWRON
          Package () { "sky1,str-pwron", 1 },
#endif
#if PCIE_X2_STD_PWRON
          Package () { "sky1,std-pwron", 1 },
#endif
        },
  })

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

  Name (CLKT, Package() {
    Package() {CLK_TREE_PCIE_CTRL2_CLK, "axi_clk", \_SB.PRC2},
    Package() {CLK_TREE_PCIE_X2CTRL_APB, "apb_clk", \_SB.PRC2},
    Package() {CLK_TREE_PCIE_REF_B2, "refclk_b", \_SB.PRC2},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_PCIE2_RESET_N, \_SB.PRC2, "pcie_reset"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.PCP2 , \_SB.PRC2, 0},
#if PCIE_X2_VCC_SUPPLY
    Package() {PCIE_X2_VCC_REGULATOR , \_SB.PRC2, 0},
#endif
  })
  Name (RSNL, Package() {
    Package() {\_SB.PRC2 , RESOURCE_MEM, 0, "reg"},
    Package() {\_SB.PRC2 , RESOURCE_MEM, 1, "rcsu"},
    Package() {\_SB.PRC2 , RESOURCE_MEM, 2, "cfg"},
    Package() {\_SB.PRC2 , RESOURCE_MEM, 3, "msg"},
    Package() {\_SB.PRC2 , RESOURCE_IRQ, 0, "aer_c"},
    Package() {\_SB.PRC2 , RESOURCE_IRQ, 1, "aer_f"},
    Package() {\_SB.PRC2 , RESOURCE_IRQ, 2, "aer_nf"},
    Package() {\_SB.PRC2 , RESOURCE_IRQ, 3, "local"},
    Package() {\_SB.PRC2 , RESOURCE_IRQ, 4, "phy_int"},
    Package() {\_SB.PRC2 , RESOURCE_IRQ, 5, "phy_sta"},
  })
}

Device (PRC3) { /* PCIE4 X1_1 */
  Name (_HID, "CIXH2020")
  Name (_UID, 0x3)
  Name (_STR, Unicode ("PCIe 3 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0x30)
  Name (_CCA, 1)

  Name (MLKS, 4)
  Name (NUML, 1)
  Name (MPAL, 512)
  Name (MAPM, 3)
  Name (ASPM, 0)

  Method(_INI, 0) {
    Local0 = GETV(ARV_PCIE_RP_03_BANDWITCH_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
      }
      if (Local1 != 0) { NUML = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_03_MAX_SPEED_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
        case (1) { Local1=2 }
        case (2) { Local1=3 }
        case (3) { Local1=4 }
      }
      if (Local1 != 0) { MLKS = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_03_MAX_PAYLOAD_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=128 }
        case (1) { Local1=256 }
        case (2) { Local1=512 }
      }
      if (Local1 != 0) { MPAL = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_03_MAX_ASPM_OFFSET)
    if (Local0 != 0) { MAPM = Local0 }
    Local0 = GETV(ARV_PCIE_RP_03_ASPM_OFFSET)
    if (Local0 != 0) { ASPM = Local0 }
  }

  Method (_STA, 0x0, Serialized) {
    Return (0xF)
  }
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0a0e0000, 0x10000)
    Memory32Fixed (ReadWrite, 0x0a060000, 0x10000)
    Memory32Fixed (ReadWrite, 0x23000000, 0x3000000)
    Memory32Fixed (ReadWrite, 0x38000000, 0x100000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_1_CORRECTABLE_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_1_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_1_NON_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_1_LOCAL_INTERRUPT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_1_PHY_INTERRUPT_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X2_1_1_PHY_STATE_CHG_STS_P_1_INTERRUPT_ID }

    WordBusNumber (
      ResourceProducer, // Specify bus ranged is passed to child devices
      MinFixed,         // Specify min address is fixed
      MaxFixed,         // Specify max address is fixed
      PosDecode,        // Positive decode of bus number
      0,                // AddressGranularity 2 power of 0
      0x30,               // AddressMinimum - Minimum Bus Number
      0x5F,              // AddressMaximum - Maximum Bus Number
      0,                // AddressTranslation - Set to 0
      48)               // RangeLength - Number of Bus

    // PCI IO space
    DWordIo ( // 32-bit BAR Windows
      ResourceProducer,
      MinFixed, MaxFixed,
      , ,
      0x00000000,               // Granularity
      0x38100000,               // Min Base Address
      0x381FFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x00100000                // Length 1M
    )

    // PCI memory space
    DWordMemory ( // 32-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x38200000,               // Min Base Address
      0x3FFFFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x07e00000                // Length 126M
    )

    QWordMemory ( // 64-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0xC00000000,             // Min Base Address
      0xFFFFFFFFF,             // Max Base Address
      0x00000000,               // Translate
      0x400000000               // Length 16G
    )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_pcie_x1_1_rc", ResourceConsumer,)
#if PCIE_X1_1_PERST
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
                PCIE_X1_1_PERST_GPIO_CTR, 0, ResourceConsumer) { PCIE_X1_1_PERST_GPIO }
#endif
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "device_type", "pci" },
          Package () { "vendor-id", 0x1f6c },
          Package () { "device-id", 0x0001 },
          Package () { "bus-range", Package () { 0x30, 0x5f } },
          Package () { "max-link-speed", MLKS },
          Package () { "num-lanes", NUML },
          Package () { "cdns,no-inbound-bar", 0 },
          Package () { "sky1,pcie-ctrl-id", 0x3 },
          Package () { "sky1,aer-uncor-panic", 0 },
          Package () { "cdns,pcie-phy", \_SB.PCP2.PX11 },
          Package () { "max-payload", MPAL },
          Package () { "max-aspm-support", MAPM },
          Package () { "aspm", ASPM },
#if PCIE_X1_1_PERST
          Package () { "reset-gpios", Package () { ^PRC3, 0, 0, PERST_GPIO_ACTIVE_LEVEL } },
#endif
#if PCIE_X1_1_VCC_SUPPLY
          Package () { "vcc-pcie-supply", PCIE_X1_1_VCC_REGULATOR },
#endif
#if PCIE_X1_1_STR_PWRON
          Package () { "sky1,str-pwron", 1 },
#endif
#if PCIE_X1_1_STD_PWRON
          Package () { "sky1,std-pwron", 1 },
#endif
        },
  })

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

  Name (CLKT, Package() {
    Package() {CLK_TREE_PCIE_CTRL4_CLK, "axi_clk", \_SB.PRC3},
    Package() {CLK_TREE_PCIE_X1_1CTRL_APB, "apb_clk", \_SB.PRC3},
    Package() {CLK_TREE_PCIE_REF_B4, "refclk_b", \_SB.PRC3},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_PCIE4_RESET_N, \_SB.PRC3, "pcie_reset"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.PCP2 , \_SB.PRC3, 0},
#if PCIE_X1_1_VCC_SUPPLY
    Package() {PCIE_X1_1_VCC_REGULATOR , \_SB.PRC3, 0},
#endif
  })
  Name (RSNL, Package() {
    Package() {\_SB.PRC3 , RESOURCE_MEM, 0, "reg"},
    Package() {\_SB.PRC3 , RESOURCE_MEM, 1, "rcsu"},
    Package() {\_SB.PRC3 , RESOURCE_MEM, 2, "cfg"},
    Package() {\_SB.PRC3 , RESOURCE_MEM, 3, "msg"},
    Package() {\_SB.PRC3 , RESOURCE_IRQ, 0, "aer_c"},
    Package() {\_SB.PRC3 , RESOURCE_IRQ, 1, "aer_f"},
    Package() {\_SB.PRC3 , RESOURCE_IRQ, 2, "aer_nf"},
    Package() {\_SB.PRC3 , RESOURCE_IRQ, 3, "local"},
    Package() {\_SB.PRC3 , RESOURCE_IRQ, 4, "phy_int"},
    Package() {\_SB.PRC3 , RESOURCE_IRQ, 5, "phy_sta"},
  })
}

Device (PRC4) { /* PCIE3 X1_0 */
  Name (_HID, "CIXH2020")
  Name (_UID, 0x4)
  Name (_STR, Unicode ("PCIe 4 Device"))
  Name (_SEG, 0)
  Name (_BBN, 0x00)
  Name (_CCA, 1)

  Name (MLKS, 4)
  Name (NUML, 1)
  Name (MPAL, 512)
  Name (MAPM, 3)
  Name (ASPM, 0)

  Method(_INI, 0) {
    Local0 = GETV(ARV_PCIE_RP_04_BANDWITCH_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
      }
      if (Local1 != 0) { NUML = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_04_MAX_SPEED_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=1 }
        case (1) { Local1=2 }
        case (2) { Local1=3 }
        case (3) { Local1=4 }
      }
      if (Local1 != 0) { MLKS = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_04_MAX_PAYLOAD_OFFSET)
    if (Local0 != 0) {
      Local1 = 0
      Switch(Local0) {
        case (0) { Local1=128 }
        case (1) { Local1=256 }
        case (2) { Local1=512 }
      }
      if (Local1 != 0) { MPAL = Local1 }
    }
    Local0 = GETV(ARV_PCIE_RP_04_MAX_ASPM_OFFSET)
    if (Local0 != 0) { MAPM = Local0 }
    Local0 = GETV(ARV_PCIE_RP_04_ASPM_OFFSET)
    if (Local0 != 0) { ASPM = Local0 }
  }

  Method (_STA, 0x0, Serialized) {
    Return (0xF)
  }
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0a0d0000, 0x10000)
    Memory32Fixed (ReadWrite, 0x0a060000, 0x10000)
    Memory32Fixed (ReadWrite, 0x20000000, 0x3000000)
    Memory32Fixed (ReadWrite, 0x30000000, 0x100000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_0_CORRECTABLE_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_0_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_0_NON_FATAL_ERROR_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_0_LOCAL_INTERRUPT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X1_0_PHY_INTERRUPT_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { PCIE_SUBSYS_X2_1_1_PHY_STATE_CHG_STS_P_0_INTERRUPT_ID }

    WordBusNumber (
      ResourceProducer, // Specify bus ranged is passed to child devices
      MinFixed,         // Specify min address is fixed
      MaxFixed,         // Specify max address is fixed
      PosDecode,        // Positive decode of bus number
      0,                // AddressGranularity 2 power of 0
      0x00,               // AddressMinimum - Minimum Bus Number
      0x2F,              // AddressMaximum - Maximum Bus Number
      0,                // AddressTranslation - Set to 0
      48)               // RangeLength - Number of Bus

    // PCI IO space
    DWordIo ( // 32-bit BAR Windows
      ResourceProducer,
      MinFixed, MaxFixed,
      , ,
      0x00000000,               // Granularity
      0x30100000,               // Min Base Address
      0x301FFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x00100000                // Length 1M
    )

    // PCI memory space
    DWordMemory ( // 32-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x30200000,               // Min Base Address
      0x37FFFFFF,               // Max Base Address
      0x00000000,               // Translate
      0x07e00000                // Length 126M
    )

    QWordMemory ( // 64-bit BAR Windows
      ResourceProducer, PosDecode,
      MinFixed, MaxFixed,
      Cacheable, ReadWrite,
      0x00000000,               // Granularity
      0x800000000,             // Min Base Address
      0xBFFFFFFFF,             // Max Base Address
      0x00000000,               // Translate
      0x400000000               // Length 16G
    )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_pcie_x1_0_rc", ResourceConsumer,)
#if PCIE_X1_0_PERST
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
                PCIE_X1_0_PERST_GPIO_CTR, 0, ResourceConsumer) { PCIE_X1_0_PERST_GPIO }
#endif
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "device_type", "pci" },
          Package () { "vendor-id", 0x1f6c },
          Package () { "device-id", 0x0001 },
          Package () { "bus-range", Package () { 0x00, 0x2f } },
          Package () { "max-link-speed", MLKS },
          Package () { "num-lanes", NUML },
          Package () { "cdns,no-inbound-bar", 0 },
          Package () { "sky1,pcie-ctrl-id", 0x4 },
          Package () { "sky1,aer-uncor-panic", 0 },
          Package () { "cdns,pcie-phy", \_SB.PCP2.PX10 },
          Package () { "max-payload", MPAL },
          Package () { "max-aspm-support", MAPM },
          Package () { "aspm", ASPM },
#if PCIE_X1_0_PERST
          Package () { "reset-gpios", Package () { ^PRC4, 0, 0, PERST_GPIO_ACTIVE_LEVEL } },
#endif
#if PCIE_X1_0_VCC_SUPPLY
          Package () { "vcc-pcie-supply", PCIE_X1_0_VCC_REGULATOR },
#endif
#if PCIE_X1_0_STR_PWRON
          Package () { "sky1,str-pwron", 1 },
#endif
#if PCIE_X1_0_STD_PWRON
          Package () { "sky1,std-pwron", 1 },
#endif
        },
  })

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

  Name (CLKT, Package() {
    Package() {CLK_TREE_PCIE_CTRL3_CLK, "axi_clk", \_SB.PRC4},
    Package() {CLK_TREE_PCIE_X1_0CTRL_APB, "apb_clk", \_SB.PRC4},
    Package() {CLK_TREE_PCIE_REF_B3, "refclk_b", \_SB.PRC4},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_PCIE3_RESET_N, \_SB.PRC4, "pcie_reset"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.PCP2 , \_SB.PRC4, 0},
#if PCIE_X1_0_VCC_SUPPLY
    Package() {PCIE_X1_0_VCC_REGULATOR , \_SB.PRC4, 0},
#endif
  })
  Name (RSNL, Package() {
    Package() {\_SB.PRC4 , RESOURCE_MEM, 0, "reg"},
    Package() {\_SB.PRC4 , RESOURCE_MEM, 1, "rcsu"},
    Package() {\_SB.PRC4 , RESOURCE_MEM, 2, "cfg"},
    Package() {\_SB.PRC4 , RESOURCE_MEM, 3, "msg"},
    Package() {\_SB.PRC4 , RESOURCE_IRQ, 0, "aer_c"},
    Package() {\_SB.PRC4 , RESOURCE_IRQ, 1, "aer_f"},
    Package() {\_SB.PRC4 , RESOURCE_IRQ, 2, "aer_nf"},
    Package() {\_SB.PRC4 , RESOURCE_IRQ, 3, "local"},
    Package() {\_SB.PRC4 , RESOURCE_IRQ, 4, "phy_int"},
    Package() {\_SB.PRC4 , RESOURCE_IRQ, 5, "phy_sta"},
  })
}

Device (PCP2) //PCIE PHY1
{
  Name (_HID, "CIXH2023")     // _HID: Hardware ID
  Name (_UID, 0x02)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute

  Method (_STA, 0x0, Serialized) {
    Return (0xF)
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0a0f0000, 0x40000)
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_PCIE_X211_PHY_APB, "pclk", \_SB.PCP2},
    Package() {CLK_TREE_PCIE_REF_PHY_X211, "refclk", \_SB.PCP2},
  })

  Device(PX10)
  {
    Method (_STA, 0x0, Serialized) {
      Return (0xF)
    }
    Name (_ADR, 0x00)
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "reg", 0 },
            Package () { "num-lanes", 1 },
      },
    })
  }
  Device(PX11)
  {
    Method (_STA, 0x0, Serialized) {
      Return (0xF)
    }
    Name (_ADR, 0x01)
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "reg", 1 },
            Package () { "num-lanes", 1 },
      },
    })
  }
  Device(PX2P)
  {
    Method (_STA, 0x0, Serialized) {
      Return (0xF)
    }
    Name (_ADR, 0x02)
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "reg", 2 },
            Package () { "num-lanes", 2 },
      },
    })
  }
}
