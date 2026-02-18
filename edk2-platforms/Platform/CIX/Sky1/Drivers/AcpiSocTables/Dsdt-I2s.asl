/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (I2S0) {
  Name (_HID, "CIXH6010")
  Name (_UID, 0x0)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s0Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S0_BASE, AUDIO_I2S0_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_SC0_INTERRUPT_ID }
    FixedDMA (32, 255, Width32Bit, ) // 0 + CSRT_AUD_REQUEST_BASE(32), idx 0 as tx
    FixedDMA (33, 255, Width32Bit, ) // 1 + CSRT_AUD_REQUEST_BASE(32), idx 1 as rx
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s0", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 0 },
      Package () { "dma-names", Package () { "tx", "rx" } },
      Package () { "cdns,mclk-idx", 0 },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S0},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S0_SW_RST_N ,\_SB.I2S0, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S0, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S0, 0},
    Package() {\_SB.DMA1, \_SB.I2S0, 0},
  })
}

Device (I2S1) {
  Name (_HID, "CIXH6010")
  Name (_UID, 0x1)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s1Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S1_BASE, AUDIO_I2S1_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_SC1_INTERRUPT_ID }
    FixedDMA (34, 255, Width32Bit, ) // 2 + CSRT_AUD_REQUEST_BASE(32), idx 0 as tx
    FixedDMA (35, 255, Width32Bit, ) // 3 + CSRT_AUD_REQUEST_BASE(32), idx 1 as rx
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s1", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 1 },
      Package () { "dma-names", Package () { "tx", "rx" } },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S1},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S1_SW_RST_N ,\_SB.I2S1, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S1, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S1, 0},
    Package() {\_SB.DMA1, \_SB.I2S1, 0},
  })
}

Device (I2S2) {
  Name (_HID, "CIXH6010")
  Name (_UID, 0x2)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s2Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S2_BASE, AUDIO_I2S2_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_SC2_INTERRUPT_ID }
    FixedDMA (36, 255, Width32Bit, ) // 5 + CSRT_AUD_REQUEST_BASE(32), idx 0 as tx
    FixedDMA (37, 255, Width32Bit, ) // 5 + CSRT_AUD_REQUEST_BASE(32), idx 1 as rx
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 2 },
      Package () { "dma-names", Package () { "rx" } },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S2},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S2},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S2_SW_RST_N ,\_SB.I2S2, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S2, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S2, 0},
    Package() {\_SB.DMA1, \_SB.I2S2, 0},
  })
}

Device (I2S3) {
  Name (_HID, "CIXH6011")
  Name (_UID, 0x3)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s3Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S3_BASE, AUDIO_I2S3_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_MC2A_INTERRUPT_ID }
    FixedDMA (38, 255, Width32Bit, )  // 6 + CSRT_AUD_REQUEST_BASE(32)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s2", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 3 },
      Package () { "dma-names", Package () { "tx" } },
      Package () { "cdns,pin-out-num", 6 },
      Package () { "cdns,pin-rx-mask", 0 },
      Package () { "cdns,pin-tx-mask", 60 },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S3},
    Package() {CLK_TREE_AUDIO_CLK1, "audio_clk1", \_SB.I2S3},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S3},
    Package() {CLK_TREE_AUDIO_CLK3, "audio_clk3", \_SB.I2S3},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S3_SW_RST_N ,\_SB.I2S3, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S3, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S3, 0},
    Package() {\_SB.DMA1, \_SB.I2S3, 0},
  })
}

Device (I2S4) {
  Name (_HID, "CIXH6011")
  Name (_UID, 0x4)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s4Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S4_BASE, AUDIO_I2S4_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_MC2B_INTERRUPT_ID }
    FixedDMA (41, 255, Width32Bit, )  // 9 + CSRT_AUD_REQUEST_BASE(32)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s3", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 4 },
      Package () { "dma-names", Package () { "rx" } },
      Package () { "cdns,pin-out-num", 4 },
      Package () { "cdns,pin-rx-mask", 15 },
      Package () { "cdns,pin-tx-mask", 0 },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S4},
    Package() {CLK_TREE_AUDIO_CLK1, "audio_clk1", \_SB.I2S4},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S4},
    Package() {CLK_TREE_AUDIO_CLK3, "audio_clk3", \_SB.I2S4},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S4_SW_RST_N ,\_SB.I2S4, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S4, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S4, 0},
    Package() {\_SB.DMA1, \_SB.I2S4, 0},
  })
}

