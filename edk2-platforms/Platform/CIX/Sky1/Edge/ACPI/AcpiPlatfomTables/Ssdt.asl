/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock("SsdtTable.aml", "SSDT", 5, "CIXTEK", "SKY1EDK2", 1) {
  Scope(_SB) {
    include("Audio.asl")
    include("Dtpm.asl")
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
