/** @PartitionTable.c
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/
#include "AutoGen.h"
#include <Library/BootLinux.h>
#include <Library/Partition.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Uefi.h>
#include <Uefi/UefiSpec.h>
#include <VerifiedBoot.h>

/* Volume Label size 11 chars, round off to 16 */
#define VOLUME_LABEL_SIZE  16

/* List of all the filters that need device path protocol in the handle to
 * filter */
#define FILTERS_NEEDING_DEVICEPATH                                             \
  (BLK_IO_SEL_PARTITIONED_MBR | BLK_IO_SEL_PARTITIONED_GPT |                   \
   BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID | BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY | \
   BLK_IO_SEL_MATCH_ROOT_DEVICE)

/* FileInfo-size = SIZE_OF_EFI_FILE_INFO + sizeof(name of directory entry)
   Since we don't know the sizeof(name of directory entry),
   we can set FileInfo-size = SIZE_OF_EFI_FILE_INFO + 256*/
#define FILE_INFO_SIZE  (SIZE_OF_EFI_FILE_INFO + 256)

struct StoragePartInfo  gPtable;
EFI_PARTITION_ENTRY     gPtnEntries[MAX_NUM_PARTITIONS];
STATIC UINT32           gPartitionCount;

#ifdef ANDROID_BOOT
BackdoorInfo  gBackdoorPart[] = {
  { L"boot",        (void *)((0x2fff80LL<<5)+0x80000000),  SIZE_64MB      },
  { L"vendor_boot", (void *)((0x50ff80LL<<5)+0x80000000),  SIZE_64MB      },
  { L"dtbo",        (void *)((0x2000000LL<<5)+0x80000000), SIZE_4MB       },
  { L"vbmeta",      (void *)((0x2400000LL<<5)+0x80000000), SIZE_1MB       },
  { L"misc",        (void *)((0x2800000LL<<5)+0x80000000), SIZE_4MB       },
  { L"s2_a",        (void *)((0x2c00000LL<<5)+0x80000000), SIZE_4MB       },
  { L"super",       (void *)((0x6000000LL<<5)+0x80000000), 2UL * SIZE_1GB },
  { L"data",        (void *)((0xa000000LL<<5)+0x80000000), 2UL * SIZE_1GB },
  { L"backdir",     (void *)((0xe000000LL<<5)+0x80000000), SIZE_512MB     }
};
#else
BackdoorInfo  gBackdoorPart[] = {
  { L"boot",   (void *)((0x1000000<<5)+0x80000000), SIZE_64MB + 26 * SIZE_1MB },
  { L"rootfs", (void *)((0x510000<<5)+0x80000000),  8UL * SIZE_1GB            },
};
#endif

/* Returns 0 if the volume label matches otherwise non zero */
STATIC UINTN
CompareVolumeLabel (
  IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Fs,
  IN CHAR8                            *ReqVolumeName
  )
{
  INT32                 CmpResult;
  UINT32                j;
  UINT16                VolumeLabel[VOLUME_LABEL_SIZE];
  EFI_FILE_PROTOCOL     *FsVolume = NULL;
  EFI_STATUS            Status;
  UINTN                 Size;
  EFI_FILE_SYSTEM_INFO  *FsInfo;

  // Get information about the volume
  Status = Fs->OpenVolume (Fs, &FsVolume);

  if (Status != EFI_SUCCESS) {
    return 1;
  }

  /* Get the Volume name */
  Size   = 0;
  FsInfo = NULL;
  Status = FsVolume->GetInfo (FsVolume, &gEfiFileSystemInfoGuid, &Size, FsInfo);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FsInfo = AllocateZeroPool (Size);
    Status = FsVolume->GetInfo (FsVolume, &gEfiFileSystemInfoGuid, &Size, FsInfo);
    if (Status != EFI_SUCCESS) {
      FreePool (FsInfo);
      return 1;
    }
  }

  if (FsInfo == NULL) {
    return 1;
  }

  /* Convert the passed in Volume name to Wide char and upper case */
  for (j = 0; (j < VOLUME_LABEL_SIZE - 1) && ReqVolumeName[j]; ++j) {
    VolumeLabel[j] = ReqVolumeName[j];

    if ((VolumeLabel[j] >= 'a') &&
        (VolumeLabel[j] <= 'z'))
    {
      VolumeLabel[j] -= ('a' - 'A');
    }
  }

  /* Null termination */
  VolumeLabel[j] = 0;

  /* Change any lower chars in volume name to upper
   * (ideally this is not needed) */
  for (j = 0; (j < VOLUME_LABEL_SIZE - 1) && FsInfo->VolumeLabel[j]; ++j) {
    if ((FsInfo->VolumeLabel[j] >= 'a') &&
        (FsInfo->VolumeLabel[j] <= 'z'))
    {
      FsInfo->VolumeLabel[j] -= ('a' - 'A');
    }
  }

  CmpResult = StrnCmp (FsInfo->VolumeLabel, VolumeLabel, VOLUME_LABEL_SIZE);

  FreePool (FsInfo);
  FsVolume->Close (FsVolume);

  return CmpResult;
}

