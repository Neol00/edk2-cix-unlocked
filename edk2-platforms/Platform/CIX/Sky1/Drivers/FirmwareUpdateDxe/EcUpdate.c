/** @CixEcUpdate.c

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "FirmwareUpdate.h"
extern EFI_BOOT_SERVICES  *gBS;

typedef struct {
  CHAR8     *StrEcVersion;
  CHAR8     *StrEcSize;
  UINT32    EcSize;
} EC_INFO;

#define ITE_EC_SIZE_OFFSET  0x64
#define ITE_VER_OFFSET      0x50
#define ITE_VER_LENGTH      4
CHAR8  *IteSig = "ITE ";

/*
EC_INFO  EcInfo[] = {
  { "ITE EC-V14.6", "ITE 128K", 0x20000 },
  { "ITE EC-V14.6", "ITE 256K", 0x40000 },
  { NULL,           NULL,       0       }
};
*/
EFI_STATUS
EFIAPI
CixFirmwareEcProgram (
  );

BOOLEAN
IsDigit (
  UINT8  c
  )
{
  return (('0' <= (c)) && ((c) <= '9'));
}

BOOLEAN
IfNeedToUpdateEcFirmware (
  VOID   *pImageBuffer,
  UINTN  ImageSize
  )
{
  EFI_STATUS  Status;
  UINT8       *pOnboardBuff;
  UINT8       VerImage[5], VerOnboard[5], EcSizeA[5];
  UINT8       *pVer, *pSize;
  BOOLEAN     NeedUpdate = FALSE;
  UINT16      MajorImage = 0, MajorOnboard = 0, MinorImage = 0, MinorOnboard = 0;
  UINTN       EcFwSize;

  pOnboardBuff = AllocateZeroPool (0x100);
  Status       = CixFlashReadWrapper (EC_OFFSET, 0x100, pOnboardBuff);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: status %r\n", __FUNCTION__, Status));
    return FALSE;
  }

  if (!CompareMem (((UINT8 *)(pImageBuffer) + ITE_VER_OFFSET), IteSig, 4)) {
    // if ((!CompareMem (((UINT8 *)(pOnboardBuff) + ITE_VER_OFFSET), IteSig, 4)) &&
    //    (!CompareMem (((UINT8 *)(pImageBuffer) + ITE_VER_OFFSET), IteSig, 4)))
    CopyMem (VerOnboard, ((UINT8 *)(pOnboardBuff) + ITE_VER_OFFSET + 0x08), 4);
    CopyMem (VerImage, ((UINT8 *)(pImageBuffer) + ITE_VER_OFFSET + 0x08), 4);
    CopyMem (EcSizeA, ((UINT8 *)(pImageBuffer) + ITE_EC_SIZE_OFFSET), 4);

    pVer = VerImage;
    while (IsDigit (*pVer)) {
      MajorImage = MajorImage * 10 + (*pVer++ - '0');
    }

    pVer++;
    if (IsDigit (*pVer)) {
      MinorImage = *pVer - '0';
    }

    DEBUG ((DEBUG_ERROR, "EcVer in image: %d.%d\n", MajorImage, MinorImage));

    pVer = VerOnboard;
    while (IsDigit (*pVer)) {
      MajorOnboard = MajorOnboard * 10 + (*pVer++ - '0');
    }

    pVer++;
    if (IsDigit (*pVer)) {
      MinorOnboard = *pVer - '0';
    }

    DEBUG ((DEBUG_ERROR, "EcVer in flash: %d.%d\n", MajorOnboard, MinorOnboard));
    if ((MajorImage > MajorOnboard) ||
        ((MajorImage == MajorOnboard) && (MinorImage >= MinorOnboard)))
    {
      pSize = EcSizeA;
      while (IsDigit (*pSize)) {
        EcFwSize = EcFwSize * 10 + (*pSize++ - '0');
        //DEBUG ((DEBUG_ERROR, "%x ", *pSize));
      }

      //DEBUG ((DEBUG_ERROR, "\n%d \n", EcFwSize));
      EcFwSize = EcFwSize*1024;
      DEBUG ((DEBUG_ERROR, "Ec fw size 0x%x, bin size:0x%x\n", EcFwSize, ImageSize));
      if (EcFwSize == ImageSize) {
        NeedUpdate = TRUE;
      }
    }
  }

  return NeedUpdate;
}

EFI_STATUS
EFIAPI
CixFirmwareEcProgram (
  )
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  UINT32                         dwflag = 0;
  UINT8                          RetVal;
  UINTN                          FvHandleCount;
  EFI_HANDLE                     *FvHandleBuffer;
  UINTN                          Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL  *Fv;
  UINT32                         AuthenticationStatus;
  VOID                           *pImageBuffer;

  UINTN   ImageSize;
  UINT32  *pTempU32, i;

  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiFirmwareVolume2ProtocolGuid,
                                    NULL,
                                    &FvHandleCount,
                                    &FvHandleBuffer
                                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, ": locate gEfiFirmwareVolume2ProtocolGuid failed- %r\n", Status));
    return Status;
  }

  DEBUG ((DEBUG_INFO, ": locate gEfiFirmwareVolume2ProtocolGuid success.\n"));

  for (Index = 0; Index < FvHandleCount; Index++) {
    gBS->HandleProtocol (
                         FvHandleBuffer[Index],
                         &gEfiFirmwareVolume2ProtocolGuid,
                         (VOID **)&Fv
                         );

    pImageBuffer = NULL;
    ImageSize    = 0;
    Status       = Fv->ReadSection (
                                    Fv,
                                    &gEcFirmwareFileGuid,
                                    EFI_SECTION_RAW,
                                    0,
                                    &pImageBuffer,
                                    &ImageSize,
                                    &AuthenticationStatus
                                    );
    if (EFI_ERROR (Status)) {
      continue;
    } else {
      // Found ec firmware
      DEBUG ((DEBUG_INFO, "found ec.bin ...\n"));
      pTempU32 = (UINT32 *)pImageBuffer;
      for (i = 0; i < 64;) {
        DEBUG ((DEBUG_INFO, "%08x  %08x  %08x  %08x\n", pTempU32[i], pTempU32[i+1], pTempU32[i+2], pTempU32[i+3]));
        i = i+4;
      }

      break;
    }
  }

  if (FvHandleBuffer != NULL) {
    gBS->FreePool (FvHandleBuffer);
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "no ec.bin ...\n"));
    return Status;
  }

  Status = LocateNorFlashDiskIoProtocol ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "No nor flash program protocol found.\n"));
    return Status;
  }

  // compare version need
  if (!IfNeedToUpdateEcFirmware (pImageBuffer, ImageSize)) {
    DEBUG ((DEBUG_INFO, "NoNeed to update EC firmware\n"));
    return Status;
  }

  RetVal = CixFlashWriteWrapper (FIRMWARE_TYPE_EC, pImageBuffer, ImageSize, dwflag, EC_OFFSET);
  if (FIRMWARE_RET_SUCCESS != RetVal) {
    DEBUG ((DEBUG_INFO, "EC firmware update failed\n"));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "EC firmware update success\n"));
  // do cold reset

  /*
  gRT->ResetSystem(
      EfiResetCold,
      0,
      0,
      NULL);
  EFI_DEADLOOP(); */

  return Status;
}
