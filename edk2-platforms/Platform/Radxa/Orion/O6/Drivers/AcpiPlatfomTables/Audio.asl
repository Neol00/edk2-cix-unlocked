/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External (\_SB.HDA, DeviceObj)

Device (SNDC) {
  Name (_HID, "CIXH6070")
  Name (_UID, 0x0)
  Name (_STA, 0xF)

  Name (_CRS, ResourceTemplate () {
//    // for alc1019
//    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
//                "\\_SB.GPI3", 0, ResourceConsumer) {
//        5,
//        6,
//        7,
//        8,
//    }
//    // for alc5682
//    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
//                "\\_SB.GPI4", 0, ResourceConsumer) {
//        13,
//    }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_sndcard", ResourceConsumer,)
  })

//  Name (_DSD, Package () {
//    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
//      Package () {
//        Package () { "pdb0-gpios"     , Package () { ^SNDC, 0, 0, 0 } },
//        Package () { "pdb1-gpios"     , Package () { ^SNDC, 0, 1, 0 } },
//        Package () { "pdb2-gpios"     , Package () { ^SNDC, 0, 2, 0 } },
//        Package () { "pdb3-gpios"     , Package () { ^SNDC, 0, 3, 0 } },
//        Package () { "hpmicdet-gpios" , Package () { ^SNDC, 1, 0, 0 } },
//      }
//  })
}
