/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _HW_MUTEX_LIB_H__
#define _HW_MUTEX_LIB_H__

#include <Base.h>
#include <Uefi.h>

#define  FCH_LOCAL_RCSU_MUTEX_ID         0x00
#define  CSU_SE_LOCAL_RCSU_MUTEX_ID      0x01
#define  CSU_PM_LOCAL_RCSU_MUTEX_ID      0x02
#define  AUDIO_LOCAL_RCSU_MUTEX_ID       0x03
#define  SENSOR_HUB_LOCAL_RCSU_MUTEX_ID  0x04
#define  USB_C_0_LOCAL_RCSU_MUTEX_ID     0x05
#define  USB_C_1_LOCAL_RCSU_MUTEX_ID     0x06
#define  USB_C_2_LOCAL_RCSU_MUTEX_ID     0x07
#define  USB_C_3_LOCAL_RCSU_MUTEX_ID     0x08
#define  USB3_LOCAL_RCSU_MUTEX_ID        0x09
#define  USB2_0_LOCAL_RCSU_MUTEX_ID      0x0A
#define  USB2_1_LOCAL_RCSU_MUTEX_ID      0x0B
#define  USB2_2_LOCAL_RCSU_MUTEX_ID      0x0C
#define  USB2_3_LOCAL_RCSU_MUTEX_ID      0x0D
#define  ETH_LOCAL_RCSU_MUTEX_ID         0x0E
#define  PCIE_X8_LOCAL_RCSU_MUTEX_ID     0x0F
#define  PCIE_X4211_LOCAL_RCSU_MUTEX_ID  0x10
#define  SMMU0_LOCAL_RCSU_MUTEX_ID       0x11
#define  SMMU1_LOCAL_RCSU_MUTEX_ID       0x12
#define  SMMU2_LOCAL_RCSU_MUTEX_ID       0x13
#define  DDR_CTLER0_LOCAL_RCSU_MUTEX_ID  0x14
#define  DDR_CTLER1_LOCAL_RCSU_MUTEX_ID  0x15
#define  DDR_CTLER2_LOCAL_RCSU_MUTEX_ID  0x16
#define  DDR_CTLER3_LOCAL_RCSU_MUTEX_ID  0x17
#define  BROADCAST_LOCAL_RCSU_MUTEX_ID   0x18
#define  TZC400_0_LOCAL_RCSU_MUTEX_ID    0x19
#define  TZC400_1_LOCAL_RCSU_MUTEX_ID    0x1A
#define  TZC400_2_LOCAL_RCSU_MUTEX_ID    0x1B
#define  TZC400_3_LOCAL_RCSU_MUTEX_ID    0x1C
#define  PCIE_HUB_LOCAL_RCSU_MUTEX_ID    0x1D
#define  MMHUB_LOCAL_RCSU_MUTEX_ID       0x1E
#define  SYSTEM_HUB_LOCAL_RCSU_MUTEX_ID  0x1F
#define  SMN_LOCAL_RCSU_MUTEX_ID         0x20
#define  GICD_LOCAL_RCSU_MUTEX_ID        0x21
#define  CORE_PCSM_RCSU_LOCAL_MUTEX_ID   0x22
#define  CI_700_LOCAL_RCSU_MUTEX_ID      0x23
#define  DPU0_LOCAL_RCSU_MUTEX_ID        0x24
#define  DP0_LOCAL_RCSU_MUTEX_ID         0x25
#define  DPU1_LOCAL_RCSU_MUTEX_ID        0x26
#define  DP1_LOCAL_RCSU_MUTEX_ID         0x27
#define  DPU2_LOCAL_RCSU_MUTEX_ID        0x28
#define  DP2_LOCAL_RCSU_MUTEX_ID         0x29
#define  DPU3_LOCAL_RCSU_MUTEX_ID        0x2A
#define  DP3_LOCAL_RCSU_MUTEX_ID         0x2B
#define  DPU4_LOCAL_RCSU_MUTEX_ID        0x2C
#define  DP4_LOCAL_RCSU_MUTEX_ID         0x2D
#define  VPU_LOCAL_RCSU_MUTEX_ID         0x2E
#define  NPU_LOCAL_RCSU_MUTEX_ID         0x2F
#define  MIPI0_LOCAL_RCSU_MUTEX_ID       0x30
#define  MIPI1_LOCAL_RCSU_MUTEX_ID       0x31
#define  ISP_0_LOCAL_RCSU_MUTEX_ID       0x32
#define  ISP_1_LOCAL_RCSU_MUTEX_ID       0x33
#define  GPU_LOCAL_RCSU_MUTEX_ID         0x34

EFI_STATUS
EFIAPI
HwMutexLock (
  UINT32  Id
  );

EFI_STATUS
EFIAPI
HwMutexUnlock (
  UINT32  Id
  );

#endif // _HW_MUTEX_LIB_H__
