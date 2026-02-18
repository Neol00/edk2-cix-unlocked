/** @file
  SocInitLib.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SOC_INIT_LIB_H__
#define __SOC_INIT_LIB_H__

#include <Uefi.h>

#define SOC_CONFIG_REG_BASE_ADDRESS   0x05040100u
#define SOC_FCH_CRU_REG_BASE_ADDRESS  0x04160000u
#define PMCTRL_S5_BASE_ADDRESS        0x16000000u

#define DEBUG_REG_RTL_Changelist_OFFSET  0x14

#define BIT(nr)  ((1) << (nr))
#define HIGH  1
#define LOW   0

typedef union {
  struct {
    UINT32    Frequency : 17;   // Range:1~131071KHz
    UINT32    Class     : 2;    // 0:Simulation 1:Emulator Virutal 2:Emulator ICE 3:FPGA
    UINT32    Reserved  : 13;   ///< Reserved, RES0
  } Bits;         ///< Bitfield definition of the register
  UINT32    Data; ///< The entire 32-bit value
} SOC_CFG_REG;

typedef union {
  struct {
    UINT32    FUSE         : 1; // 0:shell 1:RTL
    UINT32    FCH          : 1; // 0:shell 1:RTL
    UINT32    AUDIO        : 1; // 0:shell 1:RTL
    UINT32    SENSORFUSION : 1; // 0:shell 1:RTL
    UINT32    PCIEHUB      : 1; // 0:shell 1:RTL
    UINT32    SYSHUB       : 1; // 0:shell 1:RTL
    UINT32    MMHUB        : 1; // 0:shell 1:RTL
    UINT32    ISP          : 1; // 0:shell 1:RTL
    UINT32    VPU          : 1; // 0:shell 1:RTL
    UINT32    GMAC         : 2; // 0:shell 1:1GMAC 2:2GMAC
    UINT32    USB          : 2; // 0:shell 1:USB-C_0(FPGA, ICE) 2:USB-C_0,USB-C_1,USB3_1,USB2_0(Virtual)
    UINT32    PCIE         : 3; // 0:shell 1:x8 2:x4 3:x2 4:x1 5:x8+x2
    UINT32    CSI          : 2; // 0:shell 1:1CSI 2:2CSI 3:4CSI
    UINT32    NPU          : 2; // 0:shell 1:1core 2:3core
    UINT32    DP           : 2; // 0:shell 1:1DP 2:2DP 3:5DP
    UINT32    DPU          : 2; // 0:shell 1:1DPU 2:2DPU 3:5DPU
    UINT32    DDR          : 1; // 0:4Channels 1:1Channel
    UINT32    DDRMODEL     : 1; // 0: ddr controler 1: ddr axi model
    UINT32    GPU          : 2; // 0:shell 1:1core 2:2core 3:10core
    UINT32    CLUSTER      : 3; // 0:shell 1:Hunter0 only 2:Hayes0 only 3:Hunter0+Hayes0 4:Hayes4 5:12Core
    UINT32    Reserved     : 1; ///< Reserved, RES0
  } Bits;         ///< Bitfield definition of the register
  UINT32    Data; ///< The entire 32-bit value
} SOC_COMPONENT_REG;

typedef enum {
  VPU,
  GMAC,
  USB,
  PCIE,
  NPU,
  DPU,
  GPU,
  CLUSTER,
  MMHUB,
  CSI,
  AUDIO
} SOC_COMPONENT_TYPE;

typedef enum {
  FREQ,
  CLASS
} SOC_CFG_TYPE;

UINT32
GetSocConfig (
  SOC_CFG_TYPE  Type
  );

UINT32
GetRtlVersion (
  );

UINT32
GetSocComponentStatus (
  SOC_COMPONENT_TYPE  Type
  );

UINT32
GetSocUartBaseAddress (
  VOID
  );

VOID
SetSocFchIpClockAndReset (
  VOID
  );

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------PM Control Config Space------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------*/
typedef union {
  struct {
    UINT32    b00 : 1;
    UINT32    b01 : 1;
    UINT32    b02 : 1;
    UINT32    b03 : 1;
    UINT32    b04 : 1;
    UINT32    b05 : 1;
    UINT32    b06 : 1;
    UINT32    b07 : 1;
    UINT32    b08 : 1;
    UINT32    b09 : 1;
    UINT32    b10 : 1;
    UINT32    b11 : 1;
    UINT32    b12 : 1;
    UINT32    b13 : 1;
    UINT32    b14 : 1;
    UINT32    b15 : 1;
    UINT32    b16 : 1;
    UINT32    b17 : 1;
    UINT32    b18 : 1;
    UINT32    b19 : 1;
    UINT32    b20 : 1;
    UINT32    b21 : 1;
    UINT32    b22 : 1;
    UINT32    b23 : 1;
    UINT32    b24 : 1;
    UINT32    b25 : 1;
    UINT32    b26 : 1;
    UINT32    b27 : 1;
    UINT32    b28 : 1;
    UINT32    b29 : 1;
    UINT32    b30 : 1;
    UINT32    b31 : 1;
  } Bits;         ///< Bitfield definition of the register
  UINT32    Data; ///< The entire 32-bit value
} BIT_FIELD;

