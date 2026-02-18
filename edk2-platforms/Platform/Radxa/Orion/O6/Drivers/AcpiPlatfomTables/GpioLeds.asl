/** @file

  Copyright 2025 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Gpio.h"

External (\_SB.GPI4, DeviceObj)
External (\_SB.GPI5, DeviceObj)
External (\_SB.MUX1, DeviceObj)

Device (LEDS)
{
    Name (_HID, "PRP0001")
    Name (_UID, 0x20)
    Name (_DDN, "GPIO LEDs device")
    Name (_STA, 0xB)

    Name (_CRS, ResourceTemplate () {
        GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
        "\\_SB.GPI4", 0, ResourceConsumer) { 7 }    // GPIO008
        GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
        "\\_SB.GPI5", 0, ResourceConsumer) { 0 }    // GPIO015
        PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "gpio_leds", ResourceConsumer,)
    })

    Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
            Package () { "compatible", Package() { "gpio-leds" } },
        },
        ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
        Package () {
            Package () {"led@0", "LED0"},
            Package () {"led@1", "LED1"},
        }
    })

    Name (LED0, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
            Package () {"label", "blue:status"},
            Package () {"linux,default-trigger", "heartbeat"},
            Package () {"gpios", Package () {^LEDS, 0, 0, GPIO_ACTIVE_LOW}},
        }
    })
    Name (LED1, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
            Package () {"label", "green:power"},
            Package () {"linux,default-trigger", "default-on"},
            Package () {"gpios", Package () {^LEDS, 1, 0, GPIO_ACTIVE_LOW}},
        }
    })
}
