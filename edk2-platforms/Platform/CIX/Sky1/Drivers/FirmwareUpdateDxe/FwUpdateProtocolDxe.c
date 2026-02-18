/** @CixFwUpdateProtocolDxe.c

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "FirmwareUpdate.h"

// CIX_FWUP_PRIVATE_DATA    CixFwUpPrivateData;
CIX_FWUP_PRIVATE_DATA      *pFwPrivateData       = NULL;
FIRMWARE_PROGRAM_CALLBACK  FimwareUpdateCallBack = NULL;
BOOLEAN                    IsOtaPackage          = FALSE;
BOOLEAN                    NeedFullPackageUpdate = TRUE;
EFI_DISK_IO_PROTOCOL       *NorFlashDiskIo       = NULL;
UINT32                     MediaId;
UINT32                     FlashBlockSize = SIZE_4KB;
FIRMWARE_PROGRAM_STATUS    FwProgStatus;
// CHAR8   *pFirmwareHeader;

#define EFI_DEADLOOP()                  \
  {                                     \
    volatile UINTN __DeadLoopVar__ = 1; \
    while (__DeadLoopVar__)             \
      ;                                 \
  }

void
DoCallBack (
  FIRMWARE_PROGRAM_STATUS  *pFwProgSts
  )
{
  UINT32  Size = 0x2;

  if ( FimwareUpdateCallBack != NULL) {
    FimwareUpdateCallBack ((UINT8 *)pFwProgSts, Size);
  }
}

UINT16
CixFirmwareUpdateInitialize (
  UINT8   *pNewFirmwareImage,
  UINT32  ImageSize,
  UINT32  dwflag
  )
{
  UINT16           ReturnCode;
  UINT32           HeaderSize, EntriesLength;
  FIRMWARE_HEADER  *pCixNewImageFwHeader;
  FIRMWARE_HEADER  *pCixOnboardFwHeader;
  UINT8            *pFirmwareHeader;
  FIRMWARE_ENTRY   *pEntry;
  UINT8            *pBuff;
  // UINT32  pBuffSize;
  UINT32                   Index;
  CIX_FIRMWARE_ENTRY_INFO  *pEntryInfo;
  EFI_STATUS               Status;

  Status = LocateNorFlashDiskIoProtocol ();
  if (EFI_ERROR (Status)) {
    return FIRMWARE_RET_NO_FLASH_PROG_PROTOCOL;
  }

  // pCixNewImageFwHeader = (FIRMWARE_HEADER*)pNewFirmwareImage;
  // SIG right?
  if (((FIRMWARE_HEADER *)pNewFirmwareImage)->Signature == FIRMWARE_HEADER_SIGNATURE) {
    if (((FIRMWARE_HEADER *)pNewFirmwareImage)->ControlFlag & UPDATE_OTA_PACKAGE) {
      IsOtaPackage    = TRUE;
      pFirmwareHeader = pNewFirmwareImage;
    }
  } else {
    if (((FIRMWARE_HEADER *)(pNewFirmwareImage + FIRMWARE_HEADER_OFFSET))->Signature == FIRMWARE_HEADER_SIGNATURE) {
      IsOtaPackage    = FALSE;
      pFirmwareHeader = pNewFirmwareImage + FIRMWARE_HEADER_OFFSET;
    } else {
      ReturnCode = FIRMWARE_RET_ERR_HEADER;
    }
  }

  HeaderSize = sizeof (FIRMWARE_HEADER) + (((FIRMWARE_HEADER *)pFirmwareHeader)->EntryCount - 1) * sizeof (FIRMWARE_ENTRY);
  // DEBUG ((DEBUG_INFO, "[FwU] new image Entry count %d, header HeaderSize %d\n", ((FIRMWARE_HEADER *)pFirmwareHeader)->EntryCount, HeaderSize));
  pCixNewImageFwHeader = AllocateRuntimePool (HeaderSize);
  CopyMem (pCixNewImageFwHeader, pFirmwareHeader, HeaderSize);

  if (IsOtaPackage == FALSE) {
    // entry node base address align 4KB?
    for (Index = 0; Index < pCixNewImageFwHeader->EntryCount; Index++) {
      pEntry = &(pCixNewImageFwHeader->EntryNode[Index]);
      DEBUG ((DEBUG_INFO, "[FwU] new image entry node address 0x%08x,HeaderSize 0x%08x\n", pEntry->Base, pEntry->Length));
      if (pEntry->Base % FlashBlockSize) {
        return FIRMWARE_RET_ERR_4KB_ALIGN; // not align 4KB return
      }
    }
  }

  pFwPrivateData = AllocateRuntimePool (sizeof (CIX_FWUP_PRIVATE_DATA));
  // initialize Fw Private Data
  pFwPrivateData->ImageBlockNum     = ImageSize / FlashBlockSize;
  pFwPrivateData->ProgramedBlock    = 0;
  pFwPrivateData->EntryCount        = pCixNewImageFwHeader->EntryCount;
  pFwPrivateData->pNewImageFwHeader = pCixNewImageFwHeader;

  pEntryInfo    = pFwPrivateData->ImageFwEntryInfo;
  EntriesLength = HeaderSize;
  for (Index = 0; Index < pCixNewImageFwHeader->EntryCount; Index++) {
    pEntry = &(pCixNewImageFwHeader->EntryNode[Index]);
    if (IsOtaPackage == TRUE) {
      pEntryInfo->Type             = pEntry->Type;
      pEntryInfo->EntryImageOffset = EntriesLength;
      pEntryInfo->EntryImageSize   = pEntry->Length;
      EntriesLength               += pEntry->Length;
    } else {
      // DEBUG ((DEBUG_INFO, "[FwU] new image entry node address 0x%08x,Length 0x%08x\n", pEntry->Base, pEntry->Length));
      pEntryInfo->Type             = pEntry->Type;
      pEntryInfo->EntryImageOffset = pEntry->Base;
      pEntryInfo->EntryImageSize   = pEntry->Length;
    }

    pEntryInfo++;
  }

  pEntryInfo = pFwPrivateData->ImageFwEntryInfo;
  for (Index = 0; Index < pCixNewImageFwHeader->EntryCount; Index++) {
    DEBUG ((DEBUG_INFO, "[FwU] new image entry node address 0x%08x,Length 0x%08x\n", pEntryInfo->EntryImageOffset, pEntryInfo->EntryImageSize));
    pEntryInfo++;
  }

  // initialize onboard fw header

  pBuff = AllocateZeroPool (FlashBlockSize);
  // read one sector from onboard flash 0x100000 or 0x200000 ,firmware header is here
  CixFlashReadWrapper (FIRMWARE_HEADER_OFFSET, FlashBlockSize, pBuff);

  if (((FIRMWARE_HEADER *)pBuff)->Signature != FIRMWARE_HEADER_SIGNATURE) {
    ReturnCode = FIRMWARE_RET_ERR_HEADER;
  }

  HeaderSize          = sizeof (FIRMWARE_HEADER) + (((FIRMWARE_HEADER *)pBuff)->EntryCount - 1) * sizeof (FIRMWARE_ENTRY);
  pCixOnboardFwHeader = AllocateRuntimePool (HeaderSize);
  CopyMem (pCixOnboardFwHeader, pBuff, HeaderSize);
  pFwPrivateData->pOnboardFwHeader = pCixOnboardFwHeader;

  FreePool (pBuff);
  return FIRMWARE_RET_SUCCESS;
}

UINT16
CixFirmwareUpdateOnboardHeaderParse (
  )
{
  UINT16           ReturnCode;
  UINT32           HeaderSize, EntriesLength;
  FIRMWARE_HEADER  *pCixOnboardFwHeader;
  // UINT8            *pFirmwareHeader;
  FIRMWARE_ENTRY  *pEntry;
  UINT8           *pBuff;
  // UINT32  pBuffSize;
  UINT32                   Index;
  CIX_FIRMWARE_ENTRY_INFO  *pEntryInfo;
  EFI_STATUS               Status;
  UINT32                   HeaderOffset;

  Status = LocateNorFlashDiskIoProtocol ();
  if (EFI_ERROR (Status)) {
    return FIRMWARE_RET_NO_FLASH_PROG_PROTOCOL;
  }

  // initialize onboard fw header

  pBuff = AllocateZeroPool (FlashBlockSize);
  // read one sector from onboard flash 0x100000 or 0x200000 ,firmware header is here
  CixFlashReadWrapper (FIRMWARE_HEADER_OFFSET, FlashBlockSize, pBuff);
  HeaderOffset = FIRMWARE_HEADER_OFFSET;

  if (((FIRMWARE_HEADER *)pBuff)->Signature != FIRMWARE_HEADER_SIGNATURE) {
    CixFlashReadWrapper (FIRMWARE_HEADER_OFFSET_ALT, FlashBlockSize, pBuff);
    HeaderOffset = FIRMWARE_HEADER_OFFSET_ALT;
    if (((FIRMWARE_HEADER *)pBuff)->Signature != FIRMWARE_HEADER_SIGNATURE) {
      ReturnCode = FIRMWARE_RET_ERR_HEADER;
      goto ON_EXIT;
    }
  }

  // pFirmwareHeader = pBuff;

  HeaderSize          = sizeof (FIRMWARE_HEADER) + (((FIRMWARE_HEADER *)pBuff)->EntryCount - 1) * sizeof (FIRMWARE_ENTRY);
  pCixOnboardFwHeader = AllocateRuntimePool (HeaderSize);
  CopyMem (pCixOnboardFwHeader, pBuff, HeaderSize);

  pFwPrivateData = AllocateRuntimePool (sizeof (CIX_FWUP_PRIVATE_DATA));
  if (pFwPrivateData == NULL) {
    ReturnCode = FIRMWARE_RET_OUT_OF_RESOURCE;
    goto ON_EXIT;
  }

  pFwPrivateData->HeaderOffset     = HeaderOffset;
  pFwPrivateData->EntryCount       = pCixOnboardFwHeader->EntryCount;
  pFwPrivateData->pOnboardFwHeader = pCixOnboardFwHeader;

  // FreePool (pBuff);

  pEntryInfo    = pFwPrivateData->ImageFwEntryInfo;
  EntriesLength = HeaderSize;
  for (Index = 0; Index < pCixOnboardFwHeader->EntryCount; Index++) {
    pEntry = &(pCixOnboardFwHeader->EntryNode[Index]);

    // DEBUG ((DEBUG_INFO, "[FwU] new image entry node address 0x%08x,Length 0x%08x\n", pEntry->Base, pEntry->Length));
    pEntryInfo->Type             = pEntry->Type;
    pEntryInfo->EntryImageOffset = pEntry->Base;
    pEntryInfo->EntryImageSize   = pEntry->Length;

    pEntryInfo++;
  }

  pEntryInfo = pFwPrivateData->ImageFwEntryInfo;
  for (Index = 0; Index < pFwPrivateData->EntryCount; Index++) {
    // dump entries
    DEBUG ((DEBUG_INFO, "[FwU] On board entry[%d] : address 0x%08x,Length 0x%08x\n", Index, pEntryInfo->EntryImageOffset, pEntryInfo->EntryImageSize));
    pEntryInfo++;
  }

  ReturnCode = FIRMWARE_RET_SUCCESS;

ON_EXIT:
  FreePool (pBuff);
  return ReturnCode;
}

EFI_STATUS
EFIAPI
CixFirmwareGetInfo (
  UINT8   *pNewFirmwareImage,
  UINT32  dwflag
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  // need check later
  return Status;
}

void
CixFirmwareProgramProgress (
  UINT32  BlockNum
  )
{
  UINT32  PercentData;

  PercentData = (BlockNum - pFwPrivateData->ProgramedBlock) * 100 / pFwPrivateData->ImageBlockNum;
  if (PercentData == 1) {
    pFwPrivateData->ProgramedBlock = BlockNum;
    PercentData                    = pFwPrivateData->ProgramedBlock * 100 / pFwPrivateData->ImageBlockNum;
    DEBUG ((DEBUG_INFO, "[FwU] Programming ... %02d %%\r", PercentData));
  }
}

BOOLEAN
IfNeedToUpdate (
  UINT8   Type,
  UINT8   *pImageBuff,
  UINT32  ImageBuffSize,
  UINT32  EntryOnboardAddress
  )
{
  BOOLEAN  NeedToUpdate = FALSE;
  VOID     *pBuff;

 #ifdef DEBUG_MODE
  UINT32           AndVal;
  FIRMWARE_HEADER  *pCixNewImageFwHeader;
  // if debug mode and force enable ,will to update
  pCixNewImageFwHeader = pFwPrivateData->pNewImageFwHeader;
  AndVal               = 1 << Type;
  DEBUG ((DEBUG_INFO, "[FwU]ControlFlag %08x.\n", pCixNewImageFwHeader->ControlFlag));
  if (pCixNewImageFwHeader->ControlFlag && AndVal) {
    NeedToUpdate = TRUE;
  }

  /* temp remove for debug,when controlflag setting ok will remove comments
  return NeedToUpdate;
  */
 #endif

  // release build will judge version
  pBuff = AllocateZeroPool (FlashBlockSize);
  // read one sector from onboard flash 0x100000 or 0x200000 ,firmware header is here
  CixFlashReadWrapper (EntryOnboardAddress, FlashBlockSize, pBuff);
  switch (Type) {
    case FIRMWARE_TYPE_BootLoader_1:
      if (((CBFF_IMAGE_HEADER *)pImageBuff)->Cbff_version >= ((CBFF_IMAGE_HEADER *)pBuff)->Cbff_version) {
        NeedToUpdate = TRUE;
      } else {
        DEBUG ((DEBUG_INFO, "[FwU]bootloader 1 version down.\n"));
      }

      break;
    case FIRMWARE_TYPE_BootLoader_2:
      if (((FIP_TOC_HEADER *)pImageBuff)->serial_number >= ((FIP_TOC_HEADER *)pBuff)->serial_number) {
        NeedToUpdate = TRUE;
      } else {
        DEBUG ((DEBUG_INFO, "[FwU]bootloader 2 version down.\n"));
      }

      break;
    case FIRMWARE_TYPE_MEM_CONF:
    case FIRMWARE_TYPE_PM_CONF:
    case FIRMWARE_TYPE_BootLoader_3:
      NeedToUpdate = TRUE;
      break;

    default:
      break;
  }

  DEBUG ((DEBUG_INFO, "[FwU]%a:%d\n", __FUNCTION__, NeedToUpdate));
  return NeedToUpdate;
}

