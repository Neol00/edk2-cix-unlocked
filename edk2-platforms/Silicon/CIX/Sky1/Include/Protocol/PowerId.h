/* SPDX-License-Identifier: GPL-2.0-only */

/*  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

 * All Rights Reserved.
 *
 * The following programs are the sole property of Cix Technology Group Co., Ltd.,
 * and contain its proprietary and confidential information.
 */

#ifndef __POWER_ID_H__
#define __POWER_ID_H__

/* The Rich OS need flow the macro */
#define PD_AUDIO       0
#define PD_PCIE_CTRL0  1
#define PD_PCIE_DUMMY  2
#define PD_PCIEHUB     3
#define PD_MMHUB       4
#define PD_MMHUB_SMMU  5
#define PD_DPU0        6
#define PD_DPU1        7
#define PD_DPU2        8
#define PD_DPU3        9
#define PD_DPU4        10
#define PD_VPU_TOP     11
#define PD_VPU_CORE0   12
#define PD_VPU_CORE1   13
#define PD_VPU_CORE2   14
#define PD_VPU_CORE3   15
#define PD_NPU_CORE0   16
#define PD_NPU_CORE1   17
#define PD_NPU_CORE2   18
#define PD_NPU_TOP     19
#define PD_ISP         20
#define PD_GPU         21
#define PD_MAX         32
// Message Type for POWER management protocol.
typedef enum {
  POWER_DOMAIN_ATTRIBUTES = 0x3,
  POWER_STATE_SET         = 0x4,
  POWER_STATE_GET         = 0x5,
  POWER_STATE_NOTIFY      = 0x6,
} SCMI_MESSAGE_ID_POWER;

typedef struct _POWER_PROTOCOL POWER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *POWER_DOMAIN_ON)(
  IN UINT32               Domain
  );

typedef
EFI_STATUS
(EFIAPI *POWER_DOMAIN_OFF)(
  IN UINT32               Domain
  );

typedef struct _POWER_PROTOCOL {
  POWER_DOMAIN_ON     PowerDomainPowerOn;
  POWER_DOMAIN_OFF    PowerDomainPowerOff;
} CIX_POWER_PROTOCOL;

#endif
