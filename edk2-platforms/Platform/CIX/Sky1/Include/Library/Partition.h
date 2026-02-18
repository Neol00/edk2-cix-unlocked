/** @Partition.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2015-2018, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __LINUXLOADERLIB_H__
#define __LINUXLOADERLIB_H__

#include <Uefi.h>

#include "DeviceInfo.h"
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/Gpt.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/EfiFileLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <PiDxe.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EraseBlock.h>
#include <Protocol/PartitionInfo.h>

#define MAX_GET_VAR_NAME_SIZE  32

/* Selection attributes for selecting the BlkIo handles */
#define BLK_IO_SEL_MEDIA_TYPE_REMOVABLE       0x0001
#define BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE   0x0002
#define BLK_IO_SEL_PARTITIONED_GPT            0x0004
#define BLK_IO_SEL_PARTITIONED_MBR            0x0008
#define BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID  0x0010
#define BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM  0x0020
#define BLK_IO_SEL_SELECT_BY_VOLUME_NAME      0x0040

/* Select only the root device handle indicated. Doesn't return
 * any partitions within.
 * Currently this filter applies only for eMMC device, not the external
 * device connected via USB */
#define BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY  0x0080

/* Select the handle that's on the indicated root device.
 * Currently this filter applies only for eMMC device, not the external
 * device connected via USB */
#define BLK_IO_SEL_MATCH_ROOT_DEVICE  0x0100

/* Select through partition name*/
#define BLK_IO_SEL_MATCH_PARTITION_LABEL  0x0200

/* Do case insensetive string comparisons */
#define BLK_IO_SEL_STRING_CASE_INSENSITIVE  0x0400

/* Partitioning scheme types for selecting the BlkIo handles */
#define PARTITIONED_TYPE_MBR  0x01
#define PARTITIONED_TYPE_GPT  0x02

#define ZERO  0
/* #define ARRAY_SIZE(a) sizeof (a) / sizeof (*a) */
#define MAX_HANDLE_INFO_LIST  128

/* Macro to avoid integer overflow */
#define ADD_OF(a, b)  (MAX_UINT32 - b > a) ? (a + b) : ZERO

#define BOOT_DEV_MAX_LEN        32
#define BOOT_DEV_NAME_SIZE_MAX  10

/* Allocate unsafe stack size of 128KB to address worst case,
 * which is same as the  normal stack size */
#define BOOT_LOADER_MAX_UNSAFE_STACK_SIZE  (1 << 17)

/* Any data specific to additional attributes can be added here. */
typedef struct {
  EFI_GUID    *RootDeviceType; /* GUID Selecting the root device type */
  EFI_GUID    *PartitionType;  /* Partition Type to match */
  CHAR8       *VolumeName;     /* Mounted filesystem volume name to match */
  CHAR16      *PartitionLabel; /* Partition label to match */
} PartiSelectFilter;

/* Output data providing more information about the device handle */
typedef struct {
  /* Handle that has BlkIO protocol installed, returned for all type of filters
   */
  EFI_HANDLE                     *Handle;

  /* Block IO protocol interface is returned for all type of filters */
  EFI_BLOCK_IO_PROTOCOL          *BlkIo;

  /* This HDD dev path is returned only if Matching Partition type is requested
   * It should be noted that the contents of this memory should NOT be changed
   */
  const HARDDRIVE_DEVICE_PATH    *PartitionInfo;
} HandleInfo;

/**
  Returns a list of BlkIo handles based on required criteria
  SelectionAttrib : Bitmask representing the conditions that need
                    to be met for the handles returned. Based on the
                    selections filter members should have valid values.
  FilterData      : Instance of Partition Select Filter structure that
                    needs extended data for certain type flags. For example
                    Partition type and/or Volume name can be specified.
  HandleInfoPtr   : Pointer Handle info where the information can be returned
  MaxBlkIopCnt    : On input, max number of handles the buffer can hold,
                    On output, the number of handles returned.

  @retval EFI_SUCCESS if the operation was successful
 */
EFI_STATUS EFIAPI
GetBlkIOHandles (
  IN UINT32             SelectionAttrib,
  IN PartiSelectFilter  *FilterData,
  OUT HandleInfo        *HandleInfoPtr,
  IN OUT UINT32         *MaxBlkIopCnt
  );

EFI_STATUS
GetStorageHandle (
  HandleInfo  *BlockIoHandle,
  UINT32      *MaxHandles
  );

#define MAX_NUM_PARTITIONS  128

struct StoragePartInfo {
  HandleInfo    HandleInfoList[MAX_NUM_PARTITIONS];
  UINT32        MaxHandles;
};

extern struct StoragePartInfo  gPtable;
extern EFI_PARTITION_ENTRY     gPtnEntries[MAX_NUM_PARTITIONS];

EFI_STATUS
PartitionGetInfo (
  IN CHAR16                  *PartitionName,
  OUT EFI_BLOCK_IO_PROTOCOL  **BlockIo,
  OUT EFI_HANDLE             **Handle
  );

VOID
PartitionDump (
  VOID
  );

EFI_STATUS
WriteGpt (
  UINT8   *ImageBuffer,
  UINT32  Size
  );

BOOLEAN
PartitionHasMultiSlot (
  CONST CHAR16  *Pname
  );

VOID
GetPartitionCount (
  UINT32  *Val
  );

EFI_STATUS
EnumeratePartitions (
  VOID
  );

VOID
UpdatePartitionEntries (
  VOID
  );

UINT64
GetPartitionSize (
  EFI_BLOCK_IO_PROTOCOL  *BlockIo
  );

VOID
ToLower (
  CHAR8  *Str
  );

UINT64
GetTimerCountms (
  VOID
  );

EFI_STATUS
ReadFromPartition (
  EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_HANDLE          *Handle,
  IN INT64               ReadOffset,
  IN UINT64              NumBytes,
  IN VOID                *Buffer,
  IN UINT64              *OutNumRead
  );

EFI_STATUS
ReadFromPartitionByName (
  IN CHAR16  *PartitionName,
  IN INT64   Offset,
  IN UINT64  Size,
  IN VOID    *Image
  );

EFI_STATUS
WriteToPartition (
  EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_HANDLE          *Handle,
  IN INT64               Offset,
  IN UINT64              Size,
  IN VOID                *Image
  );

EFI_STATUS
WriteToPartitionByName (
  IN CHAR16  *PartitionName,
  IN INT64   Offset,
  IN UINT64  Size,
  IN VOID    *Image
  );

EFI_STATUS
ErasePartition (
  EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  EFI_HANDLE             *Handle
  );

EFI_STATUS
ErasePartitionByName (
  IN CHAR16  *PartitionName
  );

BOOLEAN
IsPartitionBackDoor (
  VOID
  );

VOID
SetPartitionBackDoor (
  BOOLEAN  flag
  );

EFI_STATUS
GetBlockSize (
  UINT32  *Size
  );

VOID
FlashAllFromDDR (
  VOID
  );

typedef struct BackdoorInfo {
  CHAR16    PartitionName[MAX_GET_VAR_NAME_SIZE];
  VOID      *Addr;
  UINT64    PartitionSize;
} BackdoorInfo;

#endif
