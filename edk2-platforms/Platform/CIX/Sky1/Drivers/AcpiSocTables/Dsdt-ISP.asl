/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//ISP V4L2 config
Device (V4L2) {
  Name (_HID, "CIXH3020")
  Name (_UID, 0x0)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_ISP_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { ISP_EXT_U_ISP_CORE_O_ISP_INT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { ISP_EXT_U_ISP_CORE_O_ISP_INT_ABNORMAL_INTERRUPT_ID }
  })
}

//ISP i7_isp
Device (ISP0) {
  Name (_HID, "CIXH3021")
  Name (_UID, 0x00)
  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_ISP_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, ISP_CONTROLLER0_BASE, ISP_CONTROLLER0_SIZE)
      Memory32Fixed (ReadWrite, ISP_CONTROLLER1_BASE, ISP_CONTROLLER1_SIZE)
  })
}

//ISP i7_isp_fake1
Device (ISP1) {
  Name (_HID, "CIXH3022")
  Name (_UID, 0x00)
  Name (_STA, 0x00)
}
//ISP i7_isp_fake2
Device (ISP2) {
  Name (_HID, "CIXH3022")
  Name (_UID, 0x01)
  Name (_STA, 0x00)
}
//ISP i7_isp_fake3
Device (ISP3) {
  Name (_HID, "CIXH3022")
  Name (_UID, 0x02)
  Name (_STA, 0x00)
}

//ISP ispmem
Device (ISPM) {
  Name (_HID, "CIXH3025")
  Name (_UID, 0x00)

  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_ISP_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CCA, 0)
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "ahb-pmctrl-res-base", ISPMEM_AHB_PMCTRL_RES_BASE},
      Package () { "ahb-pmctrl-res-size", ISPMEM_AHB_PMCTRL_RES_SIZE},
      Package () { "ahb-rcsuisp0-res-base", ISPMEM_AHB_RCSUISP0_RES_BASE},
      Package () { "ahb-rcsuisp0-res-size", ISPMEM_AHB_RCSUISP0_RES_SIZE},
      Package () { "ahb-rcsuisp1-res-base", ISPMEM_AHB_RCSUISP1_RES_BASE},
      Package () { "ahb-rcsuisp1-res-size", ISPMEM_AHB_RCSUISP1_RES_SIZE},
      Package () { "qos-read-priority", 0xF},
      Package () { "qos-write-priority", 0xF},
    }
  })

  Name (CLKT, Package() {
      Package() {CLK_TREE_ISP_ACLK, "isp_aclk", \_SB.ISPM},
      Package() {CLK_TREE_ISP_SCLK, "isp_sclk", \_SB.ISPM},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0 ,  15, \_SB.ISPM, "isp_sreset"},
    Package() {\_SB.RST0 ,  16, \_SB.ISPM, "isp_areset"},
    Package() {\_SB.RST0 ,  17, \_SB.ISPM, "isp_hreset"},
    Package() {\_SB.RST0 ,  18, \_SB.ISPM, "isp_gdcreset"},
    Package() {\_SB.RST0 ,  120, \_SB.ISPM, "rcsu_reset"},
  })

  PowerResource(PRS0, 0, 0)
  {
    OperationRegion(OPR0,SystemMemory,ISP0_RCSU_PD_REG, 0x04)
    Field (OPR0, DWordAcc, NoLock, Preserve) {
      MSK0, 32,
    }
    Method(_STA, 0, Serialized)
    {
        Local0 = MSK0
        Local1 = MSK0
        Local0 = Local0 & PGFSM_REG_CTRL
        If (Local0 > 0)
        {
          Return (1)
        }
        Else
        {
          Return (0)
        }
    }
    Method(_ON, 0, Serialized)
    {
      Local0 = MSK0
      Local0 = Local0 | PGFSM_REG_CTRL | TIME_CYCLE_CNT
      MSK0 = Local0
      \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_ISP, ISP0_RCSU_BASE_REG, BIT0)
    }
    Method(_OFF, 0, Serialized)
    {
      Local0 = MSK0
      Local0 = Local0 & ~PGFSM_REG_CTRL
      MSK0 = Local0
    }
  }

  Name(_PR0, Package(1){PRS0})
  Name(_PR3, Package(1){PRS0})
}

