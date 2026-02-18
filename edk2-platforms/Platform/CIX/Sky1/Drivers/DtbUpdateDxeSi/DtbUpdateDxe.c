/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#include "DtbUpdateDxe.h"
#include <Library/DtbUpdateLibSi.h>
#include <libfdt.h>
#include <fdt.h>

STATIC
VOID
EFIAPI
DtsImageUpdateCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       *pSimpleFileHandle = NULL;
  EFI_FILE_PROTOCOL                *pFs               = NULL;
  EFI_FILE_PROTOCOL                *pFile             = NULL;
  EFI_FILE_INFO                    *FileInfo;
  UINTN                            FileInfoSize = 0;
  UINTN                            HandleNum;
  UINT32                           index;
  UINT64                           OpenMode;
  VOID                             *ImageBuffer            = NULL;
  UINTN                            ImageSize               = 0;
  CHAR16                           *FileName               = NULL;
  BOOLEAN                          fileFound               = FALSE;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *pSimpleFileSysProtocol = NULL;

  DEBUG ((DEBUG_INFO, "%a Entry\n", __FUNCTION__));
  gBS->CloseEvent (Event);
  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiSimpleFileSystemProtocolGuid,
                                    NULL,
                                    &HandleNum,
                                    &pSimpleFileHandle
                                    );
  DEBUG ((DEBUG_INFO, "%a HandleNum:%x\n", __FUNCTION__, HandleNum));
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "%a %d %r\n", __FUNCTION__, __LINE__, Status);
    return;
  }

  FileName = (CHAR16 *)PcdGetPtr (PcdSiliconDtbUpdateFileName);
  if (NULL == FileName) {
    return;
  }

  DEBUG ((DEBUG_INFO, "%a Dtb file name:%s\n", __FUNCTION__, FileName));
  for (index = 0; index < HandleNum; index++) {
    Status = gBS->OpenProtocol (
                                pSimpleFileHandle[index],
                                &gEfiSimpleFileSystemProtocolGuid,
                                (VOID **)&pSimpleFileSysProtocol,
                                gImageHandle,
                                NULL,
                                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                                );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a %d %r\n", __FUNCTION__, __LINE__, Status);
      return;
    }

    Status = pSimpleFileSysProtocol->OpenVolume (
                                                 pSimpleFileSysProtocol,
                                                 &pFs
                                                 );
    if (EFI_ERROR (Status)) {
      continue;
    }

    OpenMode = EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;
    Status   = pFs->Open (
                          pFs,
                          &pFile,
                          FileName,
                          OpenMode,
                          0
                          );
    if (EFI_ERROR (Status)) {
      continue;
    } else {
      fileFound = TRUE;
      break;
    }
  }

  if (fileFound) {
    Status = pFile->GetInfo (pFile, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    if (EFI_ERROR (Status) && (Status == EFI_BUFFER_TOO_SMALL)) {
      FileInfo = AllocatePool (FileInfoSize);
      if (NULL == FileInfo) {
        DebugPrint (DEBUG_ERROR, "%a %d Allocate memory failed\n", __FUNCTION__, __LINE__);
        goto exit;
      }

      Status = pFile->GetInfo (pFile, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "%a %d Get file Info fail Status:%r\n", __FUNCTION__, __LINE__, Status);
        goto exit;
      }
    } else {
      DebugPrint (DEBUG_ERROR, "%a %d Get file Info fail Status:%r\n", __FUNCTION__, __LINE__, Status);
      goto exit;
    }

    ImageSize   = FileInfo->FileSize;
    ImageBuffer = AllocatePool (ImageSize + 2048);
    if (NULL == ImageBuffer) {
      DebugPrint (DEBUG_ERROR, "%a %d Allocate memory failed\n", __FUNCTION__, __LINE__);
      goto exit;
    }

    Status = pFile->Read (pFile, &ImageSize, ImageBuffer);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "Read file %a faild", FileName);
      goto exit;
    }

    DEBUG ((DEBUG_INFO, "Before UpdateDeviceTree Data size :%d\n", ImageSize));
    Status = UpdateDtbSi (ImageBuffer, &ImageSize);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "UpdateDeviceTree Status :%r\n", Status);
      goto exit;
    }

    DEBUG ((DEBUG_INFO, "After UpdateDeviceTree Data size :%d\n", ImageSize));

    Status = pFile->SetPosition (pFile, 0);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "Cannot set file position to first byte :%r\n", Status);
      goto exit;
    }

    FileInfo->FileSize = ImageSize;
    Status             = pFile->SetInfo (pFile, &gEfiFileInfoGuid, FileInfoSize, FileInfo);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a %d Set file Info fail Status:%r\n", __FUNCTION__, __LINE__, Status);
      goto exit;
    }

    Status = pFile->Write (pFile, &ImageSize, ImageBuffer);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a %d Write file fail Status:%r\n", __FUNCTION__, __LINE__, Status);
      goto exit;
    }

    DEBUG ((DEBUG_INFO, "%a %d Update device tree completed %r\n", __FUNCTION__, __LINE__, Status));
  } else {
    DebugPrint (DEBUG_ERROR, "Dtb file not found\n");
    return;
  }

exit:
  if (FileInfo) {
    FreePool (FileInfo);
  }

  if (ImageBuffer) {
    FreePool (ImageBuffer);
  }

  if (pFile) {
    pFile->Flush (pFile);
    pFile->Close (pFile);
  }

  return;
}

