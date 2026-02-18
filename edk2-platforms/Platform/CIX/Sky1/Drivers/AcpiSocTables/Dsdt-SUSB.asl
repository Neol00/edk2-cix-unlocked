/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define USB_REMOTE_PD_DSD(Match) \
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { Match, 0 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () { \
                Package () { "port@0", "PRT0" },\
            }

#define USB_REMOTE_PD(DevRef,UsbcCon,PortRole,EptRole) \
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
                Package () { "remote-endpoint", Package() { DevRef, UsbcCon, PortRole, EptRole } },\
            }\
  })

#define USB_PHY_REMOTE_PD_DSD(MatchOri, MatchMux) \
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { MatchOri, 0 }, \
                Package () { MatchMux, 0 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () { \
                Package () { "port@0", "PRT0" },\
            }

#define USB_PHY_REMOTE_PD(DevRef,UsbcCon,PortOrienSw,EptOrienSw,PortAltmodeMux,EptAltmodeMux) \
  Name (PRT0, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "endpoint@0", "EP00" },\
                Package () { "endpoint@1", "EP01" },\
            }\
  })\
  Name (EP00, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
                Package () { "remote-endpoint", Package() { DevRef, UsbcCon, PortOrienSw, EptOrienSw } },\
            }\
  }) \
  Name (EP01, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 1 }, \
                Package () { "remote-endpoint", Package() { DevRef, PortAltmodeMux, EptAltmodeMux } },\
            }\
  })

#ifdef USBC0_PD_DEVICE
External (USBC0_PD_DEVICE, DeviceObj)
#endif
#ifdef USBC1_PD_DEVICE
External (USBC1_PD_DEVICE, DeviceObj)
#endif
#ifdef USBC2_PD_DEVICE
External (USBC2_PD_DEVICE, DeviceObj)
#endif
#ifdef USBC3_PD_DEVICE
External (USBC3_PD_DEVICE, DeviceObj)
#endif

Device (SUB0)
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x00)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB3_TYPEC_DRD_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09000310, 0x4)
    Memory32Fixed (ReadWrite, 0x09000400, 0x4)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_usb0", ResourceConsumer,)
#if USBC0_OC_EN
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI4", 0, ResourceConsumer) { 25 }
#endif
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x0 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
          Package () { "oc-gpio",  Package () { ^SUB0, 0, 0, 0 } },
#if USBC0_DISABLE_USB3
          Package () { "u3-port-disable", 1 },
#endif
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3C_DRD_CLK_SOF, "sof_clk", \_SB.SUB0},
    Package() {CLK_TREE_USB3C_DRD_AXI_GATE, "usb_aclk", \_SB.SUB0},
    Package() {CLK_TREE_USB3C_DRD_CLK_LPM, "lpm_clk", \_SB.SUB0},
    Package() {CLK_TREE_USB3C_DRD_APB_GATE, "usb_pclk", \_SB.SUB0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_SS0_PRST_N, \_SB.SUB0, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_SS0_RST_N, \_SB.SUB0, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.SUB0 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.SUB0 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB0)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x00)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x09010000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09014000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09018000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_U3_TYPEC_DRD_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_U3_TYPEC_DRD_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_U3_TYPEC_DRD_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_U3_TYPEC_DRD_IRQ_INTERRUPT_ID }
    })

#if USBC0_PD_EN
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "otg" },
            Package () { "cdnsp,usb3-phy", \_SB.UCP0.USBP },
          },
      USB_REMOTE_PD_DSD("usb-role-switch")
    })
    USB_REMOTE_PD(USBC0_PD_DEVICE, "usbc_con0", "port@0", "endpoint@0")
#else
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "host" },
            Package () { "cdnsp,usb3-phy", \_SB.UCP0.USBP },
          },
    })
#endif

    Name (RSNL, Package() {
      Package() {\_SB.SUB0.CUB0 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.SUB0.CUB0 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.SUB0.CUB0 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.SUB0.CUB0 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.SUB0.CUB0 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.SUB0.CUB0 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.SUB0.CUB0 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.SUB0 , \_SB.SUB0.CUB0, 0},
    })
  }
}

Device (U2P4) //USB 2.0 PHY4
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x04)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS4_PRST_N, \_SB.U2P4, "preset"},
  })
}

Device (UCP0) //USB 3.0 PHY0
{
  Name (_HID, "CIXH2033")     // _HID: Hardware ID
  Name (_UID, 0x00)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0F)           // _STA: Device status

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09030000, 0x40000)
  })

