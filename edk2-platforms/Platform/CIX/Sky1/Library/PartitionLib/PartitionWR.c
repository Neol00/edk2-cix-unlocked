/** @LinuxLoaderLib.c
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2015-2018, 2020, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include "Partition.h"
#include "AutoGen.h"
#include <Library/BootLinux.h>

STATIC UINT32  TimerFreq, FactormS;

UINT64
GetTimerCountms (
  VOID
  )
{
  UINT64  TempFreq, StartVal, EndVal;
  UINT64  TimerCount, Ms;

  if (!TimerFreq && !FactormS) {
    TempFreq = GetPerformanceCounterProperties (&StartVal, &EndVal);

    if (StartVal > EndVal) {
      DEBUG ((EFI_D_ERROR, "Error getting counter property\n"));
      return 0;
    }

    TimerFreq = (UINT32)(TempFreq & 0xFFFFFFFFULL);
    FactormS  = TimerFreq / 1000;
  }

  TimerCount = GetPerformanceCounter ();
  Ms         = TimerCount / FactormS;
  return Ms;
}

EFI_STATUS
ReadFromPartition (
  EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_HANDLE          *Handle,
  IN INT64               ReadOffset,
  IN UINT64              NumBytes,
  IN VOID                *Buffer,
  IN UINT64              *OutNumRead
  )
{
  EFI_STATUS  Status            = EFI_SUCCESS;
  VOID        *Page             = NULL;
  UINTN       PartitionSize     = 0;
  UINT32      PageSize          = 0;
  UINT64      StartBlock        = 0;
  UINT64      LastBlock         = 0;
  UINT64      FullBlock         = 0;
  UINT64      StartPageReadSize = 0;
  UINTN       Offset            = 0;
  UINT64      FullBlockRead     = 0;

  PartitionSize = GetPartitionSize (BlockIo);
  if (!PartitionSize) {
    return EFI_VOLUME_FULL;
  }

  if (ReadOffset < 0) {
    if ((-ReadOffset) > PartitionSize) {
      DEBUG ((EFI_D_ERROR, "Negative Offset outside range.\n"));
      return EFI_VOLUME_FULL;
    }

    Offset = PartitionSize - (-ReadOffset);
    DEBUG ((
      EFI_D_VERBOSE,
      "negative Offset (%d) converted to (%u) \n",
      ReadOffset,
      Offset
      ));
  } else {
    Offset = ReadOffset;
  }

  if (PartitionSize < Offset + NumBytes) {
    DEBUG ((EFI_D_ERROR, "Partition not big enough.\n"));
    DEBUG ((
      EFI_D_ERROR,
      "Partition Size:\t%d\n Buffer Size:\t%d\n",
      PartitionSize,
      NumBytes
      ));
    return EFI_VOLUME_FULL;
  }

  if (IsPartitionBackDoor ()) {
    BackdoorInfo  *Info = (BackdoorInfo *)Handle;
    CopyMem (Buffer, Info->Addr + ReadOffset, NumBytes);
    *OutNumRead = NumBytes;
    return EFI_SUCCESS;
  }

  /* |NumBytes| and or |Offset| can be unaligned to block size/page size.
   */
  PageSize = BlockIo->Media->BlockSize;
  Page     = AllocateZeroPool (PageSize);
  if (Page == NULL) {
    DEBUG ((EFI_D_ERROR, "Allocate for partial read failed!"));
    Status = EFI_OUT_OF_RESOURCES;
    goto out;
  }

  StartBlock = Offset / PageSize;
  LastBlock  = (NumBytes + Offset) / PageSize;
  FullBlock  = StartBlock;

  if (Offset % PageSize != 0) {
    /* Offset not aligned to PageSize*/
    UINT64  StartPageReadOffset = Offset - (StartBlock * PageSize);

    if (StartBlock == LastBlock) {
      /* Offset & Offset + NumBytes are in same block */
      StartPageReadSize = NumBytes;
    } else {
      StartPageReadSize = PageSize - StartPageReadOffset;
      FullBlock++;
    }

    if ((StartPageReadSize <= 0) || (StartPageReadOffset >= PageSize) ||
        (StartPageReadSize > PageSize - StartPageReadOffset) ||
        (StartPageReadSize > NumBytes) ||
        (StartBlock > BlockIo->Media->LastBlock))
    {
      DEBUG ((
        EFI_D_ERROR,
        "StartBlock 0x%llx, StartPageReadOffset 0x%llx,"
        "StartPageReadSize 0x%x outside range.\n",
        StartBlock,
        StartPageReadOffset,
        StartPageReadSize
        ));
      Status = EFI_VOLUME_FULL;
      goto out;
    }

    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        StartBlock,
                        PageSize,
                        Page
                        );
    if (Status == EFI_SUCCESS) {
      CopyMem (Buffer, Page + StartPageReadOffset, StartPageReadSize);
      *OutNumRead += StartPageReadSize;
    } else {
      *OutNumRead = 0;
      DEBUG ((EFI_D_ERROR, "ReadBlocks failed %d\n", Status));
      goto out;
    }
  }

  if ((*OutNumRead < NumBytes) && ((NumBytes + Offset) % PageSize != 0)) {
    /* NumBytes + Offset not aligned to PageSize*/

    /* Offset for last block is always zero, start at Page boundary
     */
    UINT64  LastPageReadSize =
      (Offset + NumBytes) - (LastBlock * PageSize);

    if ((LastPageReadSize <= 0) || (LastPageReadSize >= PageSize) ||
        (LastPageReadSize > (NumBytes - *OutNumRead)) ||
        (LastBlock > BlockIo->Media->LastBlock))
    {
      Status = EFI_VOLUME_FULL;
      goto out;
    }

    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        LastBlock,
                        PageSize,
                        Page
                        );
    if (Status == EFI_SUCCESS) {
      CopyMem (
        Buffer + (NumBytes - LastPageReadSize),
        Page,
        LastPageReadSize
        );
      *OutNumRead += LastPageReadSize;
    } else {
      *OutNumRead = 0;
      DEBUG ((EFI_D_ERROR, "ReadBlocks failed %d\n", Status));
      goto out;
    }
  }

  while (*OutNumRead < NumBytes) {
    /* full block reads */
    UINT64  FillPageReadSize = NumBytes - *OutNumRead;
    /* FPGA nvme is too slow, read 64KB each time to avoid timeout */
    FillPageReadSize = FillPageReadSize > SIZE_64KB ? SIZE_64KB : FillPageReadSize;
    if (((FillPageReadSize % PageSize) != 0) ||
        (FullBlock > BlockIo->Media->LastBlock) ||
        ((NumBytes - StartPageReadSize) < FillPageReadSize))
    {
      DEBUG ((
        EFI_D_ERROR,
        "FullBlock 0x%llx, ReadOffset 0x%x, read_size "
        "0x%x outside range.\n",
        FullBlock,
        0,
        FillPageReadSize
        ));
      Status = EFI_VOLUME_FULL;
      goto out;
    }

    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        FullBlock,
                        FillPageReadSize,
                        Buffer + StartPageReadSize + FullBlockRead
                        );
    if (Status == EFI_SUCCESS) {
      *OutNumRead   += FillPageReadSize;
      FullBlock     += (FillPageReadSize/PageSize);
      FullBlockRead += FillPageReadSize;
    } else {
      *OutNumRead = 0;
      DEBUG ((EFI_D_ERROR, "ReadBlocks failed %d\n", Status));
      goto out;
    }
  }

