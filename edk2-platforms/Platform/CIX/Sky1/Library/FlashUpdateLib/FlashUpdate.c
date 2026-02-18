/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "FlashUpdate.h"
#include <Protocol/CixFwUpdateProtocol.h>
#include <Protocol/SocInfoProtocol.h>

CIX_FW_UPDATE_PROTOCOL  *FlashUpdateProtocol = NULL;
// IMAGE_VERSION_INFO             ImageVersionInfo;
UPDATE_RESULT  UpdateResult;

extern EFI_GUID  gCixFirmwareUpdateProtocolGuid;

#if 0
FIRMWARE_PROGRAM_CALLBACK
ProgressCallback (
  UINT8   *pData,
  UINT32  Size
  )
{
  // Print (L"Call back function test:%X,%X\n", *pData, Size);
  memset ((void *)&UpdateResult, 0x00, sizeof (UPDATE_RESULT));
  UpdateResult.Type   = ((UPDATE_RESULT *)pData)->Type;
  UpdateResult.Status = ((UPDATE_RESULT *)pData)->Status;
  // Print (L"\nCall back function test Type:%X, Status:%X\n", UpdateResult.Type, UpdateResult.Status);
  return 0;
}

#endif

EFI_STATUS
GetBootloaderInfo (
  CHAR8  *BL1VersionStrNum,
  CHAR8  *BL2VersionStrNum
  )
{
  EFI_STATUS    Status;
  UINT32        HdrOffset;
  VERSION_INFO  OldBL1Version;
  VERSION_INFO  OldBL2Version;
  VERSION_INFO  OldBL3Version;
  Status = gBS->LocateProtocol (&gCixFirmwareUpdateProtocolGuid, NULL, (VOID **)&FlashUpdateProtocol);
  if (EFI_ERROR (Status)) {
    Print (L"Firmware update protocol is not installed.\n");
    return Status;
  }

  // Get Onboard Image Header Info
  Status = FlashUpdateProtocol->FirmwareGetVersion (&HdrOffset, &OldBL1Version.info, &OldBL2Version.info, &OldBL3Version.info);
  if (Status != EFI_SUCCESS) {
    Print (L"Get Onboard Firmware version failed!\n");
    return EFI_NOT_FOUND;
  }

  AsciiSPrint (BL1VersionStrNum, sizeof (BL1VersionStrNum), "0x%llx%llx%llx", OldBL1Version.Bits.major, OldBL1Version.Bits.minor, OldBL1Version.Bits.revision);
  AsciiSPrint (BL2VersionStrNum, sizeof (BL2VersionStrNum), "0x%llx%llx%llx", OldBL2Version.Bits.major, OldBL2Version.Bits.minor, OldBL2Version.Bits.revision);

  return Status;
}

#if 0
EFI_STATUS
CheckAndDisplayImageInfo (
  IN UINT8  *pNewImage
  )
{
  EFI_STATUS       Status;
  UINT8            *pBuffer;
  UINT8            *pBL1Buffer, *pBL2Buffer;
  FIRMWARE_HEADER  *pFirmwareHeader;
  FIRMWARE_ENTRY   *pEntry;
  UINT8            Index;
  UINT32           HdrOffset;

  // Get Onboard Image Header Info
  pBuffer = AllocatePool (FIRMWARE_HEADER_OFFSET + FIRMWARE_HEADER_SIZE);
  Status  = FlashUpdateProtocol->FirmwareRead (0, FIRMWARE_HEADER_OFFSET + FIRMWARE_HEADER_SIZE, pBuffer);
  if (!EFI_ERROR (Status)) {
    if (((FIRMWARE_HEADER *)pBuffer)->Signature == FIRMWARE_HEADER_SIGNATURE) {
      pFirmwareHeader = (FIRMWARE_HEADER *)pBuffer;
      HdrOffset       = 0;
    } else if (((FIRMWARE_HEADER *)(pBuffer + FIRMWARE_HEADER_OFFSET))->Signature == FIRMWARE_HEADER_SIGNATURE) {
      pFirmwareHeader = (FIRMWARE_HEADER *)(pBuffer + FIRMWARE_HEADER_OFFSET);
      HdrOffset       = FIRMWARE_HEADER_OFFSET;
    } else {
      Print (L"Onboard Firmware Header Not Found.\n");
      return Status;
    }

    pEntry = pFirmwareHeader->EntryNode;
    for (Index = 0; Index < pFirmwareHeader->EntryCount; Index++) {
      if (pEntry->Type == 1) {
        // Bootloader1
        pBL1Buffer                     = AllocatePool (pEntry->Length);
        Status                         = FlashUpdateProtocol->FirmwareRead (pEntry->Base, pEntry->Length, pBL1Buffer);
        ImageVersionInfo.OldBL1Version = ((CBFF_IMAGE_HEADER *)pBL1Buffer)->Cbff_version;
        FreePool (pBL1Buffer);
      }

      if (pEntry->Type == 2) {
        // Bootloader2
        pBL2Buffer                     = AllocatePool (pEntry->Length);
        Status                         = FlashUpdateProtocol->FirmwareRead (pEntry->Base, pEntry->Length, pBL2Buffer);
        ImageVersionInfo.OldBL2Version = ((FIP_TOC_HEADER *)pBL2Buffer)->serial_number;
        FreePool (pBL2Buffer);
      }

      pEntry++;
    }

    FreePool (pBuffer);
  }

  pFirmwareHeader = (FIRMWARE_HEADER *)(pNewImage+HdrOffset);
  if (pFirmwareHeader->Signature != FIRMWARE_HEADER_SIGNATURE) {
    Print (L"This image not proper!\n");
  }

  {
    pEntry = pFirmwareHeader->EntryNode;
    for (Index = 0; Index < pFirmwareHeader->EntryCount; Index++) {
      if (pEntry->Type == 1) {
        // Bootloader1
        ImageVersionInfo.NewBL1Version = ((CBFF_IMAGE_HEADER *)(pNewImage + pEntry->Base))->Cbff_version;
      }

      if (pEntry->Type == 2) {
        // Bootloader2
        ImageVersionInfo.NewBL2Version = ((FIP_TOC_HEADER *)(pNewImage + pEntry->Base))->serial_number;
      }

      pEntry++;
    }

    Print (L"#    Old BootLoader1 Version:%04x       Old BootLoader2 Version:%04x   #\n", ImageVersionInfo.OldBL1Version, ImageVersionInfo.OldBL2Version);
    Print (L"#    New BootLoader1 Version:%04x       New BootLoader2 Version:%04x   #\n", ImageVersionInfo.NewBL1Version, ImageVersionInfo.NewBL2Version);
  }

  return Status;
}