VOID
EFIAPI
DumpDtbBootEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       *pSimpleFileHandle = NULL;
  EFI_FILE_PROTOCOL                *pFs               = NULL;
  EFI_FILE_PROTOCOL                *pFile             = NULL;
  EFI_FILE_INFO                    *FileInfo;
  UINTN                            FileInfoSize = 0;
  UINTN                            HandleNum;
  UINT32                           index;
  UINT64                           OpenMode;
  VOID                             *ImageBuffer            = NULL;
  UINTN                            ImageSize               = 0;
  CHAR16                           *FileName               = NULL;
  BOOLEAN                          fileFound               = FALSE;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *pSimpleFileSysProtocol = NULL;

  DEBUG ((DEBUG_INFO, "%a Entry\n", __FUNCTION__));
  gBS->CloseEvent (Event);
  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiSimpleFileSystemProtocolGuid,
                                    NULL,
                                    &HandleNum,
                                    &pSimpleFileHandle
                                    );
  DEBUG ((DEBUG_INFO, "%a HandleNum:%x\n", __FUNCTION__, HandleNum));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a %d %r\n", __FUNCTION__, __LINE__, Status));
    return;
  }

  FileName = (CHAR16 *)PcdGetPtr (PcdSiliconDtbUpdateFileName);

  if (NULL == FileName) {
    return;
  }

  DEBUG ((DEBUG_INFO, "%a Dtb file name:%s\n", __FUNCTION__, FileName));
  for (index = 0; index < HandleNum; index++) {
    Status = gBS->OpenProtocol (
                                pSimpleFileHandle[index],
                                &gEfiSimpleFileSystemProtocolGuid,
                                (VOID **)&pSimpleFileSysProtocol,
                                gImageHandle,
                                NULL,
                                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                                );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "%a %d %r\n", __FUNCTION__, __LINE__, Status));
      return;
    }

    Status = pSimpleFileSysProtocol->OpenVolume (
                                                 pSimpleFileSysProtocol,
                                                 &pFs
                                                 );
    if (EFI_ERROR (Status)) {
      continue;
    }

    OpenMode = EFI_FILE_MODE_READ;
    Status   = pFs->Open (
                          pFs,
                          &pFile,
                          FileName,
                          OpenMode,
                          0
                          );
    if (EFI_ERROR (Status)) {
      continue;
    } else {
      fileFound = TRUE;
      break;
    }
  }

  if (fileFound) {
    Status = pFile->GetInfo (pFile, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    if (EFI_ERROR (Status) && (Status == EFI_BUFFER_TOO_SMALL)) {
      FileInfo = AllocatePool (FileInfoSize);
      if (NULL == FileInfo) {
        DEBUG ((DEBUG_INFO, "%a %d Allocate memory failed\n", __FUNCTION__, __LINE__));
        goto exit;
      }

      Status = pFile->GetInfo (pFile, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "%a %d Get file Info fail Status:%r\n", __FUNCTION__, __LINE__, Status));
        goto exit;
      }
    } else {
      DEBUG ((DEBUG_INFO, "%a %d Get file Info fail Status:%r\n", __FUNCTION__, __LINE__, Status));
      goto exit;
    }

    ImageSize   = FileInfo->FileSize;
    ImageBuffer = AllocatePool (ImageSize);
    if (NULL == ImageBuffer) {
      DEBUG ((DEBUG_INFO, "%a %d Allocate memory failed\n", __FUNCTION__, __LINE__));
      goto exit;
    }

    Status = pFile->Read (pFile, &ImageSize, ImageBuffer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Read file %a faild", FileName));
      goto exit;
    }

    DumpDtbStatusSi (ImageBuffer);
  } else {
    DEBUG ((DEBUG_INFO, "File not found\n"));
    return;
  }

exit:
  if (FileInfo) {
    FreePool (FileInfo);
  }

  if (ImageBuffer) {
    FreePool (ImageBuffer);
  }

  if (pFile) {
    pFile->Flush (pFile);
    pFile->Close (pFile);
  }

  return;

  return;
}

EFI_STATUS
EFIAPI
DtbUpdateEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_EVENT   Event;
  EFI_EVENT   ReadyToBootEvent;

  // VOID        *Registration;

  DEBUG ((DEBUG_INFO, "DtbUpdateEnable:%d\n", PcdGetBool (PcdSiliconDtbUpdateEnable)));

  if (!PcdGetBool (PcdSiliconDtbUpdateEnable)) {
    return Status;
  }

  // Status = gBS->CreateEvent (
  //                 EVT_NOTIFY_SIGNAL,
  //                 TPL_CALLBACK,
  //                 DtsImageUpdateCallBack,
  //                 NULL,
  //                 &Event
  //                 );

  // ASSERT_EFI_ERROR (Status);

  // Status = gBS->RegisterProtocolNotify (
  //                 &gEfiSimpleFileSystemProtocolGuid,
  //                 Event,
  //                 &Registration
  //                 );

  //
  // Register notify function to dump DTB on ReadyToBoot Event.
  //
  Status = gBS->CreateEventEx (
                               EVT_NOTIFY_SIGNAL,
                               TPL_CALLBACK,
                               DumpDtbBootEventNotify,
                               NULL,
                               &gEfiEventReadyToBootGuid,
                               &ReadyToBootEvent
                               );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->CreateEventEx (
                               EVT_NOTIFY_SIGNAL,
                               TPL_CALLBACK,
                               DtsImageUpdateCallBack,
                               NULL,
                               &gEfiEventReadyToBootGuid,
                               &Event
                               );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
