/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright 2023-2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <sky1-iomux.h>

Device (I2C0) {
  Name (_HID, "CIXH200B")
  Name (_UID, 0x0)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_FCH_I2C_0_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (CLKF, 100000)
  Method(_INI, 0) {
    Local0 = GETV(ARV_FCH_I2C_0_FREQ_OFFSET)
    if (Local0 != 0) {
      CLKF = Local0 * ARV_I2C_FREQ_UNIT_HZ
    }
  }
  Name (MXID, FixedPcdGet8 (PcdI2c0MutexId))

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I2C0_BASE, I2C0_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I2C0_INTERRUPT_ID }
    // Pinctrl
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_i2c0", ResourceConsumer,)
  })

  //
  // For clock get/set in device driver
  // Device specific method
  // Arg0 = CIX CLOCK GUID
  // Arg1 = Revision
  // Arg2 = Function
  // Arg3 = Parameters
  /*
  Method (_DSM, 4, Serialized) {
    //CLOCK
    If (LEqual(Arg0,ToUUID("854BCF86-4DBF-4f3d-A19A-FBEF3C7CE730"))){
      Switch (ToInteger(Arg2)) {
        Case(GET_CLOCK_RATE)
        {
          Return(\_SB.MBX0.CLKG(CLK_TREE_FCH_I2C0_APB))
        }
        Case(SET_CLOCK_RATE) {
          Return(\_SB.MBX0.CLKS(CLK_TREE_FCH_I2C0_APB,DeRefOf(Index(Arg3, 0)),DeRefOf(Index(Arg3, 1))))
          }
      }
    }
    Return (Buffer() {ACPI_SCMI_INVALID_PARAMETERS})
  }

  Name(_DSD, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package() {
          Package(2) {"ClockRange", Package(){100000,200000,400000}},
        }
  }) // _DSD()
  */

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"ClockName", "fch_i2c0_apb"},
        Package () { "clock-frequency", CLKF },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I2C0_APB, "", \_SB.I2C0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_I2C0_RST_APB_N, \_SB.I2C0, "i2c_reset"},
  })
}

Device (I2C1) {
  Name (_HID, "CIXH200B")
  Name (_UID, 0x1)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_FCH_I2C_1_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (CLKF, 100000)
  Method(_INI, 0) {
    Local0 = GETV(ARV_FCH_I2C_1_FREQ_OFFSET)
    if (Local0 != 0) {
      CLKF = Local0 * ARV_I2C_FREQ_UNIT_HZ
    }
  }
  Name (MXID, FixedPcdGet8 (PcdI2c1MutexId))

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I2C1_BASE, I2C1_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I2C1_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"ClockName", "fch_i2c1_apb"},
        Package () { "clock-frequency", CLKF },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I2C1_APB, "", \_SB.I2C1},
  })
}

Device (I2C2) {
  Name (_HID, "CIXH200B")
  Name (_UID, 0x2)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_FCH_I2C_2_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (CLKF, 100000)
  Method(_INI, 0) {
    Local0 = GETV(ARV_FCH_I2C_2_FREQ_OFFSET)
    if (Local0 != 0) {
      CLKF = Local0 * ARV_I2C_FREQ_UNIT_HZ
    }
  }
  Name (MXID, FixedPcdGet8 (PcdI2c2MutexId))

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I2C2_BASE, I2C2_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I2C2_INTERRUPT_ID }
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
                "\\_SB.GPI0", 0, ResourceConsumer) { 12, 13 }
    // Pinctrl
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_fch_i2c2", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"ClockName", "fch_i2c2_apb"},
        Package () { "clock-frequency", CLKF },
        Package () { "scl-gpios", Package () { ^I2C2, 0, 0, 0 } },
        Package () { "sda-gpios", Package () { ^I2C2, 0, 1, 0 } },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I2C2_APB, "", \_SB.I2C2},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST1, SW_I2C2_RST_APB_N, \_SB.I2C2, "i2c_reset"},
  })
}

Device (I2C3) {
  Name (_HID, "CIXH200B")
  Name (_UID, 0x3)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_FCH_I2C_3_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (CLKF, 100000)
  Method(_INI, 0) {
    Local0 = GETV(ARV_FCH_I2C_3_FREQ_OFFSET)
    if (Local0 != 0) {
      CLKF = Local0 * ARV_I2C_FREQ_UNIT_HZ
    }
  }
  Name (MXID, FixedPcdGet8 (PcdI2c3MutexId))

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I2C3_BASE, I2C3_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I2C3_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"ClockName", "fch_i2c3_apb"},
        Package () { "clock-frequency", CLKF },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I2C3_APB, "", \_SB.I2C3},
  })
}

Device (I2C4) {
  Name (_HID, "CIXH200B")
  Name (_UID, 0x4)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_FCH_I2C_4_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (CLKF, 100000)
  Method(_INI, 0) {
    Local0 = GETV(ARV_FCH_I2C_4_FREQ_OFFSET)
    if (Local0 != 0) {
      CLKF = Local0 * ARV_I2C_FREQ_UNIT_HZ
    }
  }
  Name (MXID, FixedPcdGet8 (PcdI2c4MutexId))

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I2C4_BASE, I2C4_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I2C4_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"ClockName", "fch_i2c4_apb"},
        Package () { "clock-frequency", CLKF },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I2C4_APB, "", \_SB.I2C4},
  })
}

Device (I2C5) {
  Name (_HID, "CIXH200B")
  Name (_UID, 0x5)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_FCH_I2C_5_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (CLKF, 100000)
  Method(_INI, 0) {
    Local0 = GETV(ARV_FCH_I2C_5_FREQ_OFFSET)
    if (Local0 != 0) {
      CLKF = Local0 * ARV_I2C_FREQ_UNIT_HZ
    }
  }
  Name (MXID, FixedPcdGet8 (PcdI2c5MutexId))

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I2C5_BASE, I2C5_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I2C5_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"ClockName", "fch_i2c5_apb"},
        Package () { "clock-frequency", CLKF },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I2C5_APB, "", \_SB.I2C5},
  })
}

Device (I2C6) {
  Name (_HID, "CIXH200B")
  Name (_UID, 0x6)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_FCH_I2C_6_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (CLKF, 100000)
  Method(_INI, 0) {
    Local0 = GETV(ARV_FCH_I2C_6_FREQ_OFFSET)
    if (Local0 != 0) {
      CLKF = Local0 * ARV_I2C_FREQ_UNIT_HZ
    }
  }
  Name (MXID, FixedPcdGet8 (PcdI2c6MutexId))

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I2C6_BASE, I2C6_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I2C6_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"ClockName", "fch_i2c6_apb"},
        Package () { "clock-frequency", CLKF },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I2C6_APB, "", \_SB.I2C6},
  })
}

Device (I2C7) {
  Name (_HID, "CIXH200B")
  Name (_UID, 0x7)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_FCH_I2C_7_ENABLE_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (CLKF, 100000)
  Method(_INI, 0) {
    Local0 = GETV(ARV_FCH_I2C_7_FREQ_OFFSET)
    if (Local0 != 0) {
      CLKF = Local0 * ARV_I2C_FREQ_UNIT_HZ
    }
  }
  Name (MXID, FixedPcdGet8 (PcdI2c7MutexId))

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, I2C7_BASE, I2C7_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { FCH_INTR_I2C7_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"ClockName", "fch_i2c7_apb"},
        Package () { "clock-frequency", CLKF },
      }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_FCH_I2C7_APB, "", \_SB.I2C7},
  })
}