/**
  Returns a list of BlkIo handles based on required criteria
SelectionAttrib : Bitmask representing the conditions that need
to be met for the handles returned. Based on the
selections filter members should have valid values.
FilterData      : Instance of Partition Select Filter structure that
needs extended data for certain type flags. For example
Partition type and/or Volume name can be specified.
HandleInfoPtr   : Pointer to array of HandleInfo structures in which the
output is returned.
MaxBlkIopCnt    : On input, max number of handle structures the buffer can hold,
On output, the number of handle structures returned.

@retval EFI_SUCCESS if the operation was successful
*/
EFI_STATUS EFIAPI
GetBlkIOHandles (
  IN UINT32             SelectionAttrib,
  IN PartiSelectFilter  *FilterData,
  OUT HandleInfo        *HandleInfoPtr,
  IN OUT UINT32         *MaxBlkIopCnt
  )
{
  EFI_BLOCK_IO_PROTOCOL             *BlkIo;
  EFI_HANDLE                        *BlkIoHandles;
  UINTN                             BlkIoHandleCount;
  UINTN                             i;
  UINTN                             DevicePathDepth;
  HARDDRIVE_DEVICE_PATH             *Partition, *PartitionOut;
  EFI_STATUS                        Status;
  EFI_DEVICE_PATH_PROTOCOL          *DevPathInst;
  EFI_DEVICE_PATH_PROTOCOL          *TempDevicePath;
  CONTROLLER_DEVICE_PATH            *RootDevicePath;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs;
  UINT32                            BlkIoCnt = 0;
  EFI_PARTITION_INFO_PROTOCOL       *PartitionInfo;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL  *DevTextPath;
  CHAR16                            *TextDevPath;

  if ((MaxBlkIopCnt == NULL) || (HandleInfoPtr == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  /* Adjust some defaults first */
  if ((SelectionAttrib & (BLK_IO_SEL_MEDIA_TYPE_REMOVABLE |
                          BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE)) == 0)
  {
    SelectionAttrib |=
      (BLK_IO_SEL_MEDIA_TYPE_REMOVABLE | BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE);
  }

  if (((BLK_IO_SEL_PARTITIONED_GPT | BLK_IO_SEL_PARTITIONED_MBR) &
       SelectionAttrib) == 0)
  {
    SelectionAttrib |=
      (BLK_IO_SEL_PARTITIONED_GPT | BLK_IO_SEL_PARTITIONED_MBR);
  }

  /* If we need Filesystem handle then search based on that its narrower search
   * than BlkIo */
  if (SelectionAttrib & (BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM |
                         BLK_IO_SEL_SELECT_BY_VOLUME_NAME))
  {
    Status =
      gBS->LocateHandleBuffer (
             ByProtocol,
             &gEfiSimpleFileSystemProtocolGuid,
             NULL,
             &BlkIoHandleCount,
             &BlkIoHandles
             );
  } else {
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiBlockIoProtocolGuid,
                    NULL,
                    &BlkIoHandleCount,
                    &BlkIoHandles
                    );
  }

  if (Status != EFI_SUCCESS) {
    DEBUG (
      (EFI_D_ERROR, "Unable to get Filesystem Handle buffer %u\n", Status));
    return Status;
  }

  DEBUG ((EFI_D_VERBOSE, "GetBlkIOHandles get  BlkIoHandleCount %d\n", BlkIoHandleCount));
  /* Loop through to search for the ones we are interested in. */
  for (i = 0; i < BlkIoHandleCount; i++) {
    Status = gBS->HandleProtocol (
                    BlkIoHandles[i],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **)&BlkIo
                    );
    /* Fv volumes will not support Blk I/O protocol */
    if (Status == EFI_UNSUPPORTED) {
      continue;
    }

    if (Status != EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "Unable to get Filesystem Handle %u\n", Status));
      return Status;
    }

    /* Check if the media type criteria (for removable/not) satisfies */
    if (BlkIo->Media->RemovableMedia) {
      if ((SelectionAttrib & BLK_IO_SEL_MEDIA_TYPE_REMOVABLE) == 0) {
        DEBUG ((EFI_D_INFO, "Block media type mismatch with removable\n"));
        continue;
      }
    } else {
      if ((SelectionAttrib & BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE) == 0) {
        DEBUG ((EFI_D_INFO, "Block media type mismatch with non removable\n"));
        continue;
      }
    }

    /* Clear the pointer, we can get it if the filter is set */
    PartitionOut = NULL;

    /* Check if partition related criteria satisfies */
    if ((SelectionAttrib & FILTERS_NEEDING_DEVICEPATH) != 0) {
      Status = gBS->HandleProtocol (
                      BlkIoHandles[i],
                      &gEfiDevicePathProtocolGuid,
                      (VOID **)&DevPathInst
                      );

      /* If we didn't get the DevicePath Protocol then this handle
       * cannot be used */
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Unable to get DevPath %u\n", Status));
        continue;
      }

      DevicePathDepth = 0;

      /* Get the device path */
      TempDevicePath = DevPathInst;
      RootDevicePath = (CONTROLLER_DEVICE_PATH *)DevPathInst;
      Partition      = (HARDDRIVE_DEVICE_PATH *)TempDevicePath;
      Status         = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (void **)&DevTextPath);
      TextDevPath    = DevTextPath->ConvertDevicePathToText (DevPathInst, TRUE, TRUE);
      DEBUG ((
        EFI_D_INFO,
        "Get Device Path = %s type = %d subtype = %d  ControllerNumber = %d\n",
        TextDevPath,
        DevPathInst->Type,
        DevPathInst->SubType,
        RootDevicePath->ControllerNumber
        ));
 #ifdef FASTBOOT_USB
      if (StrStr (TextDevPath, L"USB") == NULL) {
        continue;
      }

 #else
      // For NVME
      if (StrStr (TextDevPath, L"Pci") == NULL) {
        continue;
      }

 #endif
      if ((SelectionAttrib & (BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY |
                              BLK_IO_SEL_MATCH_ROOT_DEVICE)) != 0)
      {
        if (!FilterData) {
          return EFI_INVALID_PARAMETER;
        }

        /* If this is not the root device that we are looking for, ignore this
         * handle */
      }

      /* Locate the last Device Path Node */
      while (!IsDevicePathEnd (TempDevicePath)) {
        DevicePathDepth++;
        Partition      = (HARDDRIVE_DEVICE_PATH *)TempDevicePath;
        TempDevicePath = NextDevicePathNode (TempDevicePath);
      }

      /* If we need the handle for root device only and if this is representing
       * a sub partition in the root device then ignore this handle */
      if (SelectionAttrib & BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY) {
 #ifdef FASTBOOT_USB
        // For USB, DevicePathDepth of USB Root Device is 2
        if ((DevicePathDepth != 2) && (DevicePathDepth != 1)) {
          continue;
        }

 #else
        // For NVME, DevicePathDepth of NVME Root Device is 4
        if ((DevicePathDepth != 4) && (DevicePathDepth != 1)) {
          continue;
        }

 #endif
      }

      /* Check if the last node is Harddrive Device path that contains the
       * Partition information */
      if ((Partition->Header.Type == MEDIA_DEVICE_PATH) &&
          (Partition->Header.SubType == MEDIA_HARDDRIVE_DP) &&
          ((Partition->Header.Length[0] | (Partition->Header.Length[1] << 8)) ==
           sizeof (*Partition)))
      {
        PartitionOut = Partition;

        if ((SelectionAttrib & BLK_IO_SEL_PARTITIONED_GPT) == 0) {
          if (Partition->MBRType == PARTITIONED_TYPE_GPT) {
            DEBUG ((EFI_D_INFO, "Partition type mismatch with gpt\n"));
            continue;
          }
        }

        if ((SelectionAttrib & BLK_IO_SEL_PARTITIONED_MBR) == 0) {
          if (Partition->MBRType == PARTITIONED_TYPE_MBR) {
            DEBUG ((EFI_D_INFO, "Partition type mismatch with mbr\n"));
            continue;
          }
        }

        /* PartitionDxe implementation should return partition type also */
        if ((SelectionAttrib & BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID) != 0) {
          GUID  *PartiType;
          VOID  *Interface;

          if (!FilterData ||
              (FilterData->PartitionType == NULL))
          {
            return EFI_INVALID_PARAMETER;
          }

          Status = gBS->HandleProtocol (
                          BlkIoHandles[i],
                          FilterData->PartitionType,
                          (VOID **)&Interface
                          );
          if (EFI_ERROR (Status)) {
            Status = gBS->HandleProtocol (
                            BlkIoHandles[i],
                            &gEfiPartitionTypeGuid,
                            (VOID **)&PartiType
                            );
            if (EFI_ERROR (Status)) {
              DEBUG ((EFI_D_ERROR, "Unable to handle Efi Partition Type %u\n", Status));
              continue;
            }

            if (CompareGuid (PartiType, FilterData->PartitionType) == FALSE) {
              DEBUG ((EFI_D_ERROR, "Partition type guid not match %u\n", Status));
              continue;
            }
          }
        }
      }

      /* If we wanted a particular partition and didn't get the HDD DP,
         then this handle is probably not the interested ones */
      else if ((SelectionAttrib & BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID) != 0) {
        continue;
      }
    }

    /* Check if the Filesystem related criteria satisfies */
    if ((SelectionAttrib & BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM) != 0) {
      Status = gBS->HandleProtocol (
                      BlkIoHandles[i],
                      &gEfiSimpleFileSystemProtocolGuid,
                      (VOID **)&Fs
                      );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Unable to handle Efi Simple File System Protocol %u\n", Status));
        continue;
      }

      if ((SelectionAttrib & BLK_IO_SEL_SELECT_BY_VOLUME_NAME) != 0) {
        if (!FilterData ||
            (FilterData->VolumeName == NULL))
        {
          return EFI_INVALID_PARAMETER;
        }

        if (CompareVolumeLabel (Fs, FilterData->VolumeName) != 0) {
          DEBUG ((EFI_D_ERROR, "Volume label not match\n"));
          continue;
        }
      }
    }

    /* Check if the Partition name related criteria satisfies */
    if ((SelectionAttrib & BLK_IO_SEL_MATCH_PARTITION_LABEL) != 0) {
      Status = gBS->HandleProtocol (
                      BlkIoHandles[i],
                      &gEfiPartitionInfoProtocolGuid,
                      (VOID **)&PartitionInfo
                      );
      if (Status != EFI_SUCCESS) {
        DEBUG ((EFI_D_ERROR, "Unable to handle Efi Partition Record Protocol %u\n", Status));
        continue;
      }

      DEBUG ((EFI_D_VERBOSE, "GetBlkIOHandles get PartitionName: %s Filter PartitionLabel: %s\n", PartitionInfo->Info.Gpt.PartitionName, FilterData->PartitionLabel));
      if (StrnCmp (
            PartitionInfo->Info.Gpt.PartitionName,
            FilterData->PartitionLabel,
            MAX (
              StrLen (PartitionInfo->Info.Gpt.PartitionName),
              StrLen (FilterData->PartitionLabel)
              )
            ))
      {
        continue;
      }

      DEBUG ((EFI_D_INFO, "GetBlkIOHandles read partition: %s success\n", PartitionInfo->Info.Gpt.PartitionName));
    }

    /* We came here means, this handle satisfies all the conditions needed,
     * Add it into the list */
    HandleInfoPtr[BlkIoCnt].Handle        = BlkIoHandles[i];
    HandleInfoPtr[BlkIoCnt].BlkIo         = BlkIo;
    HandleInfoPtr[BlkIoCnt].PartitionInfo = PartitionOut;
    BlkIoCnt++;
    if (BlkIoCnt >= *MaxBlkIopCnt) {
      break;
    }
  }

  *MaxBlkIopCnt = BlkIoCnt;

  /* Free the handle buffer */
  if (BlkIoHandles != NULL) {
    FreePool (BlkIoHandles);
    BlkIoHandles = NULL;
  }

  return EFI_SUCCESS;
}

