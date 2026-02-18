/*
 * Copyright 2024 - Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.
 */
#ifndef __PMIC_CONFIG_H__
#define __PMIC_CONFIG_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "pm_export_config.h"
#include "cfg_dpm_pwrrail.h"

#pragma pack(push, 1)

#define OPP_DXS_MAX         13
#define OPP_PWR_RAIL_MAX    8
typedef struct {
    config_data_t       pmic_scheme;
    uint16_t            opp_max[OPP_DXS_MAX];
    DPM_PWR_RAIL_CFG    edp_cfg[OPP_PWR_RAIL_MAX];
} shadow_pm_config_pmic_t;

#pragma pack(pop)

static shadow_pm_config_pmic_t pmic_config = {
	.pmic_scheme = {
		.fields = {
            .valid = PM_CONFIG_VALID,
            .raw_data = DPM_EDP_CFG_PHECDA,
        },
	},
    .opp_max = { 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000 },
    .edp_cfg = {
        [DPM_EDP_CPU_LIT] = { .vr_type = VR_MP2845, .pwr_cap =  2400, .i2c_port = 0, .i2c_addr = 0x45, .i2c_buck = 1, .vboot_mV = 750, .delta_mV = 0 },
        [DPM_EDP_CPU_GM0] = { .vr_type = VR_MP2845, .pwr_cap =  6700, .i2c_port = 1, .i2c_addr = 0x45, .i2c_buck = 2, .vboot_mV = 750, .delta_mV = 0 },
        [DPM_EDP_CPU_GM1] = { .vr_type = VR_MP2845, .pwr_cap =  6500, .i2c_port = 0, .i2c_addr = 0x45, .i2c_buck = 0, .vboot_mV = 750, .delta_mV = 0 },
        [DPM_EDP_CPU_GB0] = { .vr_type = VR_MP2845, .pwr_cap =  8000, .i2c_port = 0, .i2c_addr = 0x45, .i2c_buck = 2, .vboot_mV = 750, .delta_mV = 0 },
        [DPM_EDP_CPU_GB1] = { .vr_type = VR_MP2845, .pwr_cap =  8200, .i2c_port = 0, .i2c_addr = 0x45, .i2c_buck = 3, .vboot_mV = 750, .delta_mV = 0 },
        [DPM_EDP_DSU]     = { .vr_type = VR_MP2845, .pwr_cap =  5500, .i2c_port = 1, .i2c_addr = 0x45, .i2c_buck = 3, .vboot_mV = 750, .delta_mV = 0 },
        [DPM_EDP_GPU]     = { .vr_type = VR_MP2845, .pwr_cap = 12000, .i2c_port = 1, .i2c_addr = 0x45, .i2c_buck = 1, .vboot_mV = 750, .delta_mV = 0 },
        [DPM_EDP_SOC]     = { .vr_type = VR_MP2845, .pwr_cap =  9000, .i2c_port = 1, .i2c_addr = 0x45, .i2c_buck = 0, .vboot_mV = 750, .delta_mV = 0 },
    },
};

#define PMIC_CONFIG_OFFSET	12
#define PMIC_CONFIG_SIZE    sizeof(pmic_config)

#endif
