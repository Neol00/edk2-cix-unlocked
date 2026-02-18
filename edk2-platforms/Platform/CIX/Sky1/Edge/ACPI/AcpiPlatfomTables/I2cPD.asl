
/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define DP_USBC_CON_DSD(Name) \
        ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
        Package () { \
            Package () { Name, "UC00" },\
        }

#define DP_USBC_CON_NODES(DevRefUsbRole,DevRefUsbOriSwitch,DevRefDpAltMux) \
  Name (UC00, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "data-role", "host" }, \
                Package () { "power-role", "source" }, \
                Package () { "try-power-role", "source" }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "port@0", "PRT0" },\
                Package () { "port@1", "PRT1" },\
                Package () { "port@2", "PRT2" },\
            }\
  })\
  Name (PRT0, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "endpoint@0", "EP00" },\
            }\
  })\
  Name (EP00, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
                Package () { "remote-endpoint", Package() { DevRefUsbRole, "port@0", "endpoint@0" } },\
            }\
  }) \
  Name (PRT1, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 1 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "endpoint@0", "EP01" },\
            }\
  })\
  Name (EP01, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
                Package () { "remote-endpoint", Package() { DevRefUsbOriSwitch, "port@0", "endpoint@0" } },\
            }\
  }) \
  Name (PRT2, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 2 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "endpoint@1", "EP02" },\
            }\
  })\
  Name (EP02, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 1 }, \
                Package () { "remote-endpoint", Package() { DevRefDpAltMux, "port@0", "endpoint@1" } },\
            }\
  })

Scope (\_SB.I2C1)
{
  Device (PD10) {
    Name (_HID, "CIXH200D")
    Name (_UID, 0x0)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x30,
                      ControllerInitiated,
                      100000,
                      AddressingMode7Bit,
                      "\\_SB.I2C1",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      GpioInt(Level, ActiveLow, Exclusive, PullUp, , "\\_SB.GPI4") { 8 }
    })
    Name (_DSD, Package () {
          ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
              Package () {"id", 2},
          },
          DP_USBC_CON_DSD("usbc_con2")
    })
    DP_USBC_CON_NODES(\_SB.SUB2.CUB2, \_SB.UCP2, \_SB.UCP2)
  }

  Device (PD11) {
    Name (_HID, "CIXH200D")
    Name (_UID, 0x1)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x31,
                      ControllerInitiated,
                      100000,
                      AddressingMode7Bit,
                      "\\_SB.I2C1",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      GpioInt(Level, ActiveLow, Exclusive, PullUp, , "\\_SB.GPI4") { 8 }
    })
    Name (_DSD, Package () {
          ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
              Package () {"id", 3},
          },
          DP_USBC_CON_DSD("usbc_con3")
    })
    DP_USBC_CON_NODES(\_SB.SUB3.CUB3, \_SB.UCP3, \_SB.UCP3)
  }
}

Scope (\_SB.I2C7)
{
  Device (PD00) {
    Name (_HID, "CIXH200D")
    Name (_UID, 0x1)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x30,
                      ControllerInitiated,
                      100000,
                      AddressingMode7Bit,
                      "\\_SB.I2C7",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      GpioInt(Level, ActiveLow, Exclusive, PullUp, , "\\_SB.GPI4") { 7 }
    })
    Name (_DSD, Package () {
          ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
              Package () {"id", 0},
          },
          DP_USBC_CON_DSD("usbc_con0")
    })
    DP_USBC_CON_NODES(\_SB.SUB0.CUB0, \_SB.UCP0, \_SB.UCP0)
  }

  Device (PD01) {
    Name (_HID, "CIXH200D")
    Name (_UID, 0x2)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      I2cSerialBusV2 (0x31,
                      ControllerInitiated,
                      100000,
                      AddressingMode7Bit,
                      "\\_SB.I2C7",
                      0x0,
                      ResourceConsumer,
                      ,
                      Exclusive
                      ,)
      GpioInt(Level, ActiveLow, Exclusive, PullUp, , "\\_SB.GPI4") { 7 }
    })
    Name (_DSD, Package () {
          ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
              Package () {"id", 1},
          },
          DP_USBC_CON_DSD("usbc_con1")
    })
    DP_USBC_CON_NODES(\_SB.SUB1.CUB1, \_SB.UCP1, \_SB.UCP1)
  }
}
