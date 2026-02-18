/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (RST0) {
  Name (_HID, "CIXHA020")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x16000000, 0x1000)
  })
}

Device (RST1) {
  Name (_HID, "CIXHA021")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x04160000, 0x1000)
  })
}

DEVICE(CRU0) {
  Name (_HID, "CIXHA018")
  Name (_UID, 0x4)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x16000000, 0x1000)
  })
}