UINT16
CixFirmwareEntryUpdateWrapper (
  UINT8           Type,
  UINT8           *pNewFirmwareImage,
  FIRMWARE_ENTRY  *pEntryUpdate,
  FIRMWARE_ENTRY  *pEntryOnboard
  )
{
  UINT16                   ReturnCode;
  UINT8                    *pFirmwareEntryImage;
  UINT32                   EntryImageOffset, EntryImageSize;
  UINT32                   EntryOnboardLength;
  UINT32                   EntryOnboardAddress;
  UINT32                   Index;
  CIX_FIRMWARE_ENTRY_INFO  *pEntryInfo;
  UINT32                   dwflag = 0;
  BOOLEAN                  Found  = FALSE;
  UINT8                    RetVal;

  ReturnCode = Type << 8;
  pEntryInfo = pFwPrivateData->ImageFwEntryInfo;
  for (Index = 0; Index < pFwPrivateData->EntryCount; Index++) {
    if (Type == pEntryInfo->Type) {
      EntryImageOffset = pEntryInfo->EntryImageOffset;
      EntryImageSize   = pEntryInfo->EntryImageSize;
      Found            = TRUE;
      break;
    }

    pEntryInfo++;
  }

  if (!Found) {
    return (ReturnCode | FIRMWARE_RET_ERR_INPUT);
  }

  EntryOnboardLength  = pEntryOnboard->Length;
  EntryOnboardAddress = pEntryOnboard->Base;
  pFirmwareEntryImage = AllocateZeroPool (EntryImageSize);
  CopyMem (pFirmwareEntryImage, pNewFirmwareImage + EntryImageOffset, EntryImageSize);
  if (!IfNeedToUpdate (Type, pFirmwareEntryImage, EntryImageSize, EntryOnboardAddress)) {
    FreePool (pFirmwareEntryImage);
    return (ReturnCode | FIRMWARE_RET_ERR_VER_DOWN);
  }

  RetVal = CixFlashWriteWrapper (Type, pFirmwareEntryImage, EntryImageSize, dwflag, EntryOnboardAddress);
  FreePool (pFirmwareEntryImage);

  return (ReturnCode | RetVal);
}

