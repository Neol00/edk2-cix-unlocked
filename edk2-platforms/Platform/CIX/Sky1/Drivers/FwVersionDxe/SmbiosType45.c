/**
  @file SmbiosType45.c

  Copyright 2025 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved. *

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <Protocol/FwVersionProtocol.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
typedef struct {
  FW_VERSION_TYPE    FwVerType;
  CHAR8              *FwVerName;
} FW_VERTYPE_NAME;

STATIC FW_VERTYPE_NAME  gFwTypeNameMapTable[] = {
  { FwVerSE,   "SE    Firmware\0" },
  { FwVerPM,   "PM    Firmware\0" },
  { FwVerPM,   "PBL   Firmware\0" },
  { FwVerATF,  "ATF   Firmware\0" },
  { FwVerTEE,  "TEE   Firmware\0" },
  { FwVerEC,   "EC    Firmware\0" },
  { FwVerSTMM, "STMM  Firmware\0" },
  { FwVerUEFI, "UEFI  Firmware\0" }
};

STATIC SMBIOS_TABLE_TYPE45  SmbiosType45Temple = {
  {                                             // SMBIOS_STRUCTURE Hdr
    SMBIOS_TYPE_FIRMWARE_INVENTORY_INFORMATION, // UINT8 Type
    sizeof (SMBIOS_TABLE_TYPE45),               // UINT8 Length
    SMBIOS_HANDLE_PI_RESERVED,
  },
  0x02,                            // FirmwareComponentName;
  0x03,                            // FirmwareVersion;
  VersionFormatTypeFreeForm,       // FirmwareVersionFormat;    ///< The enumeration value from FIRMWARE_INVENTORY_VERSION_FORMAT_TYPE
  0x01,                            // FirmwareId;
  FirmwareIdFormatTypeFreeForm,    // FirmwareIdFormat;         ///< The enumeration value from FIRMWARE_INVENTORY_FIRMWARE_ID_FORMAT_TYPE.
  0x01,                            // ReleaseDate;
  0x04,                            // Manufacturer;
  0x01,                            // LowestSupportedVersion;
  SIZE_4KB,                        // ImageSize;
  { .Updatable      = 1,
    .WriteProtected = 1,
    .Reserved       = 0 },         // Characteristics; Updatable & WriteProtected
  FirmwareInventoryStateEnabled,   // State;                    ///< The enumeration value from FIRMWARE_INVENTORY_STATE.
  ARRAY_SIZE (gFwTypeNameMapTable) // AssociatedComponentCount; Max Firmware components that can be associated with this firmware inventory information structure.
};

VOID
EFIAPI
InstallType45Structure (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS               Status  = EFI_SUCCESS;
  EFI_SMBIOS_PROTOCOL      *Smbios = NULL;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  CHAR8                    Manufacturer[] = "Cix Technology Group Co., Ltd.";
  CHAR8                    StrNull[]      = "NULL";
  UINTN                    Type45Size;
  CHAR16                   *FwVerBuff;
  CHAR8                    AsciiFwVerBuff[64];
  UINT32                   FwVerSize;
  CHAR8                    *StrPtr;
  SMBIOS_TABLE_TYPE45      *SmbiosType45;
  CIX_FW_VERSION_PROTOCOL  *CixFwVerProtocol;

  DEBUG ((DEBUG_INFO, "%a Entry\n", __FUNCTION__));
  CixFwVerProtocol = (CIX_FW_VERSION_PROTOCOL *)Context;
  ASSERT (CixFwVerProtocol != NULL);
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **)&Smbios
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to locate SMBIOS protocol: %r\n", Status));
    return;
  }

  for (UINTN i = 0; i < ARRAY_SIZE (gFwTypeNameMapTable); i++) {
    // Reinitialize variables
    // so we don't depend on the last run's result
    Type45Size        = 0;
    FwVerBuff         = NULL;
    AsciiFwVerBuff[0] = '\0';
    FwVerSize         = 0;
    SmbiosType45      = NULL;

    Status = CixFwVerProtocol->GetFwVersion (
                                 gFwTypeNameMapTable[i].FwVerType,
                                 (CHAR16 **)&FwVerBuff,
                                 &FwVerSize
                                 );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "GetFwVersion failed for type %d: %r\n", i, Status));
      FwVerSize = 0;
    } else {
      // Calulate ascii size
      DEBUG ((DEBUG_INFO, "%a %d Calulate StrLen...\n", __FUNCTION__, __LINE__));
      FwVerSize = StrLen (FwVerBuff) + 1; // Add 1 for the null terminator
    }

    DEBUG ((DEBUG_INFO, "%a %d Sta:%r \n", __FUNCTION__, __LINE__, Status));

    Type45Size = sizeof (SMBIOS_TABLE_TYPE45) +
                 AsciiStrSize (StrNull) +
                 AsciiStrSize (gFwTypeNameMapTable[i].FwVerName) +
                 (FwVerSize == 0 ? AsciiStrSize (StrNull) : (FwVerSize)) +
                 AsciiStrSize (Manufacturer) + 1;
    DEBUG ((DEBUG_INFO, "%a: Type45Size = 0x%x\n", __FUNCTION__, Type45Size));

    SmbiosType45 = AllocateZeroPool (Type45Size);
    if (SmbiosType45 == NULL) {
      DEBUG ((DEBUG_ERROR, "Failed to allocate memory for SMBIOS Type 45\n"));
      return;
    }

    if (EFI_ERROR (Status)) {
      // Update state since CixFwVerProtocol->GetFwVersion failed
      SmbiosType45->State = FirmwareInventoryStateUnknown;
    }

    CopyMem (SmbiosType45, &SmbiosType45Temple, sizeof (SMBIOS_TABLE_TYPE45));
    StrPtr = (CHAR8 *)((UINT8 *)SmbiosType45 + sizeof (SMBIOS_TABLE_TYPE45));
    // Firmware ID & RELEASE Date & LowestSupportedVersion == NULL
    AsciiStrCpyS (StrPtr, AsciiStrSize (StrNull), StrNull);
    DEBUG ((DEBUG_INFO, "%p %a \n", StrPtr, StrPtr));
    StrPtr += AsciiStrSize (StrNull); // Null terminator for FirmwareVersion
    // Component Name
    AsciiStrCpyS (
      StrPtr,
      AsciiStrSize (gFwTypeNameMapTable[i].FwVerName),
      gFwTypeNameMapTable[i].FwVerName
      );
    DEBUG ((DEBUG_INFO, "%p %a \n", StrPtr, StrPtr));
    StrPtr += AsciiStrSize (gFwTypeNameMapTable[i].FwVerName);
    // Firmware Version
    if (FwVerSize == 0) {
      // Copy NULL for FirmwareVersion, so string index does not change
      FwVerSize = AsciiStrSize (StrNull);
      AsciiStrCpyS (StrPtr, FwVerSize, StrNull);
    } else {
      UnicodeStrToAsciiStrS (
        FwVerBuff,
        AsciiFwVerBuff,
        sizeof (AsciiFwVerBuff)
        );
      // FwVerSize was already added by 1
      AsciiStrCpyS (StrPtr, FwVerSize, (CHAR8 *)AsciiFwVerBuff);
    }

    DEBUG ((DEBUG_INFO, "%p %a \n", StrPtr, StrPtr));
    StrPtr += FwVerSize;
    // Manufacturer
    AsciiStrCpyS (StrPtr, AsciiStrSize (Manufacturer), Manufacturer);
    DEBUG ((DEBUG_INFO, "%p %a \n", StrPtr, StrPtr));

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status       = Smbios->Add (
                             Smbios,
                             NULL,
                             &SmbiosHandle,
                             (EFI_SMBIOS_TABLE_HEADER *)SmbiosType45
                             );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "adding SMBIOS type 45 failed\n"));
      // stop adding rather than continuing
    }

    FreePool (SmbiosType45);
    DEBUG ((DEBUG_INFO, "%a %d Sta:%r \n", __FUNCTION__, __LINE__, Status));
  }

  return;
}
