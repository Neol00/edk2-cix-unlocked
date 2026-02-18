/*
 * Copyright 2024 - Cix Technology Group Co., Ltd. All Rights Reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>

#include "pm_export_config.h"

#include "opp_config_custom.h"
#include "user_config.h"

#define PM_CONFIG_BIN_SIZE  (4096)

#define ARRAY_LENGTH(_a)    (sizeof(_a) / sizeof(_a[0]))

#define PROJECT_NAME    "csu_pm"
#define VALID           (0)
#define INVALID         (1)
pm_export_config_crc_t  g_config;

time_t CixEpoch ()
{
    struct tm specific_time = {0};

    specific_time.tm_year = 2021 - 1900;   // 2021
    specific_time.tm_mon  = 10 - 1;        // Oct.
    specific_time.tm_mday = 15;            // 15th
    specific_time.tm_hour = 9;             // 9 AM
    specific_time.tm_min  = 0;             // 0 min
    specific_time.tm_sec  = 0;             // 0 sec

    return mktime(&specific_time);
}

_Static_assert(((sizeof(g_config)) & 0x3) == 0,
    "PM config block size must be 4-byte aligned!");

static bool double_check_sum(void * start, uint32_t length, uint64_t * sum64, bool do_check)
{
    uint32_t * ptr = (uint32_t *) start;
    uint32_t cka = 0, ckb = 0;

    assert((length & 0x3) == 0);

    for(uint32_t i = 0; i * 4 < length; i++) {
        cka += ptr[i];
        ckb += cka;
    }

    if (do_check) {
        if (sum64) {
            if ( (uint32_t)(*sum64 >> 32) == cka && (uint32_t)(*sum64) == ckb ) {
                return true;
            }
        }
    } else if (sum64) {
        *sum64 = ((uint64_t)cka << 32) + (uint64_t)ckb;
        return true;
    }

    return false;
}

static void dump_valid_flag(config_data_t data)
{
    if (data.fields.valid == PM_CONFIG_INVALID) {
        printf("INVALID\n");
    } else {
        printf("VALID, data: 0x%08X (%0d)\n", data.fields.raw_data, data.fields.raw_data);
    }
}

#if PM_OPP_TABLE_CONFIG
static uint32_t volt_abs(uint32_t a, uint32_t b)
{
    if (a >= b) {
        return a - b;
    } else {
        return b - a;
    }
}

static bool check_opp_table(void)
{
    domain_opp_config_t *config;
    uint32_t dsu_sust_freq;
    uint32_t dsu_sust_volt;
    size_t i;

    /* sustained index */
    for (i = 0; i < ARRAY_LENGTH(dom_opps); i++) {
        config = dom_opps[i];
        if (!config) {
            continue;
        }
        if (config->sustained_idx >= config->size) {
            printf("domain %zu: Bad sustained index %u; Must be lower than size %u\n",
                i, config->sustained_idx, config->size);
            return false;
        }
        if (config->size > DOMAIN_MAX_OPP_ENTRIES) {
            printf("domain %zu: OPP table too large. Must be smaller than %u\n",
                i, config->size);
            return false;
        }
    }

    /* DSU */
    config = dom_opps[DVFS_ELEMENT_IDX_DSU];
    if (!config) {
        printf("DSU domain OPP mising\n");
        return false;
    }

    dsu_sust_freq = config->opp_table[config->sustained_idx].frequency;
    dsu_sust_volt = config->opp_table[config->sustained_idx].voltage;

    for (i = 0; i < ARRAY_LENGTH(dom_opps); i++) {
        uint32_t freq;
        uint32_t volt;

        if (i < DVFS_ELEMENT_IDX_LITTLE || i > DVFS_ELEMENT_IDX_MID_G1) {
            continue;
        }
        config = dom_opps[i];
        freq = config->opp_table[config->sustained_idx].frequency;
        volt = config->opp_table[config->sustained_idx].voltage;

        if (volt_abs(volt, dsu_sust_volt) > 200) {
            printf("domain %zu: Bad sustained voltage %u.\n", i, volt);
            return false;
        }
        if (freq / 2 > dsu_sust_freq) {
            printf("domain %zu: Bad frequency %u. Too large\n", i, freq);
            return false;
        }
    }

    return true;
}
#endif