typedef struct PMCTRL_S5 {
  UINT32       pll_lock;
  UINT32       pll_ctrl;
  UINT32       enable_sensorhub_400m_clk;
  UINT32       enable_csu_dfd_800m_clk;
  UINT32       enable_csu_se_800m_clk;
  UINT32       enable_csu_pm_800m_clk;
  UINT32       enable_tsegn_100m_clk;
  UINT32       enable_sensorhub_24m_clk;
  UINT32       enable_usbphy_x4_24m_clk;
  UINT32       enable_usbphy_x2_24m_clk;
  UINT32       enable_usbphy_x1_24m_clk;
  UINT32       enable_usbc_ss_8m_clk_sof;
  UINT32       enable_usbc_hs_8m_clk_sof;
  UINT32       enable_usbc_ss_32k_clk_lpm;
  UINT32       enable_usbc_hs_32k_clk_lpm;
  UINT32       reserved0[49];
  UINT32       slp_type;
  UINT32       slp_cnt_value;
  UINT32       reserved1[62];
  UINT32       reset_delay_timer;
  UINT32       pwr_ok_delay_timer; // power ok delay timer
  UINT32       wdt_tm_se_en;       // watchdog se timeout enable
  UINT32       wakeup_enable;
  UINT32       wakeup_source;
  UINT32       wakeup_source_usb;
  UINT32       pwr_reason;
  UINT32       csu_se_intr;
  UINT32       csu_pm_intr;
  UINT32       pm_intr;
  UINT32       csu_se_intr_en;
  UINT32       csu_pm_intr_en;
  UINT32       pm_intr_en;
  UINT32       reserved2[51];
  UINT32       warm_rst;
  UINT32       csu_pm_reset;
  UINT32       sensorhub_reset;
  UINT32       sensorhub_noc_reset;
  UINT32       reserved3[60];
  BIT_FIELD    s0_reset_group0;     // s0_reset_group0
  BIT_FIELD    s0_reset_group1;     // s0_reset_group1
  BIT_FIELD    usbphy_reset_group0; // usbphy_reset_group0
  BIT_FIELD    usbphy_reset_group1; // usbphy_reset_group1
  UINT32       reserved4[60];
  UINT32       software_use0;
  UINT32       software_use1;
  UINT32       software_use2;
  UINT32       software_use3;
  UINT32       software_use4;
  UINT32       software_use5;
  UINT32       software_use6;
  UINT32       software_use7;
  UINT32       reserved5[56];
  UINT32       tsgen_clk_sel;
  UINT32       wdt_tm_option;
  UINT32       reserved6[62];
  UINT32       ddr_retention_ctrl;
  UINT32       pm_runstall;
  UINT32       reserved7[62];
  BIT_FIELD    rcsu_reset_group0;
  BIT_FIELD    rcsu_reset_group1;
} volatile *PMCTRL_S5_PTR;

#define PMCTRL_S5_BASE_ADDRESS_PTR  ((PMCTRL_S5_PTR)PMCTRL_S5_BASE_ADDRESS)

