/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (HDAC) {
    Name (_HID, "CIXH6030")
    Name (_UID, 0x0)
    Name (_STA, 0xF)

    Name (DLKL, Package() {
      Package() {\_SB.HDA, \_SB.HDAC, 0},
    })
}


Scope (\_SB.I2C2)
{
  /* alc5682 */
  Device (RTL5) {
    Name (_HID, "RTL5682")
    Name (_UID, 0x0)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      GpioInt(Edge, ActiveBoth, SharedAndWake, PullNone, 0,"\\_SB.GPI3",) { 10 }
      PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_alc5682_irq", ResourceConsumer,)

      I2cSerialBusV2 (0x1a,
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
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "realtek,dmic1-data-pin", 1 }, /* using GPIO2 pin as dmic1 data pin */
            Package () { "realtek,dmic-clk-pin", 1 }, /* using GPIO3 pin as dmic1 clock pin */
            Package () { "realtek,dmic-clk-rate-hz", 2048000 },
            Package () { "realtek,jd-src", 1 },
          }
    })
  }
}

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

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
//        Package () { "pdb0-gpios"     , Package () { ^SNDC, 0, 0, 0 } },
//        Package () { "pdb1-gpios"     , Package () { ^SNDC, 0, 1, 0 } },
//        Package () { "pdb2-gpios"     , Package () { ^SNDC, 0, 2, 0 } },
//        Package () { "pdb3-gpios"     , Package () { ^SNDC, 0, 3, 0 } },
//        Package () { "hpmicdet-gpios" , Package () { ^SNDC, 1, 0, 0 } },
        Package () { "sndcard-idx" , 3 },
      }
  })
}