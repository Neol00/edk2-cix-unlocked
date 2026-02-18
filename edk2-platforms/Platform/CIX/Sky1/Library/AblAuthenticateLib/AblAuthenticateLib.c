/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/BmpSupportLib.h>
#include <Library/FileHandleLib.h>
#include <Library/SortLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/Shell.h>
#include <Protocol/FirmwareManagement.h>
#include <Guid/GlobalVariable.h>
#include <Guid/CapsuleReport.h>
#include <Guid/SystemResourceTable.h>
#include <Guid/FmpCapsule.h>
#include <Guid/FileInfo.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/CapsuleVendor.h>
#include <Guid/Gpt.h>
#include <IndustryStandard/WindowsUxCapsule.h>
#include <Library/AblAuthenticateLib.h>

#define CAPSULE_HEADER_SIZE  0x20

#define NESTED_CAPSULE_HEADER_SIZE  SIZE_4KB
#define SYSTEM_FIRMWARE_FLAG        0x50000
#define DEVICE_FIRMWARE_FLAG        0x78010

#define MAJOR_VERSION  1
#define MINOR_VERSION  0

#define MAX_CAPSULE_NUM  10

//
// (20 * (6+5+2))+1) unicode characters from EFI FAT spec (doubled for bytes)
//
#define MAX_FILE_NAME_SIZE  522
#define MAX_FILE_NAME_LEN   (MAX_FILE_NAME_SIZE / sizeof(CHAR16))

//
// Define how many block descriptors we want to test with.
//
UINTN  NumberOfDescriptors1 = 1;

/**
  Validate if it is valid capsule header

  This function assumes the caller provided correct CapsuleHeader pointer
  and CapsuleSize.

  This function validates the fields in EFI_CAPSULE_HEADER.

  @param[in] CapsuleHeader  Points to a capsule header.
  @param[in] CapsuleSize    Size of the whole capsule image.

**/
BOOLEAN
AblIsValidCapsuleHeader (
  IN EFI_CAPSULE_HEADER  *CapsuleHeader,
  IN UINT64              CapsuleSize
  )
{
  if (CapsuleSize < sizeof (EFI_CAPSULE_HEADER)) {
    return FALSE;
  }

  if (CapsuleHeader->CapsuleImageSize != CapsuleSize) {
    return FALSE;
  }

  if (CapsuleHeader->HeaderSize > CapsuleHeader->CapsuleImageSize) {
    return FALSE;
  }

  if (CapsuleHeader->HeaderSize < sizeof (EFI_CAPSULE_HEADER)) {
    return FALSE;
  }

  return TRUE;
}

