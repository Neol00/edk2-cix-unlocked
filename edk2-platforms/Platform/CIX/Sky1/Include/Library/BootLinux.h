/** @BootLinux.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2015-2020, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __BOOTLINUXLIB_H__
#define __BOOTLINUXLIB_H__

#include <Uefi.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/Gpt.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/EfiFileLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>

#include "BootImage.h"
#include "DeviceInfo.h"
#include "LocateDeviceTree.h"
#include "ShutdownServices.h"
#include "UpdateCmdLine.h"
#include "UpdateDeviceTree.h"
#include "VerifiedBoot.h"

#define ALIGN32_BELOW(addr)        ALIGN_POINTER (addr - 32, 32)
#define LOCAL_ROUND_TO_PAGE(x, y)  (((x) + (y - 1)) & (~(y - 1)))
#define ROUND_TO_PAGE(x, y)        ((ADD_OF ((x), (y))) & (~(y)))
#define ALIGN_PAGES(x, y)          (((x) + (y - 1)) / (y))

#define ALIGNMENT_MASK_4KB           4096
#define MAX_NUMBER_OF_LOADED_IMAGES  32

/* Size of the header that is used in case the boot image has
 * a uncompressed kernel + appended dtb */
#define PATCHED_KERNEL_HEADER_SIZE  20

/* String used to determine if the boot image has
 * a uncompressed kernel + appended dtb */
#define PATCHED_KERNEL_MAGIC  "UNCOMPRESSED_IMG"

// Size reserved for DT image
#define DT_SIZE_2MB  (2 * 1024 * 1024)

#define KERNEL_32BIT_LOAD_OFFSET  0x8000
#define KERNEL_64BIT_LOAD_OFFSET  0x80000

#define MAX_GPT_NAME_SIZE   72
#define INVALID_PTN         -1
#define MAX_SLOT_SUFFIX_SZ  3

typedef enum {
  KERNEL_32BIT = 0,
  KERNEL_64BIT
} KernelMode;

typedef enum {
  LOAD_ADDR_NONE = 0,
  LOAD_ADDR_KERNEL,
  LOAD_ADDR_RAMDISK
} AddrType;

typedef struct {
  CHAR16    Suffix[MAX_SLOT_SUFFIX_SZ];
} Slot;

typedef VOID (*LINUX_KERNEL) (
  UINT64  ParametersBase,
  UINT64  Reserved0,
  UINT64  Reserved1,
  UINT64  Reserved2
  );
typedef VOID (*LINUX_KERNEL32) (
  UINT32  Zero,
  UINT32  Arch,
  UINTN   ParametersBase
  );

typedef enum {
  IMG_BOOT = 0,
  IMG_INIT_BOOT,
  IMG_DTBO,
  IMG_VBMETA,
  IMG_VENDOR_BOOT,
  IMG_MAX
} img_type;

typedef struct {
  CHAR8    *Name;
  VOID     *ImageBuffer;
  UINTN    ImageSize;
} ImageData;

typedef struct BootInfo {
  BOOLEAN      MultiSlotBoot;
  BOOLEAN      BootIntoRecovery;
  BOOLEAN      BootIntoFastboot;
  BOOLEAN      BootReasonAlarm;
  CHAR16       Pname[MAX_GPT_NAME_SIZE];
  CHAR16       BootableSlot[MAX_GPT_NAME_SIZE];
  ImageData    Images[MAX_NUMBER_OF_LOADED_IMAGES];
  UINTN        NumLoadedImages;
  CHAR8        *VBCmdLine;
  UINT32       VBCmdLineLen;
  UINT32       VBCmdLineFilledLen;
  VOID         *VBData;
  UINT32       HeaderVersion;
} BootInfo;

#define EFI_BOOTINFO_PROTOCOL_GUID  { 0xcc9ad738, 0xb5cc, 0x4f65, {0xf0, 0x77, 0xf8, 0x63, 0x41, 0x07}}

typedef struct {
  BootInfo* Info;
} EFI_BOOTINFO_PROTOCOL;

typedef struct BootLinuxParamlist {
  // Read this from RAM partition table
  UINT64     BaseMemory;

  VOID       *ImageBuffer;
  UINT64     ImageSize;
  VOID       *DtboImgBuffer;

  // Valid only for boot image header version greater than 2
  VOID       *VendorImageBuffer;
  UINT64     VendorImageSize;

  // Valid only for boot image header version greater than 2
  VOID       *InitImageBuffer;
  UINT64     InitImageSize;

  /* Load addresses for kernel, ramdisk, dt
   * These addresses are either predefined or get from UEFI core */
  UINT64     KernelLoadAddr;
  UINT64     KernelEndAddr;
  UINT64     RamdiskLoadAddr;
  UINT64     DeviceTreeLoadAddr;
  UINT64     *HypDtboBaseAddr;
  UINT32     NumHypDtbos;

  // Get the below fields info from the bootimage header
  UINT32     PageSize;
  UINT32     KernelSize;
  UINT32     PatchedKernelHdrSize;
  UINT32     DtbOffset;

  // Get the below fields info from the init_boot image header
  UINT32     RamdiskSize;
  UINT32     RamdiskOffset;

  // Get the below fields info from the vendor_boot image header
  // Valid only for boot image header version greater than 2
  UINT32     VendorRamdiskSize;

  // Kernel size rounded off based on the page size
  UINT32     KernelSizeActual;

  // BootConfig
  UINT32 BootConfigLoadAddr;
  UINT32 BootConfigSize;

  CHAR8      *FinalCmdLine;
  CHAR8      *CmdLine;
  BOOLEAN    BootingWith32BitKernel;
  BOOLEAN    BootingWithPatchedKernel;
  BOOLEAN    BootingWithGzipPkgKernel;
} BootParamlist;

EFI_STATUS
BootLinux (
  BootInfo  *Info
  );

EFI_STATUS
CheckImageHeader (
  VOID     *ImageHdrBuffer,
  UINT32   ImageHdrSize,
  VOID     *VendorImageHdrBuffer,
  UINT32   VendorImageHdrSize,
  UINT32   *ImageSizeActual,
  UINT32   *PageSize,
  BOOLEAN  BootIntoRecovery
  );

EFI_STATUS
LoadImageHeader (
  CHAR16  *Pname,
  VOID    **ImageHdrBuffer,
  UINT32  *ImageHdrSize
  );

EFI_STATUS
LoadImage (
  CHAR16  *Pname,
  VOID    **ImageBuffer,
  UINT32  ImageSizeActual,
  UINT32  PageSize
  );

BOOLEAN
IsBuildAsSystemRootImage (
  VOID
  );

BOOLEAN
IsBuildUseRecoveryAsBoot (
  VOID
  );

EFI_STATUS
GetImage (
  CONST BootInfo  *Info,
  VOID            **ImageBuffer,
  UINTN           *ImageSize,
  CHAR8           *ImageName
  );

BOOLEAN
LoadAndValidateDtboImg (
  BootInfo       *Info,
  BootParamlist  *BootParamlistPtr
  );

BOOLEAN
IsDynamicPartitionSupport (
  VOID
  );

UINT64
SetandGetLoadAddr (
  BootParamlist  *BootParamlistPtr,
  AddrType       Type
  );

#endif