#define PLL_CTRL(val)                    (PMCTRL_S5_BASE_ADDRESS_PTR)->pll_ctrl = (val)
#define ENABLE_SENSOR_HUB_400M_CLK(val)  (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_sensorhub_400m_clk = (val)
#define ENABLE_CSU_DFD_800M_CLK(val)     (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_csu_dfd_800m_clk = (val)
#define ENABLE_CSU_SE_800M_CLK(val)      (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_csu_se_800m_clk = (val)
#define ENABLE_CSU_PM_800M_CLK(val)      (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_csu_pm_800m_clk = (val)
#define ENABLE_TSEGN_100M_CLK(val)       (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_tsegn_100m_clk = (val)
#define ENABLE_SENSOR_HUB_24M_CLK(val)   (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_sensorhub_24m_clk = (val)
#define ENABLE_USB_PHY_X4_24M_CLK(val)   (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_usbphy_x4_24m_clk = (val)
#define ENABLE_USB_PHY_X2_24M_CLK(val)   (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_usbphy_x2_24m_clk = (val)
#define ENABLE_USB_PHY_X1_24M_CLK(val)   (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_usbphy_x1_24m_clk = (val)
#define ENABLE_USBC_SS_8M_CLK_SOF(val)   (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_usbc_ss_8m_clk_sof = (val)
#define ENABLE_USBC_HS_8M_CLK_SOF(val)   (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_usbc_hs_8m_clk_sof = (val)
#define ENABLE_USBC_SS_32K_CLK_LPM(val)  (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_usbc_ss_32k_clk_lpm = (val)
#define ENABLE_USBC_HS_32K_CLK_LPM(val)  (PMCTRL_S5_BASE_ADDRESS_PTR)->enable_usbc_hs_32k_clk_lpm = (val)
#define SLP_TYPE(val)                    (PMCTRL_S5_BASE_ADDRESS_PTR)->slp_type = (val)
#define SLP_CNT_VALUE(val)               (PMCTRL_S5_BASE_ADDRESS_PTR)->slp_cnt_value = (val)
#define RESET_DELAY_TIMER(val)           (PMCTRL_S5_BASE_ADDRESS_PTR)->reset_delay_timer = (val)
#define POWER_OK_DELAY_TIMER(val)        (PMCTRL_S5_BASE_ADDRESS_PTR)->pwr_ok_delay_timer = (val)
#define WDT_SE_TIMEOUT_ENABLE(val)       (PMCTRL_S5_BASE_ADDRESS_PTR)->wdt_tm_se_en = (val)
#define WAKEUP_ENABLE(val)               (PMCTRL_S5_BASE_ADDRESS_PTR)->wakeup_enable = (val)
#define CSU_SE_INTR(val)                 (PMCTRL_S5_BASE_ADDRESS_PTR)->csu_se_intr = (val)
#define CSU_PM_INTR(val)                 (PMCTRL_S5_BASE_ADDRESS_PTR)->csu_pm_intr = (val)
#define PM_INTR(val)                     (PMCTRL_S5_BASE_ADDRESS_PTR)->pm_intr = (val)
#define CSU_SE_INTR_ENABLE(val)          (PMCTRL_S5_BASE_ADDRESS_PTR)->csu_se_intr_en = (val)
#define CSU_PM_INTR_ENABLE(val)          (PMCTRL_S5_BASE_ADDRESS_PTR)->csu_pm_intr_en = (val)
#define PM_INTR_ENABLE(val)              (PMCTRL_S5_BASE_ADDRESS_PTR)->pm_intr_en = (val)
#define WARM_RESET(val)                  (PMCTRL_S5_BASE_ADDRESS_PTR)->warm_rst = (val)
#define CSU_PM_RESET(val)                (PMCTRL_S5_BASE_ADDRESS_PTR)->csu_pm_reset = (val)
#define SENSOR_HUB_RESET(val)            (PMCTRL_S5_BASE_ADDRESS_PTR)->sensorhub_reset = (val)
#define SENSOR_HUB_NOC_RESET(val)        (PMCTRL_S5_BASE_ADDRESS_PTR)->sensorhub_noc_reset = (val)
#define TSGEN_CLK_SEL(val)               (PMCTRL_S5_BASE_ADDRESS_PTR)->tsgen_clk_sel = (val)
#define WDT_TIMER_OPTION(val)            (PMCTRL_S5_BASE_ADDRESS_PTR)->wdt_tm_option = (val)
#define DDRC_RETENTION_CTRL(val)         (PMCTRL_S5_BASE_ADDRESS_PTR)->ddr_retention_ctrl = (val)       // Write only

