/**

  Copyright 2025 Cix Technology Group Co., Ltd. All Rights Reserved

**/
#ifndef USBA_PHY_REGISTER_H_
#define USBA_PHY_REGISTER_H_

#include "UsbDpPhy.h"

CDNSP_REG_SEQ
  Usb3ARegConf[] = {
  { CMN_SSM_BIAS_TMR,                  0x18   },
  { CMN_PLLSM0_PLLPRE_TMR,             0x30   },
  { CMN_PLLSM0_PLLLOCK_TMR,            0xf0   },
  { CMN_PLLSM1_PLLPRE_TMR,             0x30   },
  { CMN_PLLSM1_PLLLOCK_TMR,            0xf0   },
  { CMN_BGCAL_INIT_TMR,                0x78   },
  { CMN_BGCAL_ITER_TMR,                0x78   },
  { CMN_IBCAL_INIT_TMR,                0x18   },
  { CMN_TXPUCAL_INIT_TMR,              0x1d   },
  { CMN_TXPDCAL_INIT_TMR,              0x1d   },
  { CMN_RXCAL_INIT_TMR,                0x2d0  },
  { CMN_SD_CAL_PLLCNT_START,           0x137  },
  { RX_SDCAL0_INIT_TMR_LANE0,          0x18   },
  { RX_SDCAL0_INIT_TMR_LANE1,          0x18   },
  { RX_SDCAL0_ITER_TMR_LANE0,          0x78   },
  { RX_SDCAL0_ITER_TMR_LANE1,          0x78   },
  { RX_SDCAL1_INIT_TMR_LANE0,          0x18   },
  { RX_SDCAL1_INIT_TMR_LANE1,          0x18   },
  { RX_SDCAL1_ITER_TMR_LANE0,          0x78   },
  { RX_SDCAL1_ITER_TMR_LANE1,          0x78   },
  { TX_RCVDET_ST_TMR_LANE0,            0x960  },
  { TX_RCVDET_ST_TMR_LANE1,            0x960  },
  { PHY_PLL_CFG_0803,                  0x0    },
  { CMN_PDIAG_PLL0_CLK_SEL_M0,         0x8600 },
  { XCVR_DIAG_HSCLK_SEL_LANE0,         0x0    },
  { XCVR_DIAG_HSCLK_SEL_LANE1,         0x0    },
  { XCVR_DIAG_HSCLK_DIV_LANE0,         0x1    },
  { XCVR_DIAG_HSCLK_DIV_LANE1,         0x1    },
  { XCVR_DIAG_PLLDRC_CTRL_LANE0,       0x41   },
  { XCVR_DIAG_PLLDRC_CTRL_LANE1,       0x41   },
  { CMN_PLL0_DSM_DIAG_M0,              0x4    },
  { CMN_PLL1_DSM_DIAG_M0,              0x4    },
  { CMN_PDIAG_PLL1_ITRIM_M0,           0x003f },
  { CMN_PDIAG_PLL0_CP_PADJ_M0,         0xb17  },
  { CMN_PDIAG_PLL1_CP_PADJ_M0,         0xb17  },
  { CMN_PDIAG_PLL0_CP_IADJ_M0,         0xe01  },
  { CMN_PDIAG_PLL1_CP_IADJ_M0,         0xe01  },
  { CMN_PDIAG_PLL0_FILT_PADJ_M0,       0xd05  },
  { CMN_PDIAG_PLL1_FILT_PADJ_M0,       0xd05  },
  { CMN_PLL0_INTDIV_M0,                0x1a0  },
  { CMN_PLL1_INTDIV_M0,                0x1a0  },
  { CMN_PLL0_FRACDIVL_M0,              0xaaab },
  { CMN_PLL1_FRACDIVL_M0,              0xaaab },
  { CMN_PLL0_FRACDIVH_M0,              0x2    },
  { CMN_PLL1_FRACDIVH_M0,              0x2    },
  { CMN_PLL0_HIGH_THR_M0,              0x116  },
  { CMN_PLL1_HIGH_THR_M0,              0x116  },
  { CMN_PDIAG_PLL0_CTRL_M0,            0x1002 },
  { CMN_PDIAG_PLL1_CTRL_M0,            0x1002 },
 #ifdef ENABLE_SPREAD_SPECTRUE
  {
    CMN_PLL0_SS_CTRL1_M0, 0x0001
  },
  { CMN_PLL1_SS_CTRL1_M0,              0x0001 },
  { CMN_PLL0_SS_CTRL2_M0,              0x0416 },
  { CMN_PLL1_SS_CTRL2_M0,              0x0416 },
  { CMN_PLL0_SS_CTRL3_M0,              0x007C },
  { CMN_PLL1_SS_CTRL3_M0,              0x007C },
  { CMN_PLL0_SS_CTRL4_M0,              0x0003 },
  { CMN_PLL1_SS_CTRL4_M0,              0x0003 },
 #endif
  {
    CMN_PLL0_VCOCAL_INIT_TMR, 0xf0
  },
  { CMN_PLL1_VCOCAL_INIT_TMR,          0xf0   },
  { CMN_PLL0_VCOCAL_ITER_TMR,          0x4    },
  { CMN_PLL1_VCOCAL_ITER_TMR,          0x4    },
  { CMN_PLL0_VCOCAL_REFTIM_START,      0x2f8  },
  { CMN_PLL1_VCOCAL_REFTIM_START,      0x2f8  },
 #ifdef ENABLE_SPREAD_SPECTRUE
  {
    CMN_PLL0_VCOCAL_PLLCNT_START, 0x02F6
  },
  { CMN_PLL1_VCOCAL_PLLCNT_START,      0x02F6 },
 #else
  {
    CMN_PLL0_VCOCAL_PLLCNT_START, 0x2f8
  },
  { CMN_PLL1_VCOCAL_PLLCNT_START,      0x2f8  },
 #endif
  {
    CMN_PLL0_VCOCAL_TCTRL, 0x3
  },
  { CMN_PLL1_VCOCAL_TCTRL,             0x3    },
  { CMN_PLL0_LOCK_REFCNT_START,        0xbf   },
  { CMN_PLL1_LOCK_REFCNT_START,        0xbf   },
  { CMN_PLL0_LOCK_PLLCNT_START,        0xbf   },
  { CMN_PLL1_LOCK_PLLCNT_START,        0xbf   },
 #ifdef ENABLE_SPREAD_SPECTRUE
  {
    CMN_PLL0_LOCK_PLLCNT_THR, 0x0005
  },
  { CMN_PLL1_LOCK_PLLCNT_THR,          0x0005 },
 #else
  {
    CMN_PLL0_LOCK_PLLCNT_THR, 0x3
  },
  { CMN_PLL1_LOCK_PLLCNT_THR,          0x3    },
 #endif
  {
    PHY_PIPE_USB3_GEN2_PRE_CFG0_0803, 0xa0a
  },
  { PHY_PIPE_USB3_GEN2_POST_CFG0_0803, 0x1000 },
  { PHY_PIPE_USB3_GEN2_POST_CFG1_0803, 0x10   },
  { CMN_CDIAG_CDB_PWRI_OVRD,           0x8200 },
  { CMN_CDIAG_XCVRC_PWRI_OVRD,         0x8200 },
  { TX_PSC_A0_LANE0,                   0x2ff  },
  { TX_PSC_A0_LANE1,                   0x2ff  },
  { TX_PSC_A1_LANE0,                   0x6af  },
  { TX_PSC_A1_LANE1,                   0x6af  },
  { TX_PSC_A2_LANE0,                   0x6ae  },
  { TX_PSC_A2_LANE1,                   0x6ae  },
  { TX_PSC_A3_LANE0,                   0x6ae  },
  { TX_PSC_A3_LANE1,                   0x6ae  },
  { RX_PSC_A0_LANE0,                   0xd1d  },
  { RX_PSC_A0_LANE1,                   0xd1d  },
  { RX_PSC_A1_LANE0,                   0xd1d  },
  { RX_PSC_A1_LANE1,                   0xd1d  },
  { RX_PSC_A2_LANE0,                   0xd00  },
  { RX_PSC_A2_LANE1,                   0xd00  },
  { RX_PSC_A3_LANE0,                   0x500  },
  { RX_PSC_A3_LANE1,                   0x500  },
  { TX_TXCC_CTRL_LANE0,                0x2a82 },
  { TX_TXCC_CTRL_LANE1,                0x2a82 },
  { TX_TXCC_CPOST_MULT_01_LANE0,       0x14   },
  { TX_TXCC_CPOST_MULT_01_LANE1,       0x14   },
  // { TX_TXCC_MGNFS_MULT_000_LANE0,     0x2    }, // TODO checkout xcvr_avdd_h < 1.1 v 0x02, > 1.2v 0x07 else default value 0x7
  // { TX_TXCC_MGNFS_MULT_000_LANE1,     0x2    }, // TODO checkout xcvr_avdd_h
  { RX_SIGDET_HL_FILT_TMR_LANE0,       0x13   },
  { RX_SIGDET_HL_FILT_TMR_LANE1,       0x13   },
  { RX_REE_GCSM1_CTRL_LANE0,           0x0    },
  { RX_REE_GCSM1_CTRL_LANE1,           0x0    },
  { RX_REE_ATTEN_THR_LANE0,            0xc02  },
  { RX_REE_ATTEN_THR_LANE1,            0xc02  },
  { RX_REE_SMGM_CTRL1_LANE0,           0x330  },
  { RX_REE_SMGM_CTRL1_LANE1,           0x330  },
  { RX_REE_SMGM_CTRL2_LANE0,           0x300  },
  { RX_REE_SMGM_CTRL2_LANE1,           0x300  },
  { XCVR_DIAG_PSC_OVRD_LANE0,          0x3    },
  { XCVR_DIAG_PSC_OVRD_LANE1,          0x3    },
  { RX_DIAG_SIGDET_TUNE_LANE0,         0x1004 },
  { RX_DIAG_SIGDET_TUNE_LANE1,         0x1004 },
  { RX_DIAG_NQST_CTRL_LANE0,           0xf9   },
  { RX_DIAG_NQST_CTRL_LANE1,           0xf9   },
  { RX_DIAG_DFE_AMP_TUNE_2_LANE0,      0xc01  },
  { RX_DIAG_DFE_AMP_TUNE_2_LANE1,      0xc01  },
  { RX_DIAG_DFE_AMP_TUNE_3_LANE0,      0x2    },
  { RX_DIAG_DFE_AMP_TUNE_3_LANE1,      0x2    },
  { RX_DIAG_PI_CAP_LANE0,              0x0    },
  { RX_DIAG_PI_CAP_LANE1,              0x0    },
  { RX_DIAG_PI_RATE_LANE0,             0x31   },
  { RX_DIAG_PI_RATE_LANE1,             0x31   },
  { RX_CDRLF_CNFG_LANE0,               0x18c  },
  { RX_CDRLF_CNFG_LANE1,               0x18c  },
  { RX_CDRLF_CNFG3_LANE0,              0x3    },
  { RX_CDRLF_CNFG3_LANE1,              0x3    }
};

#endif