out:
  if (Page != NULL) {
    FreePool (Page);
  }

  return Status;
}

EFI_STATUS
ReadFromPartitionByName (
  IN CHAR16  *PartitionName,
  IN INT64   Offset,
  IN UINT64  Size,
  IN VOID    *Buffer
  )
{
  EFI_STATUS             Status;
  EFI_BLOCK_IO_PROTOCOL  *BlockIo   = NULL;
  EFI_HANDLE             *Handle    = NULL;
  UINT64                 OutNumRead = 0;

  Status = PartitionGetInfo (PartitionName, &BlockIo, &Handle);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, "GetPartitionHandleInfo success.\n"));
  if (!BlockIo) {
    DEBUG ((EFI_D_ERROR, "BlockIo for %a is corrupted\n", PartitionName));
    return EFI_VOLUME_CORRUPTED;
  }

  if (!Handle) {
    DEBUG ((EFI_D_ERROR, "EFI handle for %a is corrupted\n", PartitionName));
    return EFI_VOLUME_CORRUPTED;
  }

  Status = ReadFromPartition (BlockIo, Handle, Offset, Size, Buffer, &OutNumRead);
  DEBUG ((
    EFI_D_INFO,
    "Read buffer header: 0x%x 0x%x 0x%x 0x%x 0x%x\n",
    ((unsigned char *)Buffer)[0],
    ((unsigned char *)Buffer)[1],
    ((unsigned char *)Buffer)[2],
    ((unsigned char *)Buffer)[3],
    ((unsigned char *)Buffer)[4]
    ));
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "ReadFromPartition Failed\n"));
  }

  DEBUG ((EFI_D_INFO, "Read %s successful\n", PartitionName));
  return Status;
}

