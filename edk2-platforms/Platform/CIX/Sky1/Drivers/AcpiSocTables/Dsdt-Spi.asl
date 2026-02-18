/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (SPI0) {
  Name (_HID, "CIXH2001")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, SPI0_BASE, SPI0_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_SPI0_INTERRUPT_ID }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_fch_spi0", ResourceConsumer,)
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () {"fifo-width", 32},
      //Package () {"num-cs", 1},
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_SPI0_APB, "pclk", \_SB.SPI0},
    Package() {CLK_TREE_FCH_SPI0_APB, "ref_clk", \_SB.SPI0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_SPI0_RST_APB_N, \_SB.SPI0, "spi_reset"},
  })
}

Device (SPI1) {
  Name (_HID, "CIXH2001")
  Name (_UID, 0x1)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, SPI1_BASE, SPI1_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_SPI1_INTERRUPT_ID }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_spi1", ResourceConsumer,)
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () {"fifo-width", 32},
      //Package () {"num-cs", 1},
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_SPI1_APB, "pclk", \_SB.SPI1},
    Package() {CLK_TREE_FCH_SPI1_APB, "ref_clk", \_SB.SPI1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_SPI1_RST_APB_N, \_SB.SPI1, "spi_reset"},
  })
}
