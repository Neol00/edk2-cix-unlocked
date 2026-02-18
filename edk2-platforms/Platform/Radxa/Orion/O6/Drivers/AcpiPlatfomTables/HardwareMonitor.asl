/** @file

  Copyright 2025 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device(HWMN){
    Name(_UID, 0)
    Name(_HID, "CIXHA024")

    Method(_STA)
    {
      Return(0x03)
    }

    //
    // Name: SFAT [Set Fan to Auto Mode]
    // Description: Function to set fan to auto mode
    // Input: None
    // Output: None
    //
    Method(SFAT, 0, Serialized){
        // Set the fan auto mode through EC
        \_SB.EC0.SFAT()
    }

    //
    // Name: SFMT [Set Fan to Mute Mode]
    // Description: Function to set fan to mute mode
    // Input: None
    // Output: None
    //
    Method(SFMT, 0, Serialized){
        // Set the fan mute mode through EC
        \_SB.EC0.SFMT()
    }

    //
    // Name: SFPF [Set Fan to Performance Mode]
    // Description: Function to set fan to performance mode
    // Input: None
    // Output: None
    //
    Method(SFPF, 0, Serialized){
        // Set the fan performance mode through EC
        \_SB.EC0.SFPF()
    }

    //
    // Name: GFPW [Get Fan PWM]
    // Description: Function to get fan pwm
    // Input: Arg0 -> Type
    //        Arg1 -> Index
    // Output: Fan PWM
    //
    Method(GFPW, 2, Serialized){
      // We have a single fan. Ignore type and index.
      Local0 = Arg0
      Local0 = Arg1
      Return(\_SB.EC0.GFPW())
    }

    //
    // Name: SFPW [Set Fan PWM]
    // Description: Function to set fan pwm
    // Input: Arg0 -> Duty
    //        Arg1 -> Type
    //        Arg2 -> Index
    // Output: None
    //
    Method(SFPW, 3, Serialized){
      // We have a single fan. Ignore type and index.
      Local0 = Arg1
      Local0 = Arg2
      \_SB.EC0.SFPW(Arg0)
    }
}
