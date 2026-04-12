/** @file
  Copyright 2024-2025 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include "CommonDefines.h"
Device (DSTD) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, RDR_RES_BASE, RDR_RES_SIZE)
  })
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "cix,dst" },
      Package () { "ramlog_addr", 0x83DA0000 },
      Package () { "ramlog_size", 0x00040000 },
      Package () { "rdr-log-max-size", 0x800000 },
      Package () { "rdr_area_num", 15 },
      Package () { "rdr_area_sizes",
        Package () { 0x100000, 0x10000, 0x10000, 0x10000, 0x10000,
                     0x10000, 0x10000, 0x10000, 0x10000, 0x10000,
                     0x10000, 0x10000, 0x10000, 0x10000, 0x10000}},
      Package () { "rdr_area_sizes", 0x00040000 },
      Package () { "rdr-log-max-nums", 6 },
      Package () { "wait-dumplog-timeout", 1000 },
      Package () { "unexpected-max-reboot-times", 1 },
      Package () { "rdr-dumpctl", "1111111111" },
      Package () { "ramlog_size2", 0x00040000 },
    }
  })
  Device (EXTR) {
    Name (_HID, "PRP0001")
    Name (_UID, 0x1)
    Name (_STA, 0xF)
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () { "compatible", "rdr,exceptiontrace" },
        Package () { "area_num", 1 },
        Package () { "area_sizes", 0x1000 },
      }
    })
  }
    Device (APAD) {
    Name (_HID, "PRP0001")
    Name (_UID, 0x2)
    Name (_STA, 0xF)
    Name (_CRS, ResourceTemplate () {
      Memory32Fixed (ReadWrite, MNTNDUMP_ADDR, MNTNDUMP_SIZE)
    })
    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () { "compatible", "rdr,rdr_ap_adapter" },
        Package () { "ap_trace_irq_size", 0x10000 },
        Package () { "ap_trace_task_size", 0x10000 },
        Package () { "ap_trace_cpu_idle_size", 0x10000 },
        Package () { "ap_trace_worker_size", 0x10000 },
        Package () { "ap_trace_time_size", 0x10000 },
        Package () { "ap_trace_cpu_on_off_size", 0x10000 },
        Package () { "ap_trace_syscall_size", 0x10000 },
        Package () { "ap_trace_hung_task_size", 0x10000 },
        Package () { "ap_trace_tasklet_size", 0x10000 },
        Package () { "ap_last_task_switch", 1 },
        Package () { "mntndump_addr", MNTNDUMP_ADDR },
        Package () { "mntndump_size", MNTNDUMP_SIZE },
        Package () { "ap_dump_mem_modu_test_size", 1024 },
        Package () { "ap_dump_mem_modu_idm_size", 0x1000 },
        Package () { "ap_dump_mem_modu_tzc400_size", 0x1000 },
        Package () { "ap_dump_mem_modu_smmu_size", 0x1000 },
        Package () { "ap_dump_mem_modu_tfa_size", 0x4000 },
        Package () { "ap_dump_mem_modu_gap_size", 256 },
        Package () { "ap_log_console_size", 0x20000 },
        Package () { "ap_log_dmesg_size", 0x80000 },
      }
    })
  }
}
// For ramoops driver
Device (RAOP) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x12)
  Name (_STA, 0x3)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, RAMOOPS_RES_BASE, RAMOOPS_RES_SIZE)
  })
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "ramoops" },
      Package () { "mem-address", RAMOOPS_RES_BASE },
      Package () { "mem-size", RAMOOPS_RES_SIZE },
      Package () { "record-size", 0x40000 },
      Package () { "console-size", 0x40000 },
      Package () { "pmsg-size", 0x1000 },
    }
  })
}
Device (DCT0) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x83)
  Name (_STA, 0x3)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0C010000, 0x20000)
    Memory32Fixed (ReadWrite, 0x83c00000, 0x1000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 239 }
  })
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "cadence,ddr_ctrl" },
      Package () { "channel_id", 0x0 },
    }
  })
}
Device (DCT1) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x84)
  Name (_STA, 0x3)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0C030000, 0x20000)
    Memory32Fixed (ReadWrite, 0x83c00000, 0x1000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 242 }
  })
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "cadence,ddr_ctrl" },
      Package () { "channel_id", 0x1 },
    }
  })
}
Device (DCT2) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x85)
  Name (_STA, 0x3)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0C050000, 0x20000)
    Memory32Fixed (ReadWrite, 0x83c00000, 0x1000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 245 }
  })
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "cadence,ddr_ctrl" },
      Package () { "channel_id", 0x2 },
    }
  })
}
Device (DCT3) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x86)
  Name (_STA, 0x3)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x0C070000, 0x20000)
    Memory32Fixed (ReadWrite, 0x83c00000, 0x1000)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 248 }
  })
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "cadence,ddr_ctrl" },
      Package () { "channel_id", 0x3 },
    }
  })
}
// For se_pm_crash driver
Device (SEPM) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x10)
  Name (_STA, 0x3)
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "cix,se_pm_crash" },
    }
  })
}
// For dst_mbox_client device
Device (DSMC) {
  Name (_HID, "PRP0001")
  Name (_UID, 0x11)
  Name (_STA, 0x3)
  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
      Package () { "compatible", "cix,cix_se2ap_mbox" },
      Package () { "mbox-names", Package () {"tx4", "rx4"}},
      Package () { "mboxes", Package (4) { MBX0, 10, MBX1, 9}},
    }
  })
}