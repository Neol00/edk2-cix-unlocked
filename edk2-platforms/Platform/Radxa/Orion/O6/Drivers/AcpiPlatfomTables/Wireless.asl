/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Gpio.h"

Device (VWL0) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x23)
  Name (_STA, 0x0B)

  Name (_CRS, ResourceTemplate () {
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "wl_radio_disable_l", ResourceConsumer,)
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI5", 0, ResourceConsumer) { 4 }  // GPIO019
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "regulator-fixed" },
      Package () { "regulator-name", "wl_radio_disable_l" },
      Package () { "regulator-min-microvolt", 1800000 },
      Package () { "regulator-max-microvolt", 1800000 },
      Package () { "gpio", Package () { ^VWL0, 0, 0, GPIO_ACTIVE_HIGH } },
      Package () { "enable-active-high", 1 },
      Package () { "regulator-always-on", 1 },
      Package () { "off-on-delay-us", 15000 },
    },
  })
}

Device (VBT0) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x24)
  Name (_STA, 0x0B)

  Name (_CRS, ResourceTemplate () {
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "bt_radio_disable_l", ResourceConsumer,)
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI5", 0, ResourceConsumer) { 5 }  // GPIO020
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "regulator-fixed" },
      Package () { "regulator-name", "bt_radio_disable_l" },
      Package () { "regulator-min-microvolt", 1800000 },
      Package () { "regulator-max-microvolt", 1800000 },
      Package () { "gpio", Package () { ^VBT0, 0, 0, GPIO_ACTIVE_HIGH } },
      Package () { "enable-active-high", 1 },
      Package () { "regulator-always-on", 1 },
      Package () { "off-on-delay-us", 15000 },
    },
  })
}
