/** @file  NorFlash.c

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022-2023, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/FirmwareParseLib.h>

STATIC
EFI_STATUS
GetFirmwareHeaderAddress (
  OUT UINTN  *Address
  )
{
  UINT32  Signature;

  Signature = MmioRead32 (FIRMWARE_LAYOUT_BASE_ADDRESS + FIRMWARE_HEADER_OFFSET);
  if (Signature == FIRMWARE_HEADER_SIGNATURE) {
    *Address =  FIRMWARE_LAYOUT_BASE_ADDRESS + FIRMWARE_HEADER_OFFSET;
  } else {
    Signature = MmioRead32 (FIRMWARE_LAYOUT_BASE_ADDRESS + FIRMWARE_HEADER_OFFSET_ALT);
    if (Signature == FIRMWARE_HEADER_SIGNATURE) {
      *Address = FIRMWARE_LAYOUT_BASE_ADDRESS + FIRMWARE_HEADER_OFFSET_ALT;
    } else {
      return EFI_NOT_FOUND;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FindFirmwareEntry (
  IN  FIRMWARE_TYPE   Type,
  OUT FIRMWARE_ENTRY  **FwEntry
  )
{
  EFI_STATUS       Status;
  UINT32           Index;
  UINTN            Address;
  FIRMWARE_HEADER  *Header;
  FIRMWARE_ENTRY   *Entry;

  Status = GetFirmwareHeaderAddress (&Address);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: firmware base address not match\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  Header = (FIRMWARE_HEADER *)Address;
  for (Index = 0; Index < Header->EntryCount; Index++) {
    Entry = &(Header->EntryNode[Index]);
    if (Entry->Type == Type) {
      *FwEntry = Entry;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
FindFirmwareRawData (
  IN  FIRMWARE_TYPE  Type,
  OUT UINT32         **Location,
  OUT UINTN          *Length
  )
{
  EFI_STATUS      Status;
  FIRMWARE_ENTRY  *Entry;

  Status = FindFirmwareEntry (Type, (FIRMWARE_ENTRY **)&Entry);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Length   = Entry->Length;
  *Location = (UINT32 *)(UINTN)Entry->Base;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetFlashConfig (
  IN  UINT32       JedecId,
  OUT XSPI_CONFIG  **Configs
  )
{
  EFI_STATUS         Status;
  UINT8              *Location;
  XSPI_CONFIG_TABLE  *ConfigTable;
  UINTN              Length;
  CHAR8              MagicString[CBFF_MAGIC_SIZE + 1] = CBFF_MAGIC;
  UINT32             Index;

  if (Configs == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = FindFirmwareRawData (BOOT_LOADER_1, (UINT32 **)&Location, &Length);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: SPI configs data not found\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  if (!AsciiStrnCmp ((CHAR8 *)(FIRMWARE_LAYOUT_BASE_ADDRESS + Location), MagicString, CBFF_MAGIC_SIZE)) {
    ConfigTable = (XSPI_CONFIG_TABLE *)(FIRMWARE_LAYOUT_BASE_ADDRESS + Location + SIZE_2KB);
    for (Index = 0; Index < ConfigTable->Number; Index++) {
      if (ConfigTable->Config[Index].JedecId == JedecId) {
        *Configs = &ConfigTable->Config[Index];
        DEBUG ((DEBUG_INFO, "SPI flash(JEDEC ID %x) configs found at %x\n", ConfigTable->Config[Index].JedecId, *Configs));
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
FindFirmwareEntryInfo (
  IN  UINT8*        pImageBuff,
  IN  UINT32*       ImageLength,
  IN  FIRMWARE_TYPE FwType,
  OUT UINT32*       FwBase,
  OUT UINT32*       FwLength
  )
{
  FIRMWARE_HEADER* FwHeader = (FIRMWARE_HEADER*)pImageBuff;
  FIRMWARE_ENTRY*  Entry;
  UINT32           ExpectedImageLength = 0;
  UINT32           InputEntyryCount = 0;
  UINT32           EntryIndex = 0;

  if (FwHeader->Signature != FIRMWARE_HEADER_SIGNATURE) {
    DEBUG ((DEBUG_ERROR, "[%a]: Firmware header signature not match\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  ExpectedImageLength = OFFSET_OF(FIRMWARE_HEADER, EntryNode) + sizeof(FIRMWARE_ENTRY) * FwHeader->EntryCount;

  InputEntyryCount = (*ImageLength - OFFSET_OF(FIRMWARE_HEADER, EntryNode)) / sizeof(FIRMWARE_ENTRY);

  if(InputEntyryCount > FwHeader->EntryCount) {
    InputEntyryCount = FwHeader->EntryCount;
  }

  for (EntryIndex = 0; EntryIndex < InputEntyryCount; EntryIndex++) {
    Entry = (FIRMWARE_ENTRY *)&FwHeader->EntryNode[EntryIndex];
    if(Entry->Type == FwType) {
      *FwBase = Entry->Base;
      *FwLength = Entry->Length;
      return EFI_SUCCESS;
    }
  }

  if(*ImageLength < ExpectedImageLength) {
    *ImageLength = ExpectedImageLength;
    DEBUG ((DEBUG_ERROR, "[%a]: Firmware header is incompleted, iamge buffer too small\n", __FUNCTION__));
    return EFI_BUFFER_TOO_SMALL;
  }

  DEBUG ((DEBUG_ERROR, "[%a]: Firmware type %d not found in firmware header\n", __FUNCTION__, FwType));
  return EFI_NOT_FOUND;
}