#endif

EFI_STATUS
SystemFirmwareUpdate (
  IN UINT8  *pImage,
  UINTN     Length
  )
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (&gCixFirmwareUpdateProtocolGuid, NULL, (VOID **)&FlashUpdateProtocol);
  if (EFI_ERROR (Status)) {
    Print (L"Firmware update protocol is not installed.\n");
    return Status;
  }

  // CheckAndDisplayImageInfo(pImage);
  //  Status = FlashUpdateProtocol->FirmwarePackageProgram (pImage, Length, (FIRMWARE_PROGRAM_CALLBACK)ProgressCallback);
  Status = FlashUpdateProtocol->FirmwarePackageProgram (pImage, Length, NULL);

  Status &= 0xFF;
  if (EFI_ERROR (Status)) {
    Print (L"\nFlash fail status:%x\n", Status);
  }

  return Status;
}

EFI_STATUS
SystemSingleFirmwareUpdate (
  IN UINT8   *pImage,
  IN UINT32  Type,
  UINTN      Length
  )
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (&gCixFirmwareUpdateProtocolGuid, NULL, (VOID **)&FlashUpdateProtocol);
  if (EFI_ERROR (Status)) {
    Print (L"Firmware update protocol is not installed.\n");
    return Status;
  }

  // CheckAndDisplayImageInfo    (pImage);
  //  Status = FlashUpdateProtocol->FirmwareSingleProgram (Type, pImage, Length, (FIRMWARE_PROGRAM_CALLBACK)ProgressCallback);
  Status = FlashUpdateProtocol->FirmwareSingleProgram (Type, pImage, Length, NULL);

  Status &= 0xFF;
  if (EFI_ERROR (Status)) {
    Print (L"\nStatus:%x\n", Status);
  }

  return Status;
}

EFI_STATUS
GetSerialNum (
  CHAR8   *StrSerialNum,
  UINT32  Len
  )
{
  // This is only a temporary solution for fastboot getvar serialno
  EFI_STATUS             Status;
  CIX_SOC_INFO_PROTOCOL  *pSocInfoProtocol;
  UINT32                 *pSocInfo;
  UINT32                 SocInfoSize;

  Status = gBS->LocateProtocol (
                  &gCixSocInfoProtocolGuid,
                  NULL,
                  (VOID **)&pSocInfoProtocol
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: soc info protocol not found\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  Status = pSocInfoProtocol->GetSocInfo (SerialNum, &pSocInfo, &SocInfoSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: GetSerialNum failed\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  gBS->SetMem (StrSerialNum, sizeof (StrSerialNum), 0);
  DEBUG ((DEBUG_INFO, "SerialNum: %llx %llx %llx %llx %llx\n", pSocInfo[0], pSocInfo[1], pSocInfo[2], pSocInfo[3], pSocInfo[4]));
  AsciiSPrint (StrSerialNum, 11, "0x%08x", pSocInfo[0]);
  AsciiSPrint (StrSerialNum + 10, 9, "%08x", pSocInfo[1]);

  return EFI_SUCCESS;
}