#if USBC0_PD_EN
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x0 },
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
          Package () { "svid", 0xff01 },
          Package () { "default_conf", USBC0_DEF_PMODE },
          Package () { "phy-status", "usb" },
        },
    USB_PHY_REMOTE_PD_DSD("orientation-switch", "mode-switch"),
  })
  USB_PHY_REMOTE_PD(USBC0_PD_DEVICE, "usbc_con0", "port@1", "endpoint@0", "port@2", "endpoint@0")
#else
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x0 },
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
          Package () { "default_conf", USBC0_DEF_PMODE },
          Package () { "phy-status", "usb" },
        },
  })
#endif

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3C_DRD_PHY3_GATE, "pclk", \_SB.UCP0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USB_DP_PHY0_PRST_N, \_SB.UCP0, "preset"},
    Package() {\_SB.RST0, SKY1_USB_DP_PHY0_RST_N, \_SB.UCP0, "reset"},
  })

  Device(USBP)
  {
    Name (_ADR, 0x00)
  }
  Device(UDPP)
  {
    Name (_ADR, 0x01)
  }
}

Device (SUB1)
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x01)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB3_TYPEC_HOST0_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09070310, 0x4)
    Memory32Fixed (ReadWrite, 0x09070400, 0x4)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_usb1", ResourceConsumer,)
#if USBC1_OC_EN
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI4", 0, ResourceConsumer) { 26 }
#endif
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x1 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
          Package () { "oc-gpio",  Package () { ^SUB1, 0, 0, 0 } },
#if USBC1_DISABLE_USB3
          Package () { "u3-port-disable", 1 },
#endif
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3C_H0_CLK_SOF, "sof_clk", \_SB.SUB1},
    Package() {CLK_TREE_USB3C_0_AXI_GATE, "usb_aclk", \_SB.SUB1},
    Package() {CLK_TREE_USB3C_H0_CLK_LPM, "lpm_clk", \_SB.SUB1},
    Package() {CLK_TREE_USB3C_0_APB_GATE, "usb_pclk", \_SB.SUB1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_SS1_PRST_N, \_SB.SUB1, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_SS1_RST_N, \_SB.SUB1, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.SUB1 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.SUB1 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB1)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x01)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x09080000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09084000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09088000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST0_U3_TYPEC_HOST0_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST0_U3_TYPEC_HOST0_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST0_U3_TYPEC_HOST0_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST0_U3_TYPEC_HOST0_IRQ_INTERRUPT_ID }
    })

#if USBC1_PD_EN
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "otg" },
            Package () { "cdnsp,usb3-phy", \_SB.UCP1.USBP },
          },
      USB_REMOTE_PD_DSD("usb-role-switch")
    })
    USB_REMOTE_PD(USBC1_PD_DEVICE, "usbc_con0", "port@0", "endpoint@0")
#else
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "host" },
            Package () { "cdnsp,usb3-phy", \_SB.UCP1.USBP },
          },
    })
#endif //USBC1_PD_EN

    Name (RSNL, Package() {
      Package() {\_SB.SUB1.CUB1 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.SUB1.CUB1 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.SUB1.CUB1 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.SUB1.CUB1 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.SUB1.CUB1 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.SUB1.CUB1 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.SUB1.CUB1 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.SUB1 , \_SB.SUB1.CUB1, 0},
    })
  }
}

Device (U2P5) //USB 2.0 PHY5
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x05)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS5_PRST_N, \_SB.U2P5, "preset"},
  })
}

Device (UCP1) //USB 3.0 PHY0
{
  Name (_HID, "CIXH2033")     // _HID: Hardware ID
  Name (_UID, 0x01)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0F)           // _STA: Device status

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x090A0000, 0x40000)
  })

#if USBC1_PD_EN
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x1 },
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
          Package () { "svid", 0xff01 },
          Package () { "default_conf", USBC1_DEF_PMODE },
          Package () { "phy-status", "usb" },
        },
    USB_PHY_REMOTE_PD_DSD("orientation-switch", "mode-switch"),
  })
  USB_PHY_REMOTE_PD(USBC1_PD_DEVICE, "usbc_con0", "port@1", "endpoint@0", "port@2", "endpoint@0")
#else
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x1 },
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
          Package () { "default_conf", USBC1_DEF_PMODE },
          Package () { "phy-status", "usb" },
        },
  })