#define MAX_WRITE_SIZE  (1024 * 1024)
EFI_STATUS
WriteToPartition (
  EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_HANDLE          *Handle,
  IN INT64               WrietOffset,
  IN UINT64              Size,
  IN VOID                *Buffer
  )
{
  EFI_STATUS  Status        = EFI_SUCCESS;
  VOID        *Page         = NULL;
  UINT64      WriteUnitSize = MAX_WRITE_SIZE;
  UINTN       Offset        = 0;
  UINT64      WriteSize     = 0;
  UINT64      PartitionSize;
  UINT32      PageSize           = 0;
  UINT64      StartBlock         = 0;
  UINT64      LastBlock          = 0;
  UINT64      FullBlock          = 0;
  UINT64      StartPageWriteSize = 0;
  UINT64      LastPageWriteSize  = 0;

  PartitionSize = GetPartitionSize (BlockIo);
  if (!PartitionSize) {
    return EFI_VOLUME_FULL;
  }

  if (WrietOffset < 0) {
    if ((-WrietOffset) > PartitionSize) {
      DEBUG ((EFI_D_ERROR, "Negative Offset outside range.\n"));
      return EFI_VOLUME_FULL;
    }

    Offset = PartitionSize - (-WrietOffset);
    DEBUG ((
      EFI_D_VERBOSE,
      "negative Offset (%d) converted to (%u) \n",
      WrietOffset,
      Offset
      ));
  } else {
    Offset = WrietOffset;
  }

  if (PartitionSize < Offset + Size) {
    DEBUG ((EFI_D_ERROR, "Partition not big enough.\n"));
    DEBUG ((
      EFI_D_ERROR,
      "Partition Size:\t%d\n Buffer Size:\t%d\n",
      PartitionSize,
      Size
      ));
    return EFI_VOLUME_FULL;
  }

  if (IsPartitionBackDoor ()) {
    BackdoorInfo  *Info = (BackdoorInfo *)Handle;
    CopyMem (Info->Addr + WrietOffset, Buffer, Size);
    return EFI_SUCCESS;
  }

  /* |Size| and or |Offset| can be unaligned to block size/page size.
   */
  PageSize = BlockIo->Media->BlockSize;
  Page     = AllocateZeroPool (PageSize);
  if (Page == NULL) {
    DEBUG ((EFI_D_ERROR, "Allocate for partial read failed!"));
    Status = EFI_OUT_OF_RESOURCES;
    goto out;
  }

  StartBlock = Offset / PageSize;
  LastBlock  = (Size + Offset) / PageSize;
  FullBlock  = StartBlock;

  if (Offset % PageSize != 0) {
    /* Offset not aligned to PageSize*/
    UINT64  StartPageWriteOffset = Offset - (StartBlock * PageSize);

    if (StartBlock == LastBlock) {
      /* Offset & Offset + NumBytes are in same block */
      StartPageWriteSize = Size;
    } else {
      StartPageWriteSize = PageSize - StartPageWriteOffset;
      FullBlock++;
    }

    if ((StartPageWriteSize <= 0) || (StartPageWriteOffset >= PageSize) ||
        (StartPageWriteSize > PageSize - StartPageWriteOffset) ||
        (StartPageWriteSize > Size) ||
        (StartBlock > BlockIo->Media->LastBlock))
    {
      DEBUG ((
        EFI_D_ERROR,
        "StartBlock 0x%llx, StartPageWriteOffset 0x%llx,"
        "StartPageWriteSize 0x%x outside range.\n",
        StartBlock,
        StartPageWriteOffset,
        StartPageWriteSize
        ));
      Status = EFI_VOLUME_FULL;
      goto out;
    }

    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        StartBlock,
                        PageSize,
                        Page
                        );
    if (Status != EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "ReadBlocks failed %d\n", Status));
      goto out;
    }

    CopyMem (Page + StartPageWriteOffset, Buffer, StartPageWriteSize);
    Status = BlockIo->WriteBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        StartBlock,
                        PageSize,
                        Page
                        );
    if (Status != EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "Write failed %d\n", Status));
      goto out;
    }

    WriteSize += StartPageWriteSize;
  }

  if ((WriteSize != Size) && ((Size + Offset) % PageSize != 0)) {
    /* Size + Offset not aligned to PageSize*/
    LastPageWriteSize = (Offset + Size) - (LastBlock * PageSize);

    if ((LastPageWriteSize <= 0) || (LastPageWriteSize >= PageSize) ||
        (LastBlock > BlockIo->Media->LastBlock))
    {
      DEBUG ((
        EFI_D_ERROR,
        "LastBlock 0x%llx, LastPageWriteSize 0x%llx outside range.\n",
        LastBlock,
        LastPageWriteSize
        ));
      Status = EFI_VOLUME_FULL;
      goto out;
    }

    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        LastBlock,
                        PageSize,
                        Page
                        );
    if (Status != EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "ReadBlocks failed %d\n", Status));
      goto out;
    }

    CopyMem (Page, Buffer + Size - LastPageWriteSize, LastPageWriteSize);
    Status = BlockIo->WriteBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        LastBlock,
                        PageSize,
                        Page
                        );
    if (Status != EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "Write failed %d\n", Status));
      goto out;
    }

    WriteSize += LastPageWriteSize;
  }

  /* The big image buffer may take a long flashing time which will block
     parallel usb image download. It will cause the fastboot  protocol host
     side timeout. So split the image into small writing units  to let usb
     have chance to champ in and doing work in parallel.
    */
  while (WriteSize < Size) {
    UINT64  ToWrite = (Size - WriteSize) > WriteUnitSize ? WriteUnitSize : (Size - WriteSize);
    Status = BlockIo->WriteBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        FullBlock,
                        ToWrite,
                        Buffer + (WriteSize - LastPageWriteSize)
                        );
    if (Status != EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "Write the divisible Image failed :%r\n", Status));
      goto out;
    }

    FullBlock += ToWrite / PageSize;
    WriteSize += ToWrite;
  }