UINT16
CixFirmwareFullUpdatewrapper (
  UINT8   *pNewFirmwareImage,
  UINT32  ImageSize,
  UINT32  dwflag
  )
{
  // EFI_STATUS  Status = EFI_SUCCESS;

  UINT8  RetVal;

  // CHAR8 *pBuff;
  // UINT16  ReturnCode;
  // UINT8   *pFirmwareEntryImage;
  // UINT32  FirmwareEntryImageSize;
  // UINT32  OffsetAddress;

  // ReturnCode = Type<<8;
  // FirmwareEntryImageSize = pEntryUpdate->Length;
  // OffsetAddress = pEntryUpdate->Base;
  // pFirmwareEntryImage = AllocateZeroPool (FirmwareEntryImageSize);
  // CopyMem(pFirmwareEntryImage,pNewFirmwareImage+pEntryUpdate->Base,FirmwareEntryImageSize);
  RetVal = CixFlashWriteWrapper (FIRMWARE_TYPE_FULL_PROGRAM, pNewFirmwareImage, ImageSize, dwflag, 0);
  // FreePool(pFirmwareEntryImage);

  /* //dump
  DEBUG ((DEBUG_ERROR, "header : 0x100000 \n"));
  pBuff = AllocateZeroPool (256);
  //read one sector from onboard flash 0x100000 or 0x200000 ,firmware header is here
  CixFlashReadWrapper(FIRMWARE_HEADER_OFFSET,256,pBuff);

  DEBUG ((DEBUG_ERROR, "bootloader 1 : 0x188000 \n"));
  //read one sector from onboard flash 0x100000 or 0x200000 ,firmware header is here
  CixFlashReadWrapper(0x188000,256,pBuff);

  DEBUG ((DEBUG_ERROR, "bootloader 2 : 0x208000 \n"));
  //read one sector from onboard flash 0x100000 or 0x200000 ,firmware header is here
  CixFlashReadWrapper(0x208000,256,pBuff);
  FreePool(pBuff);
  */

  return (FIRMWARE_TYPE_FULL_PROGRAM<<8 | RetVal);
}

