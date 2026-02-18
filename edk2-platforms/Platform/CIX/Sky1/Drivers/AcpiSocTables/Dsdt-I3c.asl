/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (I3C0) {
  Name (_HID, "CIXH200C")
  Name (_UID, 0x0)
  Name (_STA, 0x0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I3C0_BASE, I3C0_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I3C0_INTERRUPT_ID }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_i3c0", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () { "i3c-scl-hz", 100000 },
        Package () { "i2c-scl-hz", 100000 },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I3C0_APB, "pclk", \_SB.I3C0},
    Package() {CLK_TREE_FCH_I3C0_FUNC, "sysclk", \_SB.I3C0},
  })
}

Device (I3C1) {
  Name (_HID, "CIXH200C")
  Name (_UID, 0x1)
  Name (_STA, 0x0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I3C1_BASE, I3C1_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I3C1_INTERRUPT_ID }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_i3c1", ResourceConsumer,)
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I3C1_APB, "pclk", \_SB.I3C1},
    Package() {CLK_TREE_FCH_I3C1_FUNC, "sysclk", \_SB.I3C1},
  })
}