#endif //USBC1_PD_EN

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3C_0_PHY3_GATE, "pclk", \_SB.UCP1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USB_DP_PHY1_PRST_N, \_SB.UCP1, "preset"},
    Package() {\_SB.RST0, SKY1_USB_DP_PHY1_RST_N, \_SB.UCP1, "reset"},
  })

  Device(USBP)
  {
    Name (_ADR, 0x00)
  }
  Device(UDPP)
  {
    Name (_ADR, 0x01)
  }
}

Device (SUB2)
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x02)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB3_TYPEC_HOST1_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x090e0310, 0x4)
    Memory32Fixed (ReadWrite, 0x090e0400, 0x4)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_usb2", ResourceConsumer,)
#if USBC2_OC_EN
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI4", 0, ResourceConsumer) { 27 }
#endif
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x2 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
          Package () { "oc-gpio",  Package () { ^SUB2, 0, 0, 0 } },
#if USBC2_DISABLE_USB3
          Package () { "u3-port-disable", 1 },
#endif
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3C_H1_CLK_SOF, "sof_clk", \_SB.SUB2},
    Package() {CLK_TREE_USB3C_1_AXI_GATE, "usb_aclk", \_SB.SUB2},
    Package() {CLK_TREE_USB3C_H1_CLK_LPM, "lpm_clk", \_SB.SUB2},
    Package() {CLK_TREE_USB3C_1_APB_GATE, "usb_pclk", \_SB.SUB2},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_SS4_PRST_N, \_SB.SUB2, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_SS4_RST_N, \_SB.SUB2, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.SUB2 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.SUB2 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB2)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x02)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x090f0000, 0x4000)
      Memory32Fixed (ReadWrite, 0x090f4000, 0x4000)
      Memory32Fixed (ReadWrite, 0x090f8000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST1_U3_TYPEC_HOST1_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST1_U3_TYPEC_HOST1_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST1_U3_TYPEC_HOST1_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST1_U3_TYPEC_HOST1_IRQ_INTERRUPT_ID }
    })

#if USBC2_PD_EN
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "otg" },
            Package () { "cdnsp,usb3-phy", \_SB.UCP2.USBP },
          },
      USB_REMOTE_PD_DSD("usb-role-switch")
    })
    USB_REMOTE_PD(USBC2_PD_DEVICE, "usbc_con0", "port@0", "endpoint@0")
#else
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "host" },
            Package () { "cdnsp,usb3-phy", \_SB.UCP2.USBP },
          },
    })
#endif //USBC2_PD_EN

    Name (RSNL, Package() {
      Package() {\_SB.SUB2.CUB2 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.SUB2.CUB2 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.SUB2.CUB2 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.SUB2.CUB2 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.SUB2.CUB2 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.SUB2.CUB2 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.SUB2.CUB2 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.SUB2 , \_SB.SUB2.CUB2, 0},
    })
  }
}

Device (U2P8) //USB 2.0 PHY8
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x08)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS8_PRST_N, \_SB.U2P8, "preset"},
  })
}

Device (UCP2) //USB 3.0 PHY0
{
  Name (_HID, "CIXH2033")     // _HID: Hardware ID
  Name (_UID, 0x03)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0F)           // _STA: Device status

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09110000, 0x40000)
  })

#if USBC2_PD_EN
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x2 },
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
          Package () { "svid", 0xff01 },
          Package () { "default_conf", USBC2_DEF_PMODE },
          Package () { "phy-status", "usb" },
        },
    USB_PHY_REMOTE_PD_DSD("orientation-switch", "mode-switch"),
  })
  USB_PHY_REMOTE_PD(USBC2_PD_DEVICE, "usbc_con0", "port@1", "endpoint@0", "port@2", "endpoint@0")
#else
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x2 },
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
          Package () { "default_conf", USBC2_DEF_PMODE },
          Package () { "phy-status", "usb" },
        },
  })
#endif //USBC2_PD_EN

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3C_1_PHY3_GATE, "pclk", \_SB.UCP2},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USB_DP_PHY2_PRST_N, \_SB.UCP2, "preset"},
    Package() {\_SB.RST0, SKY1_USB_DP_PHY2_RST_N, \_SB.UCP2, "reset"},
  })

  Device(USBP)
  {
    Name (_ADR, 0x00)
  }
  Device(UDPP)
  {
    Name (_ADR, 0x01)
  }
}

