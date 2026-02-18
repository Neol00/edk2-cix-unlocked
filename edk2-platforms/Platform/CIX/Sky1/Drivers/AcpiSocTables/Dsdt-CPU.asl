/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Include/AcpiPlatform.h>
#include <Include/MemoryMap.h>
#include <Include/IoConfig.h>
#include "CommonDefines.h"

/* _OSC: Operating System Capabilities */
Method (_OSC, 4, Serialized) {
  CreateDWordField (Arg3, 0x00, STS0)
  CreateDWordField (Arg3, 0x04, CAP0)

  /* Platform-wide Capabilities */
  If (LEqual (Arg0, ToUUID("0811b06e-4a27-44f9-8d60-3cbbc22e7b48"))) {
    /* OSC rev 1 supported, for other version, return failure */
    If (LEqual (Arg1, One)) {
      And (STS0, Not (OSC_STS_MASK), STS0)

      If (And (CAP0, OSC_CAP_OS_INITIATED_LPI)) {
        /* OS initiated LPI not supported */
        And (CAP0, Not (OSC_CAP_OS_INITIATED_LPI), CAP0)
        Or (STS0, OSC_STS_CAPABILITY_MASKED, STS0)
      }

      If (And (CAP0, OSC_CAP_CPPC_SUPPORT)) {
        /* CPPC revision 1 and below not supported */
        And (CAP0, Not (OSC_CAP_CPPC_SUPPORT), CAP0)
        Or (STS0, OSC_STS_CAPABILITY_MASKED, STS0)
      }

    } Else {
      And (STS0, Not (OSC_STS_MASK), STS0)
      Or (STS0, Or (OSC_STS_FAILURE, OSC_STS_UNRECOGNIZED_REV), STS0)
    }
  } Else {
    And (STS0, Not (OSC_STS_MASK), STS0)
    Or (STS0, Or (OSC_STS_FAILURE, OSC_STS_UNRECOGNIZED_UUID), STS0)
  }

  Return (Arg3)
}

Method (_INI, 0, NotSerialized)
{
  ULPI()
}

Method (ULPI, 0, NotSerialized)
{
  Local0 = GETV(ARV_CPU_LPI0_ENABLE_OFFSET)
  Local1 = GETV(ARV_CPU_LPI1_ENABLE_OFFSET)
  Local2 = GETV(ARV_CPU_LPI2_ENABLE_OFFSET)

  Store(Local0, Index (DerefOf (Index(LPIB, 3)), 2))
  Store(Local0, Index (DerefOf (Index(LPIL, 3)), 2))
  Store(Local1, Index (DerefOf (Index(LPIB, 4)), 2))
  Store(Local1, Index (DerefOf (Index(LPIL, 4)), 2))
  Store(Local2, Index (DerefOf (Index(LPIB, 5)), 2))
  Store(Local2, Index (DerefOf (Index(LPIL, 5)), 2))
}

//Big core for cpuidle
Name (LPIB, Package ()
{
    0, // Revision
    0, // LevelID
    3, // Count of packages

    // Standby / WFI state.
    Package () {
        CPU_LPIB_STATE0_MIN_RES, // Min Residency (us)
        CPU_LPIB_STATE0_WAKEUP_LAT, // Wakeup Latency (us)
        0, // Flags, set bit0 to 1 to enable this state
        0, // Arch. Context Lost Flags
        0, // Residency Counter Frequency
        0, // Enabled Parent State
        ResourceTemplate () {
        // Entry method for the WFI state. See the ARM FFH Specification (ARM DEN 0048).
        Register (FFixedHW, 0x20, 0, 0x00000000FFFFFFFF, 3)
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Residency counter register
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Usage counter register
        },
        "Standby" // Name
    },

    // Core pd state.
    Package () {
        CPU_LPIB_STATE1_MIN_RES, // Min Residency (us)
        CPU_LPIB_STATE1_WAKEUP_LAT, // Wakeup Latency (us)
        0, // Flags, set bit0 to 1 to enable this state
        1, // Arch. Context Lost Flags
        0, // Residency Counter Frequency
        0, // Enabled Parent State
        ResourceTemplate () {
        Register (FFixedHW, 0x20, 0, 0x10000, 3)
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Residency counter register
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Usage counter register
        },
        "Powerdown" // Name
    },

    // Cluster state.
    Package () {
        CPU_LPIB_STATE2_MIN_RES, // Min Residency (us)
        CPU_LPIB_STATE2_WAKEUP_LAT, // Wakeup Latency (us)
        0, // Flags, set bit0 to 1 to enable this state
        1, // Arch. Context Lost Flags
        0, // Residency Counter Frequency
        0, // Enabled Parent State
        ResourceTemplate () {
        Register (FFixedHW, 0x20, 0, 0x1010000, 3)
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Residency counter register
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Usage counter register
        },
        "ClusterPD" // Name
    },
})

