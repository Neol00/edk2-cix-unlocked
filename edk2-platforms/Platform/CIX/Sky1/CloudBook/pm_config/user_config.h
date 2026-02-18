/*
 * Copyright 2024 - Cix Technology Group Co., Ltd. All Rights Reserved.
 */
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "pm_export_config.h"

#ifndef PM_PVT_SENSOR_CONFIG
#define PM_PVT_SENSOR_CONFIG 0
#endif
#if PM_PVT_SENSOR_CONFIG
static pm_config_pvt_t pvt_config = {
    .thermal_trip_soc = {
        .fields = {
            .valid    = PM_CONFIG_VALID,
            .raw_data = 105,
        }
    },
    .weight_valid = PM_CONFIG_INVALID,
    .board_sensor1 = {
        .sensor_valid ={
            .fields = {
                .valid          = PM_CONFIG_VALID,
                .raw_data       = 0,    // connect to EC;
            }
        },
        .reg_id    = 0x11
    },
    .board_sensor2 = {
        .sensor_valid ={
            .fields = {
                .valid          = PM_CONFIG_VALID,
                .raw_data       = 1,    //connect to soc i2c;
            }
        },
        .i2c_ctrl    = 0,               // valid ctrl are 0/1/2/3
        .i2c_addr    = 0x45,
    }
};
#endif

#ifndef PM_PMIC_CONFIG
#define PM_PMIC_CONFIG 1
#endif
#if PM_PMIC_CONFIG
static pm_config_pmic_t pmic_config = {
	.pmic_scheme = {
		.fields = {
            .valid    = PM_CONFIG_VALID,
            .raw_data = CONFIG_EDP_CFG_CUSTOM,
        },
	},
    /*
     * .opp_max and .edp_cfg can take effect only if pmic_scheme is valud and set to CONFIG_EDP_CFG_CUSTOM
     */
       /* Fmax of         lit,          gm0,          gm1,          gb0,          gb1,          dsu */
    .opp_max = {            0,         2000,            0,            0,         2200,         1100,
        /*                g_t,          g_c,           ci,           mm,          vpu,          npu,          mem (not taking effect)  */
                          600,          600,          800,          375,            0,          300, OPP_NO_LIMIT },
    .edp_cfg = {
        /*
         * vr_type can be one of the following:
         *   0 - FIXED
         *   1 - MP5475 (PMIC)
         *   2 - MP2845 (SVI, VR)
         *
         * pwr_cap is in mW, set to 0 to disable the domain (DSU, GPU, SOC cannot be disabled)
         *
         * i2c_port, i2c_addr, i2c_buck are used to identify the power rail, need to be set properly by following the HW design
         *
         * delta_mV is the power rail guard band, +/- Max. 500mV, which will be applied against the built OPP table
         */
        [DPM_EDP_CPU_LIT] = { .vr_type = VR_FIXED,  .pwr_cap =     0, .i2c_port = 0, .i2c_addr =    0, .i2c_buck = 0, .vboot_mV = 790, .delta_mV = 0 }, // (Optional) fixed voltage
        [DPM_EDP_CPU_GM0] = { .vr_type = VR_MP2845, .pwr_cap =  6000, .i2c_port = 0, .i2c_addr = 0x45, .i2c_buck = 1, .vboot_mV = 790, .delta_mV = 0 }, //
        [DPM_EDP_CPU_GM1] = { .vr_type = VR_FIXED,  .pwr_cap =     0, .i2c_port = 0, .i2c_addr =    0, .i2c_buck = 0, .vboot_mV = 790, .delta_mV = 0 }, // off
        [DPM_EDP_CPU_GB0] = { .vr_type = VR_FIXED,  .pwr_cap =     0, .i2c_port = 0, .i2c_addr =    0, .i2c_buck = 0, .vboot_mV = 790, .delta_mV = 0 }, // off
        [DPM_EDP_CPU_GB1] = { .vr_type = VR_MP2845, .pwr_cap =  6000, .i2c_port = 0, .i2c_addr = 0x45, .i2c_buck = 0, .vboot_mV = 790, .delta_mV = 0 }, //
        [DPM_EDP_DSU]     = { .vr_type = VR_MP2845, .pwr_cap =  5500, .i2c_port = 0, .i2c_addr = 0x45, .i2c_buck = 2, .vboot_mV = 790, .delta_mV = 0 }, //
        [DPM_EDP_GPU]     = { .vr_type = VR_MP2845, .pwr_cap =  6000, .i2c_port = 0, .i2c_addr = 0x45, .i2c_buck = 3, .vboot_mV = 790, .delta_mV = 0 }, //
        [DPM_EDP_SOC]     = { .vr_type = VR_FIXED,  .pwr_cap =  8000, .i2c_port = 0, .i2c_addr =    0, .i2c_buck = 0, .vboot_mV = 790, .delta_mV = 0 }, // SYV138D3RHC
    },
};

#define PMIC_CONFIG_SIZE    sizeof(pmic_config)
#endif

#ifndef PM_FAN_TABLE_CONFIG
#define PM_FAN_TABLE_CONFIG 0
#endif