/**
  Build Gather list for a list of capsule images.

  @param[in]  CapsuleBuffer    An array of pointer to capsule images
  @param[in]  FileSize         An array of UINTN to capsule images size
  @param[in]  CapsuleNum       The count of capsule images
  @param[out] BlockDescriptors The block descriptors for the capsule images

  @retval EFI_SUCCESS The block descriptors for the capsule images are constructed.
**/
EFI_STATUS
AblBuildGatherList (
  IN VOID                           **CapsuleBuffer,
  IN UINTN                          *FileSize,
  IN UINTN                          CapsuleNum,
  OUT EFI_CAPSULE_BLOCK_DESCRIPTOR  **BlockDescriptors
  )
{
  EFI_STATUS                    Status;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *BlockDescriptors1;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *BlockDescriptors2;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *BlockDescriptorPre;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *BlockDescriptorsHeader;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *TempBlockPtr;
  UINT8                         *TempDataPtr;
  UINTN                         SizeLeft;
  UINTN                         Size;
  INT32                         Count;
  INT32                         Number;
  UINTN                         Index;

  TempBlockPtr           = NULL;
  BlockDescriptors1      = NULL;
  BlockDescriptors2      = NULL;
  BlockDescriptorPre     = NULL;
  BlockDescriptorsHeader = NULL;

  for (Index = 0; Index < CapsuleNum; Index++) {
    //
    // Allocate memory for the descriptors.
    //
    if (NumberOfDescriptors1 == 1) {
      Count = 2;
    } else {
      Count = (INT32)(NumberOfDescriptors1 + 2) / 2;
    }

    Size              = Count * sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR);
    BlockDescriptors1 = AllocateRuntimeZeroPool (Size);
    if (BlockDescriptors1 == NULL) {
      Print (L"CapsuleApp: failed to allocate memory for descriptors\n");
      Status = EFI_OUT_OF_RESOURCES;
      goto ERREXIT;
    } else {
      Print (L"CapsuleApp: creating capsule descriptors at 0x%X\n", (UINTN)BlockDescriptors1);
      Print (L"CapsuleApp: capsule data starts          at 0x%X with size 0x%X\n", (UINTN)CapsuleBuffer[Index], FileSize[Index]);
    }

    //
    // Record descriptor header
    //
    if (Index == 0) {
      BlockDescriptorsHeader = BlockDescriptors1;
    }

    if (BlockDescriptorPre != NULL) {
      BlockDescriptorPre->Union.ContinuationPointer = (UINTN)BlockDescriptors1;
      BlockDescriptorPre->Length                    = 0;
    }

    //
    // Fill them in
    //
    TempBlockPtr = BlockDescriptors1;
    TempDataPtr  = CapsuleBuffer[Index];
    SizeLeft     = FileSize[Index];
    for (Number = 0; (Number < Count - 1) && (SizeLeft != 0); Number++) {
      //
      // Divide remaining data in half
      //
      if (NumberOfDescriptors1 != 1) {
        if (SizeLeft == 1) {
          Size = 1;
        } else {
          Size = SizeLeft / 2;
        }
      } else {
        Size = SizeLeft;
      }

      TempBlockPtr->Union.DataBlock = (UINTN)TempDataPtr;
      TempBlockPtr->Length          = Size;
      Print (L"CapsuleApp: capsule block/size              0x%X/0x%X\n", (UINTN)TempDataPtr, Size);
      SizeLeft    -= Size;
      TempDataPtr += Size;
      TempBlockPtr++;
    }

    //
    // Allocate the second list, point the first block's last entry to point
    // to this one, and fill this one in. Worst case is that the previous
    // list only had one element that pointed here, so we need at least two
    // elements -- one to point to all the data, another to terminate the list.
    //
    if ((NumberOfDescriptors1 != 1) && (SizeLeft != 0)) {
      Count = (INT32)(NumberOfDescriptors1 + 2) - Count;
      if (Count == 1) {
        Count++;
      }

      Size              = Count * sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR);
      BlockDescriptors2 = AllocateRuntimeZeroPool (Size);
      if (BlockDescriptors2 == NULL) {
        Print (L"CapsuleApp: failed to allocate memory for descriptors\n");
        Status = EFI_OUT_OF_RESOURCES;
        goto ERREXIT;
      }

      //
      // Point the first list's last element to point to this second list.
      //
      TempBlockPtr->Union.ContinuationPointer = (UINTN)BlockDescriptors2;

      TempBlockPtr->Length = 0;
      TempBlockPtr         = BlockDescriptors2;
      for (Number = 0; Number < Count - 1; Number++) {
        //
        // If second-to-last one, then dump rest to this element
        //
        if (Number == (Count - 2)) {
          Size = SizeLeft;
        } else {
          //
          // Divide remaining data in half
          //
          if (SizeLeft == 1) {
            Size = 1;
          } else {
            Size = SizeLeft / 2;
          }
        }

        TempBlockPtr->Union.DataBlock = (UINTN)TempDataPtr;
        TempBlockPtr->Length          = Size;
        Print (L"CapsuleApp: capsule block/size              0x%X/0x%X\n", (UINTN)TempDataPtr, Size);
        SizeLeft    -= Size;
        TempDataPtr += Size;
        TempBlockPtr++;
        if (SizeLeft == 0) {
          break;
        }
      }
    }

    BlockDescriptorPre = TempBlockPtr;
    BlockDescriptors1  = NULL;
  }

  //
  // Null-terminate.
  //
  if (TempBlockPtr != NULL) {
    TempBlockPtr->Union.ContinuationPointer = (UINTN)NULL;
    TempBlockPtr->Length                    = 0;
    *BlockDescriptors                       = BlockDescriptorsHeader;
  }

  return EFI_SUCCESS;

ERREXIT:
  if (BlockDescriptors1 != NULL) {
    FreePool (BlockDescriptors1);
  }

  if (BlockDescriptors2 != NULL) {
    FreePool (BlockDescriptors2);
  }

  return Status;
}

/**
  Clear the Gather list for a list of capsule images.

  @param[in]  BlockDescriptors The block descriptors for the capsule images
  @param[in]  CapsuleNum       The count of capsule images
**/
VOID
AblCleanGatherList (
  IN EFI_CAPSULE_BLOCK_DESCRIPTOR  *BlockDescriptors,
  IN UINTN                         CapsuleNum
  )
{
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *TempBlockPtr;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *TempBlockPtr1;
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *TempBlockPtr2;
  UINTN                         Index;

  if (BlockDescriptors != NULL) {
    TempBlockPtr1 = BlockDescriptors;
    while (1) {
      TempBlockPtr = TempBlockPtr1;
      for (Index = 0; Index < CapsuleNum; Index++) {
        if (TempBlockPtr[Index].Length == 0) {
          break;
        }
      }

      if (TempBlockPtr[Index].Union.ContinuationPointer == (UINTN)NULL) {
        break;
      }

      TempBlockPtr2 = (VOID *)((UINTN)TempBlockPtr[Index].Union.ContinuationPointer);
      FreePool (TempBlockPtr1);
      TempBlockPtr1 = TempBlockPtr2;
    }
  }
}