// Reserve software use 0x500~0x51C

// read only register
#define GET_PLL_LOCK()            (PMCTRL_S5_BASE_ADDRESS_PTR)->pll_lock
#define GET_WAKEUP_SOURCE()       (PMCTRL_S5_BASE_ADDRESS_PTR)->wakeup_source
#define GET_USB_WAKEUP_SOURCE()   (PMCTRL_S5_BASE_ADDRESS_PTR)->wakeup_source_usb
#define GET_POWER_RESET_REASON()  (PMCTRL_S5_BASE_ADDRESS_PTR)->pwr_reason

// s0 Reset Group0
#define DDRC_RESET_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b00 =(x)
#define GIC_RESET_N(x)         (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b01 =(x)
#define CI700_RESET_N(x)       (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b02 =(x)
#define SYS_NI700_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b03 =(x)
#define MM_NI700_RESET_N(x)    (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b04 =(x)
#define PCIE_NI700_RESET_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b05 =(x)
#define GPU_RESET_N(x)         (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b06 =(x)
#define NPU_TOP_RESET_N(x)     (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b07 =(x)
#define NPU_CORE0_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b08 =(x)
#define NPU_CORE1_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b09 =(x)
#define NPU_CORE2_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b10 =(x)
#define VPU_RESET_N(x)         (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b11 =(x)
#define ISP_S_RESET_N(x)       (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b12 =(x)
#define ISP_A_RESET_N(x)       (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b13 =(x)
#define ISP_H_RESET_N(x)       (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b14 =(x)
#define ISP_GDC_RESET_N(x)     (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b15 =(x)
#define DPU_RESET0_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b16 =(x)
#define DPU_RESET1_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b17 =(x)
#define DPU_RESET2_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b18 =(x)
#define DPU_RESET3_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b19 =(x)
#define DPU_RESET4_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b20 =(x)
#define DP0_ASYNC_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b21 =(x)
#define DP1_ASYNC_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b22 =(x)
#define DP2_ASYNC_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b23 =(x)
#define DP3_ASYNC_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b24 =(x)
#define DP4_ASYNC_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b25 =(x)
#define DP_PHY_RESET_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group0.Bits.b26 =(x)

// s0 Reset Group1
#define AUDIO_HIFI5_RESET_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b00 =(x)
#define AUDIO_HIFI5_NOC_RESET_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b01 =(x)
#define CSI_DPPHY_PRST0_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b02 =(x)
#define CSI_DPPHY_CMNRST0_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b03 =(x)
#define CSI0_RESET_N(x)             (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b04 =(x)
#define CSI_DPPHY_PRST1_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b05 =(x)
#define CSI_DPPHY_CMNRST1_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b06 =(x)
#define CSI1_RESET_N(x)             (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b07 =(x)
#define CSI2_RESET_N(x)             (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b08 =(x)
#define CSI3_RESET_N(x)             (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b09 =(x)
#define CSI_BRIDGE0_RESET_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b10 =(x)
#define CSI_BRIDGE1_RESET_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b11 =(x)
#define CSI_BRIDGE2_RESET_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b12 =(x)
#define CSI_BRIDGE3_RESET_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b13 =(x)
#define GMAC0_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b14 =(x)
#define GMAC1_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b15 =(x)
#define PCIE0_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b16 =(x)
#define PCIE1_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b17 =(x)
#define PCIE2_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b18 =(x)
#define PCIE3_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b19 =(x)
#define PCIE4_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->s0_reset_group1.Bits.b20 =(x)

// usb phy reset group0
#define USB_DP_PHY0_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b00 =(x)
#define USB_DP_PHY1_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b01 =(x)
#define USB_DP_PHY2_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b02 =(x)
#define USB_DP_PHY3_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b03 =(x)
#define USB_DP_PHY0_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b04 =(x)
#define USB_DP_PHY1_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b05 =(x)
#define USB_DP_PHY2_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b06 =(x)
#define USB_DP_PHY3_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b07 =(x)
#define USB_PHY_SS_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b08 =(x)
#define USB_PHY_SS_RST_N(x)    (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b09 =(x)
#define USBPHY_HS0_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b10 =(x)
#define USBPHY_HS1_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b11 =(x)
#define USBPHY_HS2_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b12 =(x)
#define USBPHY_HS3_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b13 =(x)
#define USBPHY_HS4_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b14 =(x)
#define USBPHY_HS5_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b15 =(x)
#define USBPHY_HS6_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b16 =(x)
#define USBPHY_HS7_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b17 =(x)
#define USBPHY_HS8_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b18 =(x)
#define USBPHY_HS9_PRST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group0.Bits.b19 =(x)

