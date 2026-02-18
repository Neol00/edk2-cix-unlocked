/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (TEE0) {
  Name (_HID, "CIXHA022")
  Name (_UID, 0x0)
  Name (_CID, "PRP0001")
  Name (_STA, 0xB)

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "linaro,optee-tz" },
      Package () { "method", "smc" },
    }
  })
}

