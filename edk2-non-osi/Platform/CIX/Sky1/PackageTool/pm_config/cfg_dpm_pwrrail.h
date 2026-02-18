/*
 * Copyright 2024 - Cix Technology Group Co., Ltd. All Rights Reserved.
 */

#ifndef __CFG_DPM_PWRRAIL_H__
#define __CFG_DPM_PWRRAIL_H__

typedef enum {
    DPM_EDP_CPU_LIT = 0,
    DPM_EDP_CPU_GM0,
    DPM_EDP_CPU_GM1,
    DPM_EDP_CPU_GB0,
    DPM_EDP_CPU_GB1,
    DPM_EDP_DSU,
    DPM_EDP_GPU,
    DPM_EDP_SOC,

    DPM_EDP_MAX
} DPM_EDP_DOMAIN;

#define VR_FIXED    0
#define VR_MP5475   1
#define VR_MP2845   2   // SVI

typedef struct {
    uint32_t vr_type  :  3;    // 0: VR (fixed voltage); 1: mp5475; 2: mp2845b; all others: reserved
    uint32_t pwr_cap  : 16;    // mW; set to 0 to disable the domain
    uint32_t i2c_port :  3;    // 0: i2c0, 1: i2c1, 2: i2c2, 3: i2c3
    uint32_t i2c_addr :  7;    // 7-bit address
    uint32_t i2c_buck :  3;    // 0: buck0, 1: buck1, 2: buck2, 3: buck3

    uint32_t vboot_mV : 12;    // mV
    int32_t  delta_mV : 10;    // +/- Max. 500mV; power rail guard band
    uint32_t rsvd1    : 10;
} DPM_PWR_RAIL_CFG;

typedef enum {
    SPT_CTRL_FAST = 0,
    SPT_CTRL_SLOW,
    SPT_CTRL_CHASSIS_T,
    SPT_CTRL_DIE_T,

    SPT_CTRL_MAX
} SPT_CTRL_TYPE;

#define CONFIG_EDP_CFG_CUSTOM  0

#endif