UINT16
CixFirmwarePackageProgram (
  UINT8                      *pNewFirmwareImage,
  UINT32                     ImageSize,
  FIRMWARE_PROGRAM_CALLBACK  CallBackFunc
  // add force flag
  )
{
  // EFI_STATUS        Status = EFI_SUCCESS;
  UINT16                   ReturnCode;
  FIRMWARE_HEADER          *pCixNewImageFwHeader;
  FIRMWARE_HEADER          *pCixOnboardFwHeader;
  FIRMWARE_ENTRY           *pEntryUpdate  = NULL;
  FIRMWARE_ENTRY           *pEntryOnboard = NULL;
  UINT32                   ImageEntryIndex, OboardEntryIndex;
  UINT32                   dwflag;
  FIRMWARE_PROGRAM_STATUS  *pFwProgSts;
  BOOLEAN                  EntryFound = FALSE;

  if ((pNewFirmwareImage == NULL) || (ImageSize == 0)) {
    ReturnCode = FIRMWARE_RET_ERR_INPUT;
    return ReturnCode;
  }

  FimwareUpdateCallBack = CallBackFunc;
  pFwProgSts            = &FwProgStatus;

  ReturnCode = CixFirmwareUpdateInitialize (pNewFirmwareImage, ImageSize, dwflag);
  if (ReturnCode != FIRMWARE_RET_SUCCESS) {
    return ReturnCode;
  }

  if (!IsOtaPackage) {
    // do full flash update
    ReturnCode = CixFirmwareFullUpdatewrapper (pNewFirmwareImage, ImageSize, dwflag);
    return ReturnCode;
  }

  pCixNewImageFwHeader = pFwPrivateData->pNewImageFwHeader;
  pCixOnboardFwHeader  = pFwPrivateData->pOnboardFwHeader;
  for (ImageEntryIndex = 0; ImageEntryIndex < pCixNewImageFwHeader->EntryCount; ImageEntryIndex++) {
    pEntryUpdate = &(pCixNewImageFwHeader->EntryNode[ImageEntryIndex]);
    EntryFound   = FALSE;
    for (OboardEntryIndex = 0; OboardEntryIndex < pCixOnboardFwHeader->EntryCount; OboardEntryIndex++) {
      pEntryOnboard = &(pCixOnboardFwHeader->EntryNode[OboardEntryIndex]);

      if (pEntryUpdate->Type == pEntryOnboard->Type) {
        EntryFound = TRUE;
        if (pEntryUpdate->Base != pEntryOnboard->Base) {
          // do full flash update
          NeedFullPackageUpdate = TRUE;
          DEBUG ((DEBUG_INFO, "[FwU]Layout changed,please update full firmware package.\n"));
          return ((pEntryUpdate->Type<<8) | FIRMWARE_RET_ERR_LAYOUT);
        }

        // flash OTA firmware entry
        ReturnCode = CixFirmwareEntryUpdateWrapper (pEntryUpdate->Type, pNewFirmwareImage, pEntryUpdate, pEntryOnboard);

        if (ReturnCode != (pEntryUpdate->Type | FIRMWARE_RET_SUCCESS)) {
          return ReturnCode;
        }

        break;
      }
    }

    if (!EntryFound) {
      DEBUG ((DEBUG_INFO, "[FwU]type %d not found,please update full firmware package.\n", pEntryUpdate->Type));
      return ((pEntryUpdate->Type<<8)|FIRMWARE_RET_TYPE_NOT_FOUND);
    }
  }

  /*
    if(NeedFullPackageUpdate == TRUE){
      //do full flash update
      ReturnCode = CixFirmwareFullUpdatewrapper(pNewFirmwareImage, ImageSize, dwflag);
    }
  */

  return ReturnCode;
}

