/** @file
  Asf message commands byte define.

  Copyright (c) 1985 - 2022, AMI. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Revision Reference:
  Data defined in Asf 2.0 Specification.
**/

#ifndef __ASF_H__
#define __ASF_H__

#include <Base.h>

//
// Boot option messages
//
#define ASFMSG_CMD_CONFIG              0x3   // ASF Configuration
#define ASFMSG_SUBCMD_CLR_BOOT_OPTION  0x15  // Clear Boot Options
#define ASFMSG_SUBCMD_RET_BOOT_OPTION  0x16  // Return Boot Options
#define ASFMSG_SUBCMD_NO_BOOT_OPTION   0x17  // No Boot Options

//
// System states
//
#define ASFMSG_SYSTEM_STATE_S0  0             // S0/G0 "Working"
#define ASFMSG_SYSTEM_STATE_S1  1             // S1
#define ASFMSG_SYSTEM_STATE_S2  2             // S2
#define ASFMSG_SYSTEM_STATE_S3  3             // S3
#define ASFMSG_SYSTEM_STATE_S4  4             // S4
#define ASFMSG_SYSTEM_STATE_S5  5             // S5/G2 "Soft-off"

//
// Asf version
//
#define ASFMSG_VERSION_NUMBER_10      0x10

//
// System firmware capabilities Bit
//
#define ASF_BOP_BIT_FORCE_PROGRESS_EVENT        BIT12

//
// Asf message command
//
#define ASFMSG_COMMAND_SYSTEM_STATE               0x1
#define ASFMSG_COMMAND_MANAGEMENT_CONTROL         0x2
#define ASFMSG_COMMAND_MESSAGING                  0x4

//
// Asf message subcommand
//
#define ASFMSG_SUBCOMMAND_STOP_WATCH_DOG          0x14
#define ASFMSG_SUBCOMMAND_NO_RETRANSMIT           0x16
#define ASFMSG_SUBCOMMAND_SET_SYSTEM_STATE        0x18

//
// Asf message event sensor type
//
#define ASFMSG_EVENT_SENSOR_TYPE_CHASSIS_INTRUSION  0x5
#define ASFMSG_EVENT_SENSOR_TYPE_FW_ERROR_PROGRESS  0xF
#define ASFMSG_EVENT_SENSOR_TYPE_BOOT_ERROR         0x1E
#define ASFMSG_EVENT_SENSOR_TYPE_ENTITY_PRESENCE    0x25

//
// Asf message event type
//
#define ASFMSG_EVENT_TYPE_SENSOR_SPECIFIC           0x6F

//
// Asf message event offset
//
#define ASFMSG_EVENT_OFFSET_ENTITY_PRESENT          0x0

#define ASFMSG_EVENT_OFFSET_SYS_FW_PROGRESS_ENTRY   0x2
#define ASFMSG_EVENT_OFFSET_SYS_FW_PROGRESS_EXIT    0x82
#define ASFMSG_EVENT_OFFSET_SYS_FW_ERROR            0x0

#define ASFMSG_EVENT_OFFSET_NO_BOOTABLE_MEDIA       0x0
#define ASFMSG_EVENT_OFFSET_CHASSIS_INTRUSION       0x0

//
// Asf message event source type
//
#define ASFMSG_EVENT_SOURCE_TYPE_ASF10              0x68

//
// Asf message event severity
//
#define ASFMSG_EVENT_SEVERITY_MONITOR               0x1
#define ASFMSG_EVENT_SEVERITY_NON_CRITICAL          0x8
#define ASFMSG_EVENT_SEVERITY_CRITICAL              0x10
#define ASFMSG_EVENT_SEVERITY_NON_RECOVERABLE       0x20

//
// Asf message sensor device
//
#define ASFMSG_SENSOR_DEVICE_UNSPECIFIED            0xFF

//
// Asf message sensor number
//
#define ASFMSG_SENSOR_NUMBER_UNSPECIFIED            0xFF

//
// Asf message Entity
//

#define ASFMSG_ENTITY_UNSPECIFIED                   0x0
#define ASFMSG_ENTITY_PROCESSOR                     0x3
#define ASFMSG_ENTITY_DISK                          0x4
#define ASFMSG_ENTITY_SYSTEM_BOARD                  0x7
#define ASFMSG_ENTITY_ADD_IN_CARD                   0xB
#define ASFMSG_ENTITY_BIOS                          0x22
#define ASFMSG_ENTITY_MEMORY_DEVICE                 0x20

//
// Asf message entity instance
//
#define ASFMSG_ENTITY_INSTANCE_UNSPECIFIED          0x0

//
// Asf message event data
//
#define ASFMSG_EVENT_DATA1                          0x40
#define ASFMSG_EVENT_DATA_UNSPECIFIED               0x0
#define ASFMSG_EVENT_DATA_MEMORY_INITIALIZATION     0x1
#define ASFMSG_EVENT_DATA_HARD_DISK_INITIALIZATION  0x2
#define ASFMSG_EVENT_DATA_AP_INITIALIZATION         0x3
#define ASFMSG_EVENT_DATA_SETUP_INITIALIZATION      0x5
#define ASFMSG_EVENT_DATA_USB_RESOURCE_CONFIG       0x6
#define ASFMSG_EVENT_DATA_PCI_RESOURCE_CONFIG       0x7
#define ASFMSG_EVENT_DATA_VIDEO_INITIALIZATION      0x9
#define ASFMSG_EVENT_DATA_CACHE_INITIALIZATION      0xA
#define ASFMSG_EVENT_DATA_KEYBOARD_INITIALIZATION   0xC
#define ASFMSG_EVENT_DATA_BOARD_INITIALIZATION      0x14
#define ASFMSG_EVENT_DATA_KEYBOARD_TEST             0x17

#define ASFMSG_EVENT_DATA_NO_MEMORY                 0x1
#define ASFMSG_EVENT_DATA_HARD_DISK_FAILURE         0x3
#define ASFMSG_EVENT_DATA_KEYBOARD_FAILURE          0x7
#define ASFMSG_EVENT_DATA_NO_VIDEO                  0xA

#endif //__ASF_H__