out:
  if (Page != NULL) {
    FreePool (Page);
  }

  return Status;
}

EFI_STATUS
WriteToPartitionByName (
  IN CHAR16  *PartitionName,
  IN INT64   Offset,
  IN UINT64  Size,
  IN VOID    *Buffer
  )
{
  EFI_STATUS             Status;
  EFI_BLOCK_IO_PROTOCOL  *BlockIo = NULL;
  EFI_HANDLE             *Handle  = NULL;

  Status = PartitionGetInfo (PartitionName, &BlockIo, &Handle);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, "GetPartitionHandleInfo success.\n"));
  if (!BlockIo) {
    DEBUG ((EFI_D_ERROR, "BlockIo for %a is corrupted\n", PartitionName));
    return EFI_VOLUME_CORRUPTED;
  }

  if (!Handle) {
    DEBUG ((EFI_D_ERROR, "EFI handle for %a is corrupted\n", PartitionName));
    return EFI_VOLUME_CORRUPTED;
  }

  DEBUG ((
    EFI_D_INFO,
    "Buffer header: 0x%x 0x%x 0x%x 0x%x 0x%x\n",
    ((unsigned char *)Buffer)[0],
    ((unsigned char *)Buffer)[1],
    ((unsigned char *)Buffer)[2],
    ((unsigned char *)Buffer)[3],
    ((unsigned char *)Buffer)[4]
    ));
  Status = WriteToPartition (BlockIo, Handle, Offset, Size, Buffer);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "WriteToPartition failed\n"));
    return Status;
  }

  DEBUG ((EFI_D_INFO, "Write %s successful\n", PartitionName));
  return Status;
}

