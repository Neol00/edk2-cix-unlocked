/** @file  NorFlashFvb.c

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
 Copyright (c) 2022, CIX, Ltd. All rights reserved.
 Copyright (c) 2011 - 2021, Arm Limited. All rights reserved.<BR>

 SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "NorFlashFvb.h"

extern UINTN  mFlashNvStorageVariableBase;
///
/// The Firmware Volume Block Protocol is the low-level interface
/// to a firmware volume. File-level access to a firmware volume
/// should not be done using the Firmware Volume Block Protocol.
/// Normal access to a firmware volume must use the Firmware
/// Volume Protocol. Typically, only the file system driver that
/// produces the Firmware Volume Protocol will bind to the
/// Firmware Volume Block Protocol.
///

/**
  Initialises the FV Header and Variable Store Header
  to support variable operations.

  @param[in]  Ptr - Location to initialise the headers

**/
EFI_STATUS
InitializeFvAndVariableStoreHeaders (
  IN NOR_FLASH_INSTANCE  *Instance
  )
{
  EFI_STATUS                  Status;
  VOID                        *Headers;
  UINTN                       HeadersLength;
  EFI_FIRMWARE_VOLUME_HEADER  *FirmwareVolumeHeader;
  VARIABLE_STORE_HEADER       *VariableStoreHeader;
  UINT32                      NvStorageFtwSpareSize;
  UINT32                      NvStorageFtwWorkingSize;
  UINT32                      NvStorageVariableSize;
  UINT64                      NvStorageFtwSpareBase;
  UINT64                      NvStorageFtwWorkingBase;
  UINT64                      NvStorageVariableBase;

  DEBUG ((DEBUG_BLKIO, "%a enter\n", __FUNCTION__));

  HeadersLength = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + sizeof (VARIABLE_STORE_HEADER);
  Headers       = AllocateZeroPool (HeadersLength);

  NvStorageFtwWorkingSize = PcdGet32 (PcdFlashNvStorageFtwWorkingSize);
  NvStorageFtwSpareSize   = PcdGet32 (PcdFlashNvStorageFtwSpareSize);
  NvStorageVariableSize   = PcdGet32 (PcdFlashNvStorageVariableSize);

  NvStorageFtwSpareBase = (PcdGet64 (PcdFlashNvStorageFtwSpareBase64) != 0) ?
                          PcdGet64 (PcdFlashNvStorageFtwSpareBase64) : PcdGet32 (PcdFlashNvStorageFtwSpareBase);
  NvStorageFtwWorkingBase = (PcdGet64 (PcdFlashNvStorageFtwWorkingBase64) != 0) ?
                            PcdGet64 (PcdFlashNvStorageFtwWorkingBase64) : PcdGet32 (PcdFlashNvStorageFtwWorkingBase);
  NvStorageVariableBase = (PcdGet64 (PcdFlashNvStorageVariableBase64) != 0) ?
                          PcdGet64 (PcdFlashNvStorageVariableBase64) : PcdGet32 (PcdFlashNvStorageVariableBase);

  // FirmwareVolumeHeader->FvLength is declared to have the Variable area AND the FTW working area AND the FTW Spare contiguous.
  if ((NvStorageVariableBase + NvStorageVariableSize) != NvStorageFtwWorkingBase) {
    DebugPrint (DEBUG_ERROR, "%a: FTW working space is not contiguous with variable space\n", __FUNCTION__);
    return EFI_INVALID_PARAMETER;
  }

  if ((NvStorageFtwWorkingBase + NvStorageFtwWorkingSize) != NvStorageFtwSpareBase) {
    DebugPrint (DEBUG_ERROR, "%a: FTW spare space is not contiguous with FTW working space\n", __FUNCTION__);
    return EFI_INVALID_PARAMETER;
  }

  // Check if the size of the area is at least one block size
  if ((NvStorageVariableSize <= 0) || (NvStorageVariableSize / Instance->Media.BlockSize <= 0)) {
    DebugPrint (DEBUG_ERROR, "%a: variable space size is 0x%x, should be at least one block size\n", __FUNCTION__, NvStorageVariableSize);
    return EFI_INVALID_PARAMETER;
  }

  if ((NvStorageFtwWorkingSize <= 0) || (NvStorageFtwWorkingSize / Instance->Media.BlockSize <= 0)) {
    DebugPrint (DEBUG_ERROR, "%a: FTW working space size is 0x%x, should be at least one block size\n", __FUNCTION__, NvStorageFtwWorkingSize);
    return EFI_INVALID_PARAMETER;
  }

  if ((NvStorageFtwSpareSize <= 0) || (NvStorageFtwSpareSize / Instance->Media.BlockSize <= 0)) {
    DebugPrint (DEBUG_ERROR, "%a: FTW spare space size is 0x%x, should be at least one block size\n", __FUNCTION__, NvStorageFtwSpareSize);
    return EFI_INVALID_PARAMETER;
  }

  // Ensure the Variable area Base Addresses are aligned on a block size boundaries
  if ((NvStorageVariableBase % Instance->Media.BlockSize != 0) ||
      (NvStorageFtwWorkingBase % Instance->Media.BlockSize != 0) ||
      (NvStorageFtwSpareBase % Instance->Media.BlockSize != 0))
  {
    DebugPrint (DEBUG_ERROR, "%a: space base addresses must be aligned to block size boundaries\n", __FUNCTION__);
    return EFI_INVALID_PARAMETER;
  }

  //
  // EFI_FIRMWARE_VOLUME_HEADER
  //
  FirmwareVolumeHeader = (EFI_FIRMWARE_VOLUME_HEADER *)Headers;
  CopyGuid (&FirmwareVolumeHeader->FileSystemGuid, &gEfiSystemNvDataFvGuid);
  FirmwareVolumeHeader->FvLength =
    PcdGet32 (PcdFlashNvStorageVariableSize) +
    PcdGet32 (PcdFlashNvStorageFtwWorkingSize) +
    PcdGet32 (PcdFlashNvStorageFtwSpareSize);
  FirmwareVolumeHeader->Signature  = EFI_FVH_SIGNATURE;
  FirmwareVolumeHeader->Attributes = (EFI_FVB_ATTRIBUTES_2)(
                                                            EFI_FVB2_READ_ENABLED_CAP   | // Reads may be enabled
                                                            EFI_FVB2_READ_STATUS        | // Reads are currently enabled
                                                            EFI_FVB2_STICKY_WRITE       | // A block erase is required to flip bits into EFI_FVB2_ERASE_POLARITY
                                                            EFI_FVB2_ERASE_POLARITY     | // After erasure all bits take this value (i.e. '1')
                                                            EFI_FVB2_WRITE_STATUS       | // Writes are currently enabled
                                                            EFI_FVB2_WRITE_ENABLED_CAP    // Writes may be enabled
                                                            );
  FirmwareVolumeHeader->HeaderLength          = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY);
  FirmwareVolumeHeader->Revision              = EFI_FVH_REVISION;
  FirmwareVolumeHeader->BlockMap[0].NumBlocks = Instance->Blocks;
  FirmwareVolumeHeader->BlockMap[0].Length    = Instance->Media.BlockSize;
  FirmwareVolumeHeader->BlockMap[1].NumBlocks = 0;
  FirmwareVolumeHeader->BlockMap[1].Length    = 0;
  FirmwareVolumeHeader->Checksum              = CalculateCheckSum16 ((UINT16 *)FirmwareVolumeHeader, FirmwareVolumeHeader->HeaderLength);

  //
  // VARIABLE_STORE_HEADER
  //
  VariableStoreHeader = (VARIABLE_STORE_HEADER *)((UINTN)Headers + FirmwareVolumeHeader->HeaderLength);
  CopyGuid (&VariableStoreHeader->Signature, &gEfiAuthenticatedVariableGuid);
  VariableStoreHeader->Size   = PcdGet32 (PcdFlashNvStorageVariableSize) - FirmwareVolumeHeader->HeaderLength;
  VariableStoreHeader->Format = VARIABLE_STORE_FORMATTED;
  VariableStoreHeader->State  = VARIABLE_STORE_HEALTHY;

  // Install the combined super-header in the NorFlash
  Status = NorFlashWrite (Instance, Instance->StartLba, 0, HeadersLength, Headers);

  FreePool (Headers);
  return Status;
}

