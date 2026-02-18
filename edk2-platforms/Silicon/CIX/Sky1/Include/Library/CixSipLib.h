/*
 * Copyright (c) 2016-2019,2021, ARM Limited and Contributors. All rights reserved.
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ARM_SIP_SVC_H
#define ARM_SIP_SVC_H
#include <Uefi/UefiBaseType.h>


/* SMC function IDs for SiP Service queries */

#define ARM_SIP_SVC_CALL_COUNT		U(0x8200ff00)
#define ARM_SIP_SVC_UID			U(0x8200ff01)
/*					U(0x8200ff02) is reserved */
#define ARM_SIP_SVC_VERSION		U(0x8200ff03)

/* PMF_SMC_GET_TIMESTAMP_32		0x82000010 */
/* PMF_SMC_GET_TIMESTAMP_64		0xC2000010 */

/* Function ID for requesting state switch of lower EL */
#define ARM_SIP_SVC_EXE_STATE_SWITCH	U(0x82000020)

/* DEBUGFS_SMC_32			0x82000030U */
/* DEBUGFS_SMC_64			0xC2000030U */

/*
 * Arm Ethos-N NPU SiP SMC function IDs
 * 0xC2000050-0xC200005F
 * 0x82000050-0x8200005F
 */

/*CIX Sip Service Call SCMI*/
#define CIX_SIP_SCMI		0xc2000001

/*Function ID for setting reboot reason*/
#define CIX_SIP_SVC_SET_REBOOT_REASON 0xc2000002
#define CIX_SIP_SVC_DSU_HW_CTL        0xc2000003
#define CIX_SIP_SVC_DSU_SET_PD        0xc2000004
#define CIX_SIP_SVC_DSU_GET_PD        0xc2000005
#define CIX_SIP_SVC_DSU_SET_TH        0xc2000006
#define CIX_SIP_SVC_DSU_GET_HITCNT    0xc2000007
#define CIX_SIP_SVC_DSU_GET_MISSCNT   0xc2000008
#define CIX_SIP_SVC_PDC_SET_WAKEUP    0xc2000009
#define CIX_SIP_POWER_DOMAIN_SET      0xc200000a
/* ARM SiP Service Calls version numbers */
#define ARM_SIP_SVC_VERSION_MAJOR		0x0
#define ARM_SIP_SVC_VERSION_MINOR		0x2

#define CIX_SIP_NOR_STORAGE             0xC200000B


//wakeup source
#define USB_C_SSP_0_HOST_IRQ                            262
#define USB_C_SSP_0_PERIPHERAL_IRQ                      262
#define USB_C_SSP_0_OTG_IRQ                             263

#define USB_C_SSP_1_HOST_IRQ                            268
#define USB_C_SSP_1_PERIPHERAL_IRQ                      268
#define USB_C_SSP_1_OTG_IRQ                             269

#define USB_C_SSP_2_HOST_IRQ                            274
#define USB_C_SSP_2_PERIPHERAL_IRQ                      274
#define USB_C_SSP_2_OTG_IRQ                             275

#define USB_C_SSP_3_HOST_IRQ                            280
#define USB_C_SSP_3_PERIPHERAL_IRQ                      280
#define USB_C_SSP_3_OTG_IRQ                             281

#define USB_SSP_0_HOST_IRQ                              252
#define USB_SSP_0_PERIPHERAL_IRQ                        252
#define USB_SSP_0_OTG_IRQ                               253

#define USB_SSP_1_HOST_IRQ                              257
#define USB_SSP_1_PERIPHERAL_IRQ                        257
#define USB_SSP_1_OTG_IRQ                               258

#define USB2_0_HOST_IRQ                                 240
#define USB2_0_PERIPHERAL_IRQ                           240
#define USB2_0_OTG_IRQ                                  241

#define USB2_1_HOST_IRQ                                 243
#define USB2_1_PERIPHERAL_IRQ                           243
#define USB2_1_OTG_IRQ                                  244

#define USB2_2_HOST_IRQ                                 246
#define USB2_2_PERIPHERAL_IRQ                           246
#define USB2_2_OTG_IRQ                                  247

#define USB2_3_HOST_IRQ                                 249
#define USB2_3_PERIPHERAL_IRQ                           249
#define USB2_3_OTG_IRQ                                  250

#define S5_GPIO_U0                                      372
#define S5_GPIO_U1                                      373
#define S5_GPIO_U2                                      374


VOID
WakeupCfg (
  IN UINT32 Source,
  IN BOOLEAN On
  );
#endif /* ARM_SIP_SVC_H */