static void dump_pmic_config(pm_config_pmic_t* config)
{
    char tmpbuf[16];
    size_t i;

    printf("\tpmic scheme: ");dump_valid_flag(config->pmic_scheme);
    if (PM_CONFIG_VALID != config->pmic_scheme.fields.valid ||
        CONFIG_EDP_CFG_CUSTOM != config->pmic_scheme.fields.raw_data) {
        /* print pmic setting only for CUSTOM */
        return;
    }

    printf("\tmargin:\n");
    printf("\topp_max:");

    for (i = 0; i < ARRAY_LENGTH(config->opp_max); i++) {
        sprintf(tmpbuf, "%u, ", config->opp_max[i]);
        printf("%s", tmpbuf);
    }
    printf("\n");
    printf("\tedp_cfg:\n");

    for (i = 0; i < OPP_PWR_RAIL_MAX; i++) {
        DPM_PWR_RAIL_CFG *cfg = &config->edp_cfg[i];
        printf("\t\tEDP %zu: vr_type %u, pwr_cap %5u, i2c_port %u, i2c_addr 0x%02x, i2c_buck %u, vboot_mV %3u, delta_mV %u\n",
            i, cfg->vr_type, cfg->pwr_cap, cfg->i2c_port, cfg->i2c_addr,
            cfg->i2c_buck, cfg->vboot_mV, cfg->delta_mV);
    }
}

static void dump_opp_config(pm_config_opp_t * opp_config)
{
    unsigned int i;
    uint16_t j;

    for (i = 0; i < DOMAIN_MAX_COUNT; i++) {
        if (opp_config->opps[i].size == 0xFFFF) {
            continue;
        }

        domain_opp_config_t * config = &opp_config->opps[i];
        printf("  domain %u\n", i);
        for (j = 0; j < config->size; j++) {
            printf("   %s opp %u: level %4u",
                j == config->sustained_idx ? "*" : " ", j, config->opp_table[j].level);

            if (config->opp_table[j].frequency) {
                printf(", freq(kHz) %7u", config->opp_table[j].frequency);
            } else {
                printf(", freq(kHz) -------");
            }

            if (config->opp_table[j].voltage) {
                printf(", volt(mV) %3u", config->opp_table[j].voltage);
            } else {
                printf(", volt(mV) ---");
            }

            if (config->opp_table[j].power) {
                printf(", pwrCost %4u", config->opp_table[j].power);
            }

            printf("\n");
        }
        printf("\n");
        config++;
    }
}

static void dump_fan_config(pm_config_fan_t* config, uint32_t num)
{
    for (uint32_t i = 0; i < num; i++) {
        printf("  fan[%d]:\n", i);
        if (config[i].fan_valid.fields.valid == PM_CONFIG_INVALID) {
            printf("\tNULL\n");
        } else {
            for (uint32_t j = 0; j < FAN_MODE_MAX; j++) {
                if (config[i]. rpm_table_valid[j] == 0) {
                    printf("\ttable[%d]:\n", j);
                    for (uint32_t k = 0; k < config[i].rpm_table_items[j]; k++) {
                        printf("\t%d: %4d   %3d   %3d\n", k, (uint32_t)config[i].rpm_table[j][k].rpm, (int32_t)config[i].rpm_table[j][k].up_temp, (int32_t)config[i].rpm_table[j][k].down_temp);
                    }
                }
            }

            printf("\tfan_id:\t\t");dump_valid_flag(config[i].fan_id);
            printf("\tpolarity:\t");dump_valid_flag(config[i].fan_polarity);
            printf("\tscaleup_margin:\t");dump_valid_flag(config[i].scaleup_margin);
            printf("\tpwm_freq:\t");dump_valid_flag(config[i].pwm_freq);
        }
    }
}

static void dump_board_sensor(board_sensor_config_t* config)
{
    if (config->sensor_valid.fields.valid == PM_CONFIG_INVALID) {
        printf("\tINVALID\n");
    } else {
        if (config->sensor_valid.fields.raw_data == 0) {
            printf("\tconnect to EC, reg:0x%x\n", config->reg_id);
        } else if (config->sensor_valid.fields.raw_data == 1) {
            printf("\tconnect to SOC, i2c_ctrl:%d, i2c_addr:0x%x\n", config->i2c_ctrl, config->i2c_addr);
        }
    }
}