/**
  Start a UEFI image in the FMP payload.

  @param[in]  ImageBuffer   A pointer to the memory location containing a copy of the image to be loaded..
  @param[in]  ImageSize     The size in bytes of ImageBuffer.

  @return The status of gBS->LoadImage and gBS->StartImage.
**/
EFI_STATUS
StartAblImage (
  IN VOID   *ImageBuffer,
  IN UINTN  ImageSize
  )
{
  MEMMAP_DEVICE_PATH        MemMapNode;
  EFI_STATUS                Status;
  EFI_HANDLE                ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL  *DriverDevicePath;
  UINTN                     ExitDataSize;

  DEBUG ((DEBUG_INFO, "%a Start\n", __FUNCTION__));
  SetDevicePathNodeLength (&MemMapNode.Header, sizeof (MemMapNode));
  MemMapNode.Header.Type     = HARDWARE_DEVICE_PATH;
  MemMapNode.Header.SubType  = HW_MEMMAP_DP;
  MemMapNode.MemoryType      = EfiBootServicesCode;
  MemMapNode.StartingAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)ImageBuffer;
  MemMapNode.EndingAddress   = (EFI_PHYSICAL_ADDRESS)(UINTN)((UINT8 *)ImageBuffer + ImageSize - 1);

  DriverDevicePath = AppendDevicePathNode (NULL, &MemMapNode.Header);
  if (DriverDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((DEBUG_INFO, "S1:LoadImage start\n"));
  Status = gBS->LoadImage (
                           FALSE,
                           gImageHandle,
                           DriverDevicePath,
                           ImageBuffer,
                           ImageSize,
                           &ImageHandle
                           );
  DEBUG ((DEBUG_INFO, "S1: LoadImage end- %r\n", Status));
  if (EFI_ERROR (Status)) {
    //
    // With EFI_SECURITY_VIOLATION retval, the Image was loaded and an ImageHandle was created
    // with a valid EFI_LOADED_IMAGE_PROTOCOL, but the image can not be started right now.
    // If the caller doesn't have the option to defer the execution of an image, we should
    // unload image for the EFI_SECURITY_VIOLATION to avoid resource leak.
    //
    if (Status == EFI_SECURITY_VIOLATION) {
      gBS->UnloadImage (ImageHandle);
    }

    FreePool (DriverDevicePath);
    return Status;
  }

  DEBUG ((DEBUG_INFO, "S1: StartImage start\n"));
  Status = gBS->StartImage (
                            ImageHandle,
                            &ExitDataSize,
                            NULL
                            );
  DEBUG ((DEBUG_INFO, "S1: StartImage end- %r\n", Status));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "StartImage failed. Status = %r\n", Status));
  }

  FreePool (DriverDevicePath);
  DEBUG ((DEBUG_INFO, "%a Success.\n", __FUNCTION__));
  return Status;
}

