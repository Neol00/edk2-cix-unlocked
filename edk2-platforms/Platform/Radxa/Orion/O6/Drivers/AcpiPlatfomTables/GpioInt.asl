/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External (\_SB.GPI4, DeviceObj)

Scope (\_SB.GPI4)
{
  Name (_AEI, ResourceTemplate() {
    GpioInt (Level, ActiveLow, Exclusive, PullUp, 0, "\\_SB.GPI4") {6}
    GpioInt(Level, ActiveLow, ExclusiveAndWake, PullUp, , "\\_SB.GPI4") { 10 }
  })

  Method (_L06) {
    \_SB.EC0.EVNT()
  }

  Method (_L0A) {
    \_SB.ERTC.CAFG()
  }
}
