/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "FirmwareConfigUpdateDxe.h"
#include <PlatformSetupVar.h>
#include <MemConfigurationTable.h>

BOOLEAN
IsNorFlashDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  VENDOR_DEVICE_PATH        *VendorDevicePath;

  TempDevicePath = DevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
    if ((DevicePathType (TempDevicePath) == HARDWARE_DEVICE_PATH) &&
        (DevicePathSubType (TempDevicePath) == HW_VENDOR_DP))
    {
      VendorDevicePath = (VENDOR_DEVICE_PATH *)TempDevicePath;
      if (CompareGuid (&(VendorDevicePath->Guid), &gCixNorFlashDevicePathGuid)) {
        return TRUE;
      }
    }

    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return FALSE;
}

EFI_STATUS
CheckPlatformVarUpdate (
  PLATFORM_SETUP_DATA  *PlatformSetupVar,
  BOOLEAN              *Update
  )
{
  EFI_STATUS                Status;
  UINTN                     HandleCount = 0;
  EFI_HANDLE                *HandleBuffer = NULL;
  UINT32                    Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DISK_IO_PROTOCOL      *NorFlashDiskIo = NULL;
  UINT32                     MediaId;
  UINT8                     *pImageBuff = NULL;
  UINT32                    ImageSize;
  UINT32                    FwHeaderOffset;
  UINT32                    MemCfgOffset;
  UINT32                    MemCfgLength;
  MEM_QUICK_CONFIG*         MemQuickHeader;

  *Update = FALSE;

  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiDiskIoProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &HandleBuffer
                                    );

  for (Index = 0; Index < HandleCount; Index++) {
    DevicePath = DevicePathFromHandle (HandleBuffer[Index]);
    if (!DevicePath || !IsNorFlashDevicePath (DevicePath)) {
      continue;
    }

    MediaId = ((NOR_FLASH_DEVICE_PATH *)DevicePath)->Index;

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDiskIoProtocolGuid, (VOID **)&NorFlashDiskIo);
    if (!EFI_ERROR (Status)) {
        break;
    }
  }

  if (HandleBuffer) {
    FreePool(HandleBuffer);
  }

  if (Index == HandleCount || NorFlashDiskIo == NULL) {
    Status =  EFI_NOT_FOUND;
    DEBUG ((DEBUG_ERROR, "[%a] [%d]: Couldn't find nor-flash device.\n", __FUNCTION__, __LINE__));
    goto Done;
  }

  FwHeaderOffset = FIRMWARE_HEADER_OFFSET;
  ImageSize = sizeof(FIRMWARE_HEADER);
  pImageBuff = AllocateZeroPool (ImageSize);
  Status = NorFlashDiskIo->ReadDisk(NorFlashDiskIo, MediaId, FwHeaderOffset, ImageSize, pImageBuff);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] [%d]: Read nor-flash address %x length %x fail, %r.\n", __FUNCTION__, __LINE__, FwHeaderOffset, ImageSize, Status));
    goto Done;
  }

  Status = FindFirmwareEntryInfo(pImageBuff, &ImageSize, MEMORY_CONFIGS, &MemCfgOffset, &MemCfgLength);

  if (Status == EFI_INVALID_PARAMETER) {
    FwHeaderOffset = FIRMWARE_HEADER_OFFSET_ALT;
    ZeroMem (pImageBuff, ImageSize);
    Status = NorFlashDiskIo->ReadDisk(NorFlashDiskIo, MediaId, FwHeaderOffset, ImageSize, pImageBuff);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "[%a] [%d]: Read nor-flash address %x length %x, %r.\n", __FUNCTION__, __LINE__, FwHeaderOffset, ImageSize, Status));
      goto Done;
    }

    Status = FindFirmwareEntryInfo(pImageBuff, &ImageSize, MEMORY_CONFIGS, &MemCfgOffset, &MemCfgLength);
  }

  if (Status == EFI_BUFFER_TOO_SMALL) {
    FreePool (pImageBuff);
    pImageBuff = AllocateZeroPool (ImageSize);
    Status = NorFlashDiskIo->ReadDisk(NorFlashDiskIo, MediaId, FwHeaderOffset, ImageSize, pImageBuff);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "[%a] [%d]: Read nor-flash address %x length %x %r.\n", __FUNCTION__, __LINE__, FwHeaderOffset, ImageSize, Status));
      goto Done;
    }

    Status = FindFirmwareEntryInfo(pImageBuff, &ImageSize, MEMORY_CONFIGS, &MemCfgOffset, &MemCfgLength);
  }

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] [%d]: Find memory config entry fail, %r.\n", __FUNCTION__, __LINE__,  Status));
    goto Done;
  }

  FreePool (pImageBuff);
  pImageBuff = AllocateZeroPool (MemCfgLength);
  Status = NorFlashDiskIo->ReadDisk(NorFlashDiskIo, MediaId, MemCfgOffset, MemCfgLength, pImageBuff);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] [%d]: Read nor-flash address %x length %x, %r.\n", __FUNCTION__, __LINE__, MemCfgOffset, MemCfgLength, Status));
    goto Done;
  }

  MemQuickHeader = (MEM_QUICK_CONFIG *)pImageBuff;

  if (PlatformSetupVar->CpuFMax != MemQuickHeader->cpufmax) {
     PlatformSetupVar->CpuFMax = MemQuickHeader->cpufmax;
     *Update = TRUE;
  }

