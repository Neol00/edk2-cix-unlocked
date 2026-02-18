/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __ACPI_SCMI_H__
#define __ACPI_SCMI_H__

#define SCMI_PLATFORM_ID  0

#define SCMI_AGENT_ID_MAX              0xFF
#define SCMI_PROTOCOL_ID_MAX           0xFF
#define SCMI_PLATFORM_PROTOCOL_ID_MIN  0x80
#define SCMI_PLATFORM_PROTOCOL_ID_MAX  SCMI_PROTOCOL_ID_MAX

// Error codes.
#define ACPI_SCMI_BUSY                6
#define ACPI_SCMI_TIMEOUT             11
#define ACPI_SCMI_INVALID_PARAMETERS  2

// SCMI error codes.
#define SCMI_SUCCESS             0
#define SCMI_NOT_SUPPORTED       0xFFFFFFFF
#define SCMI_INVALID_PARAMETERS  0xFFFFFFFE
#define SCMI_DENIED              0xFFFFFFFD
#define SCMI_NOT_FOUND           0xFFFFFFFC
#define SCMI_OUT_OF_RANGE        0xFFFFFFFB
#define SCMI_BUSY                0xFFFFFFFA
#define SCMI_COMMS_ERROR         0xFFFFFFF9
#define SCMI_GENERIC_ERROR       0xFFFFFFF8
#define SCMI_HARDWARE_ERROR      0xFFFFFFF7
#define SCMI_PROTOCOL_ERROR      0xFFFFFFF6

// Common command identifiers.
#define SCMI_PROTOCOL_VERSION             0x000
#define SCMI_PROTOCOL_ATTRIBUTES          0x001
#define SCMI_PROTOCOL_MESSAGE_ATTRIBUTES  0x002

// SCMI Base Protocol
#define SCMI_PROTOCOL_ID_BASE  0x10

// SCMI Base Protocol Message IDs
#define SCMI_BASE_DISCOVER_VENDOR                  0x003
#define SCMI_BASE_DISCOVER_SUB_VENDOR              0x004
#define SCMI_BASE_DISCOVER_IMPLEMENTATION_VERSION  0x005
#define SCMI_BASE_DISCOVER_LIST_PROTOCOLS          0x006
#define SCMI_BASE_DISCOVER_AGENT                   0x007
#define SCMI_BASE_NOTIFY_ERRORS                    0x008
#define SCMI_BASE_SET_DEVICE_PERMISSIONS           0x009
#define SCMI_BASE_SET_PROTOCOL_PERMISSIONS         0x00A
#define SCMI_BASE_RESET_AGENT_CONFIG               0x00B

// SCMI Power Domain Protocol
#define SCMI_PROTOCOL_ID_POWER_DOMAIN  0x11

// SCMI Power Domain Protocol Message IDs
#define SCMI_PD_POWER_DOMAIN_ATTRIBUTES              0x03
#define SCMI_PD_POWER_STATE_SET                      0x04
#define SCMI_PD_POWER_STATE_GET                      0x05
#define SCMI_PD_POWER_STATE_NOTIFY                   0x06
#define SCMI_PD_POWER_STATE_CHANGE_REQUESTED_NOTIFY  0x07
#define SCMI_PD_POWER_COMMAND_COUNT

// SCMI System Power Protocol
#define SCMI_PROTOCOL_ID_SYS_POWER  0x12

// SCMI System Power Protocol Message IDs
#define SCMI_SYS_POWER_STATE_SET     0x003
#define SCMI_SYS_POWER_STATE_GET     0x004
#define SCMI_SYS_POWER_STATE_NOTIFY  0x005
#define SCMI_SYS_POWER_COMMAND_COUNT

// SCMI Performance Protocol
#define SCMI_PROTOCOL_ID_PERF  0x13

// SCMI Performance Protocol Message IDs
#define SCMI_PERF_DOMAIN_ATTRIBUTES      0x003
#define SCMI_PERF_DESCRIBE_LEVELS        0x004
#define SCMI_PERF_LIMITS_SET             0x005
#define SCMI_PERF_LIMITS_GET             0x006
#define SCMI_PERF_LEVEL_SET              0x007
#define SCMI_PERF_LEVEL_GET              0x008
#define SCMI_PERF_NOTIFY_LIMITS          0x009
#define SCMI_PERF_NOTIFY_LEVEL           0x00A
#define SCMI_PERF_DESCRIBE_FAST_CHANNEL  0x00B
#define SCMI_PERF_COMMAND_COUNT

// SCMI Clock Protocol
#define SCMI_PROTOCOL_ID_CLOCK  0x14

// SCMI Clock Protocol Message IDs
#define SCMI_CLOCK_ATTRIBUTES      0x003
#define SCMI_CLOCK_DESCRIBE_RATES  0x004
#define SCMI_CLOCK_RATE_SET        0x005
#define SCMI_CLOCK_RATE_GET        0x006
#define SCMI_CLOCK_CONFIG_SET      0x007

// SCMI Sensor Protocol
#define SCMI_PROTOCOL_ID_SENSOR  0x15

// SCMI Sensor Protocol Message IDs
#define SCMI_SENSOR_DESCRIPTION_GET       0x003
#define SCMI_SENSOR_TRIP_POINT_NOTIFY     0x004
#define SCMI_SENSOR_TRIP_POINT_CONFIG     0x005
#define SCMI_SENSOR_READING_GET           0x006
#define SCMI_SENSOR_AXIS_DESCRIPTION_GET  0x007

// SCMI Reset Domain Protocol
#define SCMI_PROTOCOL_ID_RESET_DOMAIN  0x16

// SCMI Reset Domain Management Protocol Message IDs
#define SCMI_RESET_DOMAIN_ATTRIBUTES  0x03
#define SCMI_RESET_REQUEST            0x04
#define SCMI_RESET_NOTIFY             0x05

// SCMI Reset Domain Management Protocol Response IDs.
#define SCMI_RESET_ISSUED    0x00
#define SCMI_RESET_COMPLETE  0x04

// SCMI Voltage Domain Protocol
#define SCMI_PROTOCOL_ID_VOLTAGE_DOMAIN  0x17

// SCMI Voltage Domain Protocol Message IDs
#define SCMI_VOLTD_DOMAIN_ATTRIBUTES  0x003
#define SCMI_VOLTD_DESCRIBE_LEVELS    0x004
#define SCMI_VOLTD_CONFIG_SET         0x005
#define SCMI_VOLTD_CONFIG_GET         0x006
#define SCMI_VOLTD_LEVEL_SET          0x007
#define SCMI_VOLTD_LEVEL_GET          0x008

//CIX SCMI Message Attribute
#define CIX_SCMI_MESSAGE_ATTRIBUTES         0xc7f

//CIX SCMI Message Header
#define SCMI_MESSAGE_HEADER_FAN_MODE_SET    0x802

#endif