Device (SUB3)
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x03)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB3_TYPEC_HOST2_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09150310, 0x4)
    Memory32Fixed (ReadWrite, 0x09150400, 0x4)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_usb3", ResourceConsumer,)
#if USBC3_OC_EN
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly,
    "\\_SB.GPI4", 0, ResourceConsumer) { 28 }
#endif
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x3 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
          Package () { "oc-gpio",  Package () { ^SUB3, 0, 0, 0 } },
#if USBC3_DISABLE_USB3
          Package () { "u3-port-disable", 1 },
#endif
        }
  })

  Name (CLKT, Package() {
      Package() {CLK_TREE_USB3C_H2_CLK_SOF, "sof_clk", \_SB.SUB3},
      Package() {CLK_TREE_USB3C_2_AXI_GATE, "usb_aclk", \_SB.SUB3},
      Package() {CLK_TREE_USB3C_H2_CLK_LPM, "lpm_clk", \_SB.SUB3},
      Package() {CLK_TREE_USB3C_2_APB_GATE, "usb_pclk", \_SB.SUB3},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_SS5_PRST_N, \_SB.SUB3, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_SS5_RST_N, \_SB.SUB3, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.SUB3 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.SUB3 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB3)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x03)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x09160000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09164000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09168000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST2_U3_TYPEC_HOST2_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST2_U3_TYPEC_HOST2_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST2_U3_TYPEC_HOST2_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEC_HOST2_U3_TYPEC_HOST2_IRQ_INTERRUPT_ID }
    })

#if USBC3_PD_EN
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "otg" },
            Package () { "cdnsp,usb3-phy", \_SB.UCP3.USBP },
          },
      USB_REMOTE_PD_DSD("usb-role-switch")
    })
    USB_REMOTE_PD(USBC3_PD_DEVICE, "usbc_con0", "port@0", "endpoint@0")
#else
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "host" },
            Package () { "cdnsp,usb3-phy", \_SB.UCP3.USBP },
          },
    })
#endif //USBC3_PD_EN

    Name (RSNL, Package() {
      Package() {\_SB.SUB3.CUB3 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.SUB3.CUB3 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.SUB3.CUB3 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.SUB3.CUB3 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.SUB3.CUB3 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.SUB3.CUB3 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.SUB3.CUB3 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.SUB3 , \_SB.SUB3.CUB3, 0},
    })
  }
}

Device (U2P9) //USB 2.0 PHY9
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x09)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS9_PRST_N, \_SB.U2P9, "preset"},
  })
}

Device (UCP3) //USB 3.0 PHY0
{
  Name (_HID, "CIXH2033")     // _HID: Hardware ID
  Name (_UID, 0x04)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0F)           // _STA: Device status

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09180000, 0x40000)
  })

#if USBC3_PD_EN
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x3 },
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
          Package () { "svid", 0xff01 },
          Package () { "default_conf", USBC3_DEF_PMODE },
          Package () { "phy-status", "usb" },
        },
    USB_PHY_REMOTE_PD_DSD("orientation-switch", "mode-switch"),
  })
  USB_PHY_REMOTE_PD(USBC3_PD_DEVICE, "usbc_con0", "port@1", "endpoint@0", "port@2", "endpoint@0")
#else
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x3 },
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
          Package () { "default_conf", USBC3_DEF_PMODE },
          Package () { "phy-status", "usb" },
        },
  })
#endif //USBC3_PD_EN

  Name (CLKT, Package() {
      Package() {CLK_TREE_USB3C_2_PHY3_GATE, "pclk", \_SB.UCP3},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USB_DP_PHY3_PRST_N, \_SB.UCP3, "preset"},
    Package() {\_SB.RST0, SKY1_USB_DP_PHY3_RST_N, \_SB.UCP3, "reset"},
  })

  Device(USBP)
  {
    Name (_ADR, 0x00)
  }
  Device(UDPP)
  {
    Name (_ADR, 0x01)
  }
}