UINT16
CixFirmwareRead (
  UINT32  OffsetAddress,
  UINTN   ImageBuffSize,
  UINT8   *pBuff
  )
{
  EFI_STATUS  Status;

  Status = LocateNorFlashDiskIoProtocol ();
  if (EFI_ERROR (Status)) {
    return FIRMWARE_RET_NO_FLASH_PROG_PROTOCOL;
  }

  Status = CixFlashReadWrapper (OffsetAddress, ImageBuffSize, pBuff);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: status %r\n", __FUNCTION__, Status));
    return FIRMWARE_RET_READ_ERR;
  }

  return FIRMWARE_RET_SUCCESS;
}

UINT16
CixFirmwareReadNull (
  UINT32  OffsetAddress,
  UINTN   ImageBuffSize,
  UINT8   *pBuff
  )
{
  return FIRMWARE_RET_FUNC_NOT_FOUND;
}

EFI_STATUS
EFIAPI
CixFlashReadWrapper (
  UINT32  OffsetAddress,
  UINTN   ImageBuffSize,
  VOID    *pImageBuff
  )
{
  EFI_STATUS  Status;

  // CHAR8 *pBuffer;

  // The buffer must be valid
  if (pImageBuff == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Return if we have no byte to read
  if (ImageBuffSize == 0) {
    return EFI_SUCCESS;
  }

  if (NorFlashDiskIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = NorFlashDiskIo->ReadDisk (NorFlashDiskIo, MediaId, OffsetAddress, ImageBuffSize, pImageBuff);
  DebugPrint (DEBUG_INFO, "[FwU] Read NorFlash status : %r\n", Status);

  /*
  {
    UINT32 i;
    pBuffer = (CHAR8*)pImageBuff;
    for(i=0;i<256;i++,pBuffer++){

      DEBUG ((DEBUG_INFO, "%02x ",*pBuffer));
      if ( (i+1)%16 == 0 ){
       DEBUG ((DEBUG_INFO, " \n"));
      }

    }
  } */
  return Status;
}

BOOLEAN
IsNorFlashDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  // EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  VENDOR_DEVICE_PATH        *VendorDevicePath;

  // EFI_HANDLE                Handle;

  TempDevicePath = DevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
    if ((DevicePathType (TempDevicePath) == HARDWARE_DEVICE_PATH) &&
        (DevicePathSubType (TempDevicePath) == HW_VENDOR_DP))
    {
      VendorDevicePath = (VENDOR_DEVICE_PATH *)TempDevicePath;
      if (CompareGuid (&(VendorDevicePath->Guid), &gCixNorFlashDevicePathGuid)) {
        DEBUG ((DEBUG_INFO, "[FwU] gNorFlashDevicePathGuid %g found.\n", &gCixNorFlashDevicePathGuid));
        return TRUE;
      }
    }

    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return FALSE;
}

EFI_STATUS
EFIAPI
LocateNorFlashDiskIoProtocol (
  )
{
  EFI_HANDLE                *DiskIoHandles;
  UINTN                     NumberDiskIoHandles;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_STATUS                Status;
  NOR_FLASH_INSTANCE        *Instance;

  // find DiskIoProtocol
  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiDiskIoProtocolGuid,
                                    NULL,
                                    &NumberDiskIoHandles,
                                    &DiskIoHandles
                                    );

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  // DEBUG ((DEBUG_INFO, "[FwU] NumberDiskIoHandles %d\n", NumberDiskIoHandles));
  for (Index = 0; Index < NumberDiskIoHandles; Index++) {
    DevicePath = DevicePathFromHandle (DiskIoHandles[Index]);
    if (!DevicePath) {
      continue;
    }

    if (IsNorFlashDevicePath (DevicePath)) {
      MediaId = ((NOR_FLASH_DEVICE_PATH *)DevicePath)->Index;
      Status  = gBS->HandleProtocol (DiskIoHandles[Index], &gEfiDiskIoProtocolGuid, (VOID **)&NorFlashDiskIo);
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "[FwU] locate NorFlash DiskIoProtocol success\n"));
        Instance       = INSTANCE_FROM_DISKIO_THIS (NorFlashDiskIo);
        FlashBlockSize = Instance->Media.BlockSize;
        return EFI_SUCCESS;
      }
    }
  }

  if (Index == NumberDiskIoHandles) {
    DEBUG ((DEBUG_INFO, "[FwU]Not found NorFlash device.\n"));
    return EFI_NOT_FOUND;
  }

  return Status;
}

BOOLEAN
CixFirmwareVerify (
  UINT8                    *pImageBuff,
  UINT32                   ImageBuffSize,
  UINT32                   OffsetAddress,
  FIRMWARE_PROGRAM_STATUS  *pFwProgSts
  )
{
  EFI_STATUS  Status;
  UINT8       *pOnboardBuff;

  pOnboardBuff = AllocateZeroPool (ImageBuffSize);
  // read full flash image
  Status = CixFlashReadWrapper (OffsetAddress, ImageBuffSize, pOnboardBuff);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[FwU]%a: status %r\n", __FUNCTION__, Status));
    return FALSE;
  }

  if (CompareMem (pImageBuff, pOnboardBuff, ImageBuffSize)) {
    DebugPrint (DEBUG_ERROR, "[FwU] Verify failed.\n");
    return FALSE;
  }

  DebugPrint (DEBUG_ERROR, "[FwU] Verify success.\n");
  return TRUE;
}

