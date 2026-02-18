/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2017-2018, Andrey Warkentin <andrey.warkentin@gmail.com>
 *  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <PiPei.h>

#include <Library/ArmMmuLib.h>
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

VOID
BuildMemoryTypeInformationHob (
  VOID
  );

STATIC
VOID
InitMmu (
  IN ARM_MEMORY_REGION_DESCRIPTOR  *MemoryTable
  )
{
  RETURN_STATUS  Status;

  // Note: Because we called PeiServicesInstallPeiMemory() before to call InitMmu() the MMU Page Table
  //       resides in DRAM (even at the top of DRAM as it is the first permanent memory allocation)
  Status = ArmConfigureMmu (MemoryTable, NULL, NULL);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error: Failed to enable MMU\n"));
  }
}

STATIC
VOID
AddBasicMemoryRegion (
  IN ARM_MEMORY_REGION_DESCRIPTOR  *Desc
  )
{
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_TESTED,
    Desc->PhysicalBase,
    Desc->Length
    );
}

STATIC
VOID
AddRuntimeServicesRegion (
  IN ARM_MEMORY_REGION_DESCRIPTOR  *Desc
  )
{
  AddBasicMemoryRegion (Desc);

  BuildMemoryAllocationHob (
    Desc->PhysicalBase,
    Desc->Length,
    EfiRuntimeServicesData
    );
}

STATIC
VOID
AddUnmappedMemoryRegion (
  IN ARM_MEMORY_REGION_DESCRIPTOR  *Desc
  )
{
  // Do nothing
}

STATIC
VOID
AddReservedMemoryRegion (
  IN ARM_MEMORY_REGION_DESCRIPTOR  *Desc
  )
{
  AddBasicMemoryRegion (Desc);

  BuildMemoryAllocationHob (
    Desc->PhysicalBase,
    Desc->Length,
    EfiReservedMemoryType
    );
}

void  (*AddRegion[])(
  IN ARM_MEMORY_REGION_DESCRIPTOR  *Desc
  ) = {
  AddUnmappedMemoryRegion,
  AddBasicMemoryRegion,
  AddRuntimeServicesRegion,
  AddReservedMemoryRegion,
};

EFI_STATUS
ApPerformMemTest (
  IN UINT64  StartAddr,
  IN UINT64  EndAddr
  )
{
  UINT64   TargetAddr;
  BOOLEAN  Pass;

  Pass = TRUE;

  // for (TargetAddr = StartAddr; TargetAddr < EndAddr; TargetAddr += sizeof (UINT64)) {
  //   *((UINT64 *)TargetAddr) = TargetAddr;
  // }

  for (TargetAddr = StartAddr; TargetAddr < EndAddr; TargetAddr += sizeof (UINT64)) {
    DEBUG ((DEBUG_INFO, "%lx\n", TargetAddr));
    *((UINT64 *)TargetAddr) = TargetAddr;
    if (*((UINT64 *)(UINT64)TargetAddr) != TargetAddr) {
      Pass = FALSE;
    }
  }

  if (Pass) {
    // Print (L"\tAddr:0x%08x Pass\n", StartAddr);
    DEBUG ((DEBUG_INFO, "Addr:0x%lx-%lx Pass\n", StartAddr, EndAddr));
  }

  return EFI_SUCCESS;
}

/*++

Routine Description:



Arguments:

  FileHandle  - Handle of the file being invoked.
  PeiServices - Describes the list of possible PEI Services.

Returns:

  Status -  EFI_SUCCESS if the boot mode could be set

--*/
EFI_STATUS
EFIAPI
MemoryPeim (
  IN EFI_PHYSICAL_ADDRESS  UefiMemoryBase,
  IN UINT64                UefiMemorySize
  )
{
  ARM_MEMORY_REGION_DESCRIPTOR  *MemoryTable;

  UINTN  Index;

  // Get Virtual Memory Map from the Platform Library
  ArmPlatformGetVirtualMemoryMap (&MemoryTable);

  // Get additional info not provided by MemoryTable

  // Register each memory region
  for (Index = 0; MemoryTable[Index].Length != 0; Index++) {
    DEBUG ((
      DEBUG_INFO,
      "\tPhysicalBase: 0x%lX\n"
      "\tVirtualBase: 0x%lX\n"
      "\tLength: 0x%lX\n",
      MemoryTable[Index].PhysicalBase,
      MemoryTable[Index].VirtualBase,
      MemoryTable[Index].Length
      ));
  }

  // Mark the memory covering the Firmware Device as boot services data
  BuildMemoryAllocationHob (
    PcdGet64 (PcdFdBaseAddress),
    PcdGet32 (PcdFdSize),
    EfiBootServicesData
    );

  BuildMemoryAllocationHob (
    FixedPcdGet32 (PcdKernelBootImgBase),
    FixedPcdGet32 (PcdKernelBootImgSize),
    EfiReservedMemoryType
    );
 #if FixedPcdGetBool (PcdGpuEnable) == 1
  BuildMemoryAllocationHob (
    FixedPcdGet32 (PcdGpuReservedMemoryBase),
    FixedPcdGet32 (PcdGpuReservedMemorySize),
    EfiReservedMemoryType
    );
 #endif
 #if FixedPcdGetBool (PcdNpuEnable) == 1
  BuildMemoryAllocationHob (
    FixedPcdGet32 (PcdNpuReservedMemoryBase),
    FixedPcdGet32 (PcdNpuReservedMemorySize),
    EfiReservedMemoryType
    );
 #endif
  // BuildMemoryAllocationHob (
  //   FixedPcdGet32 (PcdReservedLinuxBase),
  //   FixedPcdGet32 (PcdReservedLinuxSize),
  //   EfiReservedMemoryType);

  BuildMemoryAllocationHob (
    FixedPcdGet32 (PcdArmLcdDdrFrameBufferBase),
    FixedPcdGet32 (PcdArmLcdDdrFrameBufferSize),
    EfiReservedMemoryType
    );

 #ifdef ANDROID_BOOT
  // BuildMemoryAllocationHob (
  //   FixedPcdGet32 (PcdReservedAndroidBase),
  //   FixedPcdGet32 (PcdReservedAndroidSize),
  //   EfiReservedMemoryType);
 #endif

  BuildMemoryAllocationHob (
    FixedPcdGet64 (PcdReservedSecureMemoryBase),
    FixedPcdGet64 (PcdReservedSecureMemorySize),
    EfiReservedMemoryType
    );

  BuildMemoryAllocationHob (
    FixedPcdGet64 (PcdReservedShareMemoryBase),
    FixedPcdGet64 (PcdReservedShareMemorySize),
    EfiRuntimeServicesData
    );

  // Build Memory Allocation Hob
  InitMmu (MemoryTable);

  // ApPerformMemTest(0x84600000, 0x80000000 + FixedPcdGet32(PcdSystemMemorySize) - 0x00800000);

  if (FeaturePcdGet (PcdPrePiProduceMemoryTypeInformationHob)) {
    // Optional feature that helps prevent EFI memory map fragmentation.
    BuildMemoryTypeInformationHob ();
  }

  return EFI_SUCCESS;
}