VOID
ToLower (
  CHAR8  *Str
  )
{
  UINT32  Index;

  for (Index = 0; Str[Index] != '\0'; Index++) {
    if ((Str[Index] >= 'A') && (Str[Index] <= 'Z')) {
      Str[Index] += ('a' - 'A');
    }
  }
}

EFI_STATUS
EnumeratePartitions (
  VOID
  )
{
  EFI_STATUS         Status;
  PartiSelectFilter  HandleFilter;
  UINT32             Attribs = 0;

  if (IsPartitionBackDoor ()) {
    return EFI_SUCCESS;
  }

  gBS->SetMem ((VOID *)&gPtable, (sizeof (struct StoragePartInfo)), 0);
  Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;

  gPtable.MaxHandles          = ARRAY_SIZE (gPtable.HandleInfoList);
  HandleFilter.PartitionType  = NULL;
  HandleFilter.VolumeName     = NULL;
  HandleFilter.RootDeviceType = NULL;

  Status = GetBlkIOHandles (Attribs, &HandleFilter, &gPtable.HandleInfoList[0], &gPtable.MaxHandles);
  if ((Status != EFI_SUCCESS) || (gPtable.MaxHandles == 0)) {
    DEBUG ((EFI_D_ERROR, "%s: GetBlkIOHandles failed: %u\n", __func__, Status));
    return Status;
  }

  return Status;
}

