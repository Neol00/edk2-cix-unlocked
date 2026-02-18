/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (PDC0) {
  Name (_HID, "CIXHA019")
  Name (_UID, 0x0)
  Name (_STA, 0xB)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, PDC_CONTROLLER_BASE, PDC_CONTROLLER_SIZE)
  })
}