Device (SUB4)
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x04)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB3_TYPEA_DRD0_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x091c0314, 0x4)
    Memory32Fixed (ReadWrite, 0x091c0400, 0x4)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_usb4", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x4 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3A_H0_CLK_SOF, "sof_clk", \_SB.SUB4},
    Package() {CLK_TREE_USB3A_0_AXI_GATE, "usb_aclk", \_SB.SUB4},
    Package() {CLK_TREE_USB3A_H0_CLK_LPM, "lpm_clk", \_SB.SUB4},
    Package() {CLK_TREE_USB3A_0_APB_GATE, "usb_pclk", \_SB.SUB4},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_SS2_PRST_N, \_SB.SUB4, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_SS2_RST_N, \_SB.SUB4, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.SUB4 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.SUB4 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB4)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x04)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x091d0000, 0x4000)
      Memory32Fixed (ReadWrite, 0x091d4000, 0x4000)
      Memory32Fixed (ReadWrite, 0x091d8000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEA_CTL0_U3_TYPEA_CTL0_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEA_CTL0_U3_TYPEA_CTL0_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEA_CTL0_U3_TYPEA_CTL0_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEA_CTL0_U3_TYPEA_CTL0_IRQ_INTERRUPT_ID }
    })

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "host" },
            Package () { "cdnsp,usb3-phy", \_SB.U3P4.USB0 },
          },
    })

    Name (RSNL, Package() {
      Package() {\_SB.SUB4.CUB4 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.SUB4.CUB4 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.SUB4.CUB4 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.SUB4.CUB4 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.SUB4.CUB4 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.SUB4.CUB4 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.SUB4.CUB4 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.SUB4 , \_SB.SUB4.CUB4, 0},
    })
  }
}

Device (SUB5)
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x05)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB3_TYPEA_DRD1_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x091c0324, 0x4)
    Memory32Fixed (ReadWrite, 0x091c0410, 0x4)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_usb5", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x5 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3A_H1_CLK_SOF, "sof_clk", \_SB.SUB5},
    Package() {CLK_TREE_USB3A_1_AXI_GATE, "usb_aclk", \_SB.SUB5},
    Package() {CLK_TREE_USB3A_H1_CLK_LPM, "lpm_clk", \_SB.SUB5},
    Package() {CLK_TREE_USB3A_1_APB_GATE, "usb_pclk", \_SB.SUB5},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_SS3_PRST_N, \_SB.SUB5, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_SS3_RST_N, \_SB.SUB5, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.SUB5 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.SUB5 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB5)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x05)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x091e0000, 0x4000)
      Memory32Fixed (ReadWrite, 0x091e4000, 0x4000)
      Memory32Fixed (ReadWrite, 0x091e8000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEA_CTL1_U3_TYPEA_CTL1_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEA_CTL1_U3_TYPEA_CTL1_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEA_CTL1_U3_TYPEA_CTL1_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB3_TYPEA_CTL1_U3_TYPEA_CTL1_IRQ_INTERRUPT_ID }
    })

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "super-speed-plus" },
            Package () { "dr_mode", "host" },
            Package () { "cdnsp,usb3-phy", \_SB.U3P4.USB1 },
          },
    })

    Name (RSNL, Package() {
      Package() {\_SB.SUB5.CUB5 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.SUB5.CUB5 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.SUB5.CUB5 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.SUB5.CUB5 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.SUB5.CUB5 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.SUB5.CUB5 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.SUB5.CUB5 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.SUB5 , \_SB.SUB5.CUB5, 0},
    })
  }
}

Device (U2P6) //USB 2.0 PHY6
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x06)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS6_PRST_N, \_SB.U2P6, "preset"},
  })
}

Device (U2P7) //USB 2.0 PHY7
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x07)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS7_PRST_N, \_SB.U2P7, "preset"},
  })
}

Device (U3P4) //USB 3.0 PHY0
{
  Name (_HID, "CIXH2034")     // _HID: Hardware ID
  Name (_UID, 0x04)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09210000, 0x40000)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "cix,usbphy_syscon", \_SB.CRU0 },
        },
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB3A_PHY3_GATE, "apb_clk", \_SB.U3P4},
    Package() {CLK_TREE_USB3A_PHY_x2_REF, "ref_clk", \_SB.U3P4},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_SS_PST_N, \_SB.U3P4, "preset"},
    Package() {\_SB.RST0, SKY1_USBPHY_SS_RST_N, \_SB.U3P4, "reset"},
  })

  Device(USB0)
  {
    Name (_ADR, 0x00)
  }
  Device(USB1)
  {
    Name (_ADR, 0x01)
  }
}