// usb phy reset group1
#define USBC_SS0_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b00 =(x)
#define USBC_SS1_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b01 =(x)
#define USBC_SS2_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b02 =(x)
#define USBC_SS3_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b03 =(x)
#define USBC_SS4_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b04 =(x)
#define USBC_SS5_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b05 =(x)
#define USBC_SS0_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b06 =(x)
#define USBC_SS1_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b07 =(x)
#define USBC_SS2_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b08 =(x)
#define USBC_SS3_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b09 =(x)
#define USBC_SS4_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b10 =(x)
#define USBC_SS5_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b11 =(x)
#define USBC_HS0_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b12 =(x)
#define USBC_HS1_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b13 =(x)
#define USBC_HS2_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b14 =(x)
#define USBC_HS3_PRST_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b15 =(x)
#define USBC_HS0_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b16 =(x)
#define USBC_HS1_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b17 =(x)
#define USBC_HS2_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b18 =(x)
#define USBC_HS3_RST_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->usbphy_reset_group1.Bits.b19 =(x)

// rcsu reset group0
#define AUDIO_RCSU_RESET_N(x)          (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b00 =(x)
#define CI700_RCSU_RESET_N(x)          (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b01 =(x)
#define CSI_RCSU_0_RESET_N(x)          (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b02 =(x)
#define CSI_RCSU_1_RESET_N(x)          (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b03 =(x)
#define CSU_PM_RCSU_RESET_N(x)         (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b04 =(x)
#define CSU_SE_RCSU_RESET_N(x)         (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b05 =(x)
#define DDR_BROADCAST_RCSU_RESET_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b06 =(x)
#define DDR_CTRL_RCSU_0_RESET_N(x)     (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b07 =(x)
#define DDR_CTRL_RCSU_1_RESET_N(x)     (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b08 =(x)
#define DDR_CTRL_RCSU_2_RESET_N(x)     (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b09 =(x)
#define DDR_CTRL_RCSU_3_RESET_N(x)     (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b10 =(x)
#define DDR_TZC400_RCSU_0_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b11 =(x)
#define DDR_TZC400_RCSU_1_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b12 =(x)
#define DDR_TZC400_RCSU_2_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b13 =(x)
#define DDR_TZC400_RCSU_3_RESET_N(x)   (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b14 =(x)
#define DP0_RCSU_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b15 =(x)
#define DP1_RCSU_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b16 =(x)
#define DP2_RCSU_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b17 =(x)
#define DP3_RCSU_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b18 =(x)
#define DP4_RCSU_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b19 =(x)
#define DPU0_RCSU_RESET_N(x)           (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b20 =(x)
#define DPU1_RCSU_RESET_N(x)           (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b21 =(x)
#define DPU2_RCSU_RESET_N(x)           (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b22 =(x)
#define DPU3_RCSU_RESET_N(x)           (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b23 =(x)
#define DPU4_RCSU_RESET_N(x)           (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b24 =(x)
#define DSU_RCSU_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b25 =(x)
#define FCH_RCSU_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b26 =(x)
#define GICD_RCSU_RESET_N(x)           (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b27 =(x)
#define GMAC_RCSU_RESET_N(x)           (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b28 =(x)
#define GPU_RCSU_RESET_N(x)            (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b29 =(x)
#define ISP_RCSU_0_RESET_N(x)          (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b30 =(x)
#define ISP_RCSU_1_RESET_N(x)          (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group0.Bits.b31 =(x)

