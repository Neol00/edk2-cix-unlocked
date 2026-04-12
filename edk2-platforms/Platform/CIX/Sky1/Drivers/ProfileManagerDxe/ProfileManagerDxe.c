/** @file
  Profile Manager DXE Driver.

  Provides a BIOS setup submenu for saving, loading, and deleting
  settings profiles.  Profiles are stored in a dedicated SPI flash region
  at a fixed offset (PROFILE_STORE_FLASH_OFFSET) using the NOR flash
  DiskIo protocol directly.

  Each profile is a snapshot of PLATFORM_SETUP_DATA with CRC32 integrity.
  The store survives NVRAM resets since it lives outside the variable region.

  Copyright 2025 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "ProfileManagerDxe.h"

//
// Module globals
//
STATIC EFI_GUID  mProfileManagerFormsetGuid  = PROFILE_MANAGER_FORMSET_GUID;
STATIC EFI_GUID  mProfileManagerVarstoreGuid = PROFILE_MANAGER_VARSTORE_GUID;
STATIC EFI_GUID  mCixNorFlashDevicePathGuid;

STATIC PROFILE_MANAGER_PRIVATE_DATA  mPrivate = {
  .Signature = PROFILE_MANAGER_SIGNATURE,
};

STATIC HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    PROFILE_MANAGER_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8)(END_DEVICE_PATH_LENGTH),
      (UINT8)((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

//
// String token IDs for the per-slot status lines (matched to .uni order)
//
STATIC EFI_STRING_ID  mSlotStatusTokens[PROFILE_MAX_SLOTS] = {
  STRING_TOKEN (STR_PROFILE_SLOT0_STATUS),
  STRING_TOKEN (STR_PROFILE_SLOT1_STATUS),
  STRING_TOKEN (STR_PROFILE_SLOT2_STATUS),
  STRING_TOKEN (STR_PROFILE_SLOT3_STATUS),
  STRING_TOKEN (STR_PROFILE_SLOT4_STATUS),
};

STATIC EFI_STRING_ID  mSlotNameTokens[PROFILE_MAX_SLOTS] = {
  STRING_TOKEN (STR_PROFILE_SLOT0_NAME),
  STRING_TOKEN (STR_PROFILE_SLOT1_NAME),
  STRING_TOKEN (STR_PROFILE_SLOT2_NAME),
  STRING_TOKEN (STR_PROFILE_SLOT3_NAME),
  STRING_TOKEN (STR_PROFILE_SLOT4_NAME),
};

// ============================================================
// NOR Flash DiskIo helpers
// ============================================================

/**
  Locate the NOR flash DiskIo protocol by matching the CIX NOR flash
  device path GUID.  This is the same approach used by FwUpdateProtocolDxe.
**/
STATIC
EFI_STATUS
LocateNorFlashDiskIo (
  OUT EFI_DISK_IO_PROTOCOL  **DiskIo,
  OUT UINT32                *MediaId
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                *Handles;
  UINTN                     HandleCount;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  VENDOR_DEVICE_PATH        *VendorDp;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    DevicePath = DevicePathFromHandle (Handles[Index]);
    if (DevicePath == NULL) {
      continue;
    }

    for (Node = DevicePath; !IsDevicePathEnd (Node); Node = NextDevicePathNode (Node)) {
      if ((DevicePathType (Node) == HARDWARE_DEVICE_PATH) &&
          (DevicePathSubType (Node) == HW_VENDOR_DP))
      {
        VendorDp = (VENDOR_DEVICE_PATH *)Node;
        if (CompareGuid (&VendorDp->Guid, &gCixNorFlashDevicePathGuid)) {
          *MediaId = ((UINT8 *)Node)[sizeof (VENDOR_DEVICE_PATH)];
          Status = gBS->HandleProtocol (
                          Handles[Index],
                          &gEfiDiskIoProtocolGuid,
                          (VOID **)DiskIo
                          );
          FreePool (Handles);
          return Status;
        }
      }
    }
  }

  FreePool (Handles);
  return EFI_NOT_FOUND;
}

