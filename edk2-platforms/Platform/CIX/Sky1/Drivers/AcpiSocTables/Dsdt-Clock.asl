/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Protocol/ClockId.h>

Device (CCLK) {
    Name (_HID, "CIXHA010")
    Name (_UID, 0)
    Name (_STA, 0xF)
    //For linux clock framwork
    Name (CLKT, Package(){
    })

    /*get clock method*/
    Method (GCLK, 1, Serialized)
    {
        Return(\_SB.PMMX.CLKG(Arg0))
    }

    /*set clock method*/
    Method (SCLK, 3, Serialized)
    {
        Return(\_SB.PMMX.CLKS(Arg0,Arg1,Arg2))
    }

    /*describe clock method*/
    Method (CLKD, 2, Serialized)
    {
        Return(\_SB.PMMX.CLKD(Arg0,Arg1))
    }

    /*config clock method*/
    Method (CLKC, 2, Serialized)
    {
        Return(\_SB.PMMX.CLKC(Arg0,Arg1))
    }
}
