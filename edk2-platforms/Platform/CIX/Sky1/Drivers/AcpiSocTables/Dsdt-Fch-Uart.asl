/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

OperationRegion(DBGR,SystemMemory,0x05040100,0x20)
Field (DBGR, DWordAcc, NoLock, Preserve) {
  Offset (0x0),
  UCLK, 32,
}

Device(UCRU) {
  Name (_HID, "CIXHA018")
  Name (_UID, 0x2)
  Name (_STA, 0xf)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0416009c, 0x80)
  })
}

Device (COM0) {
  Name (_HID, "ARMH0011")
  Name (_UID, 0x1)

  Method (_STA)
  {
    If(FixedPcdGetBool(PcdAcpiUart0Enable)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, UART0_BASE, UART0_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_UART0_INTERRUPT_ID }
    FixedDMA (0x0, 0x2, Width32Bit, )
    FixedDMA (0x1, 0x3, Width32Bit, )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_uart0", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "uartclk", UCLK },
          Package () { "timeout-value", 10000 },
          Package () { "dma-names", Package () { "tx", "rx"} },
          Package () { "sky1,fch_cru", UCRU },
        }
  })
  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_UART0_APB, "apb_pclk", \_SB.COM0},
    Package() {CLK_TREE_FCH_UART0_FUNC, "uartclk", \_SB.COM0},
  })
}

Device (COM1) {
  Name (_HID, "ARMH0011")
  Name (_UID, 0x2)

  Method (_STA)
  {
    If(FixedPcdGetBool(PcdAcpiUart1Enable)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, UART1_BASE, UART1_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_UART1_INTERRUPT_ID }
    FixedDMA (0x2, 0x4, Width32Bit, )
    FixedDMA (0x3, 0x5, Width32Bit, )
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_uart1", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "uartclk", UCLK },
          Package () { "timeout-value", 10000 },
          Package () { "dma-names", Package () { "tx", "rx"} },
          Package () { "sky1,fch_cru", UCRU },
        }
  })
  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_UART1_APB, "apb_pclk", \_SB.COM1},
    Package() {CLK_TREE_FCH_UART1_FUNC, "uartclk", \_SB.COM1},
  })
}

Device (COM2) {
  Name (_HID, "ARMH0011")
  Name (_UID, 0x3)

  Method (_STA)
  {
    If(FixedPcdGetBool(PcdAcpiUart2Enable)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, UART2_BASE, UART2_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_UART2_INTERRUPT_ID }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_uart2", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "uartclk", UCLK },
        }
  })
  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_UART2_APB, "apb_pclk", \_SB.COM2},
    Package() {CLK_TREE_FCH_UART2_FUNC, "uartclk", \_SB.COM2},
  })
}

Device (COM3) {
  Name (_HID, "ARMH0011")
  Name (_UID, 0x4)

  Method (_STA)
  {
    If(FixedPcdGetBool(PcdAcpiUart3Enable)){
      Return (0xF)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, UART3_BASE, UART3_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_UART3_INTERRUPT_ID }
  })
  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_UART3_APB, "apb_pclk", \_SB.COM3},
    Package() {CLK_TREE_FCH_UART3_FUNC, "uartclk", \_SB.COM3},
  })
}
