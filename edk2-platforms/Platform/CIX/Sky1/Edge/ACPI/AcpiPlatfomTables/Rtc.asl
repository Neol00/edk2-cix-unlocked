/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope (\_SB.I2C3)
{
  // RTC RX8900
  Device (RTC0) {
    Name (_HID, "RX8900")
    Name (_UID, 0x0)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x32,
                      ControllerInitiated,
                      400000,
                      AddressingMode7Bit,
                      "\\_SB.I2C3",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0,
                      "pinctrl_ra8900ce_irq", ResourceConsumer,)
      GpioInt(Level, ActiveLow, Exclusive, PullUp, , "\\_SB.GPI4") { 10 }
    })
  }
}