/**
  Read the entire profile store (24 KB) from SPI flash.
**/
STATIC
EFI_STATUS
FlashReadStore (
  IN  PROFILE_MANAGER_PRIVATE_DATA  *Private,
  OUT UINT8                         *Buffer
  )
{
  EFI_STATUS  Status;

  Status = Private->NorFlashDiskIo->ReadDisk (
                                      Private->NorFlashDiskIo,
                                      Private->NorFlashMediaId,
                                      PROFILE_STORE_FLASH_OFFSET,
                                      PROFILE_STORE_SIZE,
                                      Buffer
                                      );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ProfileMgr] FlashReadStore failed: %r\n", Status));
  }

  return Status;
}

/**
  Write the entire profile store (24 KB) to SPI flash.
**/
STATIC
EFI_STATUS
FlashWriteStore (
  IN PROFILE_MANAGER_PRIVATE_DATA  *Private,
  IN UINT8                         *Buffer
  )
{
  EFI_STATUS  Status;

  Status = Private->NorFlashDiskIo->WriteDisk (
                                      Private->NorFlashDiskIo,
                                      Private->NorFlashMediaId,
                                      PROFILE_STORE_FLASH_OFFSET,
                                      PROFILE_STORE_SIZE,
                                      Buffer
                                      );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ProfileMgr] FlashWriteStore failed: %r\n", Status));
  }

  return Status;
}

// ============================================================
// CRC32 helper
// ============================================================

STATIC
UINT32
ComputeCrc32 (
  IN VOID    *Data,
  IN UINTN   DataSize
  )
{
  UINT32  Crc32 = 0;

  gBS->CalculateCrc32 (Data, DataSize, &Crc32);
  return Crc32;
}

// ============================================================
// Store management
// ============================================================

/**
  Validate and possibly initialize the profile store header.
  If the flash contains an uninitialized (all-0xFF) or corrupt header,
  create a fresh empty header.
**/
STATIC
VOID
InitializeStoreHeader (
  IN OUT PROFILE_STORE_HEADER  *Header
  )
{
  UINT8  Slot;

  if (Header->Magic == PROFILE_STORE_MAGIC && Header->Version == PROFILE_STORE_VERSION) {
    UINT32  ExpectedCrc = Header->HeaderCrc32;
    Header->HeaderCrc32 = 0;
    UINT32  ActualCrc = ComputeCrc32 (Header->Slots, sizeof (Header->Slots));
    Header->HeaderCrc32 = ExpectedCrc;

    if (ActualCrc == ExpectedCrc) {
      DEBUG ((DEBUG_INFO, "[ProfileMgr] Store header valid, %d slots\n", Header->SlotCount));
      return;
    }

    DEBUG ((DEBUG_WARN, "[ProfileMgr] Store header CRC mismatch, reinitializing\n"));
  } else {
    DEBUG ((DEBUG_INFO, "[ProfileMgr] Store header not found (magic=0x%08X), initializing\n", Header->Magic));
  }

  ZeroMem (Header, sizeof (*Header));
  Header->Magic     = PROFILE_STORE_MAGIC;
  Header->Version   = PROFILE_STORE_VERSION;
  Header->SlotCount = PROFILE_MAX_SLOTS;

  for (Slot = 0; Slot < PROFILE_MAX_SLOTS; Slot++) {
    AsciiSPrint (Header->Slots[Slot].Name, PROFILE_NAME_BUF_SIZE, "Profile %d", Slot + 1);
    Header->Slots[Slot].Flags    = 0;
    Header->Slots[Slot].DataCrc32 = 0;
    Header->Slots[Slot].DataSize  = 0;
  }

  Header->HeaderCrc32 = ComputeCrc32 (Header->Slots, sizeof (Header->Slots));
}