Done:
  if (pImageBuff) {
    FreePool (pImageBuff);
  }
  return Status;
}

EFI_STATUS
UpdatePlatformVarFromMemCfg (
  VOID
  )
{
  EFI_STATUS                Status;
  PLATFORM_SETUP_DATA       PlatformSetupVar;
  UINTN                     VarSize;
  UINT32                    Attributes;
  BOOLEAN                   Update = FALSE;

  VarSize = sizeof (PLATFORM_SETUP_DATA);

  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  &Attributes,
                  &VarSize,
                  &PlatformSetupVar
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[%a][%d]: Get Platform Setup Variable failed, Status = %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }

  Status = CheckPlatformVarUpdate(&PlatformSetupVar, &Update);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[%a][%d]: Check Platform Setup Variable failed, Status = %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }

  if (Update) {
    Status = gRT->SetVariable (
                PLATFORM_SETUP_VAR,
                &gPlatformSetupVariableGuid,
                Attributes,
                VarSize,
                &PlatformSetupVar
                );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]: Set Platform Setup Variable failed, Status = %r\n", __FUNCTION__, __LINE__, Status);
      return Status;
    }
  }

  return Status;
}

EFI_STATUS
GetMemConfigAndUpdate (
  PLATFORM_SETUP_DATA  *pPlatformSetupData,
  UINT8                *pImage,
  BOOLEAN              *Update
  )
{
  MEM_QUICK_CONFIG  *MemQuickCfg = (MEM_QUICK_CONFIG*)pImage;

  *Update = FALSE;

  if(MemQuickCfg->cpufmax != pPlatformSetupData->CpuFMax) {
    DEBUG ((DEBUG_INFO, "Update CpuFMax %x\n", pPlatformSetupData->CpuFMax));
    MemQuickCfg->cpufmax = pPlatformSetupData->CpuFMax;
    *Update = TRUE;
  }

  return EFI_SUCCESS;
}

VOID
FwConfigUpdateCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS              Status;
  UINT8                   *pImage;
  UINTN                   Length = 0x1000;
  BOOLEAN                 Update = FALSE;
  UINTN                   VarSize;
  PLATFORM_SETUP_DATA     *pPlatformSetupVar;
  CIX_FW_UPDATE_PROTOCOL  *FlashUpdateProtocol = NULL;


  DEBUG ((DEBUG_INFO, "FwConfigUpdateCallback\n"));
  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));

  VarSize           = sizeof (PLATFORM_SETUP_DATA);
  pPlatformSetupVar = AllocateZeroPool (VarSize);
  if (pPlatformSetupVar == NULL) {
    DEBUG ((DEBUG_ERROR, "[%a][%d]: AllocateZeroPool failed\n", __FUNCTION__, __LINE__));
    goto Done;
  }

  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VarSize,
                  pPlatformSetupVar
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a][%d]: GetVariable failed, Status = %r\n", __FUNCTION__, __LINE__, Status));
    goto Done;
  }

  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));
  pImage = AllocateZeroPool (Length);
  if (pImage == NULL) {
    DEBUG ((DEBUG_ERROR, "[%a][%d]: AllocateZeroPool failed\n", __FUNCTION__, __LINE__));
    goto Done;
  }

  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));
  Status = gBS->LocateProtocol (&gCixFirmwareUpdateProtocolGuid, NULL, (VOID **)&FlashUpdateProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a][%d]: LocateProtocol failed: %r\n", __FUNCTION__, __LINE__, Status));
    goto Done;
  }

  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));
  Status = FlashUpdateProtocol->FirmwareRawEntryUpdate (FIRMWARE_TYPE_MEM_CONF, pImage, Length, ENTRY_READ, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a][%d]: FirmwareRawEntryUpdate Read failed: %r\n", __FUNCTION__, __LINE__, Status));
    goto Done;
  }

  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));
  DEBUG ((DEBUG_INFO, "[%a][%d]: FirmwareRawEntryUpdate Read success\n", __FUNCTION__, __LINE__));
  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));
  Status = GetMemConfigAndUpdate (pPlatformSetupVar, pImage, &Update);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetMemConfigBiosBlockAndUpdate Status:%r\n", Status));
    goto Done;
  }

  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));
  if (Update) {
    Status = FlashUpdateProtocol->FirmwareRawEntryUpdate (FIRMWARE_TYPE_MEM_CONF, pImage, Length, ENTRY_WRITE, NULL);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[%a][%d]: FirmwareRawEntryUpdate write failed: %r\n", __FUNCTION__, __LINE__, Status));
      goto Done;
    }
  }

  DEBUG ((DEBUG_INFO, "Debug-------------------------------------------------------[%a][%d]\n", __FUNCTION__, __LINE__));
Done:
  if (pImage != NULL) {
    FreePool (pImage);
  }

  if (pPlatformSetupVar != NULL) {
    FreePool (pPlatformSetupVar);
  }

  return;
}

EFI_STATUS
EFIAPI
FirmwareConfigUpdateDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_EVENT   Event;

  UpdatePlatformVarFromMemCfg();

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  FwConfigUpdateCallback,
                  NULL,
                  &gSetupSaveVariableCallBackGuid,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FirmwareConfigUpdateDxeEntryPoint: CreateEvent failed: %r\n", Status));
    return Status;
  }

  return Status;
}
