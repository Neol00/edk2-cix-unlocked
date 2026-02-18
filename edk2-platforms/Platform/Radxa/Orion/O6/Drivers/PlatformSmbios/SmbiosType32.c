/*
  Copyright 2025 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#define TYPE32_BOOT_STATUS_SIZE                     10
#define TYPE32_STATUS_SIZE                          1
#define TYPE32_ADDTIONAL_DATA_SIZE                  (TYPE32_BOOT_STATUS_SIZE - TYPE32_STATUS_SIZE) // 9
#define TYPE32_START_RESERVED_TABLE_SIZE            (127 - 9 + 1)
#define TYPE32_START_OEM_SPECIFIC_TABLE_SIZE        (191 - 128 + 1)
#define TYPE32_START_PRODUCT_SPECIFIC_TABLE_SIZE    (255 - 192 + 1)
#define TYPE32_MAX_STATUS                           255
// Status is one byte, so the addtional data is nine bytes.
typedef UINT8 ADDTIONDATA[TYPE32_ADDTIONAL_DATA_SIZE];

STATIC ADDTIONDATA mPreviousRequestedImage                                              = "12345678";

STATIC ADDTIONDATA mStartReservedTable[TYPE32_START_RESERVED_TABLE_SIZE]                = {
    "12345678"
};

STATIC ADDTIONDATA mStartOemSpecificTable[TYPE32_START_OEM_SPECIFIC_TABLE_SIZE]         = {
    "12345678"
};

STATIC ADDTIONDATA mStartProductSpecificTable[TYPE32_START_PRODUCT_SPECIFIC_TABLE_SIZE] = {
    "12345678"
};

#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE32    Base;
  ADDTIONDATA            addtionData;
} PLATFORM_SMBIOS_TYPE32;
#pragma pack()


// BIOS information (section 7.1)
STATIC PLATFORM_SMBIOS_TYPE32  mPlatformDefaultType32 = {
    {
      {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION,                     // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE32),   // UINT8 Length
      SMBIOS_HANDLE_PI_RESERVED,
      },
      {
      0,
      0,
      0,
      0,
      0,
      0
      },
      BootInformationStatusNoError
    },
    "\0"
};

/**
 * @brief Set the Addtional Data object
 * 
 * @param [IN] code 
 * @return EFI_INVALID_PARAMETER Error input
 *         EFI_SUCCESS           Success
 */
EFI_STATUS
SetAddtionalData(IN VOID* code) {
    if(code == NULL){
        DebugPrint (DEBUG_ERROR,"Error! [FUN]:%a[LINE]:%a\n", __FUNCTION__, __LINE__);
        return EFI_INVALID_PARAMETER;
    }
    UINT8* ptable = NULL;
    UINT8 Code    = *(UINT8*) code;
    UINT8 StrLen  = 0;

    if (Code == BootInformationStatusPreviousRequestedImage) {
        ptable = mPreviousRequestedImage;
    } else if (Code >= BootInformationStatusStartReserved && Code < BootInformationStatusStartOemSpecific) {
        ptable = mStartReservedTable[Code - BootInformationStatusStartReserved];
    } else if (Code >= BootInformationStatusStartOemSpecific && Code < BootInformationStatusStartProductSpecific) {
        ptable = mStartOemSpecificTable[Code - BootInformationStatusStartOemSpecific];
    } else if (Code >= BootInformationStatusStartProductSpecific && Code <= TYPE32_MAX_STATUS) {
        ptable = mStartProductSpecificTable[Code - BootInformationStatusStartProductSpecific];
    } else {
        return EFI_SUCCESS;
    }

    StrLen = (UINT8)AsciiStrLen((CHAR8*)ptable) + 1;
    if(StrLen > TYPE32_ADDTIONAL_DATA_SIZE)
        return EFI_INVALID_PARAMETER;

    mPlatformDefaultType32.Base.BootStatus = Code;
    mPlatformDefaultType32.Base.Hdr.Length += StrLen;
    CopyMem(mPlatformDefaultType32.addtionData, ptable, StrLen);
    return EFI_SUCCESS;
}

EFI_STATUS
AddSmbiosType32 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS         Status;
  EFI_SMBIOS_HANDLE  SmbiosHandle;
  UINT8              StatusCode = BootInformationStatusNoError;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  // This can change StatusCode
  // StatusCode = ...; interface or const
  Status = SetAddtionalData(&StatusCode);
  if (EFI_ERROR(Status)){
    DebugPrint (DEBUG_ERROR,"SetAddtionData Error!\n[Fun]:%a\n[Line]:%d\n", __FUNCTION__, __LINE__);
    return Status;
  }

  Status = Smbios->Add (
                        Smbios,
                        NULL,
                        &SmbiosHandle,
                        (EFI_SMBIOS_TABLE_HEADER *)&mPlatformDefaultType32
                        );

  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "[%a]:[%dL] Smbios Type32 Table Log Failed! %r \n",
      __FUNCTION__,
      DEBUG_LINE_NUMBER,
      Status
      ));
  }

  return EFI_SUCCESS;
}