/** @file  NorFlashStmmRuntimeDxe.c

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022 - 2024, CIX, Ltd. All rights reserved.
  Copyright (c) 2011 - 2021, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "NorFlashStmmRuntimeDxe.h"

STATIC EFI_EVENT  mNorFlashVirtualAddrChangeEvent;

//
// Global variable declarations
//
NOR_FLASH_INSTANCE  **mNorFlashInstances;
UINT32              mNorFlashDeviceCount;
UINTN               mFlashNvStorageVariableBase;
EFI_EVENT           mFvbVirtualAddrChangeEvent;

NOR_FLASH_INSTANCE  mNorFlashInstanceTemplate = {
  NOR_FLASH_SIGNATURE, // Signature
  NULL,                // Handle

  XSPI_FLASH_DIRECT_MMIO_ADDR, // DeviceBaseAddress
  SIZE_16MB,                   // Size
  NULL,                        // ShadowBuffer
  {
    0,           // MediaId
    FALSE,       // RemovableMedia
    TRUE,        // MediaPresent
    FALSE,       // LogicalPartition
    FALSE,       // ReadOnly
    FALSE,       // WriteCaching;
    SIZE_4KB,    // BlockSize
    4,           // IoAlign
    0xFFF,       // LastBlock  // 16MB flash size
    0,           // LowestAlignedLba
    1,           // LogicalBlocksPerPhysicalBlock
  }, // Media
  {
    EFI_BLOCK_IO_PROTOCOL_REVISION2, // Revision
    NULL,                            // Media ... NEED TO BE FILLED
    NorFlashBlockIoReset,            // Reset;
    NorFlashBlockIoReadBlocks,       // ReadBlocks
    NorFlashBlockIoWriteBlocks,      // WriteBlocks
    NorFlashBlockIoFlushBlocks       // FlushBlocks
  }, // BlockIoProtocol
  {
    EFI_DISK_IO_PROTOCOL_REVISION, // Revision
    NorFlashDiskIoReadDisk,        // ReadDisk
    NorFlashDiskIoWriteDisk        // WriteDisk
  }, // DiskIoProtocol
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8)(OFFSET_OF (NOR_FLASH_DEVICE_PATH, End)),
          (UINT8)(OFFSET_OF (NOR_FLASH_DEVICE_PATH, End) >> 8)
        }
      },
      {
        0  // GUID
      },
    },
    0, // Index
    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 }
    }
  }, // DevicePath
  {
    XSPI_FLASH_DIRECT_MMIO_ADDR,
    XSPI_FLASH_DMA_MMIO_ADDR,
    SIZE_64MB,
    0
  }, // AccessInfo

  XSPI_FLASH_DIRECT_MMIO_ADDR, // RegionBaseAddress
  0,                           // Blocks
  0,                           // StartLba
  {
    FvbGetAttributes,      // GetAttributes
    FvbSetAttributes,      // SetAttributes
    FvbGetPhysicalAddress, // GetPhysicalAddress
    FvbGetBlockSize,       // GetBlockSize
    FvbRead,               // Read
    FvbWrite,              // Write
    FvbEraseBlocks,        // EraseBlocks
    NULL,                  // ParentHandle
  } // FvbProtoccol
};

STATIC NOR_FLASH_REGION_DESCRIPTION  mNorFlashDevices[] = {
  {
    XSPI_FLASH_DIRECT_MMIO_ADDR,  // Memory mapped IO base address for nor flash
    SIZE_8MB,
    FixedPcdGet32 (PcdNorFlashNvramAddr),
    FixedPcdGet32 (PcdNorFlashNvramSize),
    SIZE_4KB,
  },
};

VOID
EFIAPI
FlashMemoryInitEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                    Status;
  NOR_FLASH_REGION_DESCRIPTION  *NorFlashDevices;

  NorFlashDevices = (NOR_FLASH_REGION_DESCRIPTION *)Context;
  //
  // Declare the Non-Volatile storage as EFI_MEMORY_RUNTIME
  //

  // Note: all the NOR Flash region needs to be reserved into the UEFI Runtime memory;
  //       even if we only use the small block region of the NOR Flash.
  //       The reason is when the NOR Flash memory is set into program mode, the command
  //       is written as the base of the flash region (ie: Instance->DeviceBaseAddress)
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  NorFlashDevices->DeviceBaseAddress,
                  NorFlashDevices->DeviceSize,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)) {
    DebugPrint (
      DEBUG_ERROR,
      "%a: fail to add memory space base 0x%x, size 0x%x, status %r\n",
      __FUNCTION__,
      NorFlashDevices->DeviceBaseAddress,
      NorFlashDevices->DeviceSize,
      Status
      );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "%a: add flash space base 0x%x, length 0x%x to memory space\n", __FUNCTION__, NorFlashDevices->DeviceBaseAddress, NorFlashDevices->DeviceSize));

  Status = gDS->SetMemorySpaceAttributes (
                  NorFlashDevices->DeviceBaseAddress,
                  NorFlashDevices->DeviceSize,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)) {
    DebugPrint (
      DEBUG_ERROR,
      "%a: fail to set memory attributes base 0x%x, size 0x%x, status %r\n",
      __FUNCTION__,
      NorFlashDevices->DeviceBaseAddress,
      NorFlashDevices->DeviceSize,
      Status
      );
    gDS->RemoveMemorySpace (
           NorFlashDevices->DeviceBaseAddress,
           NorFlashDevices->DeviceSize
           );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "%a: set flash memory space attribute 0x%llx\n", __FUNCTION__, EFI_MEMORY_UC | EFI_MEMORY_RUNTIME));
}

EFI_STATUS
EFIAPI
NorFlashFvbInitialize (
  IN NOR_FLASH_INSTANCE  *Instance
  )
{
  EFI_STATUS     Status;
  EFI_BOOT_MODE  BootMode;

  DEBUG ((DEBUG_INFO, "%a: enter\n", __FUNCTION__));
  ASSERT ((Instance != NULL));

  mFlashNvStorageVariableBase = (PcdGet64 (PcdFlashNvStorageVariableBase64) != 0) ?
                                PcdGet64 (PcdFlashNvStorageVariableBase64) : PcdGet32 (PcdFlashNvStorageVariableBase);

  mFlashNvStorageVariableBase += Instance->DeviceBaseAddress;
  // Set the region base address of the FVB
  Instance->RegionBaseAddress = mFlashNvStorageVariableBase;
  // Set the index of the first LBA for the FVB
  Instance->StartLba = (mFlashNvStorageVariableBase - Instance->DeviceBaseAddress) / Instance->Media.BlockSize;
  // Set the blocks number of the FVB
  Instance->Blocks = (PcdGet32 (PcdFlashNvStorageVariableSize) + PcdGet32 (PcdFlashNvStorageFtwWorkingSize) + PcdGet32 (PcdFlashNvStorageFtwSpareSize)) / \
                     Instance->Media.BlockSize;

  BootMode = GetBootModeHob ();
  if (BootMode == BOOT_WITH_DEFAULT_SETTINGS) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    // Determine if there is a valid header at the beginning of the NorFlash
    Status = ValidateFvHeader (Instance);
  }

  // Install the Default FVB header if required
  if (EFI_ERROR (Status)) {
    // There is no valid header. since we initialized one in STMM, if no valid here we exit directly
    DebugPrint (DEBUG_INFO, "%a: The FVB Header is not valid\n", __FUNCTION__);
    return Status;
  }

  //
  // The driver implementing the variable read service can now be dispatched;
  // the varstore headers are in place.
  //
  Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
                  &gEdkiiNvVarStoreFormattedGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  FvbVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mFvbVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "%a: exit\n", __FUNCTION__));

  return Status;
}

EFI_STATUS
NorFlashCreateInstance (
  IN UINTN                NorFlashDeviceBase,
  IN UINTN                NorFlashRegionBase,
  IN UINTN                NorFlashSize,
  IN UINT32               Index,
  IN UINT32               BlockSize,
  OUT NOR_FLASH_INSTANCE  **NorFlashInstance
  )
{
  EFI_STATUS          Status;
  NOR_FLASH_INSTANCE  *Instance;

  if (NorFlashInstance == NULL) {
    DebugPrint (DEBUG_ERROR, "%a: invalid NOR_FLASH_INSTANCE buffer, status %r\n", __FUNCTION__, EFI_INVALID_PARAMETER);
    return EFI_INVALID_PARAMETER;
  }

  Instance = AllocateRuntimeCopyPool (sizeof (NOR_FLASH_INSTANCE), &mNorFlashInstanceTemplate);
  if (Instance == NULL) {
    DebugPrint (DEBUG_ERROR, "%a: invalid NOR_FLASH_INSTANCE buffer, status %r\n", __FUNCTION__, EFI_OUT_OF_RESOURCES);
    return EFI_OUT_OF_RESOURCES;
  }

  Instance->DeviceBaseAddress = NorFlashDeviceBase;
  Instance->Size              = NorFlashSize;

  Instance->BlockIoProtocol.Media = &Instance->Media;
  Instance->Media.MediaId         = Index;
  Instance->Media.BlockSize       = BlockSize;
  Instance->Media.LastBlock       = (NorFlashSize / BlockSize) - 1;

  CopyGuid (&Instance->DevicePath.Vendor.Guid, &gCixNorFlashDevicePathGuid);
  Instance->DevicePath.Index = (UINT8)Index;

  Instance->AccessInfo.BaseAddress    = XSPI_FLASH_DIRECT_MMIO_ADDR;
  Instance->AccessInfo.DmaBaseAddress = XSPI_FLASH_DMA_MMIO_ADDR;
  Instance->AccessInfo.Size           = SIZE_64MB;
  Instance->AccessInfo.RemappedOffset = 0;

  Instance->ShadowBuffer = AllocateRuntimeZeroPool (BlockSize);
  if (Instance->ShadowBuffer == NULL) {
    DebugPrint (DEBUG_ERROR, "%a: invalid shadow buffer, status %r\n", __FUNCTION__, EFI_OUT_OF_RESOURCES);
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((DEBUG_INFO, "%a: block size 0x%x, region index %d\n", __FUNCTION__, BlockSize, Index));

  Status = NorFlashFvbInitialize (Instance);

  if (EFI_ERROR (Status)) {
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Instance->Handle,
                    &gEfiDevicePathProtocolGuid,
                    &Instance->DevicePath,
                    &gCixFlashBlockIoProtocolGuid,
                    &Instance->BlockIoProtocol,
                    &gEfiDiskIoProtocolGuid,
                    &Instance->DiskIoProtocol,
                    &gCixXspiInfoProtocolGuid,
                    &Instance->AccessInfo,
                    NULL
                    );
  } else {
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Instance->Handle,
                    &gEfiDevicePathProtocolGuid,
                    &Instance->DevicePath,
                    &gCixFlashBlockIoProtocolGuid,
                    &Instance->BlockIoProtocol,
                    &gEfiDiskIoProtocolGuid,
                    &Instance->DiskIoProtocol,
                    &gEfiFirmwareVolumeBlockProtocolGuid,
                    &Instance->FvbProtocol,
                    &gCixXspiInfoProtocolGuid,
                    &Instance->AccessInfo,
                    NULL
                    );
  }

  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "%a: fail to install protocols for nor flash %d, status %r\n", __FUNCTION__, Index, Status);
    FreePool (Instance);
    return Status;
  }

  *NorFlashInstance = Instance;

  return Status;
}

/**
  Fixup internal data so that EFI can be call in virtual mode.
  Call the passed in Child Notify event and convert any pointers in
  lib to virtual mode.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
NorFlashVirtualNotifyEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  UINTN  Index;

  for (Index = 0; Index < mNorFlashDeviceCount; Index++) {
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->DeviceBaseAddress);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->RegionBaseAddress);
    // Convert BlockIo protocol
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->BlockIoProtocol.FlushBlocks);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->BlockIoProtocol.ReadBlocks);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->BlockIoProtocol.Reset);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->BlockIoProtocol.WriteBlocks);
    // Convert DiskIo protocol
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->DiskIoProtocol.ReadDisk);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->DiskIoProtocol.WriteDisk);
    // Convert Fvb
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->FvbProtocol.EraseBlocks);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->FvbProtocol.GetAttributes);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->FvbProtocol.GetBlockSize);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->FvbProtocol.GetPhysicalAddress);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->FvbProtocol.Read);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->FvbProtocol.SetAttributes);
    EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->FvbProtocol.Write);

    if (mNorFlashInstances[Index]->ShadowBuffer != NULL) {
      EfiConvertPointer (0x0, (VOID **)&mNorFlashInstances[Index]->ShadowBuffer);
    }
  }

  return;
}

EFI_STATUS
EFIAPI
NorFlashInitialise (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  UINT32                        Index;
  NOR_FLASH_REGION_DESCRIPTION  *NorFlashDevices;

  EFI_EVENT  MemoryInitEvent;

  DEBUG ((DEBUG_INFO, "%a: enter\n", __FUNCTION__));

  POST_CODE (XspiInitDxeStart);

  Status = NorFlashPlatformInitialization ();
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "%a: fail to initialize nor flash devices, status %r\n", __FUNCTION__, Status);
    return Status;
  }

  NorFlashDevices      = mNorFlashDevices;
  mNorFlashDeviceCount = ARRAY_SIZE (mNorFlashDevices);

  mNorFlashInstances = AllocateRuntimePool (sizeof (NOR_FLASH_INSTANCE *) * mNorFlashDeviceCount);

  for (Index = 0; Index < mNorFlashDeviceCount; Index++) {
    EfiCreateProtocolNotifyEvent (
      &gEfiCpuArchProtocolGuid,
      TPL_CALLBACK,
      FlashMemoryInitEventNotify,
      &NorFlashDevices[Index],
      &MemoryInitEvent
      );

    Status = NorFlashCreateInstance (
               NorFlashDevices[Index].DeviceBaseAddress,
               NorFlashDevices[Index].RegionBaseAddress,
               NorFlashDevices[Index].DeviceSize,
               Index,
               NorFlashDevices[Index].BlockSize,
               &mNorFlashInstances[Index]
               );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a: fail to create instance for nor flash index %d, status %r\n", __FUNCTION__, Index, Status);
    }
  }

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  NorFlashVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mNorFlashVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  POST_CODE (XspiInitDxeEnd);

  DEBUG ((DEBUG_INFO, "%a exit\n", __FUNCTION__));

  return Status;
}
