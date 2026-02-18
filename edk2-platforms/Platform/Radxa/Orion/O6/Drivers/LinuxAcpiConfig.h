#ifndef __LINUX_ACPI_CONFIG_H__
#define __LINUX_ACPI_CONFIG_H__

#define USBC_PHY_MODE_USB    0x1
#define USBC_PHY_MODE_DP     0x2
#define USBC_PHY_MODE_COMBO  0x3 // USB and DP

/* Full function */
#define USBC0_PD_EN      1
#define USBC0_PD_DEVICE  \_SB.I2C1.PD10
#define USBC0_DEF_PMODE  USBC_PHY_MODE_COMBO
#define USBC0_DISABLE_USB3   0

/**
 * Without PD controller.
 * Work as usb host in high speed mode(USB2.0).
 * Set Phy mode to Display only.
 */
#define USBC1_PD_EN      0
#define USBC1_DISABLE_USB3   1
#define USBC1_DEF_PMODE  USBC_PHY_MODE_DP

/* Full function */
#define USBC2_PD_EN      1
#define USBC2_PD_DEVICE  \_SB.I2C1.PD11
#define USBC2_DEF_PMODE  USBC_PHY_MODE_COMBO
#define USBC2_DISABLE_USB3   0

/**
 * Without PD controller.
 * Work as usb host in high speed mode(USB2.0).
 * Set Phy mode to Display only.
 */
#define USBC3_PD_EN      0
#define USBC3_DISABLE_USB3   1
#define USBC3_DEF_PMODE  USBC_PHY_MODE_DP

// PCIe cfg for PERST and EP Vbat gpio
#define PERST_GPIO_ACTIVE_LEVEL GPIO_ACTIVE_HIGH

#define PCIE_X8_PERST           1
#define PCIE_X8_PERST_GPIO_CTR  "\\_SB.GPI4"
#define PCIE_X8_PERST_GPIO      1

#define PCIE_X4_PERST           1
#define PCIE_X4_PERST_GPIO_CTR  "\\_SB.GPI4"
#define PCIE_X4_PERST_GPIO      3

#define PCIE_X2_PERST           1
#define PCIE_X2_PERST_GPIO_CTR  "\\_SB.GPI4"
#define PCIE_X2_PERST_GPIO      4

#define PCIE_X1_1_PERST           1
#define PCIE_X1_1_PERST_GPIO_CTR  "\\_SB.GPI4"
#define PCIE_X1_1_PERST_GPIO      2

#define PCIE_X1_0_PERST           1
#define PCIE_X1_0_PERST_GPIO_CTR  "\\_SB.GPI4"
#define PCIE_X1_0_PERST_GPIO      5

#define PEWAKE_GPIO_ACTIVE_LEVEL GPIO_ACTIVE_HIGH
#define PCIE_X1_0_STR_PWRON       1
#define PCIE_X1_0_PEWAKE          1
#define PCIE_X1_0_PEWAKE_GPIO_CTR  "\\_SB.GPI4"
#define PCIE_X1_0_PEWAKE_GPIO     19

#define PCIE_X1_1_STR_PWRON       1
#define PCIE_X1_1_PEWAKE          1
#define PCIE_X1_1_PEWAKE_GPIO_CTR  "\\_SB.GPI4"
#define PCIE_X1_1_PEWAKE_GPIO     22

#define PCIE_X8_VCC_SUPPLY 1
#define PCIE_X8_VCC_REGULATOR \_SB.PVC0
#define PCIE_X4_VCC_SUPPLY 1
#define PCIE_X4_VCC_REGULATOR \_SB.PVC1
#define PCIE_X2_VCC_SUPPLY 1
#define PCIE_X2_VCC_REGULATOR \_SB.PVC2
#define PCIE_X1_1_VCC_SUPPLY 1
#define PCIE_X1_1_VCC_REGULATOR \_SB.PVC3
#define PCIE_X1_0_VCC_SUPPLY 1
#define PCIE_X1_0_VCC_REGULATOR \_SB.PVC4

#define GPI0_LINE_NAMES \_SB.GPI0.GPIN
#define GPI1_LINE_NAMES \_SB.GPI1.GPIN
#define GPI2_LINE_NAMES \_SB.GPI2.GPIN
#define GPI3_LINE_NAMES \_SB.GPI3.GPIN
#define GPI4_LINE_NAMES \_SB.GPI4.GPIN
#define GPI5_LINE_NAMES \_SB.GPI5.GPIN

/* HDA */
#define HDA_EXT_CRS \
    GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPI3", 0, ResourceConsumer) { 5 }

#define HDA_EXT_DSD_PROPERTY \
      Package () { "cix,model", "CIX SKY1 ORION O6 HDA" }, \
      Package () { "pdb-gpios", Package () { ^HDA, 0, 0, 0 } }

#endif
