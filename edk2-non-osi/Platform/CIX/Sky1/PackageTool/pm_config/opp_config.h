/*
 * Copyright 2024 - Cix Technology Group Co., Ltd. All Rights Reserved.
 */
#ifndef __OPP_CONFIG_DEF_H__
#define __OPP_CONFIG_DEF_H__

#include <stdint.h>

enum dvfs_element_idx {
    DVFS_ELEMENT_IDX_GPU_CORE, // put CORE prior to TOP as
                               //   1. driver only sets CORE in DVFS flow
                               //   2. to adapt gpuperf plugin, it can influence the TOP domain
                               //      limit just after CORE opp is set.
    DVFS_ELEMENT_IDX_GPU_TOP,

    /* !! NOTE !!
     *     The order of CPU and DSU are critical for DSU sustained level patch
     *     write in dvfs_get_element_table() can work correctly.
     */
    DVFS_ELEMENT_IDX_LITTLE,   // 4 x little cores
    DVFS_ELEMENT_IDX_BIG_G0,
    DVFS_ELEMENT_IDX_BIG_G1,
    DVFS_ELEMENT_IDX_MID_G0,
    DVFS_ELEMENT_IDX_MID_G1,
    DVFS_ELEMENT_IDX_DSU,

    // devices DFS
    DVFS_ELEMENT_IDX_NPU,
    DVFS_ELEMENT_IDX_VPU,
    DVFS_ELEMENT_IDX_CI700,
    DVFS_ELEMENT_IDX_MMHUB,

//  DVFS_ELEMENT_IDX_SOC,      // fix at 0.75v, SOC is not treated as a perf domain
    DVFS_ELEMENT_IDX_COUNT
};

#endif