/**
  Update the HII status strings to reflect current store state.
**/
STATIC
VOID
RefreshSlotStatusStrings (
  IN EFI_HII_HANDLE          HiiHandle,
  IN PROFILE_STORE_HEADER    *Header
  )
{
  UINT8    Slot;
  CHAR16   StatusStr[80];
  CHAR16   NameStr[80];
  CHAR16   NameUnicode[PROFILE_NAME_BUF_SIZE];
  UINT8    Idx;

  for (Slot = 0; Slot < PROFILE_MAX_SLOTS; Slot++) {
    for (Idx = 0; Idx < PROFILE_NAME_BUF_SIZE - 1 && Header->Slots[Slot].Name[Idx] != '\0'; Idx++) {
      NameUnicode[Idx] = (CHAR16)Header->Slots[Slot].Name[Idx];
    }
    NameUnicode[Idx] = L'\0';

    if (Header->Slots[Slot].Flags & PROFILE_SLOT_FLAG_VALID) {
      UnicodeSPrint (StatusStr, sizeof (StatusStr), L"Slot %d: %s", Slot + 1, NameUnicode);
      UnicodeSPrint (NameStr, sizeof (NameStr), L"Slot %d: %s", Slot + 1, NameUnicode);
    } else {
      UnicodeSPrint (StatusStr, sizeof (StatusStr), L"Slot %d: [Empty]", Slot + 1);
      UnicodeSPrint (NameStr, sizeof (NameStr), L"Slot %d", Slot + 1);
    }

    HiiSetString (HiiHandle, mSlotStatusTokens[Slot], StatusStr, NULL);
    HiiSetString (HiiHandle, mSlotNameTokens[Slot], NameStr, NULL);
  }
}