//ISP vi hw
Device (VIHW) {
  Name (_HID, "CIXH3026")
  Name (_UID, 0x00)

  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_ISP_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_IRQ0_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_ERR_IRQ0_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_IRQ1_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_ERR_IRQ1_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_IRQ2_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_ERR_IRQ2_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_IRQ3_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_ERR_IRQ3_INTERRUPT_ID }
  })

  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "ahb-dphy0-base", 0x142A0000},
      Package () { "ahb-dphy0-size", 0x10000},
      Package () { "ahb-dphy1-base", 0x14300000},
      Package () { "ahb-dphy1-size", 0x10000},
      Package () { "ahb-csi0-base", 0x14280000},
      Package () { "ahb-csi0-size", 0x10000},
      Package () { "ahb-csi1-base", 0x14290000},
      Package () { "ahb-csi1-size", 0x10000},
      Package () { "ahb-csi2-base", 0x142E0000},
      Package () { "ahb-csi2-size", 0x10000},
      Package () { "ahb-csi3-base", 0x142F0000},
      Package () { "ahb-csi3-size", 0x10000},
      Package () { "ahb-csidma0-base", 0x142B0000},
      Package () { "ahb-csidma0-size", 0x10000},
      Package () { "ahb-csidma1-base", 0x142C0000},
      Package () { "ahb-csidma1-size", 0x10000},
      Package () { "ahb-csidma2-base", 0x14310000},
      Package () { "ahb-csidma2-size", 0x10000},
      Package () { "ahb-csidma3-base", 0x14320000},
      Package () { "ahb-csidma3-size", 0x10000},
      Package () { "ahb-csircsu0-base", 0x14270000},
      Package () { "ahb-csircsu0-size", 0x10000},
      Package () { "ahb-csircsu1-base", 0x142D0000},
      Package () { "ahb-csircsu1-size", 0x10000},
    }
  })

  Name (CLKT, Package() {
      Package() {CLK_TREE_CSI_PHY0_PSM, "phy0_psmclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_PHY1_PSM, "phy1_psmclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_PHY0_APBCLK, "phy0_apbclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_PHY1_APBCLK, "phy1_apbclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL0_PCLK, "csi0_pclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL1_PCLK, "csi1_pclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL2_PCLK, "csi2_pclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL3_PCLK, "csi3_pclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL0_SYSCLK, "csi0_sclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL1_SYSCLK, "csi1_sclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL2_SYSCLK, "csi2_sclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL3_SYSCLK, "csi3_sclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL0_PIXEL0_CLK, "csi0_p0clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL0_PIXEL1_CLK, "csi0_p1clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL0_PIXEL2_CLK, "csi0_p2clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL0_PIXEL3_CLK, "csi0_p3clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL1_PIXEL0_CLK, "csi1_p0clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL2_PIXEL0_CLK, "csi2_p0clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL2_PIXEL1_CLK, "csi2_p1clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL2_PIXEL2_CLK, "csi2_p2clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL2_PIXEL3_CLK, "csi2_p3clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_CTRL3_PIXEL0_CLK, "csi3_p0clk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_DMA0_PCLK, "dma0_pclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_DMA1_PCLK, "dma1_pclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_DMA2_PCLK, "dma2_pclk", \_SB.VIHW},
      Package() {CLK_TREE_CSI_DMA3_PCLK, "dma3_pclk", \_SB.VIHW},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0 ,  32, \_SB.VIHW, "phy0_prst"},
    Package() {\_SB.RST0 ,  33, \_SB.VIHW, "phy0_cmnrst"},
    Package() {\_SB.RST0 ,  35, \_SB.VIHW, "phy1_prst"},
    Package() {\_SB.RST0 ,  36, \_SB.VIHW, "phy1_cmnrst"},
    Package() {\_SB.RST0 ,  93, \_SB.VIHW, "rcsu0_reset"},
    Package() {\_SB.RST0 ,  94, \_SB.VIHW, "rcsu1_reset"},
    Package() {\_SB.RST0 ,  34, \_SB.VIHW, "csi0_reset"},
    Package() {\_SB.RST0 ,  37, \_SB.VIHW, "csi1_reset"},
    Package() {\_SB.RST0 ,  38, \_SB.VIHW, "csi2_reset"},
    Package() {\_SB.RST0 ,  39, \_SB.VIHW, "csi3_reset"},
    Package() {\_SB.RST0 ,  40, \_SB.VIHW, "csibridge0_reset"},
    Package() {\_SB.RST0 ,  41, \_SB.VIHW, "csibridge1_reset"},
    Package() {\_SB.RST0 ,  42, \_SB.VIHW, "csibridge2_reset"},
    Package() {\_SB.RST0 ,  43, \_SB.VIHW, "csibridge3_reset"},
  })
}