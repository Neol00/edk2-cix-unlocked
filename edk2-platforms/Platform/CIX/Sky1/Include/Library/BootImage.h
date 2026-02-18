/** @BootImage.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2014-2019, The Linux Foundation. All rights reserved.
 *  Copyright (C) 2008 The Android Open Source Project.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef _BOOT_IMAGE_H_
#define _BOOT_IMAGE_H_

#define BOOT_MAGIC              "ANDROID!"
#define BOOT_MAGIC_SIZE         8
#define BOOT_NAME_SIZE          16
#define BOOT_ARGS_SIZE          512
#define BOOT_IMG_MAX_PAGE_SIZE  4096
#define KERNEL64_HDR_MAGIC      0x644D5241/* ARM64 */
#define BOOT_EXTRA_ARGS_SIZE    1024

#define VENDOR_BOOT_MAGIC       "VNDRBOOT"
#define VENDOR_BOOT_MAGIC_SIZE  8
#define VENDOR_BOOT_ARGS_SIZE   2048
#define VENDOR_BOOT_NAME_SIZE   16

#define VENDOR_RAMDISK_TYPE_NONE 0
#define VENDOR_RAMDISK_TYPE_PLATFORM 1
#define VENDOR_RAMDISK_TYPE_RECOVERY 2
#define VENDOR_RAMDISK_TYPE_DLKM 3
#define VENDOR_RAMDISK_NAME_SIZE 32
#define VENDOR_RAMDISK_TABLE_ENTRY_BOARD_ID_SIZE 16

#define BOOT_HEADER_VERSION_ZERO  0
#define BOOT_HEADER_VERSION_ONE   1
#define BOOT_HEADER_VERSION_TWO   2
#define BOOT_HEADER_VERSION_THREE   3
#define BOOT_HEADER_VERSION_FOUR  4



#define BOOT_HEADER_VERSION_FOUR  4
/* When the boot image header has a version of 4, the structure of the boot
 * image is as follows:
 *
 * +---------------------+
 * | boot header         | 4096 bytes
 * +---------------------+
 * | kernel              | m pages
 * +---------------------+
 * | ramdisk             | n pages
 * +---------------------+
 * | boot signature      | g pages
 * +---------------------+
 *
 * m = (kernel_size + 4096 - 1) / 4096
 * n = (ramdisk_size + 4096 - 1) / 4096
 * g = (signature_size + 4096 - 1) / 4096
 *
 * Note that in version 4 of the boot image header, page size is fixed at 4096
 * bytes.
 *
 * The structure of the vendor boot image version 4, which is required to be
 * present when a version 4 boot image is used, is as follows:
 *
 * +------------------------+
 * | vendor boot header     | o pages
 * +------------------------+
 * | vendor ramdisk section | p pages
 * +------------------------+
 * | dtb                    | q pages
 * +------------------------+
 * | vendor ramdisk table   | r pages
 * +------------------------+
 * | bootconfig             | s pages
 * +------------------------+
 *
 * o = (2128 + page_size - 1) / page_size
 * p = (vendor_ramdisk_size + page_size - 1) / page_size
 * q = (dtb_size + page_size - 1) / page_size
 * r = (vendor_ramdisk_table_size + page_size - 1) / page_size
 * s = (vendor_bootconfig_size + page_size - 1) / page_size
 *
 * Note that in version 4 of the vendor boot image, multiple vendor ramdisks can
 * be included in the vendor boot image. The bootloader can select a subset of
 * ramdisks to load at runtime. To help the bootloader select the ramdisks, each
 * ramdisk is tagged with a type tag and a set of hardware identifiers
 * describing the board, soc or platform that this ramdisk is intended for.
 *
 * The vendor ramdisk section is consist of multiple ramdisk images concatenated
 * one after another, and vendor_ramdisk_size is the size of the section, which
 * is the total size of all the ramdisks included in the vendor boot image.
 *
 * The vendor ramdisk table holds the size, offset, type, name and hardware
 * identifiers of each ramdisk. The type field denotes the type of its content.
 * The vendor ramdisk names are unique. The hardware identifiers are specified
 * in the board_id field in each table entry. The board_id field is consist of a
 * vector of unsigned integer words, and the encoding scheme is defined by the
 * hardware vendor.
 *
 * For the different type of ramdisks, there are:
 *    - VENDOR_RAMDISK_TYPE_NONE indicates the value is unspecified.
 *    - VENDOR_RAMDISK_TYPE_PLATFORM ramdisks contain platform specific bits, so
 *      the bootloader should always load these into memory.
 *    - VENDOR_RAMDISK_TYPE_RECOVERY ramdisks contain recovery resources, so
 *      the bootloader should load these when booting into recovery.
 *    - VENDOR_RAMDISK_TYPE_DLKM ramdisks contain dynamic loadable kernel
 *      modules.
 *
 * Version 4 of the vendor boot image also adds a bootconfig section to the end
 * of the image. This section contains Boot Configuration parameters known at
 * build time. The bootloader is responsible for placing this section directly
 * after the generic ramdisk, followed by the bootconfig trailer, before
 * entering the kernel.
 *
 * 0. all entities in the boot image are 4096-byte aligned in flash, all
 *    entities in the vendor boot image are page_size (determined by the vendor
 *    and specified in the vendor boot image header) aligned in flash
 * 1. kernel, ramdisk, and DTB are required (size != 0)
 * 2. load the kernel and DTB at the specified physical address (kernel_addr,
 *    dtb_addr)
 * 3. load the vendor ramdisks at ramdisk_addr
 * 4. load the generic ramdisk immediately following the vendor ramdisk in
 *    memory
 * 5. load the bootconfig immediately following the generic ramdisk. Add
 *    additional bootconfig parameters followed by the bootconfig trailer.
 * 6. set up registers for kernel entry as required by your architecture
 * 7. if the platform has a second stage bootloader jump to it (must be
 *    contained outside boot and vendor boot partitions), otherwise
 *    jump to kernel_addr
 */
