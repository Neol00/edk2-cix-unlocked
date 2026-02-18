/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (PMA) {
  Name (_HID, "CIXHA012")
  Name (_UID, 0x0)
  Name (_STA, 0x03)
  Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, FixedPcdGet32 (PcdGpuReservedMemoryBase) , FixedPcdGet32 (PcdGpuReservedMemorySize))
  })
}

Device (PMGM) {
  Name (_HID, "CIXHA013")
  Name (_UID, 0x0)
  Name (_STA, 0x03)
}

// GPU power supply
Device (GPUP) {
  Name (_HID, "CIXH5001")
  Name (_UID, 0x0)
  Name (_STA, 0xB)

  PowerResource(PPRS, 0, 0)
  {
    OperationRegion(OPR0,SystemMemory,GPU_RCSU_PD_REG, 0x04)
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
      Stall (5)
      \_SB.DMRP(MEMORY_ENABLE, MEMR_GROUP_ID_GPU, GPU_RCSU_BASE_REG, BIT0)
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
}

Device (GPU) {
  Name (_HID, "CIXH5000")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CCA, 0)  // Non-coherent — matches HW design (DPU reads non-coherently)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, GPU_RCSU_CONTROLLER_BASE, GPU_RCSU_CONTROLLER_SIZE)
    Memory32Fixed (ReadWrite, GPU_CONTROLLER_BASE, GPU_CONTROLLER_SIZE)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { GPU_EXT_VDCI_GPU_MISC_VDDGPU_VDDSOC_IRQJOB_VDCI_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { GPU_EXT_VDCI_GPU_MISC_VDDGPU_VDDSOC_IRQMMU_VDCI_OUT_INTERRUPT_ID }
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { GPU_EXT_VDCI_GPU_MISC_VDDGPU_VDDSOC_IRQGPU_VDCI_OUT_INTERRUPT_ID }
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
        //Package () {"power_policy", "always_on"},
        Package () {"protected-memory-allocator",\_SB.PMA},
        Package () {"physical-memory-group-manager",\_SB.PMGM},
        Package () {"power-domains", Package () {\_SB.SCMI.DVFS, 0}},
        Package () {"power-domain-names", Package () {"perf"}},
        Package () {"tzgt", \_SB.TZGT },
        Package () {"power-supply", \_SB.GPUP },
    },
    ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
    Package () {
      Package () {"pbha", "IIOR"},
      Package () {"power_model", "PWRM"},
    }
  })

  Name (IIOR, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
        Package () { "int_id_override", Package() {2,0x23,4,0x23,16,0x22,17,0x32,18,0x52,21,0x32,22,0x52,24,0x22,28,0x32} }
    }
  })

  Name (PWRM, Package() {
    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
        Package () { "static-coefficient", "2427750" },
        Package () { "dynamic-coefficient", "4687" },
        Package () { "ts", Package() {"20000", "2000", "-20", "2" } },
        Package () { "thermal-zone", "tzgt" },
    }
  })

  Name (CLKT, Package() {
    Package() {CLK_TREE_GPU_CLK_CORE, "gpu_clk_core", \_SB.GPU},
    Package() {CLK_TREE_GPU_CLK_STACKS, "gpu_clk_stacks", \_SB.GPU},
    Package() {CLK_TREE_GPU_CLK_200M, "gpu_clk_200M", \_SB.GPU},
    Package() {CLK_TREE_GPU_CLK_400M, "gpu_clk_400M", \_SB.GPU},
  })
  Name (RSTL, Package() {
    Package() {\_SB.RST0, SKY1_GPU_RESET_N, \_SB.GPU,  "gpu_reset"},
    Package() {\_SB.RST0, SKY1_GPU_RCSU_RESET_N, \_SB.GPU,  "gpu_rcsu_reset"},
  })
}