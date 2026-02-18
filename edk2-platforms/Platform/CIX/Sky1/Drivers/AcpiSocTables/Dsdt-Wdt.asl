/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (WDT0) {
  Name (_HID, "CIXH1005")
  Name (_CID, "CIXH1005")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x16003000,
      0x1000
      )
    Memory32Fixed (
      ReadWrite,
      0x16008000,
      0x1000
      )
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 408 }
  })
}
