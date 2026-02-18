/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#if FixedPcdGetBool(PcdAcpiCsiDmaEnable)

External (\_SB.I2C0.UXC0, DeviceObj)
External (\_SB.I2C0.UXC1, DeviceObj)
External (\_SB.I2C0.UXC2, DeviceObj)
External (\_SB.I2C0.UXC3, DeviceObj)

#define COMMON_INTERFACE Package () {0, 0}
#define CSI_BRIDGE_PORT_INIT(Interface, AxiUid, CsiDmaId, CixHwRef, RemoteDeviceReference, RemotePort, RemoteEndPoint) \
  Name (_DSD, Package () {\
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
    Package () {\
      Package () { "interface", Interface },\
      Package () { "axi-uid", AxiUid },\
      Package () { "csi-dma-id", CsiDmaId },\
      Package () { "cix,hw", CixHwRef },\
    },\
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
    Package () {\
        Package () { "port@0", "PRT0" },\
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
                Package () { "remote-endpoint", Package() { RemoteDeviceReference, RemotePort, RemoteEndPoint } },\
            }\
  })

#define MIPI_CSI_PORT_INIT(Lanes, MipiCsiId, RemoteDeviceReference1,RemotePort1,RemoteEndPoint1,RemoteDeviceReference2,RemotePort2,RemoteEndPoint2) \
     Name (_DSD, Package () { \
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "lanes", Lanes }, \
                Package () { "cix-csi", MipiCsiId }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "port@0", "PRT0" },\
                Package () { "port@1", "PRT1" },\
            }\
        })\
        Name (PRT0, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "endpoint@1", "EP00" },\
            }\
        })\
        Name (EP00, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 1 }, \
                Package () { "remote-endpoint", Package () { RemoteDeviceReference1, RemotePort1, RemoteEndPoint1 } },\
            }\
        })\
        Name (PRT1, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 1 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "endpoint@0", "EP10" },\
            }\
        })\
        Name (EP10, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
                Package () { "remote-endpoint", Package () {RemoteDeviceReference2, RemotePort2, RemoteEndPoint2 } },\
            }\
        })

#define MIPI_DPHY_PORT_INIT(DphyId, DphyHwRefence, RemoteDeviceReference1,RemotePort1,RemoteEndPoint1,RemoteDeviceReference2,RemotePort2,RemoteEndPoint2) \
     Name (_DSD, Package () { \
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "cix-dphy", DphyId }, \
                Package () { "cix,hw", DphyHwRefence }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "port@0", "PRT0" },\
                Package () { "port@1", "PRT1" },\
            }\
        })\
        Name (PRT0, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "endpoint@1", "EP00" },\
            }\
        })\
        Name (EP00, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 1 }, \
                Package () { "remote-endpoint", Package () { RemoteDeviceReference1, RemotePort1, RemoteEndPoint1 } },\
                Package () { "data-lanes", Package () { 1, 2, 3, 4 } },\
                Package () { "clock-lanes", Package () { 0 } },\
            }\
        })\
        Name (PRT1, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 1 }, \
            },\
            ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),\
            Package () {\
                Package () { "endpoint@0", "EP10" },\
            }\
        })\
        Name (EP10, Package() {\
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),\
            Package () {\
                Package () { "reg", 0 }, \
                Package () { "remote-endpoint", Package () {RemoteDeviceReference2, RemotePort2, RemoteEndPoint2 } },\
            }\
        })

//cix_csi_rcsu_0
Device (CSI0) {
  Name (_HID, "CIXH3027")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, CSI_RCSU0_CONTROLLER_BASE, CSI_RCSU0_CONTROLLER_SIZE)
  })
}

Device (CSI1) {
  Name (_HID, "CIXH3027")
  Name (_UID, 0x1)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, CSI_RCSU1_CONTROLLER_BASE, CSI_RCSU1_CONTROLLER_SIZE)
  })
}