UINT8
CixFlashWriteWrapper (
  UINT8   Type,
  UINT8   *pImageBuff,
  UINT32  ImageBuffSize,
  UINT32  dwflag,
  UINT32  OffsetAddress
  )
{
  EFI_STATUS  Status;
  BOOLEAN     bVerify = FALSE;

  if (NorFlashDiskIo == NULL) {
    return FIRMWARE_RET_NO_FLASH_PROG_PROTOCOL;
  }

  FwProgStatus.firmware_type = Type;
  FwProgStatus.status_result = FIRMWARE_START;
  DoCallBack (&FwProgStatus);
  DEBUG ((DEBUG_INFO, "[FwU] Program entry %d, address 0x%08x\n", Type, OffsetAddress));
  Status = NorFlashDiskIo->WriteDisk (NorFlashDiskIo, MediaId, OffsetAddress, ImageBuffSize, pImageBuff);
  DebugPrint (DEBUG_INFO, "[FwU] Write NorFlash status : %r\n", Status);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: status %r\n", __FUNCTION__, Status));
    FwProgStatus.status_result = FIRMWARE_WRITE_FAILED;
    DoCallBack (&FwProgStatus);
    return FIRMWARE_RET_ERR_PROG;
  }

  FwProgStatus.status_result = FIRMWARE_WRITE_SUCCESS;
  DoCallBack (&FwProgStatus);
  // do verify
  bVerify = CixFirmwareVerify (pImageBuff, ImageBuffSize, OffsetAddress, &FwProgStatus);
  if (!bVerify) {
    FwProgStatus.status_result = FIRMWARE_VERIFY_FAILED;
    DoCallBack (&FwProgStatus);
    return FIRMWARE_RET_ERR_VERIFY;
  }

  FwProgStatus.status_result = FIRMWARE_PROGRAM_SUCCESS;
  DoCallBack (&FwProgStatus);

  return FIRMWARE_RET_SUCCESS;
}

UINT16
CixFirmwareSingleProgram (
  UINT8                      Type,
  UINT8                      *pNewFirmwareImage,
  UINT32                     ImageSize,
  FIRMWARE_PROGRAM_CALLBACK  CallBackFunc
  )
{
  // EFI_STATUS        Status = EFI_SUCCESS;
  UINT16                   ReturnCode;
  FIRMWARE_HEADER          *pCixNewImageFwHeader;
  FIRMWARE_HEADER          *pCixOnboardFwHeader;
  FIRMWARE_ENTRY           *pEntryUpdate  = NULL;
  FIRMWARE_ENTRY           *pEntryOnboard = NULL;
  UINT32                   Index;
  FIRMWARE_PROGRAM_STATUS  *pFwProgSts;
  UINT32                   dwflag;
  BOOLEAN                  EntryFound = FALSE;

  FimwareUpdateCallBack = CallBackFunc;
  pFwProgSts            = &FwProgStatus;

  if ((pNewFirmwareImage == NULL) || (ImageSize == 0) ||
      ((Type != FIRMWARE_TYPE_BootLoader_1) &&
       (Type != FIRMWARE_TYPE_BootLoader_2) &&
       (Type != FIRMWARE_TYPE_MEM_CONF) &&
       (Type != FIRMWARE_TYPE_PM_CONF) &&
       (Type != FIRMWARE_TYPE_BootLoader_3) &&
       (Type != FIRMWARE_TYPE_UEFI_NVRAM)))
  {
    ReturnCode = Type << 8 | FIRMWARE_RET_TYPE_NOT_SUPPORT;
    return ReturnCode;
  }

  ReturnCode = CixFirmwareUpdateInitialize (pNewFirmwareImage, ImageSize, dwflag);
  if (ReturnCode != FIRMWARE_RET_SUCCESS) {
    return (Type << 8 | ReturnCode);
  }

  pCixNewImageFwHeader = pFwPrivateData->pNewImageFwHeader;
  pCixOnboardFwHeader  = pFwPrivateData->pOnboardFwHeader;
  for (Index = 0; Index < pCixNewImageFwHeader->EntryCount; Index++) {
    if (pCixNewImageFwHeader->EntryNode[Index].Type == Type) {
      pEntryUpdate = &(pCixNewImageFwHeader->EntryNode[Index]);
      EntryFound   = TRUE;
      break;
    }
  }

  if (!EntryFound) {
    return (Type<<8|FIRMWARE_RET_TYPE_NOT_FOUND);
  }

  EntryFound = FALSE;

  for (Index = 0; Index < pCixOnboardFwHeader->EntryCount; Index++) {
    if (pCixOnboardFwHeader->EntryNode[Index].Type == Type) {
      pEntryOnboard = &(pCixOnboardFwHeader->EntryNode[Index]);
      EntryFound    = TRUE;
      break;
    }
  }

  if (!EntryFound) {
    return (Type<<8|FIRMWARE_RET_ERR_LAYOUT);
  }

  if (pEntryUpdate->Base != pEntryOnboard->Base) {
    DEBUG ((DEBUG_ERROR, "[FwU]layout changed ,please update full firmware package. \n"));
    ReturnCode = Type << 8 | FIRMWARE_RET_ERR_LAYOUT;
    return ReturnCode;
  }

  pFwPrivateData->ImageBlockNum = pEntryUpdate->Length / FlashBlockSize; // it single firmware,update ImageBlockNum to single firmware block size

  ReturnCode = CixFirmwareEntryUpdateWrapper (Type, pNewFirmwareImage, pEntryUpdate, pEntryOnboard);

  /*
    FirmwareEntryImageSize = pEntryUpdate->Length;
    OffsetAddress = pEntryUpdate->Base;
    pFirmwareEntryImage = AllocateZeroPool (FirmwareEntryImageSize);
    CopyMem(pFirmwareEntryImage,pNewFirmwareImage+pEntryUpdate->Base,FirmwareEntryImageSize);
    Status = CixFlashWriteWrapper(Type, pFirmwareEntryImage, FirmwareEntryImageSize, dwflag, OffsetAddress);
    FreePool(pFirmwareEntryImage);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "%a: status %r\n", __FUNCTION__, Status));
      ReturnCode = Type<<8 | FIRMWARE_RET_ERR_PROG;
      return ReturnCode;
    }
  */
  return ReturnCode;
}

