/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device(DCRU) {
  Name (_HID, "CIXHA018")
  Name (_UID, 0x1)

  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x07110000, 0x10000)
  })
}

Device (DSP) {
  Name (_HID, "CIXH6000")
  Name (_UID, 0x0)

  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CCA, ONE)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x07000000,
      0x1000000,
    )
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) {
      AUDIO_IRQ_O_WDT_INTERRUPT_ID
    }
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "firmware-name", "dsp_fw.bin" },
      Package () { "mbox-names", Package () {"tx0", "rx0"}},
      Package () { "mboxes", Package (4) { MBX5, 9, MBX4, 9}},
      Package () { "cix,dsp-ctrl", DCRU },
    }
  })

  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_MB0_SW_RST_N  ,\_SB.DSP, "mb0"},
    Package() {\_SB.ADSS.ARST, AUDSS_MB1_SW_RST_N  ,\_SB.DSP, "mb1"},
    Package() {\_SB.RST0, SKY1_AUDIO_HIFI5_RESET_N, \_SB.DSP,  "dsp"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK , \_SB.DSP, 0},
    Package() {\_SB.ADSS.ARST , \_SB.DSP, 0},
  })
}