struct boot_img_hdr_v4{
  // Must be BOOT_MAGIC.
  UINT8     magic[BOOT_MAGIC_SIZE];

  UINT32    kernel_size;  /* size in bytes */
  UINT32    ramdisk_size; /* size in bytes */

  // Operating system version and security patch level.
  // For version "A.B.C" and patch level "Y-M-D":
  //   (7 bits for each of A, B, C; 7 bits for (Y-2000), 4 bits for M)
  //   os_version = A[31:25] B[24:18] C[17:11] (Y-2000)[10:4] M[3:0]
  UINT32    os_version;

  UINT32    header_size;

  UINT32    reserved[4];

  // Version of the boot image header.
  UINT32    header_version;

  UINT8     cmdline[BOOT_ARGS_SIZE + BOOT_EXTRA_ARGS_SIZE];

  UINT32 signature_size; /* size in bytes */
} __attribute__((packed));
typedef struct boot_img_hdr_v4 boot_img_hdr_v4;

struct vendor_boot_img_hdr_v4 {
  // Must be VENDOR_BOOT_MAGIC.
  UINT8     magic[VENDOR_BOOT_MAGIC_SIZE];

  // Version of the vendor boot image header.
  UINT32    header_version;

  UINT32    page_size; /* flash page size we assume */

  UINT32    kernel_addr;  /* physical load addr */
  UINT32    ramdisk_addr; /* physical load addr */

  UINT32    vendor_ramdisk_size; /* size in bytes */

  UINT8     cmdline[VENDOR_BOOT_ARGS_SIZE];

  UINT32    tags_addr;                   /* physical addr for kernel tags */
  UINT8     name[VENDOR_BOOT_NAME_SIZE]; /* asciiz product name */

  UINT32    header_size;

  UINT32    dtb_size; /* size in bytes for DTB image */
  UINT64    dtb_addr; /* physical load address for DTB image */

  UINT32 vendor_ramdisk_table_size; /* size in bytes for the vendor ramdisk table */
  UINT32 vendor_ramdisk_table_entry_num; /* number of entries in the vendor ramdisk table */
  UINT32 vendor_ramdisk_table_entry_size; /* size in bytes for a vendor ramdisk table entry */

  UINT32 bootconfig_size; /* size in bytes for the bootconfig section */
} __attribute__((packed));

typedef struct vendor_boot_img_hdr_v4 vendor_boot_img_hdr_v4;
struct vendor_ramdisk_table_entry_v4 {
    UINT32 ramdisk_size; /* size in bytes for the ramdisk image */
    UINT32 ramdisk_offset; /* offset to the ramdisk image in vendor ramdisk section */
    UINT32 ramdisk_type; /* type of the ramdisk */
    UINT8 ramdisk_name[VENDOR_RAMDISK_NAME_SIZE]; /* asciiz ramdisk name */

    // Hardware identifiers describing the board, soc or platform which this
    // ramdisk is intended to be loaded on.
    UINT32 board_id[VENDOR_RAMDISK_TABLE_ENTRY_BOARD_ID_SIZE];
} __attribute__((packed));

struct kernel64_hdr {
  UINT32    Code0;      /* Executable code */
  UINT32    Code1;      /* Executable code */
  UINT64    TextOffset; /* Image load offset, little endian */
  UINT64    ImageSize;  /* Effective Image size, little endian */
  UINT64    Flags;      /* kernel flags, little endian */
  UINT64    Res2;       /* reserved */
  UINT64    Res3;       /* reserved */
  UINT64    Res4;       /* reserved */
  UINT32    magic_64;   /* Magic number, little endian, "ARM\x64" i.e 0x644d5241*/
  UINT32    Res5;       /* reserved (used for PE COFF offset) */
};

typedef struct kernel64_hdr Kernel64Hdr;

#endif
