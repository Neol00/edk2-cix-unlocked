/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device(GCRU) {
  Name (_HID, "CIXHA018")
  Name (_UID, 0x3)
  Name (_STA, 0xf)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09310000, 0x1000)
  })
}

/* Gmac0 */
Device (MAC0) {
  Name (_HID, "CIXH7020")
  Name (_UID, 0x0)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_GMAC0_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (_CCA, ONE)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x09320000,
      0x10000
      )
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, , ,) {
       GMAC_GMAC0_INTR_Q0_INTERRUPT_ID,
       GMAC_GMAC0_INTR_Q1_INTERRUPT_ID,
       GMAC_GMAC0_INTR_Q2_INTERRUPT_ID,
       GMAC_GMAC0_INTR_Q3_INTERRUPT_ID,
       GMAC_GMAC0_INTR_Q4_INTERRUPT_ID,
       GMAC_GMAC0_INTR_Q5_INTERRUPT_ID,
       GMAC_GMAC0_INTR_Q6_INTERRUPT_ID,
       GMAC_GMAC0_INTR_Q7_INTERRUPT_ID
    }
    // Pinctrl
    PinGroupFunction(
      Exclusive,
      0x0,
      "\\_SB.MUX0",
      0,
      "gmac0",
      ResourceConsumer, ,
      RawDataBuffer () { 0x0 }
      )
    PinGroupFunction(
      Exclusive, 0x0,
      "\\_SB.MUX0",
      0,
      "gmac0-init",
      ResourceConsumer, ,
      RawDataBuffer () { 0x1 }
      )
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
                "\\_SB.GPI0", 0, ResourceConsumer) { 0 }
  })
  Device (PHY0) {
    Name (_ADR, One)
    Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
            Package () { "compatible", "ethernet-phy-ieee802.3-c22" },
        }
    })
  }
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "port-id", 0 },
          //Package () { "gop-port-id", 0 },
          Package () { "phy-mode", "rgmii-id" },
          Package () { "phy-handle", PHY0},
          Package () { "cix,gmac-ctrl", GCRU },
          Package () { "reset-gpio", Package () { ^MAC0, 0, 0, 1 } },
          Package () { "reset-delay-us", 20000 },
          Package () { "reset-post-delay-us", 100000 },
          Package () { "pinctrl-names", Package () {"default", "init"}},
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_GMAC0_ACLK, "aclk", \_SB.MAC0},
    Package() {CLK_TREE_GMAC0_PCLK, "pclk", \_SB.MAC0},
    Package() {CLK_TREE_GMAC0_DIV_TXCLK, "tx_clk", \_SB.MAC0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_GMAC0_RST_N, \_SB.MAC0, "gmac_rstn"},
  })
}

/* Gmac1 */
Device (MAC1) {
  Name (_HID, "CIXH7020")
  Name (_UID, 0x1)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_GMAC1_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (_CCA, ONE)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x09330000,
      0x10000
      )
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, , ,) {
       GMAC_GMAC1_INTR_Q0_INTERRUPT_ID,
       GMAC_GMAC1_INTR_Q1_INTERRUPT_ID,
       GMAC_GMAC1_INTR_Q2_INTERRUPT_ID,
       GMAC_GMAC1_INTR_Q3_INTERRUPT_ID,
       GMAC_GMAC1_INTR_Q4_INTERRUPT_ID,
       GMAC_GMAC1_INTR_Q5_INTERRUPT_ID,
       GMAC_GMAC1_INTR_Q6_INTERRUPT_ID,
       GMAC_GMAC1_INTR_Q7_INTERRUPT_ID
    }
  })
  Device (PHY1) {
    Name (_ADR, 2)
    Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
            Package () { "compatible", "ethernet-phy-ieee802.3-c22" },
        }
    })
  }
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "port-id", 1 },
          //Package () { "gop-port-id", 0 },
          Package () { "phy-mode", "rgmii-id" },
          Package () { "phy-handle", PHY1},
          Package () { "cix,gmac-ctrl", GCRU },
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_GMAC1_ACLK, "aclk", \_SB.MAC1},
    Package() {CLK_TREE_GMAC1_PCLK, "pclk", \_SB.MAC1},
    Package() {CLK_TREE_GMAC1_DIV_TXCLK, "tx_clk", \_SB.MAC1},
  })
}