Device (HUB0) //High Speed USB 0, USB2.0
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x06)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB2_HOST0_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09250310, 0x4)
    Memory32Fixed (ReadWrite, 0x09250400, 0x4)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x6 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB2_0_CLK_SOF, "sof_clk", \_SB.HUB0},
    Package() {CLK_TREE_USB2_0_AXI_GATE, "usb_aclk", \_SB.HUB0},
    Package() {CLK_TREE_USB2_0_CLK_LPM, "lpm_clk", \_SB.HUB0},
    Package() {CLK_TREE_USB2_0_APB_GATE, "usb_pclk", \_SB.HUB0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_HS0_PRST_N, \_SB.HUB0, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_HS0_RST_N, \_SB.HUB0, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.HUB0 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.HUB0 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB0)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x00)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x09260000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09264000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09268000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST0_U2_HOST0_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST0_U2_HOST0_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST0_U2_HOST0_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST0_U2_HOST0_IRQ_INTERRUPT_ID }
    })

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "high-speed" },
            Package () { "dr_mode", "host" },
          },
    })

    Name (RSNL, Package() {
      Package() {\_SB.HUB0.CUB0 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.HUB0.CUB0 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.HUB0.CUB0 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.HUB0.CUB0 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.HUB0.CUB0 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.HUB0.CUB0 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.HUB0.CUB0 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.HUB0 , \_SB.HUB0.CUB0, 0},
    })
  }
}

Device (HUB1) //High Speed USB 1, USB2.0
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x07)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB2_HOST1_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x09280310, 0x4)
    Memory32Fixed (ReadWrite, 0x09280400, 0x4)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_usb7", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x7 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB2_1_CLK_SOF, "sof_clk", \_SB.HUB1},
    Package() {CLK_TREE_USB2_1_AXI_GATE, "usb_aclk", \_SB.HUB1},
    Package() {CLK_TREE_USB2_1_CLK_LPM, "lpm_clk", \_SB.HUB1},
    Package() {CLK_TREE_USB2_1_APB_GATE, "usb_pclk", \_SB.HUB1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_HS1_PRST_N, \_SB.HUB1, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_HS1_RST_N, \_SB.HUB1, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.HUB1 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.HUB1 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB1)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x01)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x09290000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09294000, 0x4000)
      Memory32Fixed (ReadWrite, 0x09298000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST1_U2_HOST1_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST1_U2_HOST1_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST1_U2_HOST1_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST1_U2_HOST1_IRQ_INTERRUPT_ID }
    })

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "high-speed" },
            Package () { "dr_mode", "host" },
          },
    })

    Name (RSNL, Package() {
      Package() {\_SB.HUB1.CUB1 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.HUB1.CUB1 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.HUB1.CUB1 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.HUB1.CUB1 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.HUB1.CUB1 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.HUB1.CUB1 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.HUB1.CUB1 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.HUB1 , \_SB.HUB1.CUB1, 0},
    })
  }
}

Device (U2P0) //USB 2.0 PHY0
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x00)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS0_PRST_N, \_SB.U2P0, "preset"},
  })
}

Device (U2P1) //USB 2.0 PHY1
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x01)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS1_PRST_N, \_SB.U2P1, "preset"},
  })
}

Device (HUB2) //High Speed USB 2, USB2.0
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x08)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB2_HOST2_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x092b0310, 0x4)
    Memory32Fixed (ReadWrite, 0x092b0400, 0x4)
    PinGroupFunction(Exclusive, 0x0, "\\_SB.MUX1", 0, "pinctrl_usb8", ResourceConsumer,)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x8 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB2_2_CLK_SOF, "sof_clk", \_SB.HUB2},
    Package() {CLK_TREE_USB2_2_AXI_GATE, "usb_aclk", \_SB.HUB2},
    Package() {CLK_TREE_USB2_2_CLK_LPM, "lpm_clk", \_SB.HUB2},
    Package() {CLK_TREE_USB2_2_APB_GATE, "usb_pclk", \_SB.HUB2},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_HS2_PRST_N, \_SB.HUB2, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_HS2_RST_N, \_SB.HUB2, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.HUB2 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.HUB2 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB2)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x02)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x092c0000, 0x4000)
      Memory32Fixed (ReadWrite, 0x092c4000, 0x4000)
      Memory32Fixed (ReadWrite, 0x092c8000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST2_U2_HOST2_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST2_U2_HOST2_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST2_U2_HOST2_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST2_U2_HOST2_IRQ_INTERRUPT_ID }
    })

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "high-speed" },
            Package () { "dr_mode", "host" },
          },
    })

    Name (RSNL, Package() {
      Package() {\_SB.HUB2.CUB2 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.HUB2.CUB2 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.HUB2.CUB2 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.HUB2.CUB2 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.HUB2.CUB2 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.HUB2.CUB2 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.HUB2.CUB2 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.HUB2 , \_SB.HUB2.CUB2, 0},
    })
  }
}