/**
  Flush the in-memory header to flash (writes entire 24 KB store).
  Only block 0 (header) is modified; slot data blocks are preserved.
**/
STATIC
EFI_STATUS
FlushHeaderToFlash (
  IN PROFILE_MANAGER_PRIVATE_DATA  *Private
  )
{
  EFI_STATUS  Status;
  UINT8       *StoreBuf;

  StoreBuf = AllocateZeroPool (PROFILE_STORE_SIZE);
  if (StoreBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = FlashReadStore (Private, StoreBuf);
  if (EFI_ERROR (Status)) {
    FreePool (StoreBuf);
    return Status;
  }

  Private->StoreHeader.HeaderCrc32 = 0;
  Private->StoreHeader.HeaderCrc32 = ComputeCrc32 (
                                       Private->StoreHeader.Slots,
                                       sizeof (Private->StoreHeader.Slots)
                                       );
  CopyMem (StoreBuf, &Private->StoreHeader, sizeof (PROFILE_STORE_HEADER));

  Status = FlashWriteStore (Private, StoreBuf);
  FreePool (StoreBuf);
  return Status;
}

// ============================================================
// Profile operations
// ============================================================

/**
  Save current PLATFORM_SETUP_DATA to the selected slot.
**/
STATIC
EFI_STATUS
ProfileSave (
  IN PROFILE_MANAGER_PRIVATE_DATA  *Private,
  IN UINT8                         SlotIndex,
  IN CHAR8                         *Name
  )
{
  EFI_STATUS           Status;
  PLATFORM_SETUP_DATA  SetupData;
  UINTN                VarSize;
  UINT8                *StoreBuf;
  UINT8                *SlotPtr;
  UINT32               Crc32;

  if (SlotIndex >= PROFILE_MAX_SLOTS) {
    return EFI_INVALID_PARAMETER;
  }

  VarSize = sizeof (PLATFORM_SETUP_DATA);
  Status  = gRT->GetVariable (
                   PLATFORM_SETUP_VAR,
                   &gPlatformSetupVariableGuid,
                   NULL,
                   &VarSize,
                   &SetupData
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ProfileMgr] GetVariable failed: %r\n", Status));
    return Status;
  }

  Crc32 = ComputeCrc32 (&SetupData, sizeof (SetupData));

  StoreBuf = AllocateZeroPool (PROFILE_STORE_SIZE);
  if (StoreBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = FlashReadStore (Private, StoreBuf);
  if (EFI_ERROR (Status)) {
    FreePool (StoreBuf);
    return Status;
  }

  //
  // Write setup data into the slot block (block N+1)
  //
  SlotPtr = StoreBuf + (UINTN)((SlotIndex + 1) * PROFILE_BLOCK_SIZE);
  ZeroMem (SlotPtr, PROFILE_BLOCK_SIZE);
  CopyMem (SlotPtr, &SetupData, sizeof (SetupData));

  //
  // Update directory entry in the header within the buffer first
  //
  PROFILE_STORE_HEADER  *BufHeader = (PROFILE_STORE_HEADER *)StoreBuf;
  AsciiStrnCpyS (
    BufHeader->Slots[SlotIndex].Name,
    PROFILE_NAME_BUF_SIZE,
    Name,
    PROFILE_NAME_MAX_LEN
    );
  BufHeader->Slots[SlotIndex].Flags    = PROFILE_SLOT_FLAG_VALID;
  BufHeader->Slots[SlotIndex].DataCrc32 = Crc32;
  BufHeader->Slots[SlotIndex].DataSize  = (UINT32)sizeof (SetupData);

  BufHeader->HeaderCrc32 = 0;
  BufHeader->HeaderCrc32 = ComputeCrc32 (BufHeader->Slots, sizeof (BufHeader->Slots));

  //
  // Write entire store to flash
  //
  Status = FlashWriteStore (Private, StoreBuf);
  if (!EFI_ERROR (Status)) {
    //
    // Only update in-memory header after successful flash write
    //
    CopyMem (&Private->StoreHeader, BufHeader, sizeof (PROFILE_STORE_HEADER));
    DEBUG ((DEBUG_INFO, "[ProfileMgr] Saved slot %d: '%a'\n", SlotIndex, Name));
  }

  FreePool (StoreBuf);
  return Status;
}

/**
  Load settings from a profile slot into NVRAM.
  Caller is responsible for triggering a reboot via RESET_REQUIRED.
**/
STATIC
EFI_STATUS
ProfileLoad (
  IN PROFILE_MANAGER_PRIVATE_DATA  *Private,
  IN UINT8                         SlotIndex
  )
{
  EFI_STATUS           Status;
  UINT8                *StoreBuf;
  UINT8                *SlotPtr;
  PLATFORM_SETUP_DATA  *SetupData;
  UINT32               Crc32;

  if (SlotIndex >= PROFILE_MAX_SLOTS) {
    return EFI_INVALID_PARAMETER;
  }

  if (!(Private->StoreHeader.Slots[SlotIndex].Flags & PROFILE_SLOT_FLAG_VALID)) {
    DEBUG ((DEBUG_WARN, "[ProfileMgr] Slot %d is empty\n", SlotIndex));
    return EFI_NOT_FOUND;
  }

  StoreBuf = AllocateZeroPool (PROFILE_STORE_SIZE);
  if (StoreBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = FlashReadStore (Private, StoreBuf);
  if (EFI_ERROR (Status)) {
    FreePool (StoreBuf);
    return Status;
  }

  SlotPtr   = StoreBuf + (UINTN)((SlotIndex + 1) * PROFILE_BLOCK_SIZE);
  SetupData = (PLATFORM_SETUP_DATA *)SlotPtr;
  Crc32     = ComputeCrc32 (SetupData, sizeof (PLATFORM_SETUP_DATA));

  if (Crc32 != Private->StoreHeader.Slots[SlotIndex].DataCrc32) {
    DEBUG ((DEBUG_ERROR, "[ProfileMgr] Slot %d CRC mismatch: expected 0x%08X, got 0x%08X\n",
            SlotIndex, Private->StoreHeader.Slots[SlotIndex].DataCrc32, Crc32));
    FreePool (StoreBuf);
    return EFI_CRC_ERROR;
  }

  Status = gRT->SetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof (PLATFORM_SETUP_DATA),
                  SetupData
                  );
  FreePool (StoreBuf);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ProfileMgr] SetVariable failed: %r\n", Status));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "[ProfileMgr] Loaded slot %d into NVRAM\n", SlotIndex));
  return EFI_SUCCESS;
}

