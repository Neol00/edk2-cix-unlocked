/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define CLK_TREE_CAMERA_MCLK0     72
#define CLK_TREE_CAMERA_MCLK1     73
#define CLK_TREE_CAMERA_MCLK2     74
#define CLK_TREE_CAMERA_MCLK3     75

External (\_SB.I2C0, DeviceObj)
External (\_SB.I2C1, DeviceObj)
External (\_SB.ISP0, DeviceObj)

//Camera power enable 4
Device (CPE4) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x04)
  Name (_STA, 0x0B)

  Name (_CRS, ResourceTemplate () {
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI1", 0, ResourceConsumer) { 6 }
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "regulator-fixed" },
      Package () { "regulator-name", "cam_power_en_4" },
      Package () { "regulator-min-microvolt", 3300000 },
      Package () { "regulator-max-microvolt", 3300000 },
      Package () { "gpio", Package () { ^CPE4, 0, 0, 0 } },
    }
  })
}

Scope (\_SB.I2C0)
{
  //ISP imgsensor00
  Device (IIS0) {
    Name (_HID, "CIXH3024")
    Name (_UID, 0x00)
    Name (_STA, 0x0F)
    Name (_CCA, 0)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x34,
                      ControllerInitiated,
                      400000,
                      AddressingMode7Bit,
                      "\\_SB.I2C0",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_cam0_hw", ResourceConsumer,)
      GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
      "\\_SB.GPI1", 0, ResourceConsumer) { 13, 14 }
    })
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () {"actuator-src", \_SB.I2C0.MTR0},
          Package () {"isp-src", \_SB.ISP0},
          Package () {"cix,camera-module-index", 0},
          Package () { "power0-supply",  Package () { \_SB.CPE4 } },
          Package () { "reset-gpios",  Package () { ^IIS0, 0, 0, 0 } },
          Package () { "pwdn-gpios",  Package () { ^IIS0, 0, 1, 0 } },
      }
    })
    Name (CLKT, Package() {
      Package() {CLK_TREE_CAMERA_MCLK0, "mclk", \_SB.I2C0.IIS0},
    })
  }
  //ISP motor
  Device (MTR0) {
    Name (_HID, "CIXH3023")
    Name (_UID, 0x0)
    Name (_STA, 0xF)
    Name (_CCA, 0)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x40,
                      ControllerInitiated,
                      124999999,
                      AddressingMode7Bit,
                      "\\_SB.I2C0",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
    })
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () {"pi-max-frequency", 124999999},
      }
    })
  }
}


Scope (\_SB.I2C1)
{
  //ISP imgsensor01
  Device (IIS1) {
    Name (_HID, "CIXH3024")
    Name (_UID, 0x01)
    Name (_STA, 0x0F)
    Name (_CCA, 0)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x36,
                      ControllerInitiated,
                      400000,
                      AddressingMode7Bit,
                      "\\_SB.I2C1",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_cam1_hw", ResourceConsumer,)
      GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
      "\\_SB.GPI1", 0, ResourceConsumer) { 10, 7 }
    })
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () {"cix,camera-module-index", 1},
          Package () { "power0-supply",  Package () { \_SB.CPE4 } },
          Package () { "reset-gpios",  Package () { ^IIS1, 0, 0, 0 } },
          Package () { "pwdn-gpios",  Package () { ^IIS1, 0, 1, 0 } },
      }
    })
    Name (CLKT, Package() {
      Package() {CLK_TREE_CAMERA_MCLK2, "mclk", \_SB.I2C1.IIS1},
    })
  }
}
