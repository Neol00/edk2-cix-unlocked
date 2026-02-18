/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (RFKL) {
  Name (_HID, "CIXH7000")
  Name (_UID, 0x0)
  Name (_STA, 0x0)
  Name (_CRS, ResourceTemplate () {
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
                "\\_SB.GPI4", 0, ResourceConsumer) { 10, 12 }
    GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullNone, 0,
                "\\_SB.GPI4", 0, ResourceConsumer) { 25 }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "wifi_vbat_gpio", ResourceConsumer,)
  })
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "poweren-gpios", Package () { ^RFKL, 0, 0, 0 } },
      Package () { "vbat-gpios", Package () { ^RFKL, 0, 1, 0 } },
      Package () { "wakehost-gpios", Package () { ^RFKL, 1, 0, 0 } },
    }
  })
}