/**
  Delete a profile slot.
**/
STATIC
EFI_STATUS
ProfileDelete (
  IN PROFILE_MANAGER_PRIVATE_DATA  *Private,
  IN UINT8                         SlotIndex
  )
{
  EFI_STATUS  Status;
  UINT8       *StoreBuf;
  UINT8       *SlotPtr;

  if (SlotIndex >= PROFILE_MAX_SLOTS) {
    return EFI_INVALID_PARAMETER;
  }

  if (!(Private->StoreHeader.Slots[SlotIndex].Flags & PROFILE_SLOT_FLAG_VALID)) {
    return EFI_NOT_FOUND;
  }

  StoreBuf = AllocateZeroPool (PROFILE_STORE_SIZE);
  if (StoreBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = FlashReadStore (Private, StoreBuf);
  if (EFI_ERROR (Status)) {
    FreePool (StoreBuf);
    return Status;
  }

  //
  // Clear the slot data block on flash
  //
  SlotPtr = StoreBuf + (UINTN)((SlotIndex + 1) * PROFILE_BLOCK_SIZE);
  SetMem (SlotPtr, PROFILE_BLOCK_SIZE, 0xFF);

  //
  // Update directory entry in the buffer
  //
  PROFILE_STORE_HEADER  *BufHeader = (PROFILE_STORE_HEADER *)StoreBuf;
  AsciiSPrint (BufHeader->Slots[SlotIndex].Name, PROFILE_NAME_BUF_SIZE, "Profile %d", SlotIndex + 1);
  BufHeader->Slots[SlotIndex].Flags    = 0;
  BufHeader->Slots[SlotIndex].DataCrc32 = 0;
  BufHeader->Slots[SlotIndex].DataSize  = 0;

  BufHeader->HeaderCrc32 = 0;
  BufHeader->HeaderCrc32 = ComputeCrc32 (BufHeader->Slots, sizeof (BufHeader->Slots));

  Status = FlashWriteStore (Private, StoreBuf);
  if (!EFI_ERROR (Status)) {
    CopyMem (&Private->StoreHeader, BufHeader, sizeof (PROFILE_STORE_HEADER));
    DEBUG ((DEBUG_INFO, "[ProfileMgr] Deleted slot %d\n", SlotIndex));
  }

  FreePool (StoreBuf);
  return Status;
}

// ============================================================
// HII Config Access Protocol callbacks
// ============================================================

EFI_STATUS
EFIAPI
ProfileManagerExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Request,
  OUT EFI_STRING                            *Progress,
  OUT EFI_STRING                            *Results
  )
{
  PROFILE_MANAGER_PRIVATE_DATA  *Private;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Request == NULL) {
    return EFI_NOT_FOUND;
  }

  Private = PROFILE_MANAGER_PRIVATE_FROM_THIS (This);

  return gHiiConfigRouting->BlockToConfig (
                              gHiiConfigRouting,
                              Request,
                              (UINT8 *)&Private->VarStore,
                              sizeof (PROFILE_MANAGER_VARSTORE),
                              Results,
                              Progress
                              );
}

EFI_STATUS
EFIAPI
ProfileManagerRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Configuration,
  OUT EFI_STRING                            *Progress
  )
{
  PROFILE_MANAGER_PRIVATE_DATA  *Private;
  UINTN                         BufferSize;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private    = PROFILE_MANAGER_PRIVATE_FROM_THIS (This);
  BufferSize = sizeof (PROFILE_MANAGER_VARSTORE);

  return gHiiConfigRouting->ConfigToBlock (
                              gHiiConfigRouting,
                              Configuration,
                              (UINT8 *)&Private->VarStore,
                              &BufferSize,
                              Progress
                              );
}

