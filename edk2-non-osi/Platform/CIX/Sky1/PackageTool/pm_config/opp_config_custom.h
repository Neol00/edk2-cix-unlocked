/*
 * Copyright 2024 - Cix Technology Group Co., Ltd. All Rights Reserved.
 */
#ifndef __OPP_CONFIG_H__
#define __OPP_CONFIG_H__

#include "pm_export_config.h"
#include "opp_config.h"

/*
 * SoC voltage offset applied on top of the fixed 750 mV rail.
 * Range: -500 to +500 mV. Higher values help stability when overclocking.
 */
#define SOC_DELTA_MV  0

/* Per-domain voltage offsets (applied to OPP table voltages) */
#define CPU_LIT_DELTA_MV  0
#define CPU_GM0_DELTA_MV  0
#define CPU_GM1_DELTA_MV  0
#define CPU_GB0_DELTA_MV  0
#define CPU_GB1_DELTA_MV  0
#define DSU_DELTA_MV      0
#define GPU_DELTA_MV      10

#define PM_OPP_TABLE_CONFIG   1

#if PM_OPP_TABLE_CONFIG
/* V1.1, DFS */
static domain_opp_config_t dxs_gc = {
    .size = 8,
    .sustained_idx = 0,
    .opp_table = {
        { .level = 250UL, .frequency = 250000, .voltage = 800 }, /* safe sustained (hidden) */
        { .level = 250UL, .frequency = 250000, .voltage = 800 },
        { .level = 350UL, .frequency = 350000, .voltage = 800 },
        { .level = 600UL, .frequency = 600000, .voltage = 800 },
        { .level = 800UL, .frequency = 800000, .voltage = 800 },
        { .level = 1000UL,                     .voltage = 800 },
        { .level = 1100UL,                     .voltage = 800 },
        { .level = 1100UL,                     .voltage = 800 },
    },
};

static domain_opp_config_t dxs_gt = {
    .size = 7,
    .sustained_idx = 0,
    .opp_table = {
        { .level = 250UL, .frequency = 250000, .voltage = 800 }, /* safe sustained (hidden) */
        { .level = 250UL, .frequency = 250000, .voltage = 800 },
        { .level = 350UL, .frequency = 350000, .voltage = 800 },
        { .level = 600UL, .frequency = 600000, .voltage = 800 },
        { .level = 800UL, .frequency = 800000, .voltage = 800 },
        { .level = 1000UL,                     .voltage = 800 },
        { .level = 1000UL,                     .voltage = 800 },
    },
};

static domain_opp_config_t dxs_lit = {
    .size = 8,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL, .voltage = 790 },   /* safe sustained (hidden) */
        { .level =  800UL, .voltage = 790 },
        { .level = 1000UL, .voltage = 790 },
        { .level = 1200UL, .voltage = 800 },
        { .level = 1400UL, .voltage = 810 },
        { .level = 1600UL, .voltage = 820 },
        { .level = 1800UL, .voltage = 850 },
        { .level = 2200UL, .voltage = 930 },
    },
};

static domain_opp_config_t dxs_gb0 = {
    .size = 8,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL, .voltage = 790 },   /* safe sustained (hidden) */
        { .level =  800UL, .voltage = 790 },
        { .level = 1200UL, .voltage = 800 },
        { .level = 1500UL, .voltage = 810 },
        { .level = 1800UL, .voltage = 820 },
        { .level = 2200UL, .voltage = 830 },
        { .level = 2400UL, .voltage = 860 },
        { .level = 2600UL, .voltage = 930 },
    },
};

static domain_opp_config_t dxs_gb1 = {
    .size = 8,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL, .voltage = 790 },   /* safe sustained (hidden) */
        { .level =  800UL, .voltage = 790 },
        { .level = 1200UL, .voltage = 800 },
        { .level = 1500UL, .voltage = 810 },
        { .level = 1800UL, .voltage = 820 },
        { .level = 2200UL, .voltage = 830 },
        { .level = 2400UL, .voltage = 860 },
        { .level = 2600UL, .voltage = 930 },
    },
};

static domain_opp_config_t dxs_gm0 = {
    .size = 8,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL, .voltage = 790 },   /* safe sustained (hidden) */
        { .level =  800UL, .voltage = 790 },
        { .level = 1200UL, .voltage = 800 },
        { .level = 1500UL, .voltage = 810 },
        { .level = 1800UL, .voltage = 820 },
        { .level = 2000UL, .voltage = 820 },
        { .level = 2200UL, .voltage = 860 },
        { .level = 2400UL, .voltage = 930 },
    },
};

static domain_opp_config_t dxs_gm1 = {
    .size = 8,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL, .voltage = 790 },   /* safe sustained (hidden) */
        { .level =  800UL, .voltage = 790 },
        { .level = 1200UL, .voltage = 800 },
        { .level = 1500UL, .voltage = 810 },
        { .level = 1800UL, .voltage = 820 },
        { .level = 2000UL, .voltage = 820 },
        { .level = 2200UL, .voltage = 860 },
        { .level = 2400UL, .voltage = 930 },
    },
};

static domain_opp_config_t dxs_dsu = {
    .size = 3,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  400UL, .voltage = 790 },   /* safe sustained (hidden) */
        { .level =  400UL, .voltage = 790 },
        { .level = 1300UL, .voltage = 790 },
    },
};

static domain_opp_config_t dxs_npu = {
    .size = 5,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  400UL },   /* safe sustained (hidden) */
        { .level =  400UL },
        { .level =  600UL },
        { .level =  800UL },
        { .level = 1200UL },
    },
};

static domain_opp_config_t dxs_vpu = {
    .size = 7,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  150UL },   /* safe sustained (hidden) */
        { .level =  150UL },
        { .level =  300UL },
        { .level =  480UL },
        { .level =  600UL },
        { .level =  800UL },
        { .level = 1200UL },
    },
};

static domain_opp_config_t dxs_ci = {
    .size = 1,
    .sustained_idx = 0,
    .opp_table = {
        { .level = 1500UL },
    },
};

static domain_opp_config_t dxs_mm = {
    .size = 4,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  350UL },   /* safe sustained (hidden) */
        { .level =  350UL },
        { .level =  600UL },
        { .level =  750UL },
    },
};

static domain_opp_config_t *dom_opps[DVFS_ELEMENT_IDX_COUNT] = {
    [DVFS_ELEMENT_IDX_GPU_CORE] = &dxs_gc,
    [DVFS_ELEMENT_IDX_GPU_TOP]  = &dxs_gt,
    [DVFS_ELEMENT_IDX_LITTLE]   = &dxs_lit,
    [DVFS_ELEMENT_IDX_BIG_G0]   = &dxs_gb0,
    [DVFS_ELEMENT_IDX_BIG_G1]   = &dxs_gb1,
    [DVFS_ELEMENT_IDX_MID_G0]   = &dxs_gm0,
    [DVFS_ELEMENT_IDX_MID_G1]   = &dxs_gm1,
    [DVFS_ELEMENT_IDX_DSU]      = &dxs_dsu,
    [DVFS_ELEMENT_IDX_NPU]      = &dxs_npu,
    [DVFS_ELEMENT_IDX_VPU]      = &dxs_vpu,
    [DVFS_ELEMENT_IDX_CI700]    = &dxs_ci,
    [DVFS_ELEMENT_IDX_MMHUB]    = &dxs_mm,
};
#endif

#endif