//cix,cix-bridge0
Device (CBD0) {
  Name (_HID, "CIXH3028")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, CSI_BRIDGE0_CONTROLLER_BASE, CSI_BRIDGE0_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_CSI_BRIDGE_ERR_IRQ0_INTERRUPT_ID }
  })
  CSI_BRIDGE_PORT_INIT(COMMON_INTERFACE, 0x21, 0x00, \_SB.CSI0, \_SB.MPC0, "port@1", "endpoint@0")

  Name (CLKT, Package(){
    Package() {CLK_TREE_CSI_DMA0_PCLK, "dma_pclk", \_SB.CBD0},
    Package() {CLK_TREE_ISP_SCLK, "dma_sclk", \_SB.CBD0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSIBRDGE0_RST_N, \_SB.CBD0, "csibridge_reset"},
  })
}

//cix,cix-bridge1
Device (CBD1) {
  Name (_HID, "CIXH3028")
  Name (_UID, 0x1)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, CSI_BRIDGE1_CONTROLLER_BASE, CSI_BRIDGE1_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_CSI_BRIDGE_ERR_IRQ1_INTERRUPT_ID }
  })
  CSI_BRIDGE_PORT_INIT(COMMON_INTERFACE, 0x21, 0x01, \_SB.CSI0, \_SB.MPC1, "port@1", "endpoint@0")

  Name (CLKT, Package(){
    Package() {CLK_TREE_CSI_DMA1_PCLK, "dma_pclk", \_SB.CBD1},
    Package() {CLK_TREE_ISP_SCLK, "dma_sclk", \_SB.CBD1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSIBRDGE1_RST_N, \_SB.CBD1, "csibridge_reset"},
  })
}

//cix,cix-bridge2
Device (CBD2) {
  Name (_HID, "CIXH3028")
  Name (_UID, 0x2)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, CSI_BRIDGE2_CONTROLLER_BASE, CSI_BRIDGE2_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_CSI_BRIDGE_ERR_IRQ2_INTERRUPT_ID }
  })
  CSI_BRIDGE_PORT_INIT(COMMON_INTERFACE, 0x22, 0x02, \_SB.CSI1, \_SB.MPC2, "port@1", "endpoint@0")

  Name (CLKT, Package(){
    Package() {CLK_TREE_CSI_DMA2_PCLK, "dma_pclk", \_SB.CBD2},
    Package() {CLK_TREE_ISP_SCLK, "dma_sclk", \_SB.CBD2},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSIBRDGE2_RST_N, \_SB.CBD2, "csibridge_reset"},
  })
}

//cix,cix-bridge3
Device (CBD3) {
  Name (_HID, "CIXH3028")
  Name (_UID, 0x3)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, CSI_BRIDGE3_CONTROLLER_BASE, CSI_BRIDGE3_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_CSI_BRIDGE_ERR_IRQ3_INTERRUPT_ID }
  })
  CSI_BRIDGE_PORT_INIT(COMMON_INTERFACE, 0x21, 0x03, \_SB.CSI1, \_SB.MPC3, "port@1", "endpoint@0")

  Name (CLKT, Package(){
    Package() {CLK_TREE_CSI_DMA3_PCLK, "dma_pclk", \_SB.CBD3},
    Package() {CLK_TREE_ISP_SCLK, "dma_sclk", \_SB.CBD3},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSIBRDGE3_RST_N, \_SB.CBD3, "csibridge_reset"},
  })
}