EFI_STATUS
AuthenticateAbl (
  IN OUT  VOID   **CapsuleBuffer,
  IN OUT  UINTN  *CapsuleBufferSize,
  OUT UINT32     *Version,
  OUT UINT32     *LowestSupportedVersion
  )
{
  EFI_STATUS  Status;
  // UINTN                         CapsuleBufferSize[MAX_CAPSULE_NUM];
  // VOID                          *CapsuleBuffer[MAX_CAPSULE_NUM];
  EFI_CAPSULE_BLOCK_DESCRIPTOR  *BlockDescriptors;
  EFI_CAPSULE_HEADER            *CapsuleHeaderArray[MAX_CAPSULE_NUM + 1];
  UINT64                        MaxCapsuleSize;
  EFI_RESET_TYPE                ResetType;
  BOOLEAN                       NeedReset;
  BOOLEAN                       NoReset;
  CHAR16                        *MapFsStr;
  UINTN                         CapsuleNum;
  UINTN                         Index;
  EFI_ABL_INFO_PROTOCOL         *AblInfo;
  EFI_GUID                      gEfiAblInfoProtocolGuid = EFI_ABL_INFO_PROTOCOL_GUID;
  UINT32                        FmpHeaderSize           = 0;
  FMP_PAYLOAD_HEADER            *FmpPayloadHeader;
  //UINTN                         AuthenticatedImageSize;
  UINT8                         *AuthenticatedImage;

  BlockDescriptors = NULL;
  MapFsStr         = NULL;
  CapsuleNum       = 1;

  NoReset = FALSE;

  CapsuleNum       = 1;
  BlockDescriptors = NULL;
  for (Index = 0; Index < CapsuleNum; Index++) {
    if (!AblIsValidCapsuleHeader (CapsuleBuffer[Index], CapsuleBufferSize[Index])) {
      Print (L"CapsuleApp: Capsule image () is not a valid capsule.\n");
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Every capsule use 2 descriptor 1 for data 1 for end
  //
  Status = AblBuildGatherList (CapsuleBuffer, CapsuleBufferSize, CapsuleNum, &BlockDescriptors);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Call the runtime service capsule.
  //
  NeedReset = FALSE;
  for (Index = 0; Index < CapsuleNum; Index++) {
    CapsuleHeaderArray[Index] = (EFI_CAPSULE_HEADER *)CapsuleBuffer[Index];
    if ((CapsuleHeaderArray[Index]->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) != 0) {
      NeedReset = TRUE;
    }
  }

  CapsuleHeaderArray[CapsuleNum] = NULL;

  //
  // Inquire platform capability of UpdateCapsule.
  //
  Status = gRT->QueryCapsuleCapabilities (CapsuleHeaderArray, CapsuleNum, &MaxCapsuleSize, &ResetType);
  if (EFI_ERROR (Status)) {
    Print (L"CapsuleApp: failed to query capsule capability - %r\n", Status);
    goto Done;
  }

  for (Index = 0; Index < CapsuleNum; Index++) {
    if (CapsuleBufferSize[Index] > MaxCapsuleSize) {
      Print (L"CapsuleApp: capsule is too large to update, %ld is allowed\n", MaxCapsuleSize);
      Status = EFI_UNSUPPORTED;
      goto Done;
    }
  }

  //
  // Check whether the input capsule image has the flag of persist across system reset.
  //
  if (NeedReset) {
  } else {
    //
    // For capsule who has no reset flag, only call UpdateCapsule Service without a
    // system reset. The service will process the capsule immediately.
    //
    Status = gRT->UpdateCapsule (CapsuleHeaderArray, CapsuleNum, (UINTN)BlockDescriptors);
    if (Status != EFI_SUCCESS) {
      Print (L"CapsuleApp: failed to update capsule - %r\n", Status);
    }
  }

  Status = gBS->LocateProtocol (
                                &gEfiAblInfoProtocolGuid,
                                NULL,
                                (void**)&AblInfo
                                );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: locate gEfiAblInfoProtocolGuid failed\n", __FUNCTION__));
    return Status;
  }
  DEBUG ((DEBUG_INFO, "[%a]: AuthenticatedImage-0x%08x,AuthenticatedImageSize-0x%08x\n", __FUNCTION__, (UINTN)(AblInfo->AuthenticatedImage), AblInfo->AuthenticatedImageSize));

  AuthenticatedImage = AllocatePool (AblInfo->AuthenticatedImageSize);
  if (AuthenticatedImage == NULL) {
    Print (L"AuthenticatedImage (0x%x) too big.\n", AblInfo->AuthenticatedImageSize);
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  CopyMem (AuthenticatedImage, AblInfo->AuthenticatedImage, AblInfo->AuthenticatedImageSize);

  // {
  //   UINT8  *Buffer = AuthenticatedImage;
  //   for (int i = 0; i < 48; i++) {
  //     DEBUG ((DEBUG_INFO, "0x%02x ", Buffer[i]));
  //   }
  // }

  FmpHeaderSize    = sizeof (FMP_PAYLOAD_HEADER);
  FmpPayloadHeader = (FMP_PAYLOAD_HEADER *)(AuthenticatedImage);
  DEBUG ((DEBUG_INFO, "%08x %08x %08x %08x", FmpPayloadHeader->Signature, FmpPayloadHeader->HeaderSize,   FmpPayloadHeader->FwVersion, FmpPayloadHeader->LowestSupportedVersion));
  if (FmpPayloadHeader->Signature != 0x3153534D) {
    DEBUG ((DEBUG_ERROR, "FmpPayloadHeader->Signature is not match. %x\n", FmpPayloadHeader->Signature));
    return EFI_INVALID_PARAMETER;
  }

  *Version                = FmpPayloadHeader->FwVersion;
  *LowestSupportedVersion = FmpPayloadHeader->LowestSupportedVersion;
  *CapsuleBufferSize = AblInfo->AuthenticatedImageSize - FmpHeaderSize;
  *CapsuleBuffer = AuthenticatedImage + FmpHeaderSize;

  // Status = StartAblImage (
  //                         (AuthenticatedImage + FmpHeaderSize),
  //                         (AblInfo->AuthenticatedImageSize-FmpHeaderSize)
  //                         );

  Status = EFI_SUCCESS;

Done:
  // for (Index = 0; Index < CapsuleNum; Index++) {
  //   if (CapsuleBuffer[Index] != NULL) {
  //     FreePool (CapsuleBuffer[Index]);
  //   }
  // }

  AblCleanGatherList (BlockDescriptors, CapsuleNum);

  return Status;
}
