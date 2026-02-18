/** @file
  SetImage instance to report system firmware and act as agent to system update.
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

  Caution: This module requires additional review when modified.
  This module will have external input - capsule image.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.

  FmpSetImage() will receive untrusted input and do basic validation.

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SystemFirmwareDxe.h"

//
// SystemFmp driver private data
//
SYSTEM_FMP_PRIVATE_DATA  *mSystemFmpPrivate = NULL;
EFI_ABL_INFO_PROTOCOL  AblInfo;
/**
  Start a UEFI image in the FMP payload.

  @param[in]  ImageBuffer   A pointer to the memory location containing a copy of the image to be loaded..
  @param[in]  ImageSize     The size in bytes of ImageBuffer.

  @return The status of gBS->LoadImage and gBS->StartImage.
**/
EFI_STATUS
StartABLImage (
  IN VOID   *ImageBuffer,
  IN UINTN  ImageSize
  )
{
  MEMMAP_DEVICE_PATH        MemMapNode;
  EFI_STATUS                Status;
  EFI_HANDLE                ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL  *DriverDevicePath;
  UINTN                     ExitDataSize;

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
    DEBUG ((DEBUG_ERROR, "Driver Return Status = %r\n", Status));
  }

  FreePool (DriverDevicePath);
  return Status;
}

/**
  Dispatch system FMP images.

  Caution: This function may receive untrusted input.

  @param[in]  Image              The EDKII system FMP capsule image.
  @param[in]  ImageSize          The size of the EDKII system FMP capsule image in bytes.
  @param[out] LastAttemptVersion The last attempt version, which will be recorded in ESRT and FMP EFI_FIRMWARE_IMAGE_DESCRIPTOR.
  @param[out] LastAttemptStatus  The last attempt status, which will be recorded in ESRT and FMP EFI_FIRMWARE_IMAGE_DESCRIPTOR.

  @retval EFI_SUCCESS           Process Capsule Image successfully.
  @retval EFI_UNSUPPORTED       Capsule image is not supported by the firmware.
  @retval EFI_VOLUME_CORRUPTED  FV volume in the capsule is corrupted.
  @retval EFI_OUT_OF_RESOURCES  Not enough memory.
**/
EFI_STATUS
DispatchSystemFmpImages (
  IN VOID     *Image,
  IN UINTN    ImageSize,
  OUT UINT32  *LastAttemptVersion,
  OUT UINT32  *LastAttemptStatus
  )
{
  EFI_STATUS  Status;
  VOID        *AuthenticatedImage;
  UINTN       AuthenticatedImageSize;

  EFI_HANDLE             Handle                  = NULL;
  EFI_GUID               gEfiAblInfoProtocolGuid = EFI_ABL_INFO_PROTOCOL_GUID;

  AuthenticatedImage     = NULL;
  AuthenticatedImageSize = 0;

  DEBUG ((DEBUG_INFO, "DispatchSystemFmpImages\n"));

  //
  // Verify
  //
  Status = CapsuleAuthenticateABL (Image, ImageSize, FALSE, LastAttemptVersion, LastAttemptStatus, &AuthenticatedImage, &AuthenticatedImageSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "SystemFirmwareAuthenticateImage - %r\n", Status));
    return Status;
  }

  // install abl info protocol

  Handle  = NULL;


  AblInfo.AuthenticatedImage     = AuthenticatedImage;
  AblInfo.AuthenticatedImageSize = AuthenticatedImageSize;

  DEBUG ((DEBUG_ERROR, "ABLUpdate: AuthenticatedImage-0x%08x,AuthenticatedImageSize-0x%08x\n", (UINTN)AuthenticatedImage,AuthenticatedImageSize));
  Status = gBS->InstallProtocolInterface (
                                          &Handle,
                                          &gEfiAblInfoProtocolGuid,
                                          EFI_NATIVE_INTERFACE,
                                          &AblInfo
                                          );

  // Status = StartABLImage (
  //            (UINT8 *)AuthenticatedImage,
  //            AuthenticatedImageSize
  //            );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "IN Return Status = %r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Updates the firmware image of the device.

  This function updates the hardware with the new firmware image.
  This function returns EFI_UNSUPPORTED if the firmware image is not updatable.
  If the firmware image is updatable, the function should perform the following minimal validations
  before proceeding to do the firmware image update.
  - Validate the image authentication if image has attribute
    IMAGE_ATTRIBUTE_AUTHENTICATION_REQUIRED. The function returns
    EFI_SECURITY_VIOLATION if the validation fails.
  - Validate the image is a supported image for this device. The function returns EFI_ABORTED if
    the image is unsupported. The function can optionally provide more detailed information on
    why the image is not a supported image.
  - Validate the data from VendorCode if not null. Image validation must be performed before
    VendorCode data validation. VendorCode data is ignored or considered invalid if image
    validation failed. The function returns EFI_ABORTED if the data is invalid.

  VendorCode enables vendor to implement vendor-specific firmware image update policy. Null if
  the caller did not specify the policy or use the default policy. As an example, vendor can implement
  a policy to allow an option to force a firmware image update when the abort reason is due to the new
  firmware image version is older than the current firmware image version or bad image checksum.
  Sensitive operations such as those wiping the entire firmware image and render the device to be
  non-functional should be encoded in the image itself rather than passed with the VendorCode.
  AbortReason enables vendor to have the option to provide a more detailed description of the abort
  reason to the caller.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Image              Points to the new image.
  @param[in]  ImageSize          Size of the new image in bytes.
  @param[in]  VendorCode         This enables vendor to implement vendor-specific firmware image update policy.
                                 Null indicates the caller did not specify the policy or use the default policy.
  @param[in]  Progress           A function used by the driver to report the progress of the firmware update.
  @param[out] AbortReason        A pointer to a pointer to a null-terminated string providing more
                                 details for the aborted operation. The buffer is allocated by this function
                                 with AllocatePool(), and it is the caller's responsibility to free it with a
                                 call to FreePool().

  @retval EFI_SUCCESS            The device was successfully updated with the new image.
  @retval EFI_ABORTED            The operation is aborted.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI
FmpSetImage (
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL               *This,
  IN  UINT8                                          ImageIndex,
  IN  CONST VOID                                     *Image,
  IN  UINTN                                          ImageSize,
  IN  CONST VOID                                     *VendorCode,
  IN  EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS  Progress,
  OUT CHAR16                                         **AbortReason
  )
{
  SYSTEM_FMP_PRIVATE_DATA  *SystemFmpPrivate;
  // EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *SystemFmp;
  EFI_STATUS  Status;

  // EFI_STATUS                        VarStatus;

  if ((Image == NULL) || (ImageSize == 0) || (AbortReason == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  SystemFmpPrivate = SYSTEM_FMP_PRIVATE_DATA_FROM_FMP (This);
  *AbortReason     = NULL;

  // if ((ImageIndex == 0) || (ImageIndex > SystemFmpPrivate->DescriptorCount)) {
  //   return EFI_INVALID_PARAMETER;
  // }

  //
  // Process FV
  //
  Status = DispatchSystemFmpImages ((VOID *)Image, ImageSize, &SystemFmpPrivate->LastAttempt.LastAttemptVersion, &SystemFmpPrivate->LastAttempt.LastAttemptStatus);
  DEBUG ((DEBUG_INFO, "ImageIndex 0x%x\n", ImageIndex));

  return EFI_SUCCESS;
}

/**
  System FMP module entrypoint

  @param[in]  ImageHandle       The firmware allocated handle for the EFI image.
  @param[in]  SystemTable       A pointer to the EFI System Table.

  @return EFI_SUCCESS System FMP module is initialized.
**/
EFI_STATUS
EFIAPI
SystemFirmwareReportMainDxe (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "[FMP ABL] start\n"));
  //
  // Initialize SystemFmpPrivateData
  //
  mSystemFmpPrivate = AllocateZeroPool (sizeof (SYSTEM_FMP_PRIVATE_DATA));
  if (mSystemFmpPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = InitializePrivateData (mSystemFmpPrivate);
  if (EFI_ERROR (Status)) {
    FreePool (mSystemFmpPrivate);
    mSystemFmpPrivate = NULL;
    return Status;
  }

  DEBUG ((DEBUG_INFO, "[FMP ABL] ImageTypeID:%g\n", &mSystemFmpPrivate->ImageDescriptor->ImageTypeId));
  //
  // Install FMP protocol.
  //
  Status = gBS->InstallProtocolInterface (
                                          &mSystemFmpPrivate->Handle,
                                          &gEfiFirmwareManagementProtocolGuid,
                                          EFI_NATIVE_INTERFACE,
                                          &mSystemFmpPrivate->Fmp
                                          );
  if (EFI_ERROR (Status)) {
    FreePool (mSystemFmpPrivate);
    mSystemFmpPrivate = NULL;
    return Status;
  }

  DEBUG ((DEBUG_INFO, "[FMP ABL] FMP protocol install success\n"));
  DEBUG ((DEBUG_INFO, "[FMP ABL] end\n"));
  return Status;
}
