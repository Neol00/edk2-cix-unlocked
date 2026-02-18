/*
 * Copyright 2024 - Cix Technology Group Co., Ltd. All Rights Reserved.
 */
#ifndef __OPP_CONFIG_H__
#define __OPP_CONFIG_H__

#include "pm_export_config.h"
#include "opp_config.h"

#define PM_OPP_TABLE_CONFIG   1

#if PM_OPP_TABLE_CONFIG

static domain_opp_config_t dxs_gc = {
    .size = 2,
    .sustained_idx = 1,
    .opp_table = {
        { .level = 31UL,  .frequency = 300000, .voltage = 790 }, // 300MHz, 1 core
        { .level = 61UL,  .frequency = 600000, .voltage = 790 }, // 600MHz, 1 core
    },
};

static domain_opp_config_t dxs_gt = {
    .size = 2,
    .sustained_idx = 1,
    .opp_table = {
        { .level = 31UL,  .frequency = 300000, .voltage = 790 }, // 300MHz, 1 core
        { .level = 61UL,  .frequency = 600000, .voltage = 790 }, // 600MHz, 1 core
    },
};

// not used for cloudbook
static domain_opp_config_t dxs_lit = {
    .size = 1,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL, .voltage = 790 },
    },
};

// not used for cloudbook
static domain_opp_config_t dxs_gb0 = {
    .size = 1,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL, .voltage = 790 },
    },
};

static domain_opp_config_t dxs_gb1 = {
    .size = 7,
    .sustained_idx = 2,
    .opp_table = {
        /* the field of .power has highest priority than measured_power or calculated by cfg_pwr */
        { .level =  800UL, .voltage = 770, .power =  500 },
        { .level = 1200UL, .voltage = 820, .power = 1600 },
        { .level = 1500UL, .voltage = 830, .power = 2000 },
        { .level = 1900UL, .voltage = 830, .power = 2914 },   /* sustained */
        { .level = 2000UL, .voltage = 830, .power = 3142 },
        { .level = 2100UL, .voltage = 830, .power = 3371 },
        { .level = 2200UL, .voltage = 830, .power = 3600 },
    },
};

static domain_opp_config_t dxs_gm0 = {
    .size = 6,
    .sustained_idx = 2,
    .opp_table = {
        /* the field of .power has highest priority than measured_power or calculated by cfg_pwr */
        { .level =  800UL, .voltage = 770, .power =  500 },
        { .level = 1200UL, .voltage = 820, .power = 1500 },
        { .level = 1500UL, .voltage = 830, .power = 1900 },
        { .level = 1800UL, .voltage = 830, .power = 2450 },   /* sustained */
        { .level = 1900UL, .voltage = 830, .power = 2633 },
        { .level = 2000UL, .voltage = 830, .power = 3200 },
    },
};

// not used for cloudbook
static domain_opp_config_t dxs_gm1 = {
    .size = 1,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL, .voltage = 790 },
    },
};

static domain_opp_config_t dxs_dsu = {
    .size = 2,
    .sustained_idx = 1,
    .opp_table = {
        { .level =  500UL, .voltage = 790 },
        { .level = 1100UL, .voltage = 790 },   /* sustained */
    },
};

static domain_opp_config_t dxs_npu = {
    .size = 1,
    .sustained_idx = 0,
    .opp_table = {
        { .level = 300 },
    },
};

// not used for cloudbook
static domain_opp_config_t dxs_vpu = {
    .size = 1,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  300UL },
    },
};

static domain_opp_config_t dxs_ci = {
    .size = 1,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  800UL },
    },
};

static domain_opp_config_t dxs_mm = {
    .size = 1,
    .sustained_idx = 0,
    .opp_table = {
        { .level =  375UL },
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