static void dump_pvt_config(pm_config_pvt_t *config)
{
    char* name[] = {
        "vpu_sensor",
        "gpu_bottom_sensor",
        "gpu_top_sensor",
        "soc_brc_sensor",
        "ddr_bottom_sensor",
        "ddr_top_sensor",
        "ci700_sensor",
        "npu_sensor",
        "cpu_m1_sensor",
        "cpu_b1_sensor",
        "cpu_m0_sensor",
        "cpu_b0_sensor",
        "soc_trc_sensor",
    };
    printf("\tthermal trip point: ");dump_valid_flag(config->thermal_trip_soc);
    if (config->weight_valid == PM_CONFIG_VALID) {
        printf("\tsensor weight:\n");
        for (uint32_t i = 0; i < 13; i++) {
            printf("\t  - %-18s: %d\n", name[i], config->weight[i]);
        }
    } else {
        printf("\tsensor weight: INVALID\n");
    }

    printf("\tboard sensor 1:\n");
    dump_board_sensor(&config->board_sensor1);
    printf("\tboard sensor 2:\n");
    dump_board_sensor(&config->board_sensor2);
}

static void dump_log_config(pm_config_log_t * log_config)
{
    printf("\tlog flag: "); dump_valid_flag(log_config->log_enable);
    printf("\tbaudrate: "); dump_valid_flag(log_config->uart_baudrate);
}

static void dump_vmin_config(pm_config_vmin_t * vmin_config)
{
    printf("\tvmin disable: "); dump_valid_flag(vmin_config->vmin_disable);
    printf("\tvmin profile: "); dump_valid_flag(vmin_config->vmin_profile);
}

static void dump_noc_idle_config(pm_config_noc_idle_t * noc_idle_config)
{
    printf("\tenable      : "); dump_valid_flag(noc_idle_config->noc_idle_enable);
    if (noc_idle_config->noc_idle_enable.fields.valid == PM_CONFIG_INVALID ||
        (noc_idle_config->noc_idle_enable.fields.valid == PM_CONFIG_VALID &&
         noc_idle_config->noc_idle_enable.fields.raw_data == 0) ) {
        return;
    }

    printf("\tamu1 thro0  : "); dump_valid_flag(noc_idle_config->noc_idle_amu1_thro0);
    printf("\tamu1 thro1  : "); dump_valid_flag(noc_idle_config->noc_idle_amu1_thro1);
    printf("\tddr_dym_pwr : "); dump_valid_flag(noc_idle_config->noc_idle_ddr_dym_pwr_thro);
    printf("\thysteresis  : "); dump_valid_flag(noc_idle_config->noc_idle_hysteresis);
}

static void dump_spt_config(pm_config_spt_t * spt_config)
{
    printf("\t Enable mask: "); dump_valid_flag(spt_config->spt_enable_mask);
    if (spt_config->spt_enable_mask.fields.valid == PM_CONFIG_INVALID) {
        return;
    }

    for (uint32_t i = 0; i < SPT_CTRL_MAX; i++) {
        printf("\tSetPoint [%d]: ", i);
        dump_valid_flag(spt_config->spt_setpoints[i]);
    }

    printf("\tspt skin margin: "); dump_valid_flag(spt_config->spt_skin_margin);
}

static void dump_config()
{
    pm_export_config_t *config = &g_config.config;

    time_t timestamp = CixEpoch() + g_config.timestamp;

    printf("version   : %u.%u\n", g_config.version_major, g_config.version_minor);
    printf("timestamp : %u - %s\n", g_config.timestamp, ctime((const time_t *)&timestamp));
    printf("sizeof(g_config)=%zu, sizeof(opps)=%zu\n", sizeof(g_config), sizeof(config->opp_config.opps));
    printf("pmic config:\n");
    dump_pmic_config(&config->pmic_config);

    printf("OPP config:\n");
    dump_opp_config(&config->opp_config);

    printf("PVT config:\n");
    dump_pvt_config(&config->pvt_config);

    printf("Fan config:\n");
    dump_fan_config(config->fan_config, MAX_FAN_NUM);

    printf("Log config:\n");
    dump_log_config(&config->log_config);

    printf("Vmin config:\n");
    dump_vmin_config(&config->vmin_config);

    printf("NOC idle config:\n");
    dump_noc_idle_config(&config->noc_idle_config);

    printf("SPT config:\n");
    dump_spt_config(&config->spt_config);

    printf("WDT timeout: "); dump_valid_flag(config->wdt_timeout);

    printf("\ncrc check: cka:0x%08x, ckb:0x%08x\n", g_config.crc1, g_config.crc2);
}