Device (U2P2) //USB 2.0 PHY2
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x02)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS2_PRST_N, \_SB.U2P2, "preset"},
  })
}


Device (HUB3) //High Speed USB 3, USB2.0
{
  Name (_HID, "CIXH2030")     // _HID: Hardware ID
  Name (_UID, 0x09)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Method (_STA)               // _STA: Device status
  {
    If(\_SB.GETV(ARV_USB2_HOST3_ENABLE_OFFSET)){
      Return (0xB)
    } else {
      Return (0x0)
    }
  }

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x092e0310, 0x4)
    Memory32Fixed (ReadWrite, 0x092e0400, 0x4)
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
          Package () { "id", 0x9 },
          Package () { "axi_bmax_value", 0x7 },
          Package () { "cix,usb_syscon", \_SB.CRU0 },
          Package () { "sof_clk_freq", 8000000 },
          Package () { "lpm_clk_freq", 32000 },
        }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_USB2_3_CLK_SOF, "sof_clk", \_SB.HUB3},
    Package() {CLK_TREE_USB2_3_AXI_GATE, "usb_aclk", \_SB.HUB3},
    Package() {CLK_TREE_USB2_3_CLK_LPM, "lpm_clk", \_SB.HUB3},
    Package() {CLK_TREE_USB2_3_APB_GATE, "usb_pclk", \_SB.HUB3},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBC_HS3_PRST_N, \_SB.HUB3, "usb_preset"},
    Package() {\_SB.RST0, SKY1_USBC_HS3_RST_N, \_SB.HUB3, "usb_reset"},
  })
  Name (RSNL, Package() {
    Package() {\_SB.HUB3 , RESOURCE_MEM, 0, "axi_property"},
    Package() {\_SB.HUB3 , RESOURCE_MEM, 1, "controller_status"},
  })

  Device(CUB3)
  {
    Name (_HID, "CIXH2031")     // _HID: Hardware ID
    Name (_UID, 0x03)           // _UID: Unique ID
    Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
    Method (_STA)               // _STA: Device status
    {
        Return (0xB)
    }

    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x092f0000, 0x4000)
      Memory32Fixed (ReadWrite, 0x092f4000, 0x4000)
      Memory32Fixed (ReadWrite, 0x092f8000, 0x8000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST3_U2_HOST3_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST3_U2_HOST3_IRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST3_U2_HOST3_OTGIRQ_INTERRUPT_ID }
      Interrupt (ResourceConsumer, Level, ActiveHigh, ExclusiveAndWake, 0, "\\_SB.PDC0") { USB2_HOST3_U2_HOST3_IRQ_INTERRUPT_ID }
    })

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
            Package () { "maximum-speed", "high-speed" },
            Package () { "dr_mode", "host" },
          },
    })

    Name (RSNL, Package() {
      Package() {\_SB.HUB3.CUB3 , RESOURCE_IRQ, 0, "host"},
      Package() {\_SB.HUB3.CUB3 , RESOURCE_IRQ, 1, "peripheral"},
      Package() {\_SB.HUB3.CUB3 , RESOURCE_IRQ, 2, "otg"},
      Package() {\_SB.HUB3.CUB3 , RESOURCE_IRQ, 3, "wakeup"},
      Package() {\_SB.HUB3.CUB3 , RESOURCE_MEM, 0, "otg"},
      Package() {\_SB.HUB3.CUB3 , RESOURCE_MEM, 1, "dev"},
      Package() {\_SB.HUB3.CUB3 , RESOURCE_MEM, 2, "xhci"},
    })
    Name (DLKL, Package() {
      Package() {\_SB.HUB3 , \_SB.HUB3.CUB3, 0},
    })
  }
}

Device (U2P3) //USB 2.0 PHY3
{
  Name (_HID, "CIXH2032")     // _HID: Hardware ID
  Name (_UID, 0x03)           // _UID: Unique ID
  Name (_CCA, 0x00)           // _CCA: Cache Coherency Attribute
  Name (_STA, 0x0B)           // _STA: Device status

  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_USBPHY_HS3_PRST_N, \_SB.U2P3, "preset"},
  })
}