EFI_STATUS
ErasePartition (
  EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  EFI_HANDLE             *Handle
  )
{
  EFI_STATUS                   Status;
  EFI_ERASE_BLOCK_TOKEN        EraseToken;
  EFI_ERASE_BLOCK_PROTOCOL     *EraseProt = NULL;
  EFI_PARTITION_INFO_PROTOCOL  *PartitionInfo;
  UINTN                        PartitionSize;
  UINTN                        TokenIndex;

  PartitionSize = GetPartitionSize (BlockIo);
  if (!PartitionSize) {
    return EFI_BAD_BUFFER_SIZE;
  }

  if (IsPartitionBackDoor ()) {
    BackdoorInfo  *Info = (BackdoorInfo *)Handle;
    SetMem (Info->Addr, Info->PartitionSize, 0);
    return EFI_SUCCESS;
  }

  Status = gBS->HandleProtocol (Handle, &gEfiPartitionInfoProtocolGuid, (VOID **)&PartitionInfo);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Unable to locate Partition record protocol: %r\n", Status));
    return Status;
  }

  Status = gBS->HandleProtocol (Handle, &gEfiEraseBlockProtocolGuid, (VOID **)&EraseProt);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Unable to locate Erase block protocol handle: %r\n", Status));
    return Status;
  }

  gBS->SetMem ((VOID *)&EraseToken, sizeof (EraseToken), 0);
  Status = EraseProt->EraseBlocks (
                        EraseProt,
                        BlockIo->Media->MediaId,
                        PartitionInfo->Info.Gpt.StartingLBA,
                        &EraseToken,
                        PartitionSize
                        );
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Unable to Erase Block: %r\n", Status));
    return Status;
  } else {
    if (EraseToken.Event != NULL) {
      DEBUG ((EFI_D_INFO, "Waiting for the erase event to signal the completion\n"));
      gBS->WaitForEvent (1, &EraseToken.Event, &TokenIndex);
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ErasePartitionByName (
  IN CHAR16  *PartitionName
  )
{
  EFI_STATUS             Status;
  EFI_BLOCK_IO_PROTOCOL  *BlockIo = NULL;
  EFI_HANDLE             *Handle  = NULL;

  Status = PartitionGetInfo (PartitionName, &BlockIo, &Handle);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  if (!BlockIo) {
    DEBUG ((EFI_D_ERROR, "BlockIo for %s is corrupted\n", PartitionName));
    return EFI_VOLUME_CORRUPTED;
  }

  if (!Handle) {
    DEBUG ((EFI_D_ERROR, "EFI handle for %s is corrupted\n", PartitionName));
    return EFI_VOLUME_CORRUPTED;
  }

  Status = ErasePartition (BlockIo, Handle);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Partition Erase failed: %r\n", Status));
    return Status;
  }

  return Status;
}

#ifdef FLASH_ALL_FROM_DDR
VOID
FlashAllFromDDR (
  VOID
  )
{
  WriteGpt ((VOID *)(0x3000000LL <<5)+0x80000000, SIZE_1MB);
  WriteToPartitionByName (L"boot_b", 0, SIZE_64MB, (VOID *)((0x2fff80LL <<5)+0x80000000));
  WriteToPartitionByName (L"vendor_boot_b", 0, SIZE_64MB, (VOID *)(0x50ff80LL <<5)+0x80000000);
  WriteToPartitionByName (L"dtbo_b", 0, SIZE_4MB, (VOID *)(0x2000000LL<<5)+0x80000000);
  WriteToPartitionByName (L"vbmeta_b", 0, SIZE_1MB, (VOID *)(0x2400000LL <<5)+0x80000000);
  WriteToPartitionByName (L"s2_b", 0, SIZE_4MB, (VOID *)(0x2c00000LL <<5)+0x80000000);
  WriteToPartitionByName (L"boot_a", 0, SIZE_64MB, (VOID *)((0x2fff80LL <<5)+0x80000000));
  WriteToPartitionByName (L"vendor_boot_a", 0, SIZE_64MB, (VOID *)(0x50ff80LL <<5)+0x80000000);
  WriteToPartitionByName (L"dtbo_a", 0, SIZE_4MB, (VOID *)(0x2000000LL<<5)+0x80000000);
  WriteToPartitionByName (L"vbmeta_a", 0, SIZE_1MB, (VOID *)(0x2400000LL <<5)+0x80000000);
  WriteToPartitionByName (L"s2_a", 0, SIZE_4MB, (VOID *)(0x2c00000LL <<5)+0x80000000);
  WriteToPartitionByName (L"misc", 0, SIZE_4MB, (VOID *)(0x2800000LL <<5)+0x80000000);
  WriteToPartitionByName (L"super", 0, SIZE_1GB+SIZE_512MB, (VOID *)(0x6000000LL <<5)+0x80000000);
  gBS->SetMem ((VOID *)((0x2fff80LL <<5)+0x80000000), SIZE_64MB, 0x0);
  WriteToPartitionByName (L"misc", 0, SIZE_4MB, (VOID *)(0x2fff80LL<<5)+0x80000000);
  WriteToPartitionByName (L"metadata", 0, SIZE_16MB, (VOID *)(0x2fff80LL<<5)+0x80000000);
  WriteToPartitionByName (L"presistdata", 0, SIZE_1MB, (VOID *)(0x2fff80LL<<5)+0x80000000);
  WriteToPartitionByName (L"userdata", 0, SIZE_64MB, (VOID *)(0x2fff80LL<<5)+0x80000000);
  WriteToPartitionByName (L"fbmisc", 0, SIZE_1MB, (VOID *)(0x2fff80LL<<5)+0x80000000);
}

#endif
