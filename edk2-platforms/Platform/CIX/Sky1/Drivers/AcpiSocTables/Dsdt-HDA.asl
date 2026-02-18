/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (HDA) {
  Name (_HID, "CIXH6020")
  Name (_UID, 0x0)

  Method (_STA, 0x0, Serialized) {
    If(FixedPcdGetBool(PcdAcpiHdaEnable) && \_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x070c0000,
      0x10000
      )
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { AUDIO_IRQ_O_HDA_INTERRUPT_ID }
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX0", 0, "pinctrl_hda", ResourceConsumer,)
#ifdef HDA_EXT_CRS
    HDA_EXT_CRS
#endif
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "cru-ctrl", \_SB.ACRU },
#ifdef HDA_EXT_DSD_PROPERTY
      HDA_EXT_DSD_PROPERTY,
#endif
    }
  })

  Name (RSTL, Package() {
    Package() {\_SB.ADSS.ARST, AUDSS_HDA_SW_RST_N  ,\_SB.HDA, "hda"},
  })
  Name (DLKL, Package() {
    Package() {\_SB.ADSS.ACLK, \_SB.HDA, 0},
    Package() {\_SB.ADSS.ARST, \_SB.HDA, 0},
  })
}