EFI_STATUS
EFIAPI
ProfileManagerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  )
{
  PROFILE_MANAGER_PRIVATE_DATA  *Private;
  EFI_STATUS                    Status;
  UINT8                         SlotIndex;
  CHAR8                         AsciiName[PROFILE_NAME_BUF_SIZE];
  PROFILE_MANAGER_VARSTORE      BrowserData;
  UINTN                         BufferSize;

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  Private = PROFILE_MANAGER_PRIVATE_FROM_THIS (This);

  //
  // Read the current slot selection from the browser's varstore.
  // The oneof is non-interactive so no callback fires for it,
  // but the browser still tracks its value internally.
  //
  BufferSize = sizeof (PROFILE_MANAGER_VARSTORE);
  ZeroMem (&BrowserData, BufferSize);
  if (!HiiGetBrowserData (&mProfileManagerVarstoreGuid, L"ProfileMgrVarStore", BufferSize, (UINT8 *)&BrowserData)) {
    BrowserData.SelectedSlot = 0;
  }

  SlotIndex = BrowserData.SelectedSlot;
  if (SlotIndex >= PROFILE_MAX_SLOTS) {
    SlotIndex = 0;
  }

  Status = EFI_SUCCESS;

  switch (QuestionId) {
    case QUESTION_PROFILE_SAVE:
      AsciiSPrint (AsciiName, PROFILE_NAME_BUF_SIZE, "Profile %d", SlotIndex + 1);
      Status = ProfileSave (Private, SlotIndex, AsciiName);
      if (!EFI_ERROR (Status)) {
        RefreshSlotStatusStrings (Private->HiiHandle, &Private->StoreHeader);
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
      }

      break;

    case QUESTION_PROFILE_LOAD:
      Status = ProfileLoad (Private, SlotIndex);
      if (!EFI_ERROR (Status)) {
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
      }

      break;

    case QUESTION_PROFILE_DELETE:
      Status = ProfileDelete (Private, SlotIndex);
      if (!EFI_ERROR (Status)) {
        RefreshSlotStatusStrings (Private->HiiHandle, &Private->StoreHeader);
      }

      break;

    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

// ============================================================
// Auto-generated string package array
// ============================================================

extern UINT8  ProfileManagerDxeStrings[];

// ============================================================
// Driver entry point
// ============================================================

EFI_STATUS
EFIAPI
ProfileManagerDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT8       *StoreBuf;

  //
  // Copy the NOR flash device path GUID from the PCD/dec global
  //
  CopyMem (&mCixNorFlashDevicePathGuid, &gCixNorFlashDevicePathGuid, sizeof (EFI_GUID));

  //
  // Locate NOR flash DiskIo for direct SPI flash access
  //
  Status = LocateNorFlashDiskIo (&mPrivate.NorFlashDiskIo, &mPrivate.NorFlashMediaId);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ProfileMgr] LocateNorFlashDiskIo failed: %r\n", Status));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "[ProfileMgr] NOR flash DiskIo located, MediaId=%d\n", mPrivate.NorFlashMediaId));

  //
  // Read current store from flash and validate/initialize
  //
  StoreBuf = AllocateZeroPool (PROFILE_STORE_SIZE);
  if (StoreBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = FlashReadStore (&mPrivate, StoreBuf);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "[ProfileMgr] Flash read failed, starting with empty store\n"));
    ZeroMem (&mPrivate.StoreHeader, sizeof (PROFILE_STORE_HEADER));
  } else {
    CopyMem (&mPrivate.StoreHeader, StoreBuf, sizeof (PROFILE_STORE_HEADER));
  }

  FreePool (StoreBuf);

  InitializeStoreHeader (&mPrivate.StoreHeader);

  //
  // Initialize default varstore values
  //
  mPrivate.VarStore.SelectedSlot = 0;

  //
  // Install HII Config Access protocol
  //
  mPrivate.ConfigAccess.ExtractConfig = ProfileManagerExtractConfig;
  mPrivate.ConfigAccess.RouteConfig   = ProfileManagerRouteConfig;
  mPrivate.ConfigAccess.Callback      = ProfileManagerCallback;

  mPrivate.DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mPrivate.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mPrivate.ConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ProfileMgr] InstallProtocol failed: %r\n", Status));
    return Status;
  }

  //
  // Publish HII form
  //
  mPrivate.HiiHandle = HiiAddPackages (
                          &mProfileManagerFormsetGuid,
                          mPrivate.DriverHandle,
                          ProfileManagerVfrBin,
                          ProfileManagerDxeStrings,
                          NULL
                          );
  if (mPrivate.HiiHandle == NULL) {
    DEBUG ((DEBUG_ERROR, "[ProfileMgr] HiiAddPackages failed\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Set initial status strings
  //
  RefreshSlotStatusStrings (mPrivate.HiiHandle, &mPrivate.StoreHeader);

  //
  // If the store was freshly initialized (no valid header on flash),
  // write the empty header to flash now so subsequent reads find it.
  //
  FlushHeaderToFlash (&mPrivate);

  DEBUG ((DEBUG_INFO, "[ProfileMgr] Driver initialized\n"));
  return EFI_SUCCESS;
}
