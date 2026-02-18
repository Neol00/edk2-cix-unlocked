/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Protocol/sky1-audss.h>

Device (ADSS) {
  Name (_HID, "CIXH6060")
  Name (_UID, 0x0)

  Method (_STA, 0x0, Serialized) {
    If(\_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
        Return (0xF)
    } else {
        Return (0x0)
    }
  }

  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x07110000, 0x10000)
  })

  Device (ACLK) {

    Name (_HID, "CIXH6061")
    Name (_UID, 0x0)

    Method (_STA, 0x0, Serialized) {
      If(\_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
          Return (0xF)
      } else {
          Return (0x0)
      }
    }
  
    Name (_CCA, 0)

    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () { "audss_cru", \_SB.ACRU },
      }
    })

    Name (CLKA, Package(){
        Package() {CLK_I2S0_APB, "hst", \_SB.I2S0},
        Package() {CLK_I2S1_APB, "hst", \_SB.I2S1},
        Package() {CLK_I2S2_APB, "hst", \_SB.I2S2},
        Package() {CLK_I2S3_APB, "hst", \_SB.I2S3},
        Package() {CLK_I2S4_APB, "hst", \_SB.I2S4},
        Package() {CLK_I2S5_APB, "hst", \_SB.I2S5},
        Package() {CLK_I2S6_APB, "hst", \_SB.I2S6},
        Package() {CLK_I2S7_APB, "hst", \_SB.I2S7},
        Package() {CLK_I2S8_APB, "hst", \_SB.I2S8},
        Package() {CLK_I2S9_APB, "hst", \_SB.I2S9},
        Package() {CLK_I2S0, "i2s", \_SB.I2S0},
        Package() {CLK_I2S1, "i2s", \_SB.I2S1},
        Package() {CLK_I2S2, "i2s", \_SB.I2S2},
        Package() {CLK_I2S3, "i2s", \_SB.I2S3},
        Package() {CLK_I2S4, "i2s", \_SB.I2S4},
        Package() {CLK_I2S5, "i2s", \_SB.I2S5},
        Package() {CLK_I2S6, "i2s", \_SB.I2S6},
        Package() {CLK_I2S7, "i2s", \_SB.I2S7},
        Package() {CLK_I2S8, "i2s", \_SB.I2S8},
        Package() {CLK_I2S9, "i2s", \_SB.I2S9},
        Package() {CLK_MCLK0, "mclk", \_SB.I2S0},
        Package() {CLK_DMAC_AXI, "", \_SB.DMA1},
        Package() {CLK_HDA_SYS, "sysclk", \_SB.HDA},
        Package() {CLK_HDA_HDA, "clk48m", \_SB.HDA},
        Package() {CLK_DSP_CLK, "clk", \_SB.DSP},
        Package() {CLK_DSP_BCLK, "bclk", \_SB.DSP},
        Package() {CLK_DSP_PBCLK, "pbclk", \_SB.DSP},
        Package() {CLK_SRAM_AXI, "sramclk", \_SB.DSP},
        Package() {CLK_MB_0_APB, "mb0clk", \_SB.DSP},
        Package() {CLK_MB_1_APB, "mb1clk", \_SB.DSP},
    })
    Name (CLKT, Package() {
      Package() {CLK_TREE_AUDIO_CLK0, "audio_clk0", \_SB.ADSS.ACLK},
      Package() {CLK_TREE_AUDIO_CLK1, "audio_clk1", \_SB.ADSS.ACLK},
      Package() {CLK_TREE_AUDIO_CLK2, "audio_clk2", \_SB.ADSS.ACLK},
      Package() {CLK_TREE_AUDIO_CLK3, "audio_clk3", \_SB.ADSS.ACLK},
      Package() {CLK_TREE_AUDIO_CLK4, "audio_clk4", \_SB.ADSS.ACLK},
      Package() {CLK_TREE_AUDIO_CLK5, "audio_clk5", \_SB.ADSS.ACLK},
    })
    Name (RSTL, Package() {
      Package() {\_SB.RST0, SKY1_AUDIO_HIFI5_NOC_RESET_N, \_SB.ADSS.ACLK, "noc"},
    })

    PowerResource(PPRS, 0, 0)
    {
      OperationRegion(OPR0,SystemMemory,AUDIO_RCSU_PD_REG, 0x04)
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
        sleep(1)
        \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_AUDIO, AUDIO_RCSU_BASE_REG, GROUP_INDEX_NONE)
      }
      Method(_OFF, 0, Serialized)
      {
        Local0 = MSK0
        Local0 = Local0 & ~PGFSM_REG_CTRL
        MSK0 = Local0
        sleep(1)
      }
    }

    Name(_PR0, Package(1){PPRS})
    Name(_PR3, Package(1){PPRS})
  }

  Device (ARST) {
    Name (_HID, "CIXH6062")
    Name (_UID, 0x0)

    Method (_STA, 0x0, Serialized) {
      If(\_SB.GETV(ARV_AUDIO_SUPPORT_OFFSET)){
          Return (0xF)
      } else {
          Return (0x0)
      }
    }
  

    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () { "audss_cru", \_SB.ACRU },
      }
    })
  }
}

Device (ACRU) {
  Name (_HID, "CIXHA018")
  Name (_UID, 0x0)

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x07110000, 0x10000)
  })
}
