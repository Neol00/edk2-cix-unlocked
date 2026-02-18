/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (XSPI) {
  Name (_HID, "CIXH2002")
  Name (_UID, 0x0)
  Name (_STA, 0x0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, XSPI_BASE, XSPI_SIZE)
    Memory32Fixed (ReadWrite, XSPI_MMIO_BASE, XSPI_MMIO_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_XSPI_INTERRUPT_ID }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_fch_xspi", ResourceConsumer,)
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_XSPI_APB, "pclk", \_SB.XSPI},
    Package() {CLK_TREE_FCH_XSPI_MACLK, "maclk", \_SB.XSPI},
    Package() {CLK_TREE_FCH_XSPI_FUNC, "funcclk", \_SB.XSPI},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_XSPI_REG_RST_N, \_SB.XSPI, "xspi_reg_reset"},
    Package() {\_SB.RST1, SW_XSPI_SYS_RST_N, \_SB.XSPI, "xspi_sys_reset"},
  })
}
