/** @file
  Profile Manager DXE driver header.

  Defines the on-flash profile store layout and data structures for
  saving / loading / deleting / renaming BIOS settings profiles.

  The profile store lives in a dedicated SPI flash region at a fixed
  offset (0x108000).  Flash I/O is done via the NOR flash DiskIo
  protocol directly.  The store is entirely outside NVRAM, so profiles
  survive "Reset All Defaults" and "Clear NVRAM" operations.

  Flash layout (24 KB total = 6 x 4 KB blocks):
    Block 0  -- PROFILE_STORE_HEADER  (directory + metadata)
    Block 1  -- Slot 0 data           (PLATFORM_SETUP_DATA snapshot)
    Block 2  -- Slot 1 data
    Block 3  -- Slot 2 data
    Block 4  -- Slot 3 data
    Block 5  -- Slot 4 data

  Copyright 2025 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PROFILE_MANAGER_DXE_H_
#define _PROFILE_MANAGER_DXE_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiHiiServicesLib.h>

#include <Protocol/HiiConfigAccess.h>
#include <Protocol/DiskIo.h>
#include <Guid/MdeModuleHii.h>

#include <PlatformSetupVar.h>

//
// ============================================================
// Constants
// ============================================================
//

#define PROFILE_MAX_SLOTS       5
#define PROFILE_NAME_MAX_LEN    31    // max characters (excluding NUL)
#define PROFILE_NAME_BUF_SIZE   32    // PROFILE_NAME_MAX_LEN + 1

#define PROFILE_STORE_MAGIC     0x464F5250   // 'PROF' little-endian
#define PROFILE_STORE_VERSION   1

#define PROFILE_BLOCK_SIZE      0x1000       // 4 KB (NOR flash erase block)
#define PROFILE_STORE_SIZE      (PROFILE_BLOCK_SIZE * (1 + PROFILE_MAX_SLOTS))  // 24 KB

//
// Fixed SPI flash offset for the profile store region.
// Must match the entry in spi_flash_config_all.json (image_type 8 @ 0x108000).
//
#define PROFILE_STORE_FLASH_OFFSET  0x108000

//
// Slot flags
//
#define PROFILE_SLOT_FLAG_VALID   BIT0

//
// ============================================================
// On-flash data structures (packed, stored in block 0)
// ============================================================
//

#pragma pack(1)

typedef struct {
  CHAR8     Name[PROFILE_NAME_BUF_SIZE];    // UTF-8 profile name (alphanumeric + NUL)
  UINT32    Flags;                           // PROFILE_SLOT_FLAG_*
  UINT32    DataCrc32;                       // CRC32 of the PLATFORM_SETUP_DATA in the slot block
  UINT32    DataSize;                        // sizeof(PLATFORM_SETUP_DATA) at save time
  UINT8     Reserved[20];                    // pad to 64 bytes per entry
} PROFILE_DIRECTORY_ENTRY;

//
// Compile-time check: each directory entry must be exactly 64 bytes.
//
STATIC_ASSERT (sizeof (PROFILE_DIRECTORY_ENTRY) == 64, "PROFILE_DIRECTORY_ENTRY must be 64 bytes");

typedef struct {
  UINT32                   Magic;             // PROFILE_STORE_MAGIC
  UINT32                   Version;           // PROFILE_STORE_VERSION
  UINT32                   SlotCount;         // PROFILE_MAX_SLOTS
  UINT32                   HeaderCrc32;       // CRC32 of directory entries
  PROFILE_DIRECTORY_ENTRY  Slots[PROFILE_MAX_SLOTS];
  // remainder of block 0 is reserved/padding
} PROFILE_STORE_HEADER;

#pragma pack()

//
// ============================================================
// HII Form IDs and Question IDs
// ============================================================
//

#define PROFILE_MANAGER_FORM_ID         0x17

//
// Question IDs for the VFR form.
// The slot selector uses 0x1900.  Action buttons start at 0x1901.
//
#define QUESTION_PROFILE_SLOT_SELECT    0x1900
#define QUESTION_PROFILE_SAVE           0x1901
#define QUESTION_PROFILE_LOAD           0x1902
#define QUESTION_PROFILE_DELETE         0x1903

//
// Varstore ID for the small scratch varstore used by the form.
//
#define PROFILE_VARSTORE_ID             0x1910

//
// ============================================================
// HII Varstore -- small structure used only for the form UI
// ============================================================
//

#pragma pack(1)

typedef struct {
  UINT8   SelectedSlot;                         // 0 .. PROFILE_MAX_SLOTS-1
} PROFILE_MANAGER_VARSTORE;

#pragma pack()

//
// ============================================================
// GUID definitions
// ============================================================
//

// {C7B3E0A1-5D2F-4816-9A3B-7E1C6F8D4A50}
#define PROFILE_MANAGER_FORMSET_GUID \
  { 0xc7b3e0a1, 0x5d2f, 0x4816, { 0x9a, 0x3b, 0x7e, 0x1c, 0x6f, 0x8d, 0x4a, 0x50 } }

// {D4E8F1A2-6C3B-4927-8B4C-9F2D7E0A5B61}
#define PROFILE_MANAGER_VARSTORE_GUID \
  { 0xd4e8f1a2, 0x6c3b, 0x4927, { 0x8b, 0x4c, 0x9f, 0x2d, 0x7e, 0x0a, 0x5b, 0x61 } }

//
// ============================================================
// Driver private data
// ============================================================
//

#define PROFILE_MANAGER_SIGNATURE  SIGNATURE_32 ('P', 'R', 'F', 'M')

///
/// HII-specific vendor device path
///
typedef struct {
  VENDOR_DEVICE_PATH          VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    End;
} HII_VENDOR_DEVICE_PATH;

typedef struct {
  UINTN                             Signature;
  EFI_HII_HANDLE                    HiiHandle;
  EFI_HANDLE                        DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL    ConfigAccess;

  //
  // Cached profile store header (read from flash once at init)
  //
  PROFILE_STORE_HEADER              StoreHeader;

  //
  // NOR flash DiskIo for direct SPI flash read/write
  //
  EFI_DISK_IO_PROTOCOL              *NorFlashDiskIo;
  UINT32                            NorFlashMediaId;

  //
  // Working copy of the varstore for the form
  //
  PROFILE_MANAGER_VARSTORE          VarStore;
} PROFILE_MANAGER_PRIVATE_DATA;

#define PROFILE_MANAGER_PRIVATE_FROM_THIS(a) \
  CR (a, PROFILE_MANAGER_PRIVATE_DATA, ConfigAccess, PROFILE_MANAGER_SIGNATURE)

//
// ============================================================
// VFR binary (generated by build)
// ============================================================
//

extern UINT8  ProfileManagerVfrBin[];

//
// ============================================================
// Function prototypes
// ============================================================
//

EFI_STATUS
EFIAPI
ProfileManagerExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Request,
  OUT EFI_STRING                            *Progress,
  OUT EFI_STRING                            *Results
  );

EFI_STATUS
EFIAPI
ProfileManagerRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Configuration,
  OUT EFI_STRING                            *Progress
  );

EFI_STATUS
EFIAPI
ProfileManagerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  );

#endif // _PROFILE_MANAGER_DXE_H_
