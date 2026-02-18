/** @file

  Copyright 2024-2025 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Include/AcpiScmi.h>

// External CPU device declarations
External (\_SB.CPB0, PkgObj)          //(CPU8, CPU9)
External (\_SB.CPB1, PkgObj)          //(CPU10, CPU11)
External (\_SB.CPM0, PkgObj)          //(CPU4, CPU5)
External (\_SB.CPM1, PkgObj)          //(CPU6, CPU7)
External (\_SB.CPUL, PkgObj)          // All CPUs

// Temperature conversion method (Celsius to Kelvin)
// Celsius to Kelvin conversion formula: K = 10 * C + 2732
Method(C2DK, 1, Serialized) {
  Multiply(Arg0, 10, Local0)
  Add(Local0, 2732, Local0)
  Return(Local0)
}

// Thermal Zone for CPU-B0 (Big cluster 0: CPU 8-9, A720)
ThermalZone(TZB0) {
  Method(_PSV) { Return(3582) }       // Passive trip point: 85°C
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TC1) { Return(4) }          // Thermal Constant1
  Method(_TC2) { Return(3) }          // Thermal Constant2
  Method(_TSP) { Return(1) }          // Sampling Period: 100ms
  Method(_PSL) { Return(\_SB.CPB0)}   // Passive cooling list
  Method(SWIT) { Return(3332) }       // Switch-On trip point: 60°C
  Method(SSTP) { Return(5500) }       // sustainable power in mW
  Method(_TZD) { Return(\_SB.CPB0) }  // Thermal Zone Devices
  Method(_TMP, 0, Serialized) {       // Temperature reading
    Store(\_SB.PMMX.SENG(CPU_B0_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_SCP, 1, Serialized) {}      // Set Cooling Policy
  Method(_TZP) { Return(10) }         // Polling Interval: 1000ms
  Name (_STR, Unicode ("CPU Big Cluster 0 (CPU 8-9)"))
}

// Thermal Zone for CPU-B1 (Big cluster 1: CPU 10-11, A720)
ThermalZone(TZB1) {
  Method(_PSV) { Return(3582) }       // Passive trip point: 85°C
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TC1) { Return(4) }          // Thermal Constant1
  Method(_TC2) { Return(3) }          // Thermal Constant2
  Method(_TSP) { Return(1) }          // Sampling Period: 100ms
  Method(_PSL) { Return(\_SB.CPB1) }  // Passive cooling list
  Method(SWIT) { Return(3332) }       // Switch-On trip point: 60°C
  Method(SSTP) { Return(6000) }       // sustainable power in mW
  Method(_TZD) { Return(\_SB.CPB1) }  // Thermal Zone Devices
  Method(_TMP, 0, Serialized) {       // Temperature reading
    Store(\_SB.PMMX.SENG(CPU_B1_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_SCP, 1, Serialized) {}      // Set Cooling Policy
  Method(_TZP) { Return(10) }         // Polling Interval: 1000ms
  Name (_STR, Unicode ("CPU Big Cluster 1 (CPU 10-11)"))
}

// Thermal Zone for CPU-M0 (Mid cluster 0: CPU 4-5, A720)
ThermalZone(TZM0) {
  Method(_PSV) { Return(3582) }       // Passive trip point: 85°C
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TC1) { Return(4) }          // Thermal Constant1
  Method(_TC2) { Return(3) }          // Thermal Constant2
  Method(_TSP) { Return(1) }          // Sampling Period: 100ms
  Method(_PSL) { Return(\_SB.CPM0) }  // Passive cooling list
  Method(SWIT) { Return(3332) }       // Switch-On trip point: 60°C
  Method(SSTP) { Return(5000) }       // sustainable power in mW
  Method(_TZD) { Return(\_SB.CPM0) }  // Thermal Zone Devices
  Method(_TMP, 0, Serialized) {       // Temperature reading
    Store(\_SB.PMMX.SENG(CPU_M0_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_SCP, 1, Serialized) {}      // Set Cooling Policy
  Method(_TZP) { Return(10) }         // Polling Interval: 1000ms
  Name (_STR, Unicode ("CPU Mid Cluster 0 (CPU 4-5)"))
}

// Thermal Zone for CPU-M1 (Mid cluster 1: CPU 6-7, A720)
ThermalZone(TZM1) {
  Method(_PSV) { Return(3582) }       // Passive trip point: 85°C
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TC1) { Return(4) }          // Thermal Constant1
  Method(_TC2) { Return(3) }          // Thermal Constant2
  Method(_TSP) { Return(1) }          // Sampling Period: 100ms
  Method(_PSL) { Return(\_SB.CPM1) }  // Passive cooling list
  Method(SWIT) { Return(3332) }       // Switch-On trip point: 60°C
  Method(SSTP) { Return(4500) }       // sustainable power in mW
  Method(_TZD) { Return(\_SB.CPM1) }  // Thermal Zone Devices
  Method(_TMP, 0, Serialized) {       // Temperature reading
    Store(\_SB.PMMX.SENG(CPU_M1_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_SCP, 1, Serialized) {}      // Set Cooling Policy
  Method(_TZP) { Return(10) }         // Polling Interval: 1000ms
  Name (_STR, Unicode ("CPU Mid Cluster 1 (CPU 6-7)"))
}

//
// Name: SPFA [Set PM Fan to Auto Mode]
// Description: Function to set PM fan to auto mode
// Input: None
// Output: None
//
Method(SPFA, 0, Serialized){
  \_SB.PMMX.SFMD(1)
}

//
// Name: SPFM [Set PM Fan to Mute Mode]
// Description: Function to set PM fan to mute mode
// Input: None
// Output: None
//
Method(SPFM, 0, Serialized){
  \_SB.PMMX.SFMD(0)
}

//
// Name: SPFP [Set PM Fan to Performance Mode]
// Description: Function to set PM fan to performance mode
// Input: None
// Output: None
//
Method(SPFP, 0, Serialized){
  \_SB.PMMX.SFMD(2)
}

OperationRegion (IPBF, SystemMemory, 0x83BF0300, 0x400)
Field (IPBF, ByteAcc, NoLock, Preserve)
{
  Offset (0x0),
  BUF_, 8192
}
// static power of CPU
Method (SPRG, 1, Serialized)
{
    // static_power offset = 0x3c + CPU index * 0x40
    Multiply (Arg0, 0x40, Local0)
    Add (Local0, 0x3c, Local1)

    CreateDWordField (BUF_, Local1, SPWR)
    Return (SPWR)
}


// dynamic power of CPU
Method (DPRG, 1, Serialized)
{
    // dynamic_power offset = 0x38 + CPU index * 0x40
    Multiply (Arg0, 0x40, Local0)
    Add (Local0, 0x38, Local1)

    CreateDWordField (BUF_, Local1, DPWR)
    Return (DPWR)
}

// Thermal Zone for GPU (average temperature)
ThermalZone(TZGT) {
  Method(_PSV) { Return (3582) }      // Passive trip point: 85°C
  Method(SWIT) { Return(3432) }       // Switch-On trip point: 70°C
  Method(SSTP) { Return(15000) }      // sustainable power in mW
  Method(_TC1) { Return(4) }          // Thermal Constant1
  Method(_TC2) { Return(3) }          // Thermal Constant2
  Method(_TSP) { Return(1) }          // Sampling Period: 100ms
  Name(_PSL, Package (){ \_SB.GPU })  // Passive cooling list
  Name(_TZD, Package (){ \_SB.GPU })  // Thermal Zone Devices
  Method(_TMP, 0, Serialized) {       // Temperature reading
    Store(\_SB.PMMX.SENG(GPU_AVERAGE_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_SCP, 1, Serialized) {}      // Set Cooling Policy
  Method(_TZP) { Return(10) }         // Polling Interval: 1000ms
  Name (_STR, Unicode ("GPU Average"))
}

// Thermal Zone for VPU (Video Processing Unit)
ThermalZone(TZVP) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(VPU_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("VPU"))
}

// Thermal Zone for GPU Bottom
ThermalZone(TZGB) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Name(_TZD, Package (){ \_SB.GPU })
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(GPU_BOTTOM_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("GPU Bottom"))
}

// Thermal Zone for GPU Top
ThermalZone(TZGP) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Name(_TZD, Package (){ \_SB.GPU })
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(GPU_TOP_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("GPU Top"))
}

// Thermal Zone for SoC Bridge/Interconnect
ThermalZone(TZBR) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(SOC_BRC_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("SoC Bridge"))
}

// Thermal Zone for DDR Bottom
ThermalZone(TZD0) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(DDR_BOTTOM4_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("DDR Bottom"))
}

// Thermal Zone for DDR Top
ThermalZone(TZD1) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(DDR_TOP_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("DDR Top"))
}

// Thermal Zone for CI700 Interconnect (cache coherent mesh)
ThermalZone(TZCI) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TZD) { Return(\_SB.CPUL) }  // Affects all CPUs via interconnect
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(CI700_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("CI700 Interconnect"))
}

// Thermal Zone for NPU (Neural Processing Unit)
ThermalZone(TZNP) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(NPU_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("NPU"))
}

// Thermal Zone for SoC Trace
ThermalZone(TZTR) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(SOC_TRC_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("SoC Trace"))
}

// Thermal Zone for Board NTC Thermistor 0
ThermalZone(TZN0) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(NTC0_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("Board NTC 0"))
}

// Thermal Zone for Board NTC Thermistor 1
ThermalZone(TZN1) {
  Method(_CRT) { Return(3712) }       // Critical trip point: 98°C
  Method(_TMP, 0, Serialized) {
    Store(\_SB.PMMX.SENG(NTC1_TEMP_SENSOR_ID, 0), Local0)
    CreateDWordField(Local0, 0x00, STAT)
    If (STAT == SCMI_SUCCESS) {
      CreateQWordField(Local0, 0x04, TEMP)
      TEMP = ToInteger(TEMP)
      Return(C2DK(TEMP))
    } Else {
      Return (0xFFFFFFFFFFFFFFFF)
    }
  }
  Method(_TZP) { Return(10) }
  Name (_STR, Unicode ("Board NTC 1"))
}