Device (I2S5) {
  Name (_HID, "CIXH6011")
  Name (_UID, 0x5)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s5Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S5_BASE, AUDIO_I2S5_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_MC5A_INTERRUPT_ID }
    FixedDMA (42, 255, Width32Bit, )  // 10 + CSRT_AUD_REQUEST_BASE(32)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s5_dbg", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 5 },
      Package () { "dma-names", Package () { "tx" } },
      Package () { "cdns,pin-out-num", 4 },
      Package () { "cdns,pin-rx-mask", 0 },
      Package () { "cdns,pin-tx-mask", 15 },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
      Package () { "dp_pair_id", 0 },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S5},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S5},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S5_SW_RST_N ,\_SB.I2S5, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S5, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S5, 0},
    Package() {\_SB.DMA1, \_SB.I2S5, 0},
  })
}

Device (I2S6) {
  Name (_HID, "CIXH6011")
  Name (_UID, 0x6)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s6Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S6_BASE, AUDIO_I2S6_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_MC5B_INTERRUPT_ID }
    FixedDMA (44, 255, Width32Bit, )  // 12 + CSRT_AUD_REQUEST_BASE(32)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s6_dbg", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 6 },
      Package () { "dma-names", Package () { "tx" } },
      Package () { "cdns,pin-out-num", 4 },
      Package () { "cdns,pin-rx-mask", 0 },
      Package () { "cdns,pin-tx-mask", 15 },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
      Package () { "dp_pair_id", 1 },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S6},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S6},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S6_SW_RST_N ,\_SB.I2S6, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S6, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S6, 0},
    Package() {\_SB.DMA1, \_SB.I2S6, 0},
  })
}

Device (I2S7) {
  Name (_HID, "CIXH6011")
  Name (_UID, 0x7)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s7Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S7_BASE, AUDIO_I2S7_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_MC5C_INTERRUPT_ID }
    FixedDMA (46, 255, Width32Bit, )  // 14 + CSRT_AUD_REQUEST_BASE(32)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s7_dbg", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 7 },
      Package () { "dma-names", Package () { "tx" } },
      Package () { "cdns,pin-out-num", 4 },
      Package () { "cdns,pin-rx-mask", 0 },
      Package () { "cdns,pin-tx-mask", 15 },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
      Package () { "dp_pair_id", 2 },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S7},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S7},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S7_SW_RST_N ,\_SB.I2S7, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S7, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S7, 0},
    Package() {\_SB.DMA1, \_SB.I2S7, 0},
  })
}

Device (I2S8) {
  Name (_HID, "CIXH6011")
  Name (_UID, 0x8)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s8Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S8_BASE, AUDIO_I2S8_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_MC5D_INTERRUPT_ID }
    FixedDMA (48, 255, Width32Bit, )  // 16 + CSRT_AUD_REQUEST_BASE(32)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s8_dbg", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 8 },
      Package () { "dma-names", Package () { "tx" } },
      Package () { "cdns,pin-out-num", 4 },
      Package () { "cdns,pin-rx-mask", 0 },
      Package () { "cdns,pin-tx-mask", 15 },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
      Package () { "dp_pair_id", 3 },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S8},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S8},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S8_SW_RST_N ,\_SB.I2S8, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S8, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S8, 0},
    Package() {\_SB.DMA1, \_SB.I2S8, 0},
  })
}

Device (I2S9) {
  Name (_HID, "CIXH6011")
  Name (_UID, 0x9)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiI2s9Enable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_I2S9_BASE, AUDIO_I2S9_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_I2S_MC5E_INTERRUPT_ID }
    FixedDMA (50, 255, Width32Bit, )  // 18 + CSRT_AUD_REQUEST_BASE(32)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_substrate_i2s9_dbg", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "id", 9 },
      Package () { "dma-names", Package () { "tx" } },
      Package () { "cdns,pin-out-num", 4 },
      Package () { "cdns,pin-rx-mask", 0 },
      Package () { "cdns,pin-tx-mask", 15 },
      Package () { "cdns,cru-ctrl", \_SB.ACRU },
      Package () { "dp_pair_id", 4 },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.I2S9},
    Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.I2S9},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_I2S9_SW_RST_N ,\_SB.I2S9, "i2s"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.I2S9, 0},
    Package() {\_SB.ADSS.ARST, \_SB.I2S9, 0},
    Package() {\_SB.DMA1, \_SB.I2S9, 0},
  })
}
