/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

**/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/PeCoffLib.h>
#include <Library/PrintLib.h>
#include <Library/NorFlashLib.h>
#include <Protocol/CixFwUpdateProtocol.h>
#include <string.h>

#pragma pack(push)
#pragma pack(1)

typedef struct {
  UINT32    Type;
  UINT32    Base;
  UINT32    Length;
  UINT32    Reserved;
} FIRMWARE_ENTRY;

typedef struct {
  UINT32            Signature; // 0x55AA55AA
  UINT32            Version;
  UINT32            EntryCount;
  UINT32            ControlFlag;
  FIRMWARE_ENTRY    EntryNode[1];
} FIRMWARE_HEADER;

typedef struct {
  UINT32    Type;
  UINT32    EntryOnboardOffset;
  UINT32    EntryOnboardSize;
  UINT32    EntryImageOffset;
  UINT32    EntryImageSize;
} CIX_FIRMWARE_ENTRY_INFO;

typedef struct {
  UINT32                     ProgramedBlock;
  UINT32                     ImageBlockNum;
  UINT32                     EntryCount;
  UINT32                     HeaderOffset;
  CIX_FIRMWARE_ENTRY_INFO    ImageFwEntryInfo[10];
  FIRMWARE_HEADER        *pOnboardFwHeader;
  FIRMWARE_HEADER        *pNewImageFwHeader;
} CIX_FWUP_PRIVATE_DATA;

typedef struct {
  UINT8    status_result;  /* 0 – program success, 1 - start, 2 – write failed,  3 - write success, 4 – verify failed */
  UINT8    firmware_type;
} FIRMWARE_PROGRAM_STATUS;

typedef struct {
  CHAR8     Magic[8];
  UINT32    Cbff_version;
  UINT32    Encrypt_type;
  UINT32    Keys_number;
} CBFF_IMAGE_HEADER;

typedef struct {
  UINT32    name;
  UINT32    serial_number;
  UINT64    flags;
} FIP_TOC_HEADER;

#pragma pack(pop)

/*
Update strategy is  defined in firmware header offset 0x0C
Bit0: global update control, update by version
Bit1: set 1 to force update bootloader 2
Bit2: set 1 to force update bootloader 1
Bit3: set 1 to force need update memory config
Bit4: set 1 to force update CSU_PM config
*/
#define UPDATE_GLOBAL         BIT0
#define UPDATE_BOOTLOADER_1   BIT1
#define UPDATE_BOOTLOADER_2   BIT2
#define UPDATE_MEMORY_CONFIG  BIT3
#define UPDATE_CSUPM_CONFIG   BIT4
#define UPDATE_OTA_PACKAGE    BIT31

#define EC_OFFSET                   0x10000
#define FIRMWARE_HEADER_OFFSET      0x100000
#define FIRMWARE_HEADER_OFFSET_ALT  0x200000
#define SECURE_DEBUG_OFFSET         0x107000
#define SECURE_DEBUG_OFFSET_ALT     0x207000
#define FIRMWARE_HEADER_SIGNATURE   0x55AA55AA
#define SECURE_DEBUG_SIZE           0x1000

#define FW_UPDATE_PROTOCOL_VERSION  1

EFI_STATUS
EFIAPI
LocateNorFlashDiskIoProtocol (
  );

BOOLEAN
CixFirmwareVerify (
  UINT8                    *pImageBuff,
  UINT32                   ImageBuffSize,
  UINT32                   OffsetAddress,
  FIRMWARE_PROGRAM_STATUS  *pFwProgSts
  );

UINT8
CixFlashWriteWrapper (
  UINT8   Type,
  UINT8   *pImageBuff,
  UINT32  ImageBuffSize,
  UINT32  dwflag,
  UINT32  OffsetAddress
  );

EFI_STATUS
EFIAPI
CixFlashReadWrapper (
  UINT32  OffsetAddress,
  UINTN   ImageBuffSize,
  VOID    *pImageBuff
  );

EFI_STATUS
EFIAPI
CixFirmwareEcProgram (
  );
