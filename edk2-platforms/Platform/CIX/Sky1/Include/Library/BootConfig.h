/** @BootConfig.c
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2012-2021, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/
#ifndef __BOOT_COFNIG_H__
#define __BOOT_COFNIG_H__

#include <Library/DebugLib.h>
#include <Library/DeviceInfo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Uefi.h>
#include <Library/BootLinux.h>
#include <DeviceInfo.h>

/*
 * Add a string of boot config parameters to memory appended by the trailer.
 * This memory needs to be immediately following the end of the ramdisks.
 * The new boot config trailer will be written to the end of the entire
 * parameter section(previous + new). The trailer contains a 4 byte size of the
 * parameters, followed by a 4 byte checksum of the parameters, followed by a 12
 * byte magic string.
 *
 * @param params pointer to string of boot config parameters
 * @param params_size size of params string in bytes
 * @param bootconfig_start_addr address that the boot config section is starting
 *        at in memory.
 * @param bootconfig_size size of the current bootconfig section in bytes.
 * @return number of bytes added to the boot config section. -1 for error.
 */
INT32 addBootConfigParameters(CHAR8 *params, UINT32 params_size,
                            UINT64 bootconfig_start_addr,
                            UINT32 bootconfig_size);

/*
 * Add the boot config trailer to the end of the boot config parameter section.
 * This can be used after the vendor bootconfig section has been placed into
 * memory if there are no additional parameters that need to be added.
 * The new boot config trailer will be written to the end of the entire
 * parameter section at (bootconfig_start_addr + bootconfig_size).
 * The trailer contains a 4 byte size of the parameters, followed by a 4 byte
 * checksum of the parameters, followed by a 12 byte magic string.
 *
 * @param bootconfig_start_addr address that the boot config section is starting
 *        at in memory.
 * @param bootconfig_size size of the current bootconfig section in bytes.
 * @return number of bytes added to the boot config section. -1 for error.
 */
INT32 addBootConfigTrailer(UINT64 bootconfig_start_addr,
                         UINT32 bootconfig_size);

EFI_STATUS UpdateBootConfig (BootInfo *Info,
                             BootParamlist *BootParamlistPtr);

#endif