// Type      : 5
// ImageSize : 4KB
// UpateState: 1 - protram ; 0 - erase
UINT16
CixFirmwareSecureDebugUpdate (
  UINT8                      Type,
  UINT8                      *pNewFirmwareImage,
  UINT32                     ImageSize,
  UINT8                      UpateState,
  FIRMWARE_PROGRAM_CALLBACK  CallBackFunc
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      dwflag = 0;
  UINT8       RetVal;
  UINT8       *pImageBuffer;
  UINT16      ReturnCode;

  if ((Type != FIRMWARE_TYPE_SECURE_DEBUG) || (ImageSize != SECURE_DEBUG_SIZE)) {
    ReturnCode = Type << 8 | FIRMWARE_RET_ERR_INPUT;
    return ReturnCode;
  }

  Status = LocateNorFlashDiskIoProtocol ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "No nor flash program protocol found.\n"));
    return Status;
  }

  if (UpateState) {
    RetVal = CixFlashWriteWrapper (FIRMWARE_TYPE_SECURE_DEBUG, pNewFirmwareImage, SECURE_DEBUG_SIZE, dwflag, SECURE_DEBUG_OFFSET);
  } else {
    pImageBuffer = AllocateZeroPool (SECURE_DEBUG_SIZE);
    memset (pImageBuffer, 0xff, SECURE_DEBUG_SIZE);
    RetVal = CixFlashWriteWrapper (FIRMWARE_TYPE_SECURE_DEBUG, pImageBuffer, SECURE_DEBUG_SIZE, dwflag, SECURE_DEBUG_OFFSET);
    FreePool (pImageBuffer);
  }

  if (FIRMWARE_RET_SUCCESS != RetVal) {
    DEBUG ((DEBUG_INFO, "secure debug update failed\n"));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "secure debug update success\n"));

  return Status;
}

UINT16
CixFirmwareRawEntryUpdateNull (
  UINT8                      Type,
  UINT8                      *pEntryImage,
  UINT32                     ImageSize,
  ENTRY_UPDATE_METHOD        UpateState,
  FIRMWARE_PROGRAM_CALLBACK  CallBackFunc
  )
{
  return FIRMWARE_RET_FUNC_NOT_FOUND;
}

// Type      : 3/4/5
// ImageSize : entry size
// UpateState: 1 - protram ; 0 - erase ; 2 - read
UINT16
CixFirmwareRawEntryUpdate (
  UINT8                      Type,
  UINT8                      *pEntryImage,
  UINT32                     ImageSize,
  ENTRY_UPDATE_METHOD        UpateState,
  FIRMWARE_PROGRAM_CALLBACK  CallBackFunc
  )
{
  EFI_STATUS               Status = EFI_SUCCESS;
  UINT32                   dwflag = 0;
  UINT8                    RetVal;
  UINT8                    *pImageBuffer;
  UINT16                   ReturnCode;
  UINT32                   Index;
  CIX_FIRMWARE_ENTRY_INFO  *pEntryInfo;
  UINT32                   EntryOnboardSize;
  UINT32                   EntryOnboardAddress;

  DEBUG ((DEBUG_ERROR, "type:%d,ImageSize:0x%x\n", Type, ImageSize));

  /*
  {
    UINT32 i;
    pBuffer = (CHAR8*)pImageBuff;
    for(i=0;i<256;i++,pBuffer++){

      DEBUG ((DEBUG_INFO, "%02x ",*pBuffer));
      if ( (i+1)%16 == 0 ){
       DEBUG ((DEBUG_INFO, " \n"));
      }

    }
  } */

  if (((Type != FIRMWARE_TYPE_SECURE_DEBUG) && (Type != FIRMWARE_TYPE_MEM_CONF) && (Type != FIRMWARE_TYPE_PM_CONF)) || (ImageSize != SIZE_4KB)) {
    ReturnCode = Type << 8 | FIRMWARE_RET_ERR_INPUT;
    return ReturnCode;
  }

  ReturnCode = CixFirmwareUpdateOnboardHeaderParse ();
  if (ReturnCode != FIRMWARE_RET_SUCCESS) {
    return (Type << 8 | ReturnCode);
  }

  ReturnCode = Type << 8;
  pEntryInfo = pFwPrivateData->ImageFwEntryInfo;
  for (Index = 0; Index < pFwPrivateData->EntryCount; Index++) {
    if (Type == pEntryInfo->Type) {
      EntryOnboardAddress = pEntryInfo->EntryImageOffset;
      EntryOnboardSize    = pEntryInfo->EntryImageSize;
      // Found            = TRUE;
      break;
    }

    pEntryInfo++;
  }

  switch (UpateState) {
    case ENTRY_ERASE:
      pImageBuffer = AllocateZeroPool (SIZE_4KB);
      memset (pImageBuffer, 0xff, SIZE_4KB);
      RetVal = CixFlashWriteWrapper (Type, pImageBuffer, SIZE_4KB, dwflag, EntryOnboardAddress);
      FreePool (pImageBuffer);
      break;
    case ENTRY_WRITE:
      RetVal = CixFlashWriteWrapper (Type, pEntryImage, SIZE_4KB, dwflag, EntryOnboardAddress);
      break;
    case ENTRY_READ:
      Status = CixFlashReadWrapper (EntryOnboardAddress, ImageSize, pEntryImage);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: status %r\n", __FUNCTION__, Status));
        return FIRMWARE_RET_READ_ERR;
      }

      break;
    default:
      ReturnCode = Type << 8 | FIRMWARE_RET_ERR_INPUT;
      return ReturnCode;
  }

  ReturnCode |= RetVal;
  if (FIRMWARE_RET_SUCCESS != RetVal) {
    DEBUG ((DEBUG_INFO, "[FwU] Raw entry update failed\n"));

    return ReturnCode;
  }

  DEBUG ((DEBUG_INFO, "[FwU] Raw entry update success\n"));

  return ReturnCode;
}