//Little core for cpuidle
Name (LPIL, Package ()
{
    0, // Revision
    0, // LevelID
    3, // Count of packages

    // Standby / WFI state.
    Package () {
        CPU_LPIL_STATE0_MIN_RES, // Min Residency (us)
        CPU_LPIL_STATE0_WAKEUP_LAT, // Wakeup Latency (us)
        0, // Flags, set bit0 to 1 to enable this state
        0, // Arch. Context Lost Flags
        0, // Residency Counter Frequency
        0, // Enabled Parent State
        ResourceTemplate () {
        // Entry method for the WFI state. See the ARM FFH Specification (ARM DEN 0048).
        Register (FFixedHW, 0x20, 0, 0x00000000FFFFFFFF, 3)
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Residency counter register
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Usage counter register
        },
        "Standby" // Name
    },

    // Core pd state.
    Package () {
        CPU_LPIL_STATE1_MIN_RES, // Min Residency (us)
        CPU_LPIL_STATE1_WAKEUP_LAT, // Wakeup Latency (us)
        0, // Flags, set bit0 to 1 to enable this state
        1, // Arch. Context Lost Flags
        0, // Residency Counter Frequency
        0, // Enabled Parent State
        ResourceTemplate () {
        Register (FFixedHW, 0x20, 0, 0x10000, 3)
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Residency counter register
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Usage counter register
        },
        "Powerdown" // Name
    },

    // Cluster state.
    Package () {
        CPU_LPIL_STATE2_MIN_RES, // Min Residency (us)
        CPU_LPIL_STATE2_WAKEUP_LAT, // Wakeup Latency (us)
        0, // Flags, set bit0 to 1 to enable this state
        1, // Arch. Context Lost Flags
        0, // Residency Counter Frequency
        0, // Enabled Parent State
        ResourceTemplate () {
        Register (FFixedHW, 0x20, 0, 0x1010000, 3)
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Residency counter register
        },
        ResourceTemplate () {
        Register (SystemMemory,0,0,0,0) // Usage counter register
        },
        "ClusterPD" // Name
    },
})

#ifndef DYNAMIC_ACPI_CPU_ENABLE
//
// Description: This is a Processor #0 Device
//
Device (CPU0)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x0)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIL)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_0_TO_3_DESIRED_PERF_REG, 0, 1, CORE_0_TO_3_HIGHEST_PERF, CORE_0_TO_3_NOMINAL_PERF, CORE_0_TO_3_LOWEST_NON_LINEAR_PERF, CORE_0_TO_3_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 0, 0xFD, 4}
    })

}
#ifdef MULTICORE_ENABLE
Device (CPU1)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x1)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIL)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_0_TO_3_DESIRED_PERF_REG, 0, 1, CORE_0_TO_3_HIGHEST_PERF, CORE_0_TO_3_NOMINAL_PERF, CORE_0_TO_3_LOWEST_NON_LINEAR_PERF, CORE_0_TO_3_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 0, 0xFD, 4}
    })
}

Device (CPU2)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x2)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIL)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_0_TO_3_DESIRED_PERF_REG, 0, 1, CORE_0_TO_3_HIGHEST_PERF, CORE_0_TO_3_NOMINAL_PERF, CORE_0_TO_3_LOWEST_NON_LINEAR_PERF, CORE_0_TO_3_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 0, 0xFD, 4}
    })
}

