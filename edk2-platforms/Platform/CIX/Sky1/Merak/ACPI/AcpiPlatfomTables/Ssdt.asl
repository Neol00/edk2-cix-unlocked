/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock("SsdtTable.aml", "SSDT", 5, "CIXTEK", "SKY1EDK2", 1) {
  /* External declarations for DSDT-defined objects referenced by this SSDT */
  External (\_SB.I2C0, DeviceObj)
  External (\_SB.I2C1, DeviceObj)
  External (\_SB.I2C2, DeviceObj)
  External (\_SB.I2C3, DeviceObj)
  External (\_SB.I2C4, DeviceObj)
  External (\_SB.I2C5, DeviceObj)
  External (\_SB.I2C7, DeviceObj)
  External (\_SB.I3C0, DeviceObj)
  External (\_SB.SPI0, DeviceObj)
  External (\_SB.SPI1, DeviceObj)
  External (\_SB.GPI1, DeviceObj)
  External (\_SB.GPI3, DeviceObj)
  External (\_SB.GPI4, DeviceObj)
  External (\_SB.HDA, DeviceObj)
  External (\_SB.ISP0, DeviceObj)
  External (\_SB.MUX0, DeviceObj)
  External (\_SB.MUX1, DeviceObj)
  External (\_SB.SUB0.CUB0, DeviceObj)
  External (\_SB.SUB1.CUB1, DeviceObj)
  External (\_SB.SUB2.CUB2, DeviceObj)
  External (\_SB.SUB3.CUB3, DeviceObj)
  External (\_SB.UCP0, DeviceObj)
  External (\_SB.UCP1, DeviceObj)
  External (\_SB.UCP2, DeviceObj)
  External (\_SB.UCP3, DeviceObj)
  External (UDBG, MethodObj)

  Scope(_SB) {
    include("Audio.asl")
    include("Rtc.asl")
    include("Sensor.asl")
    include("MipiCamera.asl")
    include("Wlan.asl")
    include("EC.asl")
    include("I2cHid.asl")
    include("I2cPD.asl")
    include("CixWmi.asl")
  }
}