//cix,cix-mipi-csi2-0
Device (MPC0) {
  Name (_HID, "CIXH3029")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, MIPI_CSI0_CONTROLLER_BASE, MIPI_CSI0_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_IRQ0_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_ERR_IRQ0_INTERRUPT_ID }
  })
  MIPI_CSI_PORT_INIT(4, 0, \_SB.DPR1, "port@1", "endpoint@0", \_SB.CBD0, "port@0", "endpoint@0")

  Name (CLKT, Package() {
    Package() {CLK_TREE_CSI_CTRL0_PCLK, "csi_pclk", \_SB.MPC0},
    Package() {CLK_TREE_CSI_CTRL0_SYSCLK, "csi_sclk", \_SB.MPC0},
    Package() {CLK_TREE_CSI_CTRL0_PIXEL0_CLK, "csi_p0clk", \_SB.MPC0},
    Package() {CLK_TREE_CSI_CTRL0_PIXEL1_CLK, "csi_p1clk", \_SB.MPC0},
    Package() {CLK_TREE_CSI_CTRL0_PIXEL2_CLK, "csi_p2clk", \_SB.MPC0},
    Package() {CLK_TREE_CSI_CTRL0_PIXEL3_CLK, "csi_p3clk", \_SB.MPC0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSI_RCSU0_RESET_N, \_SB.MPC0, "rcsu_reset"},
    Package() {\_SB.RST0, SKY1_CSI0_RST_N, \_SB.MPC0, "csi_reset"},
  })
}
//cix,cix-mipi-csi2-1
Device (MPC1) {
  Name (_HID, "CIXH3029")
  Name (_UID, 0x1)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, MIPI_CSI1_CONTROLLER_BASE, MIPI_CSI1_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_IRQ1_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_ERR_IRQ1_INTERRUPT_ID }
  })
  MIPI_CSI_PORT_INIT(4, 1, \_SB.DPR2, "port@1", "endpoint@0", \_SB.CBD1, "port@0", "endpoint@0")

  Name (CLKT, Package() {
    Package() {CLK_TREE_CSI_CTRL1_PCLK, "csi_pclk", \_SB.MPC1},
    Package() {CLK_TREE_CSI_CTRL1_SYSCLK, "csi_sclk", \_SB.MPC1},
    Package() {CLK_TREE_CSI_CTRL1_PIXEL0_CLK, "csi_p0clk", \_SB.MPC1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSI1_RST_N, \_SB.MPC1, "csi_reset"},
  })
}
//cix,cix-mipi-csi2-2
Device (MPC2) {
  Name (_HID, "CIXH3029")
  Name (_UID, 0x2)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, MIPI_CSI2_CONTROLLER_BASE, MIPI_CSI2_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_IRQ2_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_ERR_IRQ2_INTERRUPT_ID }
  })
  MIPI_CSI_PORT_INIT(4, 2, \_SB.DPR4, "port@1", "endpoint@0", \_SB.CBD2, "port@0", "endpoint@0")

  Name (CLKT, Package() {
      Package() {CLK_TREE_CSI_CTRL2_PCLK, "csi_pclk", \_SB.MPC2},
      Package() {CLK_TREE_CSI_CTRL2_SYSCLK, "csi_sclk", \_SB.MPC2},
      Package() {CLK_TREE_CSI_CTRL2_PIXEL0_CLK, "csi_p0clk", \_SB.MPC2},
      Package() {CLK_TREE_CSI_CTRL2_PIXEL1_CLK, "csi_p1clk", \_SB.MPC2},
      Package() {CLK_TREE_CSI_CTRL2_PIXEL2_CLK, "csi_p2clk", \_SB.MPC2},
      Package() {CLK_TREE_CSI_CTRL2_PIXEL3_CLK, "csi_p3clk", \_SB.MPC2},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSI_RCSU1_RESET_N, \_SB.MPC2, "rcsu_reset"},
    Package() {\_SB.RST0, SKY1_CSI2_RST_N, \_SB.MPC2, "csi_reset"},
  })
}
//cix,cix-mipi-csi2-3
Device (MPC3) {
  Name (_HID, "CIXH3029")
  Name (_UID, 0x3)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, MIPI_CSI3_CONTROLLER_BASE, MIPI_CSI3_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_IRQ3_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { CSI_ERR_IRQ3_INTERRUPT_ID }
  })
  MIPI_CSI_PORT_INIT(4, 3, \_SB.DPR5, "port@1", "endpoint@0", \_SB.CBD3, "port@0", "endpoint@0")

  Name (CLKT, Package() {
    Package() {CLK_TREE_CSI_CTRL3_PCLK, "csi_pclk", \_SB.MPC3},
    Package() {CLK_TREE_CSI_CTRL3_SYSCLK, "csi_sclk", \_SB.MPC3},
    Package() {CLK_TREE_CSI_CTRL3_PIXEL0_CLK, "csi_p0clk", \_SB.MPC3},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSI3_RST_N, \_SB.MPC3, "csi_reset"},
  })
}

