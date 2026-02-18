/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __DMACOMMON_H__
#define __DMACOMMON_H__

#define CSRT_AUD_REQUEST_BASE  32
#define CSRT_FCH_REQUEST_BASE  0
#define CSRT_REQUEST_SIZE      32

/* --------------------- irq type match ------------------------------
 * | Polarity \ Mode  | ACPI_EDGE_SENSITIVE   | ACPI_LEVEL_SECSITIVE |
 * | ACPI_ACTIVE_LOW  | IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_LEVEL_LOW   |
 * | ACPI_ACTIVE_HIGH | IRQ_TYPE_EDGE_RISING  | IRQ_TYPE_LEVEL_HITH  |
 * | ACPI_ACTIVE_BOTH | IRQ_TYPE_EDGE_BOTH    | IRQ_TYPE_NONE        |
 * ------------------------------------------------------------------- */

#define ACPI_LEVEL_SENSITIVE  0x00
#define ACPI_EDGE_SENSITIVE   0x01

#define ACPI_ACTIVE_HIGH  0x00
#define ACPI_ACTIVE_LOW   0x01
#define ACPI_ACTIVE_BOTH  0x02

#endif
