/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (TMR0) {
  Name (_HID, "CIXH1007")
  Name (_CID, "CIXH1007")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x04116000,
      0x2000
      )
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 502 }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_TIMER_APB, "fch_timer_apb_clk", \_SB.TMR0},
    Package() {CLK_TREE_FCH_TIMER_FUN, "fch_timer_func_clk", \_SB.TMR0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_TIMER_RST_FUNC_N, \_SB.TMR0, "func_reset"},
  })
}