//cix,cix-mipi-dphy-hw
Device (DPH0) {
  Name (_HID, "CIXH302A")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, CSI_DPHY_HW0_CONTROLLER_BASE, CSI_DPHY_HW0_CONTROLLER_SIZE)
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "cix-dphy-hw", 0},
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_CSI_PHY0_PSM, "phy_psmclk", \_SB.DPH0},
    Package() {CLK_TREE_CSI_PHY0_APBCLK, "phy_apbclk", \_SB.DPH0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSIDPHY_PRST0_N, \_SB.DPH0, "phy_prst"},
    Package() {\_SB.RST0, SKY1_CSIDPHY_CMNRST0_N, \_SB.DPH0, "phy_cmnrst"},
  })
}

Device (DPH1) {
  Name (_HID, "CIXH302A")
  Name (_UID, 0x1)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, CSI_DPHY_HW1_CONTROLLER_BASE, CSI_DPHY_HW1_CONTROLLER_SIZE)
  })
  Name (_DSD, Package () {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "cix-dphy-hw", 1},
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_CSI_PHY1_PSM, "phy_psmclk", \_SB.DPH1},
    Package() {CLK_TREE_CSI_PHY1_APBCLK, "phy_apbclk", \_SB.DPH1},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_CSIDPHY_PRST1_N, \_SB.DPH1, "phy_prst"},
    Package() {\_SB.RST0, SKY1_CSIDPHY_CMNRST1_N, \_SB.DPH1, "phy_cmnrst"},
  })
}

//cix,cix-mipi-dphy-rx
Device (DPR0) {
  Name (_HID, "CIXH302B")
  Name (_UID, 0x0)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  MIPI_DPHY_PORT_INIT(0, \_SB.DPH0, \_SB.I2C0.UXC0, "port@0", "endpoint@0", \_SB.MPC0, "port@0", "endpoint@1")
}

Device (DPR1) {
  Name (_HID, "CIXH302B")
  Name (_UID, 0x1)
  Name (_STA, 0xF)
  Name (_CCA, 0)
  MIPI_DPHY_PORT_INIT(1, \_SB.DPH0, \_SB.I2C0.UXC0, "port@0", "endpoint@0", \_SB.MPC0, "port@0", "endpoint@1")
}

Device (DPR2) {
  Name (_HID, "CIXH302B")
  Name (_UID, 0x2)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  MIPI_DPHY_PORT_INIT(2, \_SB.DPH0, \_SB.I2C0.UXC1, "port@0", "endpoint@0", \_SB.MPC1, "port@0", "endpoint@1")
}

Device (DPR3) {
  Name (_HID, "CIXH302B")
  Name (_UID, 0x3)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  MIPI_DPHY_PORT_INIT(3, \_SB.DPH1, \_SB.I2C0.UXC0, "port@0", "endpoint@0", \_SB.MPC2, "port@0", "endpoint@1")
}

Device (DPR4) {
  Name (_HID, "CIXH302B")
  Name (_UID, 0x4)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  MIPI_DPHY_PORT_INIT(4, \_SB.DPH1, \_SB.I2C0.UXC2, "port@0", "endpoint@0", \_SB.MPC2, "port@0", "endpoint@1")
}

Device (DPR5) {
  Name (_HID, "CIXH302B")
  Name (_UID, 0x5)
  Name (_STA, 0x0)
  Name (_CCA, 0)
  MIPI_DPHY_PORT_INIT(5, \_SB.DPH1, \_SB.I2C0.UXC3, "port@0", "endpoint@0", \_SB.MPC3, "port@0", "endpoint@1")
}

#endif