/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope (\_SB.I2C2)
{
  /* mmc5633 */
  Device (MMC0) {
    Name (_HID, "CIXHA011")
    Name (_UID, 0x0)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x30,
                      ControllerInitiated,
                      400000,
                      AddressingMode7Bit,
                      "\\_SB.I2C2",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
    })
  }
}

Scope (\_SB.I3C0)
{
  /* mmc5633 */
  Device (MMC1) {
    Name (_ADR, 0x0)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x30,
                      ControllerInitiated,
                      400000,
                      AddressingMode7Bit,
                      "\\_SB.I3C0",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
    })
  }
}