/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (VPU0) {
  Name (_HID, "CIXH3010")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (
      ReadWrite,
      0x14230000,
      0x10000,
    )
    Memory32Fixed (
      ReadWrite,
      0x14240000,
      0x10000,
    )
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { VPU_EXT_U_VPU_CORE_IRQ_INTERRUPT_ID }
  })

/*                                             scmi_perf_dev,         domain_id
    Package () {"power-domains", Package () { \_SB.SCMI.DVFS,       0(domain_id)
                                              \_SB.SCMI.DVFS,       1(domain_id)
                                              \_SB.SCMI.DVFS,       2(domain_id)
                                            }},
*/

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
        Package () {"power-domains", Package () {\_SB.SCMI.DVFS, 9}},
        Package () {"power-domain-names", Package () {"perf"}},
    }
  })

  PowerResource(PPRS, 0, 0)
  {
    OperationRegion(OPR0,SystemMemory,VPU_RCSU_PD_REG, 0x04)
    Field (OPR0, DWordAcc, NoLock, Preserve) {
      MSK0, 32,
    }
    Method(_STA, 0, Serialized)
    {
      Local0 = MSK0
      Local0 = Local0 & PGFSM_REG_CTRL2
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
      Local0 = Local0 | PGFSM_REG_CTRL2 | TIME_CYCLE_CNT
      MSK0 = Local0
      \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT0)
    }
    Method(_OFF, 0, Serialized)
    {
      Local0 = MSK0
      Local0 = Local0 & ~PGFSM_REG_CTRL2
      MSK0 = Local0
    }
  }

  Name(_PR0, Package(1){PPRS})
  Name(_PR3, Package(1){PPRS})

  Name (CLKT, Package() {
    Package() {CLK_TREE_VPU_APBCLK, "vpu_clk", \_SB.VPU0},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_VPU_RESET_N, \_SB.VPU0, "vpu_reset"},
    Package() {\_SB.RST0, SKY1_VPU_RCSU_RESET_N, \_SB.VPU0, "vpu_rcsu_reset"},
  })

  Device (CRE0) {
    Name(_ADR, 0x00)
    Name (_STA, 0xB)
    PowerResource(PRS0, 0, 0)
    {
      Method(_STA, 0, Serialized)
      {
        return (0)
      }
      Method(_ON, 0, Serialized)
      {
        \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT1)
      }
      Method(_OFF, 0, Serialized)
      {
        /* Do nothing */
      }
    }

    Name(_PR0, Package(1){PRS0})
    Name(_PR3, Package(1){PRS0})
    Method(REPR, 0, Serialized)
    {
      \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT1)
    }
  }

  Device (CRE1) {
    Name(_ADR, 0x01)
    Name (_STA, 0xB)
    PowerResource(PRS1, 0, 0)
    {
      Method(_STA, 0, Serialized)
      {
        return (0)
      }
      Method(_ON, 0, Serialized)
      {
        \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT2)
      }
      Method(_OFF, 0, Serialized)
      {
        /* Do nothing */
      }
    }

    Name(_PR0, Package(1){PRS1})
    Name(_PR3, Package(1){PRS1})
    Method(REPR, 0, Serialized)
    {
      \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT2)
    }
  }

  Device (CRE2) {
    Name(_ADR, 0x02)
    Name (_STA, 0xB)
    PowerResource(PRS2, 0, 0)
    {
      Method(_STA, 0, Serialized)
      {
        return (0)
      }
      Method(_ON, 0, Serialized)
      {
        \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT3)
      }
      Method(_OFF, 0, Serialized)
      {
        /* Do nothing */
      }
    }

    Name(_PR0, Package(1){PRS2})
    Name(_PR3, Package(1){PRS2})
    Method(REPR, 0, Serialized)
    {
      \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT3)
    }
  }

  Device (CRE3) {
    Name(_ADR, 0x03)
    Name (_STA, 0xB)
    PowerResource(PRS3, 0, 0)
    {
      Method(_STA, 0, Serialized)
      {
        return (0)
      }
      Method(_ON, 0, Serialized)
      {
        \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT4)
      }
      Method(_OFF, 0, Serialized)
      {
        /* Do nothing */
      }
    }

    Name(_PR0, Package(1){PRS3})
    Name(_PR3, Package(1){PRS3})
    Method(REPR, 0, Serialized)
    {
      \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_VPU, VPU_RCSU_BASE_REG, BIT4)
    }
  }
}