// rcsu reset group1
#define NI700_MMHUB_RCSU_RESET_N(x)       (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b00 =(x)
#define NPU_RCSU_RESET_N(x)               (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b01 =(x)
#define NI700_PCIE_RCSU_RESET_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b02 =(x)
#define PCIE_X421_RCSU_RESET_N(x)         (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b03 =(x)
#define PCIE_X8_RCSU_RESET_N(x)           (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b04 =(x)
#define SF_RCSU_RESET_N(x)                (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b05 =(x)
#define SMMU_MMHUB_RCSU_RESET_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b06 =(x)
#define SMMU_PCIEHUB_RCSU_RESET_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b07 =(x)
#define SMMU_SYSHUB_RCSU_RESET_N(x)       (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b08 =(x)
#define NI700_SMN_RCSU_RESET_N(x)         (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b09 =(x)
#define NI700_SYSHUB_RCSU_RESET_N(x)      (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b10 =(x)
#define USB2_HOST0_RCSU_RESET_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b11 =(x)
#define USB2_HOST1_RCSU_RESET_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b12 =(x)
#define USB2_HOST2_RCSU_RESET_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b13 =(x)
#define USB2_HOST3_RCSU_RESET_N(x)        (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b14 =(x)
#define USB3_TYPEA_DRD_RCSU_RESET_N(x)    (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b15 =(x)
#define USB3_TYPEC_DRD_RCSU_RESET_N(x)    (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b16 =(x)
#define USB3_TYPEC_HOST0_RCSU_RESET_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b17 =(x)
#define USB3_TYPEC_HOST1_RCSU_RESET_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b18 =(x)
#define USB3_TYPEC_HOST2_RCSU_RESET_N(x)  (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b19 =(x)
#define VPU_RCSU_RESET_N(x)               (PMCTRL_S5_BASE_ADDRESS_PTR)->rcsu_reset_group1.Bits.b20 =(x)

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------CSU_PM Clock Generate Block---------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------*/
#define CSU_PM_BASE_ADDRESS  0x06570000u

typedef struct _PLL_CMN {
  // pll_parameter1
  union {
    struct {
      UINT32    FBDIV     : 12;
      UINT32    Reserved0 : 20;
    } bits;
    UINT32    Data32;
  } pll_parameter1;

  // pll_parameter2
  union {
    struct {
      UINT32    FRAC      : 24;
      UINT32    PLLEN     : 1;
      UINT32    Reserved0 : 3;
      UINT32    DSMEN     : 1;
      UINT32    Reserved1 : 3;
    } bits;
    UINT32    Data32;
  } pll_parameter2;

  // pll_parameter3
  union {
    struct {
      UINT32    POSTDIV1  : 3;
      UINT32    Reserved0 : 1;
      UINT32    POSTDIV2  : 3;
      UINT32    Reserved1 : 1;
      UINT32    REFDIV    : 6;
      UINT32    Reserved2 : 18;
    } bits;
    UINT32    Data32;
  } pll_parameter3;

  // pll_ctrl
  union {
    struct {
      UINT32    BYPASS        : 1;
      UINT32    DACEN         : 1;
      UINT32    FOUT4PHASEEN  : 1;
      UINT32    FOUTPOSTDIVEN : 1;
      UINT32    pll_lock      : 1;
      UINT32    time_out_en   : 1;
      UINT32    en_retiming   : 1;
      UINT32    Reserved0     : 25;
    } bits;
    UINT32    Data32;
  } pll_ctrl;

  // pll_ssmod
  union {
    struct {
      UINT32    DIVVAL       : 6;
      UINT32    Reserved0    : 2;
      UINT32    SPREAD       : 5;
      UINT32    Reserved1    : 3;
      UINT32    DOWNSPREAD   : 1;
      UINT32    Reserved2    : 3;
      UINT32    RESET        : 1;
      UINT32    Reserved3    : 3;
      UINT32    DISABLE_SSCG : 1;
      UINT32    Reserved4    : 3;
      UINT32    RESETPTR     : 1;
      UINT32    Reserved5    : 3;
    } bits;
    UINT32    Data32;
  } pll_ssmod;
} PLL_CMN;
/*-----------------------------------------------SYSTEM PLL -------------------------------------------------------------------*/
typedef union {
  struct {
    UINT32    gpu_clk_400m        : 1;
    UINT32    reserved0           : 3;
    UINT32    soc_apbclk          : 1;
    UINT32    reserved1           : 3;
    UINT32    gpu_clk_200m        : 1;
    UINT32    reserved2           : 3;
    UINT32    test_clk_system_pll : 1;
    UINT32    reserved3           : 19;
  } bits;
  UINT32    Data32;
} SYSTEM_PLL_GATER;
typedef struct SYSTEM_PLL {
  PLL_CMN             pll_cmn;
  SYSTEM_PLL_GATER    gater;
} volatile *SYSTEM_PLL_PTR;

