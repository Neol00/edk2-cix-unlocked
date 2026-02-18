/** @LocateDeviceTree.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2015, 2017-2021, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __LOCATEDEVICETREE_H__
#define __LOCATEDEVICETREE_H__

#include "libfdt.h"
#include <../../../Library/LibUfdt/include/ufdt_overlay.h>
#include <Library/BootLinux.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/VerifiedBoot.h>
#include <Uefi.h>

#define DEV_TREE_SUCCESS     0
#define DEV_TREE_MAGIC       0x54444351/* "QCDT" */
#define DEV_TREE_MAGIC_LEN   4
#define DEV_TREE_VERSION_V1  1
#define DEV_TREE_VERSION_V2  2
#define DEV_TREE_VERSION_V3  3

#define DEV_TREE_HEADER_SIZE      12
#define DEVICE_TREE_IMAGE_OFFSET  0x5F8800

#define DTB_MAGIC   0xedfe0dd0
#define DTB_OFFSET  0X2C

#define DTB_PAD_SIZE               2048
#define DTBO_TABLE_MAGIC           0xD7B7AB1E
#define DTBO_CUSTOM_MAX            4
#define PLATFORM_FOUNDRY_SHIFT     16
#define DTBO_MAX_SIZE_ALLOWED      (24 * 1024 * 1024)
#define SOC_MASK                   (0xffff)
#define VARIANT_MASK               (0x000000ff)
#define VARIANT_MINOR_MASK         (0x0000ff00)
#define VARIANT_MAJOR_MASK         (0x00ff0000)
#define PMIC_MODEL_MASK            (0x000000ff)
#define PMIC_REV_MASK              (0xffffff00)
#define PMIC_SHIFT_IDX             (2)
#define PLATFORM_SUBTYPE_SHIFT_ID  (0x18)
#define FOUNDRY_ID_MASK            (0x00ff0000)
#define PLATFORM_SUBTYPE_MASK      (0x000000ff)
#define DDR_MASK                   (0x00000700)

typedef enum {
  PMIC_IDX0,
  PMIC_IDX1,
  PMIC_IDX2,
  PMIC_IDX3,
  PMIC_IDX4,
  MAX_PMIC_IDX = 0xF,
} PMIC_INDEXES;

typedef struct PmicIdInfo {
  UINT32    DtPmicModel[MAX_PMIC_IDX];
  UINT32    DtPmicRev[MAX_PMIC_IDX];
  UINT64    DtMatchVal;
} PmicIdInfo;

typedef struct DtInfo {
  UINT32    DtPlatformId;
  UINT32    DtSocRev;
  UINT32    DtFoundryId;
  UINT32    DtVariantId;
  UINT32    DtVariantMajor;
  UINT32    DtVariantMinor;
  UINT32    DtPlatformSubtype;
  UINT32    DtPmicModel[MAX_PMIC_IDX];
  UINT32    DtPmicRev[MAX_PMIC_IDX];
  UINT64    DtMatchVal;
  VOID      *Dtb;
} DtInfo;

#define DT_ENTRY_V1_SIZE  0xC

/*Struct def for device tree entry*/
struct dt_entry {
  UINT32    platform_id;
  UINT32    variant_id;
  UINT32    board_hw_subtype;
  UINT32    soc_rev;
  UINT32    pmic_rev[4];
  UINT64    offset;
  UINT32    size;
  UINT32    Idx;
};

/*Struct def for device tree entry*/
struct dt_entry_v1 {
  UINT32    platform_id;
  UINT32    variant_id;
  UINT32    soc_rev;
  UINT32    offset;
  UINT32    size;
};

/*Struct def for device tree entry*/
struct dt_entry_v2 {
  UINT32    platform_id;
  UINT32    variant_id;
  UINT32    board_hw_subtype;
  UINT32    soc_rev;
  UINT32    offset;
  UINT32    size;
};

/*Struct def for device tree table*/
struct dt_table {
  UINT32    magic;
  UINT32    version;
  UINT32    num_entries;
};

struct plat_id {
  UINT32    platform_id;
  UINT32    soc_rev;
};

struct board_id {
  UINT32    variant_id;
  UINT32    platform_subtype;
};

struct pmic_id {
  UINT32    pmic_version[4];
};

#define PLAT_ID_SIZE   sizeof (struct plat_id)
#define BOARD_ID_SIZE  sizeof (struct board_id)
#define PMIC_ID_SIZE   sizeof (struct pmic_id)

struct dt_mem_node_info {
  UINT32    offset;
  UINT32    mem_info_cnt;
  UINT32    addr_cell_size;
  UINT32    size_cell_size;
};

enum dt_entry_info {
  DTB_FOUNDRY = 0,
  DTB_DDR,
  DTB_SOC,
  DTB_MAJOR_MINOR,
  DTB_PMIC0,
  DTB_PMIC1,
  DTB_PMIC2,
  DTB_PMIC3,
  DTB_PMIC_MODEL,
  DTB_PANEL_TYPE,
  DTB_BOOT_DEVICE,
};

struct DtboTableHdr {
  UINT32    Magic;         // DTB TABLE MAGIC
  UINT32    TotalSize;     // includes DtTableHdr + all DtTableEntry and all dtb/dtbo
  UINT32    HeaderSize;    // sizeof(DtTableHdr)
  UINT32    DtEntrySize;   // sizeof(DtTableEntry)
  UINT32    DtEntryCount;  // number of DtTableEntry
  UINT32    DtEntryOffset; // offset to the first DtTableEntry
  UINT32    PageSize;      // flash pagesize we assume
  UINT32    Reserved[1];   // must zeros
};

struct DtboTableEntry {
  UINT32    DtSize;
  UINT32    DtOffset;                // offset from head of DtTableHdr
  UINT32    Id;                      // optional, must zero if unused
  UINT32    Rev;                     // optional, must zero if unused
  UINT32    Custom[DTBO_CUSTOM_MAX]; // optional, must zero if unused
};

VOID *
GetSocDtb (
  void    *kernel,
  UINT32  kernel_size,
  UINT32  dtb_offset,
  void    *tags
  );

VOID *
GetBoardDtb (
  BootInfo  *Info,
  VOID      *DtboImgBuffer
  );

int
DeviceTreeValidate (
  UINT8   *DeviceTreeBuff,
  UINT32  PageSize,
  UINT32  *DeviceTreeSize
  );

INT32
GetDtboIdx (
  VOID
  );

INT32
GetDtbIdx (
  VOID
  );

VOID
DeleteDtList (
  struct fdt_entry_node  **DtList
  );

BOOLEAN
AppendToDtList (
  struct fdt_entry_node  **DtList,
  UINT64                 Address,
  UINT64                 Size
  );

#endif