VOID
UpdatePartitionEntries (
  VOID
  )
{
  UINT32                       i;
  UINT32                       Index = 0;
  EFI_STATUS                   Status;
  EFI_PARTITION_INFO_PROTOCOL  *PartitionInfo;

  gPartitionCount = 0;
  gBS->SetMem ((VOID *)gPtnEntries, (sizeof (gPtnEntries[0]) * MAX_NUM_PARTITIONS), 0);

  if (IsPartitionBackDoor ()) {
    for (i = 0; (i < sizeof (gBackdoorPart) / sizeof (BackdoorInfo)) && (Index < MAX_NUM_PARTITIONS); i++, Index++) {
      gPartitionCount++;
      gBS->CopyMem (&gPtnEntries[Index].PartitionName, gBackdoorPart[i].PartitionName, MAX_GET_VAR_NAME_SIZE);
    }

    return;
  }

  for (i = 0; (i < gPtable.MaxHandles) && (Index < MAX_NUM_PARTITIONS); i++, Index++) {
    Status = gBS->HandleProtocol (
                    gPtable.HandleInfoList[i].Handle,
                    &gEfiPartitionInfoProtocolGuid,
                    (VOID **)&PartitionInfo
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, "Selected handle: %d does not have partition record, ignore\n", i));
      continue;
    }

    gPartitionCount++;

    gBS->CopyMem ((&gPtnEntries[Index]), &PartitionInfo->Info.Gpt, sizeof (EFI_PARTITION_ENTRY));
  }

  DEBUG ((EFI_D_ERROR, "UpdatePartitionEntries get gPartitionCount = %d\n", gPartitionCount));
}