#if PM_FAN_TABLE_CONFIG
static pm_config_fan_t fan_config[MAX_FAN_NUM] = {
	[0] = {
        .fan_valid = {
            .fields = {
                .valid = PM_CONFIG_VALID,
            }
        },
        .rpm_table_valid = {
            PM_CONFIG_VALID,   // valid bit of NORMAL table
            PM_CONFIG_VALID,   // valid bit of PERFORMANCE table
            PM_CONFIG_VALID,   // valid bit of QUIET table
        }, /* table valid bit of NORMAL/PERF/QUIET respectively */

        .rpm_table_items = {
            6,                   // table size of NORMAL,      i.e. the valid items number of fan_config[0].rpm_table[FAN_MODE_NORMAL]
            4,                   // table size of PERFORMANCE, i.e. the valid items number of fan_config[0].rpm_table[FAN_MODE_PERFORMANCE]
            4,                   // table size of QUIET,       i.e. the valid items number of fan_config[0].rpm_table[FAN_MODE_QUIET]
        }, /* valid items number of NORMAL/PERF/QUIET respectively */

        /* fan tables definition */
        .rpm_table[FAN_MODE_NORMAL] = {
              /* RPM    temp      temp
               *        increase  decrease */
                { 0,       0,      20 },
                { 1100,    40,     25 },
                { 1600,    50,     46 },
                { 2700,    60,     56 },
                { 4100,    65,     62 },
                { 5500,    72,     67 },
            },
        .rpm_table[FAN_MODE_PERFORMANCE] = {
                {    0,     0,     20 },
                { 2700,    60,     56 },
                { 4100,    65,     62 },
                { 5500,    72,     67 },
            },
        .rpm_table[FAN_MODE_QUIET] = {
                {    0,     0,     20 },
                { 1100,    40,     25 },
                { 1600,    50,     46 },
                { 2700,    60,     56 },
            },

        .fan_id = {
            .fields = {
                .valid = PM_CONFIG_VALID,
                .raw_data = 0, // which pwm&tach is used to control the fan, it should be correspond to hardware design. 0/1/2 are valid value.
            }
        },
        .fan_polarity = {
            .fields = {
                .valid = PM_CONFIG_VALID,
                .raw_data = 2, // it equals tachmeter counter value(per minute) divided by the real rpm;
            }
        },
        .scaleup_margin = {
            .fields = {
                .valid = PM_CONFIG_VALID,
                .raw_data = 500, //if the difference between immediate rpm and expected rpm is greater than raw_data,
                                 //pwm duty will be adjusted in coarse granularity to accelarate the progress,
            }                    // the value depends on the fan's characteristic, (max rpm / 10) ~ (max rpm / 5) is recommended
        },
        .pwm_freq = {
            .fields = {
                .valid = PM_CONFIG_VALID,
                .raw_data = 100, // the pwm frequence. depending on the fan's characteristic, 50~100 is recommended.
            }
        }
    },

    [1] = {
        .fan_valid = {
            .fields = {
                .valid = PM_CONFIG_INVALID,
            }
        },
        .rpm_table_valid = {
            PM_CONFIG_INVALID,   // valid bit of NORMAL table
            PM_CONFIG_INVALID,   // valid bit of PERFORMANCE table
            PM_CONFIG_INVALID,   // valid bit of QUIET table
        }, /* table valid bit of NORMAL/PERF/QUIET respectively */
        .fan_id = {
            .fields = {
                .valid = PM_CONFIG_INVALID,
            }
        },
        .fan_polarity = {
            .fields = {
                .valid = PM_CONFIG_INVALID,
            }
        },
        .scaleup_margin = {
            .fields = {
                .valid = PM_CONFIG_INVALID,
            }
        },
        .pwm_freq = {
            .fields = {
                .valid = PM_CONFIG_INVALID,
            }
        }
    }
};
#define FAN_CONFIG_SIZE    sizeof(fan_config)
#endif

#ifndef PM_LOG_CONFIG
#define PM_LOG_CONFIG 1
#endif
#if PM_LOG_CONFIG
#define LOG_OVER_UART_MAGIC  0x2DBA0C1D
static pm_config_log_t log_config = {
    .log_enable = {
        .fields = {
            .valid    = PM_CONFIG_VALID,
            .raw_data = LOG_OVER_UART_MAGIC,
        }
    },
    .uart_baudrate = {
        .fields = {
            .valid    = PM_CONFIG_VALID,
            .raw_data = 115200,
        }
    }
};
#endif

#ifndef PM_VMIN_CONFIG
#define PM_VMIN_CONFIG 1
#endif
#if PM_VMIN_CONFIG
#define VMIN_DISABLE_MAGIC 0x6BCD12CE
static pm_config_vmin_t vmin_config = {
    .vmin_disable = {
        .fields = {
            .valid    = PM_CONFIG_VALID,
            .raw_data = VMIN_DISABLE_MAGIC,
        }
    },
    .vmin_profile = {
        .fields = {
            .valid    = PM_CONFIG_VALID,
            .raw_data = 0,  // round-up + index 0 (3%)
        }
    },
};
#endif

#ifndef PM_NOC_IDLE_CONFIG
#define PM_NOC_IDLE_CONFIG 1
#endif
#if PM_NOC_IDLE_CONFIG
static pm_config_noc_idle_t noc_idle_config = {
    .noc_idle_enable = {
        .fields = {
            .valid    = PM_CONFIG_VALID,
            .raw_data = 1,  // 0: disable, 1: enable
        }
    },
    /* default threshold are loaded if not specificed below */
    .noc_idle_amu1_thro0 = {
        .fields = { .valid = PM_CONFIG_INVALID }
    },
    .noc_idle_amu1_thro1 = {
        .fields = { .valid = PM_CONFIG_INVALID }
    },
    .noc_idle_ddr_dym_pwr_thro = {
        .fields = { .valid = PM_CONFIG_INVALID }
    },
    .noc_idle_hysteresis = {
        .fields = { .valid = PM_CONFIG_INVALID }
    },
};
#endif

#endif // end of __USER_CONFIG_H__
