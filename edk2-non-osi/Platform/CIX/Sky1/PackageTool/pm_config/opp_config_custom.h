/*
 * Copyright 2024 - Cix Technology Group Co., Ltd. All Rights Reserved.
 */
#ifndef __OPP_CONFIG_H__
#define __OPP_CONFIG_H__

#include "pm_export_config.h"
#include "opp_config.h"

#define PM_OPP_TABLE_CONFIG   0

#if PM_OPP_TABLE_CONFIG
/* V1.1, DFS */
static domain_opp_config_t dxs_gc = {
    .size = 6,
    .sustained_idx = 5,
    .opp_table = {
        { .level = 72UL,  .frequency = 350000, .voltage = 800 }, // 350MHz, 2 core
        { .level = 216UL, .frequency = 350000, .voltage = 800 }, // 350MHz, 6 core
        { .level = 350UL, .frequency = 350000, .voltage = 800 }, // 350MHz, 10 core
        { .level = 600UL,                      .voltage = 800 }, // 600MHz, 10 core
        { .level = 800UL,                      .voltage = 800 }, // 800MHz, 10 core
        { .level = 1100UL,                     .voltage = 800 },
    },
};

static domain_opp_config_t dxs_gt = {
    .size = 6,
    .sustained_idx = 5,
    .opp_table = {
        { .level = 72UL,  .frequency = 350000, .voltage = 800 }, // 350MHz, 2 core
        { .level = 216UL, .frequency = 350000, .voltage = 800 }, // 350MHz, 6 core
        { .level = 350UL, .frequency = 350000, .voltage = 800 }, // 350MHz, 10 core
        { .level = 600UL,                      .voltage = 800 }, // 600MHz, 10 core
        { .level = 800UL,                      .voltage = 800 }, // 800MHz, 10 core
        { .level = 1000UL,                     .voltage = 800 },
    },
};

static domain_opp_config_t dxs_lit = {
    .size = 2,
    .sustained_idx = 1,
    .opp_table = {
        { .level =  800UL, .voltage = 790, .power = 138 },
        { .level = 1800UL, .voltage = 790, .power = 790 },
    },
};

static domain_opp_config_t dxs_gb0 = {
    .size = 7,
    .sustained_idx = 3,
    .opp_table = {
        { .level =  800UL, .voltage = 750, .power =  156 },
        { .level = 1200UL, .voltage = 750, .power =  372 },
        { .level = 1500UL, .voltage = 750, .power =  614 },
        { .level = 1800UL, .voltage = 790, .power =  841 },   /* sustained */
        { .level = 2200UL, .voltage = 790, .power = 1360 },
        { .level = 2400UL, .voltage = 850, .power = 1663 },
        { .level = 2500UL, .voltage = 920, .power = 2292 },
    },
};

static domain_opp_config_t dxs_gb1 = {
    .size = 7,
    .sustained_idx = 3,
    .opp_table = {
        { .level =  800UL, .voltage = 750, .power =  156 },
        { .level = 1200UL, .voltage = 750, .power =  372 },
        { .level = 1500UL, .voltage = 750, .power =  614 },
        { .level = 1800UL, .voltage = 790, .power =  841 },   /* sustained */
        { .level = 2200UL, .voltage = 790, .power = 1360 },
        { .level = 2500UL, .voltage = 850, .power = 1663 },
        { .level = 2600UL, .voltage = 920, .power = 2292 },
    },
};

static domain_opp_config_t dxs_gm0 = {
    .size = 7,
    .sustained_idx = 3,
    .opp_table = {
        { .level =  800UL, .voltage = 750, .power =  149 },
        { .level = 1200UL, .voltage = 750, .power =  355 },
        { .level = 1500UL, .voltage = 750, .power =  584 },
        { .level = 1800UL, .voltage = 790, .power =  799 },   /* sustained */
        { .level = 2100UL, .voltage = 790, .power = 1114 },
        { .level = 2200UL, .voltage = 850, .power = 1292 },
        { .level = 2300UL, .voltage = 890, .power = 1396 },
    },
};

static domain_opp_config_t dxs_gm1 = {
    .size = 6,
    .sustained_idx = 3,
    .opp_table = {
        { .level =  800UL, .voltage = 750, .power =  149 },
        { .level = 1200UL, .voltage = 750, .power =  355 },
        { .level = 1500UL, .voltage = 750, .power =  584 },
        { .level = 1800UL, .voltage = 790, .power =  799 },   /* sustained */
        { .level = 2100UL, .voltage = 850, .power = 1114 },
        { .level = 2200UL, .voltage = 890, .power = 1292 },
    },
};

static domain_opp_config_t dxs_dsu = {
    .size = 2,
    .sustained_idx = 1,
    .opp_table = {
        { .level =  500UL, .voltage = 790 },
        { .level = 1300UL, .voltage = 790 },   /* sustained */
    },
};

static domain_opp_config_t dxs_npu = {
    .size = 4,
    .sustained_idx = 2,
    .opp_table = {
        { .level =  400UL },
        { .level =  600UL },
        { .level =  800UL },
        { .level = 1200UL },
    },
};

static domain_opp_config_t dxs_vpu = {
    .size = 6,
    .sustained_idx = 5,
    .opp_table = {
        { .level =  150UL },
        { .level =  300UL },
        { .level =  480UL },
        { .level =  600UL },
        { .level =  800UL },
        { .level = 1200UL },
    },
};

static domain_opp_config_t dxs_ci = {
    .size = 2,
    .sustained_idx = 1,
    .opp_table = {
        { .level =  500UL },
        { .level = 1500UL },
    },
};

static domain_opp_config_t dxs_mm = {
    .size = 3,
    .sustained_idx = 2,
    .opp_table = {
        { .level =  375UL },
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