/**
  Check the integrity of firmware volume header.

  @param[in] FwVolHeader - A pointer to a firmware volume header

  @retval  EFI_SUCCESS   - The firmware volume is consistent
  @retval  EFI_NOT_FOUND - The firmware volume has been corrupted.

**/
EFI_STATUS
ValidateFvHeader (
  IN  NOR_FLASH_INSTANCE  *Instance
  )
{
  EFI_STATUS                  Status;
  UINT16                      Checksum;
  EFI_FIRMWARE_VOLUME_HEADER  *FwVolHeader = NULL;
  VARIABLE_STORE_HEADER       *VariableStoreHeader;
  UINTN                       VariableStoreLength;
  UINTN                       FvLength;

  FwVolHeader = AllocateZeroPool (sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + sizeof (VARIABLE_STORE_HEADER));
  if (FwVolHeader != NULL) {
    Status = NorFlashRead (Instance, Instance->StartLba, 0, sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + sizeof (VARIABLE_STORE_HEADER), FwVolHeader);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a: fail to read out firmware volume header, status %r\n", __FUNCTION__, Status);
      return Status;
    }
  } else {
    return EFI_OUT_OF_RESOURCES;
  }

  FvLength = PcdGet32 (PcdFlashNvStorageVariableSize) + PcdGet32 (PcdFlashNvStorageFtwWorkingSize) + PcdGet32 (PcdFlashNvStorageFtwSpareSize);

  //
  // Verify the header revision, header signature, length
  // Length of FvBlock cannot be 2**64-1
  // HeaderLength cannot be an odd number
  //
  if ((FwVolHeader->Revision  != EFI_FVH_REVISION) || (FwVolHeader->Signature != EFI_FVH_SIGNATURE) || (FwVolHeader->FvLength  != FvLength)) {
    DebugPrint (DEBUG_ERROR, "%a: no firmware volume header present\n", __FUNCTION__);
    return EFI_NOT_FOUND;
  }

  // Check the Firmware Volume Guid
  if (CompareGuid (&FwVolHeader->FileSystemGuid, &gEfiSystemNvDataFvGuid) == FALSE) {
    DebugPrint (DEBUG_ERROR, "%a: firmware volume guid non-compatible\n", __FUNCTION__);
    return EFI_NOT_FOUND;
  }

  // Verify the header checksum
  Checksum = CalculateSum16 ((UINT16 *)FwVolHeader, FwVolHeader->HeaderLength);
  if (Checksum != 0) {
    DebugPrint (DEBUG_ERROR, "%a: FV checksum is invalid (checksum:0x%X)\n", __FUNCTION__, Checksum);
    return EFI_NOT_FOUND;
  }

  VariableStoreHeader = (VARIABLE_STORE_HEADER *)((UINTN)FwVolHeader + FwVolHeader->HeaderLength);

  // Check the Variable Store Guid
  if (!CompareGuid (&VariableStoreHeader->Signature, &gEfiVariableGuid) &&
      !CompareGuid (&VariableStoreHeader->Signature, &gEfiAuthenticatedVariableGuid))
  {
    DebugPrint (DEBUG_ERROR, "%a: variable store guid non-compatible\n", __FUNCTION__);
    return EFI_NOT_FOUND;
  }

  VariableStoreLength = PcdGet32 (PcdFlashNvStorageVariableSize) - FwVolHeader->HeaderLength;
  if (VariableStoreHeader->Size != VariableStoreLength) {
    DebugPrint (DEBUG_ERROR, "%a: variable store length does not match\n", __FUNCTION__);
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
 The GetAttributes() function retrieves the attributes and
 current settings of the block.

 @param This         Indicates the EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL instance.

 @param Attributes   Pointer to EFI_FVB_ATTRIBUTES_2 in which the attributes and
                     current settings are returned.
                     Type EFI_FVB_ATTRIBUTES_2 is defined in EFI_FIRMWARE_VOLUME_HEADER.

 @retval EFI_SUCCESS The firmware volume attributes were returned.

 **/
EFI_STATUS
EFIAPI
FvbGetAttributes (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  OUT       EFI_FVB_ATTRIBUTES_2                 *Attributes
  )
{
  EFI_FVB_ATTRIBUTES_2  FlashFvbAttributes;
  NOR_FLASH_INSTANCE    *Instance;

  Instance = INSTANCE_FROM_FVB_THIS (This);

  FlashFvbAttributes = (EFI_FVB_ATTRIBUTES_2)(
                                              EFI_FVB2_READ_ENABLED_CAP | // Reads may be enabled
                                              EFI_FVB2_READ_STATUS      | // Reads are currently enabled
                                              EFI_FVB2_STICKY_WRITE     | // A block erase is required to flip bits into EFI_FVB2_ERASE_POLARITY
                                              EFI_FVB2_ERASE_POLARITY     // After erasure all bits take this value (i.e. '1')
                                              );

  // Check if it is write protected
  if (Instance->Media.ReadOnly != TRUE) {
    FlashFvbAttributes = FlashFvbAttributes         |
                         EFI_FVB2_WRITE_STATUS      | // Writes are currently enabled
                         EFI_FVB2_WRITE_ENABLED_CAP;  // Writes may be enabled
  }

  *Attributes = FlashFvbAttributes;

  DEBUG ((DEBUG_BLKIO, "%a: attributes 0x%X\n", __FUNCTION__, *Attributes));

  return EFI_SUCCESS;
}

/**
 The SetAttributes() function sets configurable firmware volume attributes
 and returns the new settings of the firmware volume.


 @param This                     Indicates the EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL instance.

 @param Attributes               On input, Attributes is a pointer to EFI_FVB_ATTRIBUTES_2
                                 that contains the desired firmware volume settings.
                                 On successful return, it contains the new settings of
                                 the firmware volume.
                                 Type EFI_FVB_ATTRIBUTES_2 is defined in EFI_FIRMWARE_VOLUME_HEADER.

 @retval EFI_SUCCESS             The firmware volume attributes were returned.

 @retval EFI_INVALID_PARAMETER   The attributes requested are in conflict with the capabilities
                                 as declared in the firmware volume header.

 **/
EFI_STATUS
EFIAPI
FvbSetAttributes (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  IN OUT    EFI_FVB_ATTRIBUTES_2                 *Attributes
  )
{
  DEBUG ((DEBUG_BLKIO, "%a: 0x%X is not supported\n", __FUNCTION__, *Attributes));

  return EFI_UNSUPPORTED;
}

/**
 The GetPhysicalAddress() function retrieves the base address of
 a memory-mapped firmware volume. This function should be called
 only for memory-mapped firmware volumes.

 @param This               Indicates the EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL instance.

 @param Address            Pointer to a caller-allocated
                           EFI_PHYSICAL_ADDRESS that, on successful
                           return from GetPhysicalAddress(), contains the
                           base address of the firmware volume.

 @retval EFI_SUCCESS       The firmware volume base address was returned.

 @retval EFI_NOT_SUPPORTED The firmware volume is not memory mapped.

 **/
EFI_STATUS
EFIAPI
FvbGetPhysicalAddress (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  OUT       EFI_PHYSICAL_ADDRESS                 *Address
  )
{
  return EFI_UNSUPPORTED;
}

/**
 The GetBlockSize() function retrieves the size of the requested
 block. It also returns the number of additional blocks with
 the identical size. The GetBlockSize() function is used to
 retrieve the block map (see EFI_FIRMWARE_VOLUME_HEADER).


 @param This                     Indicates the EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL instance.

 @param Lba                      Indicates the block for which to return the size.

 @param BlockSize                Pointer to a caller-allocated UINTN in which
                                 the size of the block is returned.

 @param NumberOfBlocks           Pointer to a caller-allocated UINTN in
                                 which the number of consecutive blocks,
                                 starting with Lba, is returned. All
                                 blocks in this range have a size of
                                 BlockSize.


 @retval EFI_SUCCESS             The firmware volume base address was returned.

 @retval EFI_INVALID_PARAMETER   The requested LBA is out of range.

 **/
EFI_STATUS
EFIAPI
FvbGetBlockSize (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  IN        EFI_LBA                              Lba,
  OUT       UINTN                                *BlockSize,
  OUT       UINTN                                *NumberOfBlocks
  )
{
  EFI_STATUS          Status;
  NOR_FLASH_INSTANCE  *Instance;

  Instance = INSTANCE_FROM_FVB_THIS (This);

  DEBUG ((DEBUG_BLKIO, "%a: LBA %ld, block size 0x%x, last block %ld\n", __FUNCTION__, Lba, Instance->Media.BlockSize, Instance->Blocks));

  if (Lba >= Instance->Blocks) {
    DebugPrint (DEBUG_ERROR, "%a: LBA %ld is beyond the region last LBA(%ld)\n", __FUNCTION__, Lba, Instance->Blocks);
    Status = EFI_INVALID_PARAMETER;
  } else {
    // This is easy because in this platform each NorFlash device has equal sized blocks.
    *BlockSize      = (UINTN)Instance->Media.BlockSize;
    *NumberOfBlocks = (UINTN)(Instance->Blocks - Lba);

    DEBUG ((DEBUG_BLKIO, "%a: block size 0x%x, number of blocks 0x%x\n", __FUNCTION__, *BlockSize, *NumberOfBlocks));

    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
 Reads the specified number of bytes into a buffer from the specified block.

 The Read() function reads the requested number of bytes from the
 requested block and stores them in the provided buffer.
 Implementations should be mindful that the firmware volume
 might be in the ReadDisabled state. If it is in this state,
 the Read() function must return the status code
 EFI_ACCESS_DENIED without modifying the contents of the
 buffer. The Read() function must also prevent spanning block
 boundaries. If a read is requested that would span a block
 boundary, the read must read up to the boundary but not
 beyond. The output parameter NumBytes must be set to correctly
 indicate the number of bytes actually read. The caller must be
 aware that a read may be partially completed.

 @param This                 Indicates the EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL instance.

 @param Lba                  The starting logical block index from which to read.

 @param Offset               Offset into the block at which to begin reading.

 @param NumBytes             Pointer to a UINTN.
                             At entry, *NumBytes contains the total size of the buffer.
                             At exit, *NumBytes contains the total number of bytes read.

 @param Buffer               Pointer to a caller-allocated buffer that will be used
                             to hold the data that is read.

 @retval EFI_SUCCESS         The firmware volume was read successfully,  and contents are
                             in Buffer.

 @retval EFI_BAD_BUFFER_SIZE Read attempted across an LBA boundary.
                             On output, NumBytes contains the total number of bytes
                             returned in Buffer.

 @retval EFI_ACCESS_DENIED   The firmware volume is in the ReadDisabled state.

 @retval EFI_DEVICE_ERROR    The block device is not functioning correctly and could not be read.

 **/
EFI_STATUS
EFIAPI
FvbRead (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  IN        EFI_LBA                              Lba,
  IN        UINTN                                Offset,
  IN OUT    UINTN                                *NumBytes,
  IN OUT    UINT8                                *Buffer
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  UINTN               BlockSize;
  NOR_FLASH_INSTANCE  *Instance;
  BOOLEAN             Spaned = FALSE;

  Instance = INSTANCE_FROM_FVB_THIS (This);

  DEBUG ((DEBUG_BLKIO, "%a: LBA %ld, offset 0x%x, number 0x%x, buffer @ 0x%08x\n", __FUNCTION__, Instance->StartLba + Lba, Offset, *NumBytes, Buffer));

  // Cache the block size to avoid de-referencing pointers all the time
  BlockSize = Instance->Media.BlockSize;

  // The read must not span block boundaries.
  // We need to check each variable individually because adding two large values together overflows.
  if ((Offset >= BlockSize) || (*NumBytes > BlockSize)) {
    DebugPrint (DEBUG_ERROR, "%a: read exceed block size 0x%x with offset 0x%x or bytes 0x%x\n", __FUNCTION__, BlockSize, Offset, *NumBytes);
    *NumBytes = 0;
    return EFI_BAD_BUFFER_SIZE;
  } else if ((Offset + *NumBytes) > BlockSize) {
    DebugPrint (DEBUG_ERROR, "%a: read exceed block size 0x%x with offset 0x%x, bytes 0x%x\n", __FUNCTION__, BlockSize, Offset, *NumBytes);
    Spaned = TRUE;
  }

  // We must have some bytes to read
  if (*NumBytes == 0) {
    DebugPrint (DEBUG_ERROR, "%a: no data to read\n", __FUNCTION__);
    return EFI_BAD_BUFFER_SIZE;
  }

  // Decide if we are doing full block reads or not.
  if (*NumBytes % BlockSize != 0) {
    if (Spaned) {
      *NumBytes = (UINT32)(BlockSize - Offset);
    }

    Status = NorFlashRead (Instance, Instance->StartLba + Lba, Offset, *NumBytes, Buffer);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a: fail to read from LBA %ld, offset 0x%x, number 0x%x, status %r\n", __FUNCTION__, Instance->StartLba + Lba, Offset, *NumBytes, Status);
      return EFI_DEVICE_ERROR;
    } else {
      if (Spaned) {
        Status = EFI_BAD_BUFFER_SIZE;
      }
    }
  } else {
    // Read NOR Flash data into buffer
    Status = NorFlashReadBlocks (Instance, Instance->StartLba + Lba, BlockSize, Buffer);
    if (EFI_ERROR (Status)) {
      // Return one of the pre-approved error statuses
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

/**
 Writes the specified number of bytes from the input buffer to the block.

 The Write() function writes the specified number of bytes from
 the provided buffer to the specified block and offset. If the
 firmware volume is sticky write, the caller must ensure that
 all the bits of the specified range to write are in the
 EFI_FVB_ERASE_POLARITY state before calling the Write()
 function, or else the result will be unpredictable. This
 unpredictability arises because, for a sticky-write firmware
 volume, a write may negate a bit in the EFI_FVB_ERASE_POLARITY
 state but cannot flip it back again.  Before calling the
 Write() function,  it is recommended for the caller to first call
 the EraseBlocks() function to erase the specified block to
 write. A block erase cycle will transition bits from the
 (NOT)EFI_FVB_ERASE_POLARITY state back to the
 EFI_FVB_ERASE_POLARITY state. Implementations should be
 mindful that the firmware volume might be in the WriteDisabled
 state. If it is in this state, the Write() function must
 return the status code EFI_ACCESS_DENIED without modifying the
 contents of the firmware volume. The Write() function must
 also prevent spanning block boundaries. If a write is
 requested that spans a block boundary, the write must store up
 to the boundary but not beyond. The output parameter NumBytes
 must be set to correctly indicate the number of bytes actually
 written. The caller must be aware that a write may be
 partially completed. All writes, partial or otherwise, must be
 fully flushed to the hardware before the Write() service
 returns.

 @param This                 Indicates the EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL instance.

 @param Lba                  The starting logical block index to write to.

 @param Offset               Offset into the block at which to begin writing.

 @param NumBytes             The pointer to a UINTN.
                             At entry, *NumBytes contains the total size of the buffer.
                             At exit, *NumBytes contains the total number of bytes actually written.

 @param Buffer               The pointer to a caller-allocated buffer that contains the source for the write.

 @retval EFI_SUCCESS         The firmware volume was written successfully.

 @retval EFI_BAD_BUFFER_SIZE The write was attempted across an LBA boundary.
                             On output, NumBytes contains the total number of bytes
                             actually written.

 @retval EFI_ACCESS_DENIED   The firmware volume is in the WriteDisabled state.

 @retval EFI_DEVICE_ERROR    The block device is malfunctioning and could not be written.


 **/
EFI_STATUS
EFIAPI
FvbWrite (
  IN CONST  EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  IN        EFI_LBA                              Lba,
  IN        UINTN                                Offset,
  IN OUT    UINTN                                *NumBytes,
  IN        UINT8                                *Buffer
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  NOR_FLASH_INSTANCE  *Instance;
  UINTN               BlockSize;
  BOOLEAN             Spaned = FALSE;

  Instance = INSTANCE_FROM_FVB_THIS (This);

  DEBUG ((DEBUG_BLKIO, "%a: LBA %ld, offset 0x%x, bytes 0x%x, buffer @ 0x%08x\n", __FUNCTION__, Instance->StartLba + Lba, Offset, *NumBytes, Buffer));

  // Detect WriteDisabled state
  if (Instance->Media.ReadOnly == TRUE) {
    DebugPrint (DEBUG_ERROR, "%a: device is in WriteDisabled state\n", __FUNCTION__);
    // It is in WriteDisabled state, return an error right away
    return EFI_ACCESS_DENIED;
  }

  // Cache the block size to avoid de-referencing pointers all the time
  BlockSize = Instance->Media.BlockSize;

  // The write must not span block boundaries.
  // We need to check each variable individually because adding two large values together overflows.
  if ((Offset >= BlockSize) || (*NumBytes > BlockSize)) {
    DebugPrint (DEBUG_ERROR, "%a: write exceed block size 0x%x with offset 0x%x or bytes 0x%x)\n", __FUNCTION__, BlockSize, Offset, *NumBytes);
    *NumBytes = 0;
    return EFI_BAD_BUFFER_SIZE;
  } else if ((*NumBytes + Offset) > BlockSize) {
    DebugPrint (DEBUG_ERROR, "%a: write exceed block size 0x%x with offset 0x%x, bytes 0x%x)\n", __FUNCTION__, BlockSize, Offset, *NumBytes);
    Spaned = TRUE;
  }

  // We must have some bytes to read
  if (*NumBytes == 0) {
    DebugPrint (DEBUG_ERROR, "%a: no data to write\n", __FUNCTION__);
    return EFI_BAD_BUFFER_SIZE;
  }

  // Decide if we are doing full block reads or not.
  if (*NumBytes % BlockSize != 0) {
    if (Spaned) {
      *NumBytes = (UINT32)(BlockSize - Offset);
    }

    Status = NorFlashWrite (Instance, Instance->StartLba + Lba, Offset, *NumBytes, Buffer);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a: fail to write data to LBA %ld, offset 0x%x, bytes 0x%x, status %r\n", __FUNCTION__, Instance->StartLba + Lba, Offset, *NumBytes % BlockSize, Status);
      return Status;
    } else {
      if (Spaned) {
        Status = EFI_BAD_BUFFER_SIZE;
      }
    }
  } else {
    // Write buffer data to NOR Flash
    Status = NorFlashWriteBlocks (Instance, Instance->StartLba + Lba, BlockSize, Buffer);
    if (EFI_ERROR (Status)) {
      // Return one of the pre-approved error statuses
      return EFI_DEVICE_ERROR;
    }
  }

  return Status;
}

/**
 Erases and initialises a firmware volume block.

 The EraseBlocks() function erases one or more blocks as denoted
 by the variable argument list. The entire parameter list of
 blocks must be verified before erasing any blocks. If a block is
 requested that does not exist within the associated firmware
 volume (it has a larger index than the last block of the
 firmware volume), the EraseBlocks() function must return the
 status code EFI_INVALID_PARAMETER without modifying the contents
 of the firmware volume. Implementations should be mindful that
 the firmware volume might be in the WriteDisabled state. If it
 is in this state, the EraseBlocks() function must return the
 status code EFI_ACCESS_DENIED without modifying the contents of
 the firmware volume. All calls to EraseBlocks() must be fully
 flushed to the hardware before the EraseBlocks() service
 returns.

 @param This                     Indicates the EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL
 instance.

 @param ...                      The variable argument list is a list of tuples.
                                 Each tuple describes a range of LBAs to erase
                                 and consists of the following:
                                 - An EFI_LBA that indicates the starting LBA
                                 - A UINTN that indicates the number of blocks to erase.

                                 The list is terminated with an EFI_LBA_LIST_TERMINATOR.
                                 For example, the following indicates that two ranges of blocks
                                 (5-7 and 10-11) are to be erased:
                                 EraseBlocks (This, 5, 3, 10, 2, EFI_LBA_LIST_TERMINATOR);

 @retval EFI_SUCCESS             The erase request successfully completed.

 @retval EFI_ACCESS_DENIED       The firmware volume is in the WriteDisabled state.

 @retval EFI_DEVICE_ERROR        The block device is not functioning correctly and could not be written.
                                 The firmware device may have been partially erased.

 @retval EFI_INVALID_PARAMETER   One or more of the LBAs listed in the variable argument list do
                                 not exist in the firmware volume.

 **/
EFI_STATUS
EFIAPI
FvbEraseBlocks (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL  *This,
  ...
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  VA_LIST             Args;
  UINTN               BlockAddress;   // Physical address of LBA to erase
  EFI_LBA             StartingLba;    // LBA from which we start erasing
  UINTN               NumOfLba;       // Number of LBA blocks to erase
  NOR_FLASH_INSTANCE  *Instance;

  Instance = INSTANCE_FROM_FVB_THIS (This);

  // Detect WriteDisabled state
  if (Instance->Media.ReadOnly == TRUE) {
    // Firmware volume is in WriteDisabled state
    DebugPrint (DEBUG_ERROR, "%a: device is in WriteDisabled state.\n", __FUNCTION__);
    return EFI_ACCESS_DENIED;
  }

  // Before erasing, check the entire list of parameters to ensure all specified blocks are valid
  VA_START (Args, This);
  do {
    // Get the Lba from which we start erasing
    StartingLba = VA_ARG (Args, EFI_LBA);

    // Have we reached the end of the list?
    if (StartingLba == EFI_LBA_LIST_TERMINATOR) {
      // Exit the while loop
      break;
    }

    // How many Lba blocks are we requested to erase?
    NumOfLba = VA_ARG (Args, UINTN);

    // All blocks must be within range
    DEBUG ((DEBUG_BLKIO, "%a: start block %ld, number of blocks %Lu, last block %ld\n", __FUNCTION__, Instance->StartLba + StartingLba, (UINT64)NumOfLba, Instance->StartLba + Instance->Blocks - 1));
    if ((NumOfLba == 0) || ((StartingLba + NumOfLba) > Instance->Blocks)) {
      VA_END (Args);
      DebugPrint (DEBUG_ERROR, "%a: LBA range goes past the last LBA\n", __FUNCTION__);
      Status = EFI_INVALID_PARAMETER;
      goto EXIT;
    }
  } while (TRUE);

  VA_END (Args);

  //
  // To get here, all must be ok, so start erasing
  //
  VA_START (Args, This);
  do {
    StartingLba = VA_ARG (Args, EFI_LBA);
    if (StartingLba == EFI_LBA_LIST_TERMINATOR) {
      // Exit the while loop
      break;
    }

    // Get the physical address of Lba to erase
    BlockAddress = GET_NOR_BLOCK_ADDRESS (Instance->DeviceBaseAddress, Instance->StartLba + StartingLba, Instance->Media.BlockSize);
    DEBUG ((DEBUG_BLKIO, "%a: erasing LBA %ld @ 0x%08x.\n", __FUNCTION__, Instance->StartLba + StartingLba, BlockAddress));

    NumOfLba = VA_ARG (Args, UINTN);

    Status = NorFlashErase (Instance, Instance->StartLba + StartingLba, NumOfLba * Instance->Media.BlockSize);
    if (EFI_ERROR (Status)) {
      VA_END (Args);
      Status = EFI_DEVICE_ERROR;
      goto EXIT;
    }
  } while (TRUE);

  VA_END (Args);

EXIT:
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
FvbVirtualNotifyEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EfiConvertPointer (0x0, (VOID **)&mFlashNvStorageVariableBase);
  return;
}
