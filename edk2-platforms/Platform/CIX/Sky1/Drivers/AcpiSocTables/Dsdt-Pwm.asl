/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (PWM0) {
  Name (_HID, "CIXH2011")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x04110000,
      0x1000
      )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_pwm0", ResourceConsumer,)
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_TIMER_APB, "fch_pwm_apb_clk", \_SB.PWM0},
    Package() {CLK_TREE_FCH_TIMER_FUN, "fch_pwm_func_clk", \_SB.PWM0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_TIMER_RST_FUNC_N, \_SB.PWM0, "func_reset"},
  })
}

Device (PWM1) {
  Name (_HID, "CIXH2011")
  Name (_UID, 0x1)
  Name (_STA, 0x0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x04111000,
      0x1000
      )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_pwm1", ResourceConsumer,)
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_TIMER_APB, "fch_pwm_apb_clk", \_SB.PWM1},
    Package() {CLK_TREE_FCH_TIMER_FUN, "fch_pwm_func_clk", \_SB.PWM1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_TIMER_RST_FUNC_N, \_SB.PWM1, "func_reset"},
  })
}
