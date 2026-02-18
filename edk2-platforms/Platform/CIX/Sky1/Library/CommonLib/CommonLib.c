/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Guid/FileInfo.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>
#include <Library/FileHandleLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/RamDisk.h>
#include <Protocol/BlockIo.h>
#include <Library/CommonLib.h>

VOID
AsciiToUnicode (
  IN    CHAR8   *AsciiString,
  IN    CHAR16  *UnicodeString
  )
{
  UINT8  Index;

  Index = 0;
  while (AsciiString[Index] != 0) {
    UnicodeString[Index] = (CHAR16)AsciiString[Index];
    Index++;
  }
}

UINTN
UnicodeToAscii (
  IN CONST CHAR16  *UStr,
  IN CONST UINTN   Length,
  OUT CHAR8        *AStr
  )
{
  UINTN  Index;

  //
  // just buffer copy, not character copy
  //
  for (Index = 0; Index < Length; Index++) {
    *AStr++ = (CHAR8)*UStr++;
  }

  return Index;
}

VOID
PrintDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  // EFI_DEVICE_PATH_PROTOCOL *DevicePathNode;
  CHAR16  *DevicePathString;

  DevicePathString = ConvertDevicePathToText (
                                              DevicePath,
                                              FALSE,
                                              FALSE
                                              );

  DEBUG ((DEBUG_INFO, "%s\n", DevicePathString));
  Print (L"%s\n", DevicePathString);
  if (DevicePathString != NULL) {
    FreePool (DevicePathString);
  }
}

BOOLEAN
IsRamdiskDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;

  // VENDOR_DEVICE_PATH        *VendorDevicePath;
  // EFI_HANDLE                Handle;

  TempDevicePath = DevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
    if ((DevicePathType (TempDevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (TempDevicePath) == MEDIA_RAM_DISK_DP))
    {
      // VendorDevicePath = (VENDOR_DEVICE_PATH *)TempDevicePath;

      DEBUG ((DEBUG_INFO, "ramdisk found.\n"));
      Print (L"ramdisk found.\n");
      return TRUE;
    }

    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return FALSE;
}

EFI_STATUS
CreatePlatformBootOptionFromDevicePath (
  IN     EFI_DEVICE_PATH               *DevicePath,
  IN     CHAR16                        *Description,
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  )
{
  EFI_STATUS  Status;

  // EFI_DEVICE_PATH  *DevicePath;

  // DevicePath = (EFI_DEVICE_PATH *)ConvertTextToDevicePath (PathStr);
  ASSERT (DevicePath != NULL);
  Status = EfiBootManagerInitializeLoadOption (
                                               BootOption,
                                               LoadOptionNumberUnassigned,
                                               LoadOptionTypeBoot,
                                               LOAD_OPTION_ACTIVE,
                                               Description,
                                               DevicePath,
                                               NULL,
                                               0
                                               );
  // FreePool (DevicePath);
  return Status;
}

VOID
EFIAPI
FindRamdiskDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  **DevicePath
  )
{
  EFI_STATUS  Status;
  // EFI_HANDLE  *pSimpleFileHandle = NULL;
  EFI_HANDLE  *pBlockIoHandle = NULL;

  UINTN   HandleNum;
  UINT32  index;

  // EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *pSimpleFileSysProtocol = NULL;
  EFI_BLOCK_IO_PROTOCOL  *pBlockIoProtocol = NULL;

  // EFI_DEVICE_PATH_PROTOCOL         *DevicePath;

  Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiBlockIoProtocolGuid,// &gEfiSimpleFileSystemProtocolGuid,
                                    NULL,
                                    &HandleNum,
                                    &pBlockIoHandle
                                    );
  for (index = 0; index < HandleNum; index++) {
    Status = gBS->OpenProtocol (
                                pBlockIoHandle[index],
                                &gEfiBlockIoProtocolGuid,
                                (VOID **)&pBlockIoProtocol,
                                gImageHandle,
                                NULL,
                                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                                );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "open BlockIo protocol failed:%r\n", Status));
      return;
    }

    // found ramdisk
    *DevicePath = DevicePathFromHandle (pBlockIoHandle[index]);
    PrintDevicePath (*DevicePath);
    if (!IsRamdiskDevicePath (*DevicePath)) {
      *DevicePath = NULL;
      continue;
    }

    return;
  }
}


VOID
TimeStampPrint()
{
  CHAR8  Buffer[32];
  UINT64 Ticker,TimeStamp,Second,Remainder,MicroSecond;
  Ticker    = GetPerformanceCounter ();
  TimeStamp = GetTimeInNanoSecond (Ticker);
  Second   = TimeStamp/(1000*1000*1000);
  Remainder = TimeStamp%(1000*1000*1000);
  MicroSecond = Remainder/(1000);

  AsciiSPrint (Buffer, 13,"[%02lld.%06lld] ", Second,MicroSecond);
  SerialPortWrite ((UINT8 *)Buffer, AsciiStrLen (Buffer));
    // Index++;
}