#if (PM_FAN_TABLE_CONFIG)
static int32_t check_rpm_table(rpm_entry_t*  rpm_table, uint32_t num)
{

    for (uint32_t i = 1; i < num; i++) {
        if (rpm_table[i].rpm <= rpm_table[i - 1].rpm ||
            rpm_table[i].up_temp <= rpm_table[i - 1].up_temp ||
            rpm_table[i].down_temp <= rpm_table[i - 1].down_temp) {
                printf("rpm entry %d error\n", i);
            return -1;
        }
    }
    return 0;
}

static int32_t check_fan_table(pm_config_fan_t* config, uint32_t num)
{
    uint32_t i = 0;
    pm_config_fan_t* ptr = NULL;
    for (i = 0; i < num; i++) {
        ptr = &config[i];
        if (ptr->fan_id.fields.valid == PM_CONFIG_VALID && ptr->fan_id.fields.raw_data >= MAX_FAN_NUM) {
            return -1;
        }
        for(fan_mode_t m = FAN_MODE_NORMAL; m < FAN_MODE_MAX; m++) {
            if (ptr->rpm_table_valid[m] == PM_CONFIG_INVALID) {
                continue;
            }

            if (ptr->rpm_table_items[m] < 2 || ptr->rpm_table_items[m] > MAX_FAN_TABLE_ENTRIES) {
                return -1;
            }

            if (check_rpm_table(ptr->rpm_table[m], ptr->rpm_table_items[m])) {
                printf("fan_config[%d].rpm_table[%d] error\n", i, m);
                return -1;
            }
        }
    }

    return 0;
}
#endif

#if PM_PVT_SENSOR_CONFIG
static int32_t check_board_sensor(board_sensor_config_t* config)
{
    if (config->sensor_valid.fields.valid == PM_CONFIG_VALID) {
        if (config->sensor_valid.fields.raw_data > 1) {
            printf("board_sensor.sensor_valid invalid\n");
            return -1;
        }
        if (config->sensor_valid.fields.raw_data == 0) {//memory type
            if (config->reg_id < 0x10 || config->reg_id >= 0x20) {
                printf("board_sensor.reg_id invalid\n");
                return -1;
            }
        }
        if (config->sensor_valid.fields.raw_data == 1) {//i2c type
            if (config->i2c_ctrl > 3) {
                printf("board_sensor.i2c_ctrl invalid\n");
                return -1;
            }
        }
    }

    return 0;
}

static int32_t check_pvt_config(pm_config_pvt_t* config)
{
    uint32_t i = 0;
    uint32_t total_weight = 0;
    if (config->weight_valid == PM_CONFIG_VALID) {
        for (i = 0; i < 13; i ++) {
            total_weight += config->weight[i];
        }
        if (total_weight != 1024) {
            printf("all weight sum should equal 1024\n");
            return -1;
        }
    }


    if (check_board_sensor(&config->board_sensor1) || check_board_sensor(&config->board_sensor2)) {
        return -1;
    }

    return 0;
}
#endif

