/*
 * Copyright 2024 Cix Technology Group Co., Ltd.
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FW_BOOT_PERF_LIB_H_
#define _FW_BOOT_PERF_LIB_H_
#include <Uefi.h>
#define MAX_BOOT_PHASE_NAME (16)
enum FW_BOOT_PHASE {
	BROM_PHASE = 0,
	SE_PHASE,
	PM_PHASE,
	PBL_PHASE, // bl2
	TFA_PHASE, // bl31
	TEE_PHASE, // bl32
	BLOADER_PHASE, // uefi or uboot
	GRUB_PHASE, // grub
	FW_BOOT_PHASE_MAX,
};
enum RECORD_POINT {
	RECORD_START = 0,
	RECORD_END,
	RECORD_POINT_MAX,
};
struct fw_boot_phase_point {
	UINT64 start;
	UINT64 end;
	char fw_name[MAX_BOOT_PHASE_NAME]; // "BROM", "SE", "PM", "PBL", "TFA", "TEE",
		// "BLOADER", "GRUB"
};

void cix_set_boot_phase(enum FW_BOOT_PHASE phase, enum RECORD_POINT point);
UINT64 cix_get_boot_phase (enum FW_BOOT_PHASE phase, enum RECORD_POINT point);
#endif // _FW_BOOT_PERF_LIB_H_