/*
 * Copyright 2024 - Cix Technology Group Co., Ltd. All Rights Reserved.
 */
#ifndef __PM_EXPORT_CONFIG_H__
#define __PM_EXPORT_CONFIG_H__

#include <stdint.h>

#define TABLE_SIGNATURE( x, y, m, n )                                              \
    ( uint32_t )( ( 0xFF & (uint8_t)( x ) ) | ( ( 0xFF & (uint8_t)( y ) ) << 8 ) | \
                  ( ( 0xFF & (uint8_t)( m ) ) << 16 ) | ( ( 0xFF & (uint8_t)( n ) ) << 24 ) )
#define TABLE_ALIGN_U32(_size)            (((_size) + 0x3) & ~0x3)
#define TABLE_ALIGN_U32_PADDING(_size)    (TABLE_ALIGN_U32(_size) - (_size))

#define PM_CONFIG_SIGNATURE      TABLE_SIGNATURE( 'P', 'M', 'C', 'F' )

/* pm-config current supported version (v2.6) */
#define PM_CONFIG_VERSION_MAJOR  2
#define PM_CONFIG_VERSION_MINOR  6

#include "cfg_dpm_pwrrail.h"

#pragma pack(push, 1)

/* basics */
typedef union {
    uint32_t data;
    struct {
        /*0: valid, 1: invalid*/
#define PM_CONFIG_VALID       0
#define PM_CONFIG_INVALID     1
        uint32_t valid      :   1;
        uint32_t raw_data   :   31;
    } fields;
} config_data_t;

/* PMIC */
#define OPP_DXS_MAX         13
#define OPP_PWR_RAIL_MAX    8
typedef struct {
    config_data_t    pmic_scheme;
#define OPP_NO_LIMIT        4000
    uint16_t         opp_max[OPP_DXS_MAX];
    DPM_PWR_RAIL_CFG edp_cfg[OPP_PWR_RAIL_MAX];
} pm_config_pmic_t;

typedef struct {
    config_data_t   sensor_valid;   //raw_data 0:connect to EC, 1:connect to soc i2c, others:invalid.
    union {
        uint32_t reg_id;
        struct {
            uint8_t i2c_ctrl;       // 0/1/2/3 is valid
            uint8_t i2c_addr;
            uint8_t sensor_type;
        };
    };
} board_sensor_config_t;

/* Thermal Sensor */
typedef struct {
    config_data_t   thermal_trip_soc;          // SoC thermal-trip trigger point in degree centigrade

    uint8_t         weight_valid;              // 0: the following union of weight is valid, 1: invalid
    union {
        uint8_t     weight[13];
        struct {
            uint8_t weight_vpu_sensor;         // Sensor 0
            uint8_t weight_gpu_bottom_sensor;  // Sensor 1
            uint8_t weight_gpu_top_sensor;     // Sensor 2
            uint8_t weight_soc_brc_sensor;     // Sensor 3  bottom-right corner
            uint8_t weight_ddr_bottom_sensor;  // Sensor 4
            uint8_t weight_ddr_top_sensor;     // Sensor 5
            uint8_t weight_ci700_sensor;       // Sensor 6
            uint8_t weight_npu_sensor;         // Sensor 7
            uint8_t weight_cpu_m1_sensor;      // Sensor 8
            uint8_t weight_cpu_b1_sensor;      // Sensor 9
            uint8_t weight_cpu_m0_sensor;      // Sensor 10
            uint8_t weight_cpu_b0_sensor;      // Sensor 11
            uint8_t weight_soc_trc_sensor;     // Sensor 12 top-right corner
        };
    };
    board_sensor_config_t board_sensor1;
    board_sensor_config_t board_sensor2;

} pm_config_pvt_t;

#define MAX_FAN_NUM (2)
#define MAX_FAN_TABLE_ENTRIES (9)
typedef enum {
    FAN_MODE_NORMAL = 0,
    FAN_MODE_PERFORMANCE,
    FAN_MODE_QUIET,
    FAN_MODE_MAX
} fan_mode_t;

typedef struct {
    uint16_t rpm;
    int8_t up_temp;
    int8_t down_temp;
} rpm_entry_t;

typedef struct pm_config_fan_config {
    config_data_t fan_valid;
    uint8_t       rpm_table_valid[FAN_MODE_MAX]; // 0:valid, 1:invalid (for flash space default 1)
    uint8_t       rpm_table_items[FAN_MODE_MAX];
    rpm_entry_t   rpm_table[FAN_MODE_MAX][MAX_FAN_TABLE_ENTRIES];
    config_data_t fan_id;
    config_data_t fan_polarity;
    config_data_t scaleup_margin;
    config_data_t pwm_freq;
} pm_config_fan_t;

/* OPP table */
#define DOMAIN_MAX_OPP_ENTRIES  13
#define DOMAIN_MAX_COUNT        13

typedef struct dvfs_opp {
    uint32_t    level;      /*!< Level value of the OPP */
    uint32_t    voltage;    /*!< Power supply voltage in millivolts (mV) */
    uint32_t    frequency;  /*!< Clock rate in Hertz (Hz) */
    uint32_t    power;      /*!< Power draw in milliwatts (mW) */
} dvfs_opp_t;

typedef struct domain_opp_config {
    uint16_t	size;
    uint16_t    sustained_idx;
    dvfs_opp_t  opp_table[DOMAIN_MAX_OPP_ENTRIES];
} domain_opp_config_t;

typedef struct pm_config_opp {
    uint8_t             opp_valid;
    domain_opp_config_t opps[DOMAIN_MAX_COUNT];
} pm_config_opp_t;

typedef struct pm_config_log {
    config_data_t       log_enable;
    config_data_t       uart_baudrate;
} pm_config_log_t;

typedef struct pm_config_vmin {
    config_data_t       vmin_disable;
    config_data_t       vmin_profile;
} pm_config_vmin_t;

typedef struct pm_config_noc_idle {
    config_data_t       noc_idle_enable;
    config_data_t       noc_idle_amu1_thro0;
    config_data_t       noc_idle_amu1_thro1;
    config_data_t       noc_idle_ddr_dym_pwr_thro;
    config_data_t       noc_idle_hysteresis;
} pm_config_noc_idle_t;

typedef struct pm_config_spt {
    config_data_t       spt_enable_mask;
    config_data_t       spt_setpoints[SPT_CTRL_MAX];
    config_data_t       spt_skin_margin;
} pm_config_spt_t;

typedef struct {
    pm_config_pmic_t        pmic_config;
    pm_config_pvt_t         pvt_config;
    pm_config_opp_t         opp_config;
    pm_config_fan_t         fan_config[MAX_FAN_NUM];
    pm_config_log_t         log_config;
    pm_config_vmin_t        vmin_config;
    pm_config_noc_idle_t    noc_idle_config;
    pm_config_spt_t         spt_config;
    config_data_t           wdt_timeout;
    uint8_t                 reserved[111];
} pm_export_config_t;

typedef struct {
    uint32_t                version_major : 16;
    uint32_t                version_minor : 16;

    uint32_t                timestamp;
    uint32_t                length;
    uint32_t                signature;

    uint32_t                crc1;
    uint32_t                crc2;

    pm_export_config_t      config;

    uint8_t                 padding[TABLE_ALIGN_U32_PADDING(sizeof(pm_export_config_t))];
} pm_export_config_crc_t;

#pragma pack(pop)

#endif