#define SOC_SYSTEM_PLL_BASE_ADDRESS      (CSU_PM_BASE_ADDRESS  + 0x5300)
#define SOC_SYSTEM_PLL_BASE_ADDRESS_PTR  ((SYSTEM_PLL_PTR)SOC_SYSTEM_PLL_BASE_ADDRESS)

#define SYSTEM_PLL_PARAMETER1_FBDIV(x)        (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter1.bits.FBDIV = (x)
#define SYSTEM_PLL_PARAMETER2_FRAC(x)         (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter2.bits.FRAC = (x)
#define SYSTEM_PLL_PARAMETER2_DSMEN(x)        (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter2.bits.DSMEN = (x)
#define SYSTEM_PLL_PARAMETER3_POSTDIV1(x)     (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter3.bits.POSTDIV1 = (x)
#define SYSTEM_PLL_PARAMETER3_POSTDIV2(x)     (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter3.bits.POSTDIV2 = (x)
#define SYSTEM_PLL_PARAMETER3_REFDIV(x)       (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter3.bits.REFDIV = (x)
#define SYSTEM_PLL_PLL_CTRL_BYPASS(x)         (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.BYPASS = (x)
#define SYSTEM_PLL_PLL_CTRL_DACEN(x)          (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.DACEN = (x)
#define SYSTEM_PLL_PLL_CTRL_FOUT4PHASEEN(x)   (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.FOUT4PHASEEN = (x)
#define SYSTEM_PLL_PLL_CTRL_FOUTPOSTDIVEN(x)  (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.FOUTPOSTDIVEN = (x)
#define SYSTEM_PLL_PLL_CTRL_PLL_LOCK()        (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.pll_lock
#define SYSTEM_PLL_PLL_CTRL_TIME_OUT_EN(x)    (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.time_out_en = (x)
#define SYSTEM_PLL_PLL_CTRL_EN_RETIMING(x)    (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.en_retiming = (x)
#define SYSTEM_PLL_PLL_SSMOD_DIVVAL(x)        (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.DIVVAL = (x)
#define SYSTEM_PLL_PLL_SSMOD_SPREAD(x)        (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.SPREAD = (x)
#define SYSTEM_PLL_PLL_SSMOD_DOWNSPREAD(x)    (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.DOWNSPREAD = (x)
#define SYSTEM_PLL_PLL_SSMOD_RESET(x)         (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.RESET = (x)
#define SYSTEM_PLL_PLL_SSMOD_DISABLE_SSCG(x)  (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.DISABLE_SSCG = (x)
#define SYSTEM_PLL_PLL_SSMOD_RESETPTR(x)      (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.RESETPTR = (x)
#define SYSTEM_PLL_EN_GATER_GPU_CLK_400M(x)   (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->gater.bits.gpu_clk_400m = (x)
#define SYSTEM_PLL_EN_GATER_SOC_APBCLK(x)     (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->gater.bits.soc_apbclk = (x)
#define SYSTEM_PLL_EN_GATER_GPU_CLK_200M(x)   (SOC_SYSTEM_PLL_BASE_ADDRESS_PTR)->gater.bits.gpu_clk_200m = (x)

/*-----------------------------------------------PCIE AXI PLL -----------------------------------------------------------------*/
typedef union {
  struct {
    UINT32    pcie_ni700_clk           : 1;
    UINT32    reserved0                : 3;
    UINT32    pcie_controller0_axi_clk : 1;
    UINT32    reserved1                : 3;
    UINT32    pcie_controller1_axi_clk : 1;
    UINT32    reserved2                : 3;
    UINT32    pcie_controller2_axi_clk : 1;
    UINT32    reserved3                : 3;
    UINT32    pcie_controller3_axi_clk : 1;
    UINT32    reserved4                : 3;
    UINT32    pcie_controller4_axi_clk : 1;
    UINT32    reserved5                : 11;
  } bits;
  UINT32    Data32;
} AXI_GATER;

