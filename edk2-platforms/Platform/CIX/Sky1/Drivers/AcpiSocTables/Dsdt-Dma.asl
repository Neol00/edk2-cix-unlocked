/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (DMA0) {
  Name (_HID, "CIXHA014")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0x0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, DMA_BASE, DMA_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_DMA_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "dma-channels", 8 },
      Package () { "dma-requests", 8 },
    }
  })

  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_DMA_RST_AXI_N, \_SB.DMA0, "dma_reset"},
  })
}

Device (DMA1) {
  Name (_HID, "CIXH1006")
  Name (_UID, 0x0)

  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CCA, 0x0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, AUDIO_DMAC_BASE, AUDIO_DMAC_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_AP_DMA_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "dma-channels", 8 },
      Package () { "dma-requests", 20 },
      Package () { "arm,clk-enable-atomic", 0 },
      Package () { "arm,reg-map", Package () { 0x07010000, 0x20000000 } },
      Package () { "arm,ram-map", Package () { 0xc0000000, 0x30000000 } },
      Package () { "arm,remote-ctrl", \_SB.ACRU },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_DMA_ACLK, "", \_SB.DMA0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_DMAC_SW_RST_N ,\_SB.DMA1, "dma_reset"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.DMA1, 0},
    Package() {\_SB.ADSS.ARST, \_SB.DMA1, 0},
  })
}