Device (CPU3)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x3)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIL)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_0_TO_3_DESIRED_PERF_REG, 0, 1, CORE_0_TO_3_HIGHEST_PERF, CORE_0_TO_3_NOMINAL_PERF, CORE_0_TO_3_LOWEST_NON_LINEAR_PERF, CORE_0_TO_3_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 0, 0xFD, 4}
    })
}

Device (CPU4)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x4)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIB)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_4_5_DESIRED_PERF_REG, 0, 1, CORE_4_5_HIGHEST_PERF, CORE_4_5_NOMINAL_PERF, CORE_4_5_LOWEST_NON_LINEAR_PERF, CORE_4_5_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 1, 0xFD, 2}
    })
}

Device (CPU5)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x5)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIB)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_4_5_DESIRED_PERF_REG, 0, 1, CORE_4_5_HIGHEST_PERF, CORE_4_5_NOMINAL_PERF, CORE_4_5_LOWEST_NON_LINEAR_PERF, CORE_4_5_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 1, 0xFD, 2}
    })
}

Device (CPU6)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x6)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIB)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_6_7_DESIRED_PERF_REG, 0, 1, CORE_6_7_HIGHEST_PERF, CORE_6_7_NOMINAL_PERF, CORE_6_7_LOWEST_NON_LINEAR_PERF, CORE_6_7_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 2, 0xFD, 2}
    })
}

Device (CPU7)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x7)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIB)
    }

    Name (_CPC, Package()
    CPPC_PACKAGE_INIT (CORE_6_7_DESIRED_PERF_REG, 0, 1, CORE_6_7_HIGHEST_PERF, CORE_6_7_NOMINAL_PERF, CORE_6_7_LOWEST_NON_LINEAR_PERF, CORE_6_7_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 2, 0xFD, 2}
    })
}

Device (CPU8)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x8)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIB)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_8_9_DESIRED_PERF_REG, 0, 1, CORE_8_9_HIGHEST_PERF, CORE_8_9_NOMINAL_PERF, CORE_8_9_LOWEST_NON_LINEAR_PERF, CORE_8_9_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 3, 0xFD, 2}
    })
}

Device (CPU9)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0x9)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIB)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_8_9_DESIRED_PERF_REG, 0, 1, CORE_8_9_HIGHEST_PERF, CORE_8_9_NOMINAL_PERF, CORE_8_9_LOWEST_NON_LINEAR_PERF, CORE_8_9_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 3, 0xFD, 2}
    })
}

Device (CP10)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0xA)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIB)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_10_11_DESIRED_PERF_REG, 0, 1, CORE_10_11_HIGHEST_PERF, CORE_10_11_NOMINAL_PERF, CORE_10_11_LOWEST_NON_LINEAR_PERF, CORE_10_11_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 4, 0xFD, 2}
    })
}

Device (CP11)
{
    Name (_HID, "ACPI0007")
    Name (_UID, 0xB)
    Method (_STA)
    {
        Return(0xf)
    }

    Method (_LPI, 0, NotSerialized)  // _LPI: Low Power Idle States
    {
        Return (LPIB)
    }

    Name (_CPC, Package()
      CPPC_PACKAGE_INIT (CORE_10_11_DESIRED_PERF_REG, 0, 1, CORE_10_11_HIGHEST_PERF, CORE_10_11_NOMINAL_PERF, CORE_10_11_LOWEST_NON_LINEAR_PERF, CORE_10_11_LOWEST_PERF, REF_PERF)
    )
    Name (_PSD, Package () {
      Package(){5, 0, 4, 0xFD, 2}
    })
}
#endif
#endif

// DSU PMU (DynamIQ Shared Unit Performance Monitoring Unit)
// Provides L3 cache, bus, and interconnect performance counters via perf
Device (DSUP)
{
    Name (_HID, "ARMHD500")
    Name (_UID, 0)
    Method (_STA)
    {
        Return(0xF)
    }
    Name (_CRS, ResourceTemplate () {
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 34 }
    })
}
