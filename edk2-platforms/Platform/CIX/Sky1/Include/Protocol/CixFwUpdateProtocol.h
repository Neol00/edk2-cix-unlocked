/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *   SPDX-License-Identifier: BSD-2-Clause-Patent
 **/

#ifndef _CIXFWUPDATE_PROTOCOL_H_
#define _CIXFWUPDATE_PROTOCOL_H_

typedef enum {
  ENTRY_ERASE = 0,
  ENTRY_WRITE = 1,
  ENTRY_READ  = 2,
} ENTRY_UPDATE_METHOD;

typedef enum {
  FIRMWARE_PROGRAM_SUCCESS = 0,
  FIRMWARE_START           = 1,
  FIRMWARE_WRITE_FAILED    = 2,
  FIRMWARE_WRITE_SUCCESS   = 3,
  FIRMWARE_VERIFY_FAILED   = 4
} FIRMWARE_UPDATE_STATUS;

typedef enum {
  FIRMWARE_TYPE_FULL_PROGRAM    = 0,
  FIRMWARE_TYPE_BootLoader_1    = 1,
  FIRMWARE_TYPE_BootLoader_2    = 2,
  FIRMWARE_TYPE_MEM_CONF        = 3,
  FIRMWARE_TYPE_PM_CONF         = 4,
  FIRMWARE_TYPE_SECURE_DEBUG    = 5,
  FIRMWARE_TYPE_BootLoader_3    = 7,
  FIRMWARE_TYPE_EC              = 10,
  FIRMWARE_TYPE_FIRMWARE_HEADER = 11,
  FIRMWARE_TYPE_UEFI_NVRAM      = 100
} FIRMWARE_PACKAGE_TYPE;

typedef enum {
  FIRMWARE_RET_SUCCESS                = 0,
  FIRMWARE_RET_ERR_INPUT              = 1,
  FIRMWARE_RET_ERR_HEADER             = 2,
  FIRMWARE_RET_ERR_SIZE               = 3,
  FIRMWARE_RET_ERR_INTEGRITY          = 4,
  FIRMWARE_RET_ERR_SIG                = 5,
  FIRMWARE_RET_ERR_VER_DOWN           = 6,
  FIRMWARE_RET_ERR_LAYOUT             = 7,
  FIRMWARE_RET_ERR_4KB_ALIGN          = 8,
  FIRMWARE_RET_NO_FLASH_PROG_PROTOCOL = 9,
  FIRMWARE_RET_TYPE_NOT_SUPPORT       = 10,
  FIRMWARE_RET_TYPE_NOT_FOUND         = 11,

  FIRMWARE_RET_ERR_PROG   = 20,
  FIRMWARE_RET_ERR_VERIFY = 21,
  FIRMWARE_RET_READ_ERR   = 22,

  FIRMWARE_RET_OUT_OF_RESOURCE   = 30,
  FIRMWARE_RET_FUNC_NOT_FOUND    = 40
} FIRMWARE_RETURN_VALUE;

typedef void (*FIRMWARE_PROGRAM_CALLBACK)(
  UINT8   *pBuff,
  UINT32  Size
  );

typedef UINT16 (*CIX_FIRMWARE_PACKAGE_UPDATE) (
  UINT8                      *pNewFirmwareImage,
  UINT32                     ImageSize,
  FIRMWARE_PROGRAM_CALLBACK  CallBackFunc
  );

typedef UINT16 (*CIX_FIRMWARE_SINGLE_UPDATE) (
  UINT8                      Type,
  UINT8                      *pNewFirmwareImage,
  UINT32                     ImageSize,
  FIRMWARE_PROGRAM_CALLBACK  CallBackFunc
  );

typedef EFI_STATUS EFIAPI (*CIX_FIRMWARE_EC_UPDATE) (

  );

typedef UINT16 (*CIX_FIRMWARE_READ) (
  UINT32  OffsetAddress,
  UINTN   ImageBuffSize,
  UINT8   *pBuff
  );

typedef UINT16 (*CIX_FIRMWARE_ENTRY_UPDATE) (
  UINT8                      Type,
  UINT8                      *pNewFirmwareImage,
  UINT32                     ImageSize,
  ENTRY_UPDATE_METHOD        UpateState,
  FIRMWARE_PROGRAM_CALLBACK  CallBackFunc
  );

typedef UINT16 (*CIX_FIRMWARE_GET_VERSION) (
  UINT32                      *HeaderOffset,
  UINT32                      *Bootloader1Ver,
  UINT32                      *Bootloader2Ver,
  UINT32                    *Bootloader3Ver
  );

//
// Protocol definition
//
typedef struct {
  UINT32                         Version;
  CIX_FIRMWARE_PACKAGE_UPDATE    FirmwarePackageProgram;
  CIX_FIRMWARE_SINGLE_UPDATE     FirmwareSingleProgram;
  CIX_FIRMWARE_READ              FirmwareRead;
  CIX_FIRMWARE_ENTRY_UPDATE      FirmwareRawEntryUpdate;
  CIX_FIRMWARE_GET_VERSION       FirmwareGetVersion;
} CIX_FW_UPDATE_PROTOCOL;

#endif