int main(int argc, char **argv)
{
    uint64_t ck = 0ULL;
    char bin_name[256] = {0};
    FILE* bin_file = NULL;
    pm_export_config_t* config = &g_config.config;

    if (argc > 1) {
        printf("Parse %s ... ", argv[1]);
        FILE *bin_file;
        if ((bin_file = fopen(argv[1], "r")) == NULL) {
            printf("\n\nfile %s open failed!\n", argv[1]);
            return -1;
        }

        size_t rd_len = fread(&g_config, sizeof(g_config), 1, bin_file);
        printf(" size %zu\n\n", rd_len);
        dump_config();
        return 0;
    }

    (void)config;
    memset(&g_config, 0xff, sizeof(g_config));

    // version & timestamp
    g_config.version_major = PM_CONFIG_VERSION_MAJOR;
    g_config.version_minor = PM_CONFIG_VERSION_MINOR;
    g_config.timestamp = (uint32_t)(time(NULL) - CixEpoch());
    g_config.length = sizeof(pm_export_config_crc_t);
    g_config.signature = PM_CONFIG_SIGNATURE;
    g_config.crc1 = g_config.crc2 = 0;

    // pmic config
#if PM_PMIC_CONFIG
    memcpy(&config->pmic_config, &pmic_config, PMIC_CONFIG_SIZE);
#endif

#if PM_PVT_SENSOR_CONFIG
    if (check_pvt_config(&pvt_config)) {
        printf("Bad Pvt Config\n");
        return -1;
    }
    memcpy(&config->pvt_config,   &pvt_config,   sizeof(config->pvt_config));
#endif

#if PM_FAN_TABLE_CONFIG
    // fan config
    if (check_fan_table(fan_config, MAX_FAN_NUM)) {

        printf("Bad Fan Table\n");
        return -1;
    }
    memcpy(config->fan_config,   fan_config,   FAN_CONFIG_SIZE);
#endif

#if PM_OPP_TABLE_CONFIG
    if (!check_opp_table()) {
        printf("Bad OPP table\n");
        return -1;
    }

    // update to valid which was invalid setting by memset()
    config->opp_config.opp_valid = PM_CONFIG_VALID;

    for (size_t i = 0; i < ARRAY_LENGTH(dom_opps); i++) {
        if (dom_opps[i]) {
            memcpy(&config->opp_config.opps[i], dom_opps[i], sizeof(config->opp_config.opps[i]));
        } else {
            memset(&config->opp_config.opps[i], 0x00, sizeof(config->opp_config.opps[i]));
        }
    }
#endif

#if PM_LOG_CONFIG
    memcpy(&config->log_config, &log_config, sizeof(config->log_config));
#endif

#if PM_VMIN_CONFIG
    memcpy(&config->vmin_config, &vmin_config, sizeof(config->vmin_config));
#endif

#if PM_NOC_IDLE_CONFIG
    memcpy(&config->noc_idle_config, &noc_idle_config, sizeof(config->noc_idle_config));
#endif

#if PM_SPT_CONFIG
    memcpy(&config->spt_config, &spt_config, sizeof(config->spt_config));
#endif

#if PM_WDT_CONFIG
    memcpy(&config->wdt_timeout, &wdt_timeout, sizeof(config->wdt_timeout));
#endif

    // checksum
    (void)double_check_sum(&g_config, sizeof(g_config), &ck, false);
    g_config.crc1 = ck >> 32;
    g_config.crc2 = (uint32_t)(ck & (0xFFFFFFFFull));

    dump_config();

    // write to file
    strcpy(bin_name, PROJECT_NAME);
    strcat(bin_name, "_config.bin");
    printf("generate file [%s]\n", bin_name);
    if ((bin_file = fopen(bin_name, "w+")) == NULL) {
      printf("file %s open failed!\n", bin_name);
      exit(-1);
    }
    size_t wr_len = fwrite(&g_config, sizeof(g_config), 1, bin_file);
    if (1 != wr_len) {
        printf("write length error!!, write %zu objects, expect %zu objects\n", wr_len, (size_t)1);
        fclose(bin_file);
        exit(-1);
    }

    // stuff 0xFF up to 4KB
    assert(sizeof(g_config) <= PM_CONFIG_BIN_SIZE);
    size_t remain = PM_CONFIG_BIN_SIZE - sizeof(g_config);
    uint8_t *remain_b;
    remain_b = malloc(remain);
    assert(remain_b != NULL);
    memset(remain_b, 0xFF, remain);
    wr_len = fwrite(remain_b, remain, 1, bin_file);
    free(remain_b);
    if (1 != wr_len) {
        printf("write 0xFF error!!\n");
        fclose(bin_file);
        exit(-1);
    }

    printf("write %u Bytes to file %s\n", PM_CONFIG_BIN_SIZE, bin_name);
    fclose(bin_file);
    return 0;
}
