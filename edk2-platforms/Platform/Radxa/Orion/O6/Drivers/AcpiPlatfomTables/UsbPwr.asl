/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Gpio.h"

Device (VUS0) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x25)
  Name (_STA, 0x0B)

  Name (_CRS, ResourceTemplate () {
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "usb_drive_vbus0", ResourceConsumer,)
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI4", 0, ResourceConsumer) { 29 } // GPIO040
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "regulator-fixed" },
      Package () { "regulator-name", "vdd_usb_drive_vbus0" },
      Package () { "regulator-min-microvolt", 5000000 },
      Package () { "regulator-max-microvolt", 5000000 },
      Package () { "gpio", Package () { ^VUS0, 0, 0, GPIO_ACTIVE_HIGH } },
      Package () { "regulator-always-on", 1 },
      Package () { "regulator-pull-down", 1 },
      Package () { "enable-active-high", 1 },
      Package () { "off-on-delay-us", 15000 },
    }
  })
}

Device (VUS4) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x26)
  Name (_STA, 0x0B)

  Name (_CRS, ResourceTemplate () {
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "usb_drive_vbus4", ResourceConsumer,)
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI4", 0, ResourceConsumer) { 30 } // GPIO041
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "regulator-fixed" },
      Package () { "regulator-name", "vdd_usb_drive_vbus4" },
      Package () { "regulator-min-microvolt", 5000000 },
      Package () { "regulator-max-microvolt", 5000000 },
      Package () { "gpio", Package () { ^VUS4, 0, 0, GPIO_ACTIVE_HIGH } },
      Package () { "regulator-always-on", 1 },
      Package () { "regulator-pull-down", 1 },
      Package () { "enable-active-high", 1 },
      Package () { "off-on-delay-us", 15000 },
    }
  })
}

Device (VUS5) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x27)
  Name (_STA, 0x0B)

  Name (_CRS, ResourceTemplate () {
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "usb_drive_vbus5", ResourceConsumer,)
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI4", 0, ResourceConsumer) { 31 } // GPIO042
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "regulator-fixed" },
      Package () { "regulator-name", "vdd_usb_drive_vbus5" },
      Package () { "regulator-min-microvolt", 5000000 },
      Package () { "regulator-max-microvolt", 5000000 },
      Package () { "gpio", Package () { ^VUS5, 0, 0, GPIO_ACTIVE_HIGH } },
      Package () { "regulator-always-on", 1 },
      Package () { "regulator-pull-down", 1 },
      Package () { "enable-active-high", 1 },
      Package () { "off-on-delay-us", 15000 },
    }
  })
}
