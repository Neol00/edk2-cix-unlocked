/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope (\_SB.SPI0)
{
    Device(TP1) {
        Name (_HID, "SPT0001")
        Name (_UID, 0x0)
        Method(_CRS, 0x0, NotSerialized)
        {
        Name (RBUF, ResourceTemplate ()
        {
            SpiSerialBus (0x0000, PolarityLow, FourWireMode, 0x08, ControllerInitiated, 0x0007A120, ClockPolarityLow, ClockPhaseFirst, "\\_SB.SPI0", 0x00, ResourceConsumer, , )
        })
        Return(RBUF)
    }
    }
}