UINT32
ReadFwVersionOnboard (
  UINT8  Type
  )
{
  FIRMWARE_HEADER  *pCixOnboardFwHeader;
  FIRMWARE_ENTRY   *pEntryOnboard;
  UINT32           Index, FwVersion = 0xFFFFFFFF;
  BOOLEAN          EntryFound = FALSE;

  VOID  *pBuff;

  pCixOnboardFwHeader = pFwPrivateData->pOnboardFwHeader;

  for (Index = 0; Index < pCixOnboardFwHeader->EntryCount; Index++) {
    if (pCixOnboardFwHeader->EntryNode[Index].Type == Type) {
      pEntryOnboard = &(pCixOnboardFwHeader->EntryNode[Index]);
      EntryFound    = TRUE;
      break;
    }
  }

  if (!EntryFound) {
    return FwVersion;
  }

  pBuff = AllocateZeroPool (FlashBlockSize);

  CixFlashReadWrapper (pEntryOnboard->Base, FlashBlockSize, pBuff);
  switch (Type) {
    case FIRMWARE_TYPE_BootLoader_1:
      FwVersion = ((CBFF_IMAGE_HEADER *)pBuff)->Cbff_version;
      break;
    case FIRMWARE_TYPE_BootLoader_2:
    case FIRMWARE_TYPE_BootLoader_3:
      FwVersion = ((FIP_TOC_HEADER *)pBuff)->serial_number;
      break;
    case FIRMWARE_TYPE_MEM_CONF:
    case FIRMWARE_TYPE_PM_CONF:
    default:
      break;
  }

  DEBUG ((DEBUG_INFO, "[FwU]FwType:%d FwVersion:0x%08x\n", Type, FwVersion));
  return FwVersion;
}

UINT16
CixFirmwareGetVersion (
  UINT32  *HeaderOffset,
  UINT32  *Bootloader1Ver,
  UINT32  *Bootloader2Ver,
  UINT32  *Bootloader3Ver
  )
{
  UINT16  ReturnCode;

  if ((Bootloader1Ver == NULL) || (Bootloader2Ver == NULL) ||(Bootloader3Ver == NULL)|| (HeaderOffset == NULL)) {
    ReturnCode = FIRMWARE_RET_ERR_INPUT;
    return ReturnCode;
  }

  ReturnCode = CixFirmwareUpdateOnboardHeaderParse ();
  if (ReturnCode != FIRMWARE_RET_SUCCESS) {
    return ReturnCode;
  }

  *HeaderOffset   = pFwPrivateData->HeaderOffset;
  *Bootloader1Ver = ReadFwVersionOnboard (FIRMWARE_TYPE_BootLoader_1);
  *Bootloader2Ver = ReadFwVersionOnboard (FIRMWARE_TYPE_BootLoader_2);
  *Bootloader3Ver = ReadFwVersionOnboard (FIRMWARE_TYPE_BootLoader_3);
  return FIRMWARE_RET_SUCCESS;
}

CIX_FW_UPDATE_PROTOCOL  CixFwUpdateFuncList = {
  FW_UPDATE_PROTOCOL_VERSION,
  CixFirmwarePackageProgram,
  CixFirmwareSingleProgram,
  CixFirmwareReadNull,
  CixFirmwareRawEntryUpdate,
  CixFirmwareGetVersion
};

EFI_STATUS
EFIAPI
CixFirmwareUpdateDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "[FwU]%a start.\n", __FUNCTION__));

 #ifdef DEBUG_MODE
  CixFwUpdateFuncList.FirmwareRead           =  CixFirmwareRead;
  //CixFwUpdateFuncList.FirmwareRawEntryUpdate = CixFirmwareRawEntryUpdate;
 #endif

  Status = gBS->InstallProtocolInterface (
                                          &ImageHandle,
                                          &gCixFirmwareUpdateProtocolGuid,
                                          EFI_NATIVE_INTERFACE,
                                          &CixFwUpdateFuncList
                                          );
  if (EFI_ERROR (Status)) {
    DEBUG (
           (DEBUG_ERROR,
            "[FwU]%a: failed to install FirmwareUpdate protocol (Status == %r)\n",
            __FUNCTION__, Status)
           );
    return Status;
  }

  // update ec firmware
  CixFirmwareEcProgram ();

  return EFI_SUCCESS;
}
