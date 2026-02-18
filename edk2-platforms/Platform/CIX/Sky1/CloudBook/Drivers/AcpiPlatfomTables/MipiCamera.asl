/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define CLK_TREE_CAMERA_MCLK0     72
#define CLK_TREE_CAMERA_MCLK1     73
#define CLK_TREE_CAMERA_MCLK2     74
#define CLK_TREE_CAMERA_MCLK3     75

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
      "\\_SB.GPI1", 0, ResourceConsumer) { 18, 16, 12, 15 }
    })
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () {"actuator-src", \_SB.I2C0.MTR0},
          Package () {"isp-src", \_SB.ISP0},
          Package () {"cix,camera-module-index", 0},
          Package () { "pwren-gpios",  Package () { ^IIS0, 0, 0, 0 } },
          Package () { "pwren0-gpios",  Package () { ^IIS0, 0, 1, 0 } },
          Package () { "reset-gpios",  Package () { ^IIS0, 0, 2, 0 } },
          Package () { "pwdn-gpios",  Package () { ^IIS0, 0, 3, 0 } },
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
  //ISP imgsensor02
  Device (IIS2) {
    Name (_HID, "CIXH3024")
    Name (_UID, 0x02)
    Name (_STA, 0x00)
    Name (_CCA, 0)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x38,
                      ControllerInitiated,
                      400000,
                      AddressingMode7Bit,
                      "\\_SB.I2C1",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_cam2_hw", ResourceConsumer,)
      GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
      "\\_SB.GPI1", 0, ResourceConsumer) { 11, 8, 10, 7 }
    })
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () {"cix,camera-module-index", 2},
          Package () { "pwren-gpios",  Package () { ^IIS2, 0, 0, 0 } },
          Package () { "pwren0-gpios",  Package () { ^IIS2, 0, 1, 0 } },
          Package () { "reset-gpios",  Package () { ^IIS2, 0, 2, 0 } },
          Package () { "pwdn-gpios",  Package () { ^IIS2, 0, 3, 0 } },
      }
    })
    Name (CLKT, Package() {
      Package() {CLK_TREE_CAMERA_MCLK2, "mclk", \_SB.I2C1.IIS2},
    })
  }
}

Scope (\_SB.I2C3)
{
  //ISP imgsensor01
  Device (IIS1) {
    Name (_HID, "CIXH3024")
    Name (_UID, 0x1)
    Name (_STA, 0xF)
    Name (_CCA, 0)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x36,
                      ControllerInitiated,
                      400000,
                      AddressingMode7Bit,
                      "\\_SB.I2C3",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_cam1_hw", ResourceConsumer,)
      GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
      "\\_SB.GPI1", 0, ResourceConsumer) { 18, 16, 17, 19 }
    })
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () {"cix,camera-module-index", 1 },
          Package () { "pwren-gpios",  Package () { ^IIS1, 0, 0, 0 } },
          Package () { "pwren0-gpios",  Package () { ^IIS1, 0, 1, 0 } },
          Package () { "reset-gpios",  Package () { ^IIS1, 0, 2, 0 } },
          Package () { "pwdn-gpios",  Package () { ^IIS1, 0, 3, 0 } },
      }
    })
    Name (CLKT, Package() {
      Package() {CLK_TREE_CAMERA_MCLK1, "mclk", \_SB.I2C3.IIS1},
    })
  }
}

Scope (\_SB.I2C4)
{
  //ISP imgsensor03
  Device (IIS3) {
    Name (_HID, "CIXH3024")
    Name (_UID, 0x3)
    Name (_STA, 0xF)
    Name (_CCA, 0)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x3a,
                      ControllerInitiated,
                      400000,
                      AddressingMode7Bit,
                      "\\_SB.I2C4",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_cam3_hw", ResourceConsumer,)
      GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
      "\\_SB.GPI1", 0, ResourceConsumer) { 11, 8, 13, 14 }
    })
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () {"cix,camera-module-index", 3},
          Package () { "pwren-gpios",  Package () { ^IIS3, 0, 0, 0 } },
          Package () { "pwren0-gpios",  Package () { ^IIS3, 0, 1, 0 } },
          Package () { "reset-gpios",  Package () { ^IIS3, 0, 2, 0 } },
          Package () { "pwdn-gpios",  Package () { ^IIS3, 0, 3, 0 } },
      }
    })
    Name (CLKT, Package() {
      Package() {CLK_TREE_CAMERA_MCLK3, "mclk", \_SB.I2C4.IIS3},
    })
  }
}