typedef struct PCIE_AXI_PLL {
  PLL_CMN      pll_cmn;
  UINT32       sel_mux4_pcie_ni700_clk;
  AXI_GATER    gater;
} volatile *PCIE_AXI_PLL_PTR;

#define SOC_PCIE_AXI_PLL_BASE_ADDRESS      (CSU_PM_BASE_ADDRESS + 0x6000)
#define SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR  ((PCIE_AXI_PLL_PTR)SOC_PCIE_AXI_PLL_BASE_ADDRESS)

#define PCIE_AXI_PLL_PARAMETER1_FBDIV(x)             (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter1.bits.FBDIV = (x)
#define PCIE_AXI_PLL_PARAMETER2_FRAC(x)              (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter2.bits.FRAC = (x)
#define PCIE_AXI_PLL_PARAMETER2_PLLEN(x)             (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter2.bits.PLLEN = (x)
#define PCIE_AXI_PLL_PARAMETER3_POSTDIV1(x)          (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter3.bits.POSTDIV1 = (x)
#define PCIE_AXI_PLL_PARAMETER3_POSTDIV2(x)          (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter3.bits.POSTDIV2 = (x)
#define PCIE_AXI_PLL_PARAMETER3_REFDIV(x)            (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_parameter3.bits.REFDIV = (x)
#define PCIE_AXI_PLL_PLL_CTRL_BYPASS(x)              (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.BYPASS = (x)
#define PCIE_AXI_PLL_PLL_CTRL_DACEN(x)               (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.DACEN = (x)
#define PCIE_AXI_PLL_PLL_CTRL_FOUT4PHASEEN(x)        (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.FOUT4PHASEEN = (x)
#define PCIE_AXI_PLL_PLL_CTRL_PLL_LOCK()             (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.pll_lock
#define PCIE_AXI_PLL_PLL_CTRL_TIME_OUT_EN(x)         (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.time_out_en = (x)
#define PCIE_AXI_PLL_PLL_CTRL_EN_RETIMING(x)         (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ctrl.bits.en_retiming = (x)
#define PCIE_AXI_PLL_PLL_SSMOD_DIVVAL(x)             (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.DIVVAL = (x)
#define PCIE_AXI_PLL_PLL_SSMOD_SPREAD(x)             (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.SPREAD = (x)
#define PCIE_AXI_PLL_PLL_SSMOD_DOWNSPREAD(x)         (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.DOWNSPREAD = (x)
#define PCIE_AXI_PLL_PLL_SSMOD_RESET(x)              (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.RESET = (x)
#define PCIE_AXI_PLL_PLL_SSMOD_DISABLE_SSCG(x)       (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.DISABLE_SSCG = (x)
#define PCIE_AXI_PLL_PLL_SSMOD_RESETPTR(x)           (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->pll_cmn.pll_ssmod.bits.RESETPTR = (x)
#define PCIE_AXI_PLL_SEL_MUX4_PCIE_NI700_CLK(x)      (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->sel_mux4_pcie_ni700_clk = (x)
#define PCIE_AXI_PLL_EN_GATER_PCIE_NI700_CLK(x)      (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->gater.bits.pcie_ni700_clk = (x)
#define PCIE_AXI_PLL_EN_GATER_PCIE_CTRL0_AXI_CLK(x)  (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->gater.bits.pcie_controller0_axi_clk = (x)
#define PCIE_AXI_PLL_EN_GATER_PCIE_CTRL1_AXI_CLK(x)  (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->gater.bits.pcie_controller1_axi_clk = (x)
#define PCIE_AXI_PLL_EN_GATER_PCIE_CTRL2_AXI_CLK(x)  (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->gater.bits.pcie_controller2_axi_clk = (x)
#define PCIE_AXI_PLL_EN_GATER_PCIE_CTRL3_AXI_CLK(x)  (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->gater.bits.pcie_controller3_axi_clk = (x)
#define PCIE_AXI_PLL_EN_GATER_PCIE_CTRL4_AXI_CLK(x)  (SOC_PCIE_AXI_PLL_BASE_ADDRESS_PTR)->gater.bits.pcie_controller4_axi_clk = (x)
#endif
