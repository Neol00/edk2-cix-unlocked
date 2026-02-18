/** @file
  PM Config Update DXE driver header.

  Defines pm_export_config structures adapted for UEFI context,
  matching the v2.1 binary layout used by the old BIOS SCP firmware.
  Derived from pm_export_config.h / cfg_dpm_pwrrail.h / opp_config.h.

  Copyright 2025 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PM_CONFIG_UPDATE_DXE_H_
#define _PM_CONFIG_UPDATE_DXE_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/CixFwUpdateProtocol.h>
#include <PlatformSetupVar.h>

//
// PM Config Binary Constants
//
#define PM_CONFIG_BIN_SIZE       4096
#define PM_CONFIG_SIGNATURE      0x46434D50  // 'PMCF' little-endian
#define PM_CONFIG_VALID          0
#define PM_CONFIG_INVALID        1

//
// DVFS Element Indices (from opp_config.h)
//
#define DVFS_ELEMENT_IDX_GPU_CORE  0
#define DVFS_ELEMENT_IDX_GPU_TOP   1
#define DVFS_ELEMENT_IDX_LITTLE    2
#define DVFS_ELEMENT_IDX_BIG_G0    3
#define DVFS_ELEMENT_IDX_BIG_G1    4
#define DVFS_ELEMENT_IDX_MID_G0    5
#define DVFS_ELEMENT_IDX_MID_G1    6
#define DVFS_ELEMENT_IDX_DSU       7
#define DVFS_ELEMENT_IDX_NPU       8
#define DVFS_ELEMENT_IDX_VPU       9
#define DVFS_ELEMENT_IDX_CI700     10
#define DVFS_ELEMENT_IDX_MMHUB     11
#define DVFS_ELEMENT_IDX_COUNT     12

//
// EDP Domain Indices (from cfg_dpm_pwrrail.h)
//
#define DPM_EDP_CPU_LIT  0
#define DPM_EDP_CPU_GM0  1
#define DPM_EDP_CPU_GM1  2
#define DPM_EDP_CPU_GB0  3
#define DPM_EDP_CPU_GB1  4
#define DPM_EDP_DSU      5
#define DPM_EDP_GPU      6
#define DPM_EDP_SOC      7
#define DPM_EDP_MAX      8

//
// Config data union (from pm_export_config.h)
//
#define CONFIG_EDP_CFG_CUSTOM  0

#pragma pack(1)

typedef union {
  UINT32 Data;
  struct {
    UINT32 Valid    : 1;   // 0: valid, 1: invalid
    UINT32 RawData  : 31;
  } Fields;
} CONFIG_DATA_T;

//
// DPM Power Rail Config (from cfg_dpm_pwrrail.h)
//
typedef struct {
  UINT32 VrType   : 3;
  UINT32 PwrCap   : 16;  // mW
  UINT32 I2cPort  : 3;
  UINT32 I2cAddr  : 7;
  UINT32 I2cBuck  : 3;

  UINT32 VbootMv  : 12;  // mV
  INT32  DeltaMv  : 10;
  UINT32 Rsvd1    : 10;
} DPM_PWR_RAIL_CFG_T;

//
// OPP Table Structures (from pm_export_config.h)
//
#define DOMAIN_MAX_OPP_ENTRIES  13
#define DOMAIN_MAX_COUNT        13
#define OPP_DXS_MAX             13
#define OPP_PWR_RAIL_MAX        8

typedef struct {
  UINT32 Level;
  UINT32 Voltage;     // mV
  UINT32 Frequency;   // kHz
  UINT32 Power;       // mW
} DVFS_OPP_T;

typedef struct {
  UINT16     Size;
  UINT16     SustainedIdx;
  DVFS_OPP_T OppTable[DOMAIN_MAX_OPP_ENTRIES];
} DOMAIN_OPP_CONFIG_T;

//
// PMIC Config
//
typedef struct {
  CONFIG_DATA_T     PmicScheme;
  UINT16            OppMax[OPP_DXS_MAX];
  DPM_PWR_RAIL_CFG_T EdpCfg[OPP_PWR_RAIL_MAX];
} PM_CONFIG_PMIC_T;

//
// Board Sensor Config (v2.1: 3-byte inner struct, no RegAddr)
//
typedef struct {
  CONFIG_DATA_T SensorValid;
  union {
    UINT32 RegId;
    struct {
      UINT8 I2cCtrl;
      UINT8 I2cAddrB;
      UINT8 SensorType;
    };
  };
} BOARD_SENSOR_CONFIG_T;

//
// Thermal Sensor Config
//
typedef struct {
  CONFIG_DATA_T       ThermalTripSoc;
  UINT8               WeightValid;
  UINT8               Weight[13];
  BOARD_SENSOR_CONFIG_T BoardSensor1;
  BOARD_SENSOR_CONFIG_T BoardSensor2;
} PM_CONFIG_PVT_T;

//
// Fan Config (v2.1: MAX_FAN_NUM=2, no SensorId field)
//
#define MAX_FAN_NUM           2
#define MAX_FAN_TABLE_ENTRIES 9
#define FAN_MODE_MAX          3

typedef struct {
  UINT16 Rpm;
  INT8   UpTemp;
  INT8   DownTemp;
} RPM_ENTRY_T;

typedef struct {
  CONFIG_DATA_T FanValid;
  UINT8         RpmTableValid[FAN_MODE_MAX];
  UINT8         RpmTableItems[FAN_MODE_MAX];
  RPM_ENTRY_T   RpmTable[FAN_MODE_MAX][MAX_FAN_TABLE_ENTRIES];
  CONFIG_DATA_T FanId;
  CONFIG_DATA_T FanPolarity;
  CONFIG_DATA_T ScaleupMargin;
  CONFIG_DATA_T PwmFreq;
} PM_CONFIG_FAN_T;

//
// OPP Config
//
typedef struct {
  UINT8               OppValid;
  DOMAIN_OPP_CONFIG_T Opps[DOMAIN_MAX_COUNT];
} PM_CONFIG_OPP_T;

//
// Log Config
//
typedef struct {
  CONFIG_DATA_T LogEnable;
  CONFIG_DATA_T UartBaudrate;
} PM_CONFIG_LOG_T;

//
// Full PM Export Config (v2.1 layout — no Vmin, NocIdle, SPT, Wdt, Opp100m)
//
typedef struct {
  PM_CONFIG_PMIC_T     PmicConfig;
  PM_CONFIG_PVT_T      PvtConfig;
  PM_CONFIG_OPP_T      OppConfig;
  PM_CONFIG_FAN_T      FanConfig[MAX_FAN_NUM];
  PM_CONFIG_LOG_T      LogConfig;
  UINT8                Reserved[115];
} PM_EXPORT_CONFIG_T;

//
// CRC wrapper structure (v2.1: CRC fields are at the END, not before Config)
//
// Binary layout:
//   VersionMajor(4) | VersionMinor(4) | Timestamp(4) | Signature(4)
//   | Config(...) | Padding(to 4-byte align) | Crc1(4) | Crc2(4)
//
// We access the CRC by computing offset from the buffer, not struct fields,
// since the padding between Config and CRC depends on sizeof(PM_EXPORT_CONFIG_T).
//
typedef struct {
  UINT32             VersionMajor;
  UINT32             VersionMinor;
  UINT32             Timestamp;
  UINT32             Signature;
  PM_EXPORT_CONFIG_T Config;
  // padding to 4-byte alignment follows (computed at runtime)
  // then Crc1 (UINT32), Crc2 (UINT32)
} PM_EXPORT_CONFIG_CRC_T;

//
// Offset of CRC fields from start of binary
//
#define PM_CRC_STRUCT_SIZE_UNALIGNED  (sizeof(PM_EXPORT_CONFIG_CRC_T))
#define PM_CRC_STRUCT_SIZE_ALIGNED    ((PM_CRC_STRUCT_SIZE_UNALIGNED + 3) & ~3)
#define PM_CRC1_OFFSET                PM_CRC_STRUCT_SIZE_ALIGNED
#define PM_CRC2_OFFSET                (PM_CRC_STRUCT_SIZE_ALIGNED + 4)
#define PM_TOTAL_STRUCT_SIZE          (PM_CRC_STRUCT_SIZE_ALIGNED + 8)

#pragma pack()

#endif // _PM_CONFIG_UPDATE_DXE_H_