BOOLEAN
PartitionHasMultiSlot (
  CONST CHAR16  *Pname
  )
{
  UINT32  i;
  UINT32  SlotCount = 0;
  UINT32  Len       = StrLen (Pname);

  for (i = 0; i < gPartitionCount; i++) {
    if (!(StrnCmp (gPtnEntries[i].PartitionName, Pname, Len))) {
      if ((gPtnEntries[i].PartitionName[Len] == L'_') &&
          ((gPtnEntries[i].PartitionName[Len + 1] == L'a') ||
           (gPtnEntries[i].PartitionName[Len + 1] == L'b')))
      {
        if (++SlotCount > 1) {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

VOID
GetPartitionCount (
  UINT32  *Val
  )
{
  *Val = gPartitionCount;
  return;
}

UINT64
GetPartitionSize (
  EFI_BLOCK_IO_PROTOCOL  *BlockIo
  )
{
  UINT64  PartitionSize;

  if (!BlockIo) {
    DEBUG ((EFI_D_ERROR, "Invalid parameter, pleae check BlockIo info!!!\n"));
    return 0;
  }

  if (IsPartitionBackDoor ()) {
    BackdoorInfo  *Info = (BackdoorInfo *)BlockIo;
    return Info->PartitionSize;
  }

  if (CHECK_ADD64 (BlockIo->Media->LastBlock, 1)) {
    DEBUG ((EFI_D_ERROR, "Integer overflow while adding LastBlock and 1\n"));
    return 0;
  }

  if ((MAX_UINT64 / (BlockIo->Media->LastBlock + 1)) <
      (UINT64)BlockIo->Media->BlockSize)
  {
    DEBUG ((EFI_D_ERROR, "Integer overflow while multiplying LastBlock and BlockSize\n"));
    return 0;
  }

  PartitionSize = (BlockIo->Media->LastBlock + 1) * BlockIo->Media->BlockSize;
  return PartitionSize;
}

EFI_STATUS
PartitionGetInfo (
  IN CHAR16                  *PartitionName,
  OUT EFI_BLOCK_IO_PROTOCOL  **BlockIo,
  OUT EFI_HANDLE             **Handle
  )
{
  EFI_STATUS                   Status;
  EFI_PARTITION_INFO_PROTOCOL  *PartitionInfo;
  UINT16                       i;

  if (IsPartitionBackDoor ()) {
    for (i = 0; i < sizeof (gBackdoorPart)/sizeof (BackdoorInfo); i++) {
      if (!(StrCmp (PartitionName, gBackdoorPart[i].PartitionName))) {
        *BlockIo = (EFI_BLOCK_IO_PROTOCOL *)&gBackdoorPart[i];
        *Handle  = (EFI_HANDLE *)&gBackdoorPart[i];
        return EFI_SUCCESS;
      }
    }

    DEBUG ((EFI_D_ERROR, "Partition not found : %s\n", PartitionName));
    return EFI_NOT_FOUND;
  }

  for (i = 0; i < gPtable.MaxHandles; i++) {
    Status = gBS->HandleProtocol (
                    gPtable.HandleInfoList[i].Handle,
                    &gEfiPartitionInfoProtocolGuid,
                    (VOID **)&PartitionInfo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    if (!(StrCmp (PartitionName, PartitionInfo->Info.Gpt.PartitionName))) {
      *BlockIo = gPtable.HandleInfoList[i].BlkIo;
      *Handle  = gPtable.HandleInfoList[i].Handle;
      return Status;
    }
  }

  DEBUG ((EFI_D_ERROR, "Partition not found : %s\n", PartitionName));
  return EFI_NOT_FOUND;
}

EFI_STATUS
GetStorageHandle (
  HandleInfo  *BlockIoHandle,
  UINT32      *MaxHandles
  )
{
  EFI_STATUS         Status  = EFI_INVALID_PARAMETER;
  UINT32             Attribs = 0;
  PartiSelectFilter  HandleFilter;

  Attribs                    |= BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY;
  HandleFilter.PartitionType  = NULL;
  HandleFilter.VolumeName     = NULL;
  HandleFilter.RootDeviceType = NULL;
  Status                      = GetBlkIOHandles (Attribs, &HandleFilter, BlockIoHandle, MaxHandles);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Error getting block IO handle for Emmc\n"));
    return Status;
  }

  return Status;
}

VOID
PartitionDump (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_PARTITION_INFO_PROTOCOL  *PartitionInfo;
  UINT16                       i;

  for (i = 0; i < gPtable.MaxHandles; i++) {
    Status = gBS->HandleProtocol (
                    gPtable.HandleInfoList[i].Handle,
                    &gEfiPartitionInfoProtocolGuid,
                    (VOID **)&PartitionInfo
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Error getting partition record for Handle: %d : %u\n", i, Status));
      continue;
    }

    DEBUG ((
      EFI_D_INFO,
      "Name:[%s] StartLba: %u EndLba:%u\n",
      PartitionInfo->Info.Gpt.PartitionName,
      PartitionInfo->Info.Gpt.StartingLBA,
      PartitionInfo->Info.Gpt.EndingLBA
      ));
  }
}

#define MAX_HANDLEINF_LST_SIZE  128
EFI_STATUS
WriteGpt (
  UINT8   *Buffer,
  UINT32  Size
  )
{
  EFI_STATUS             Status;
  EFI_BLOCK_IO_PROTOCOL  *BlockIo = NULL;
  HandleInfo             BlockIoHandle[MAX_HANDLEINF_LST_SIZE];
  UINT32                 MaxHandles = MAX_HANDLEINF_LST_SIZE;
  EFI_HANDLE             *Handle    = NULL;

  if (IsPartitionBackDoor ()) {
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "Attemping to update partition table\n"));
  DEBUG ((
    EFI_D_INFO,
    "*************** Current partition Table Dump Start "
    "*******************\n"
    ));
  PartitionDump ();
  DEBUG ((
    EFI_D_INFO,
    "*************** Current partition Table Dump End "
    "*******************\n"
    ));
  Status = GetStorageHandle (BlockIoHandle, &MaxHandles);
  if (Status || (MaxHandles != 1)) {
    DEBUG ((EFI_D_ERROR, "Failed to get BlockIo status = %d maxhandles = %d\n", Status, MaxHandles));
    return Status;
  }

  BlockIo = BlockIoHandle[0].BlkIo;
  DEBUG ((EFI_D_INFO, "gpt bootloader %u %u\n", Size, MaxHandles));
  Status = WriteToPartition (BlockIo, Handle, 0, Size, Buffer);
  DEBUG ((EFI_D_INFO, "gpt write status %u\n", Status));

  gBS->DisconnectController (BlockIoHandle[0].Handle, NULL, NULL);
  gBS->ConnectController (BlockIoHandle[0].Handle, NULL, NULL, TRUE);

  Status = EnumeratePartitions ();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Enumeration of partitions failed\n"));
    return Status;
  }

  UpdatePartitionEntries ();

  DEBUG ((
    EFI_D_INFO,
    "*************** New partition Table Dump Start "
    "*******************\n"
    ));
  PartitionDump ();
  DEBUG ((
    EFI_D_INFO,
    "*************** New partition Table Dump End "
    "*******************\n"
    ));
  return Status;
}

BOOLEAN
IsPartitionBackDoor (
  VOID
  )
{
 #ifndef PARTIITON_BACKDOOR_NOT_SUPPORT
  DEBUG ((EFI_D_INFO, "IsPartitionBackDoor %d\n", PcdGetBool (AvbWithBackDoor)));
  return PcdGetBool (AvbWithBackDoor);
 #else
  return FALSE;
 #endif
}

VOID
SetPartitionBackDoor (
  BOOLEAN  flag
  )
{
 #ifndef PARTIITON_BACKDOOR_NOT_SUPPORT
  DEBUG ((EFI_D_INFO, "SetPartitionBackDoor: old %d new %d\n", PcdGetBool (AvbWithBackDoor), flag));
  PcdSetBoolS (AvbWithBackDoor, flag);
 #endif
}

EFI_STATUS
GetBlockSize (
  UINT32  *Size
  )
{
  EFI_STATUS             Status;
  EFI_BLOCK_IO_PROTOCOL  *BlockIo = NULL;
  HandleInfo             BlockIoHandle[MAX_HANDLEINF_LST_SIZE];
  UINT32                 MaxHandles = MAX_HANDLEINF_LST_SIZE;

  if (IsPartitionBackDoor ()) {
    *Size = 512;
    return EFI_SUCCESS;
  }

  Status = GetStorageHandle (BlockIoHandle, &MaxHandles);
  if (Status || (MaxHandles != 1)) {
    DEBUG ((EFI_D_ERROR, "GetBlockSize Failed to get BlockIo status = %d maxhandles = %d\n", Status, MaxHandles));
    return Status;
  }

  BlockIo = BlockIoHandle[0].BlkIo;
  *Size   = BlockIo->Media->BlockSize;
  DEBUG ((EFI_D_INFO, "GetBlockSize %u %u\n", *Size, MaxHandles));

  return Status;
}
