/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "PlatformSmbios.h"
#include <Protocol/FwVersionProtocol.h>
#include <Protocol/EcPlatformProtocol.h>

#define TYPE2_STRINGS                                                         \
  "Cix Technology Group Co., Ltd.\0"   /* Manufacturer */                                \
  "CIX Board\0"                        /* Product Name */                                \
  "1.0\0"                              /* Version */                                     \
  "Base Board Serial Number\0"         /* Serial */                                      \
  "Base Board Asset Tag\0"             /* Assert Tag */                                  \
  "Part Component\0"                   /* board location */                              \
  "CIX Merak Board\0"                  /* Product Name 0*/                               \
  "CIX Phecda Board\0"                 /* Product Name 1*/
#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE2    Base;
  UINT8                 Strings[sizeof (TYPE2_STRINGS)];
} PLATFORM_SMBIOS_TYPE2;
#pragma pack()

// Baseboard (section 7.3)
STATIC PLATFORM_SMBIOS_TYPE2  mPlatformDefaultType2 = {
  {
    {
      // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION,       // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE2),                 // UINT8 Length
      SMBIOS_HANDLE_MOTHERBOARD,
    },
    1,     // Manufacturer
    2,     // Product Name
    3,     // Version
    4,     // Serial
    5,     // Asset tag
    {
      1,
    },     // motherboard, not replaceable
    6,     // location of board
    SMBIOS_HANDLE_CHASSIS,
    BaseBoardTypeMotherBoard,
    1,
    { SMBIOS_HANDLE_CLUSTER },   // ,SMBIOS_HANDLE_CLUSTER,SMBIOS_HANDLE_MEMORY},
  },
  TYPE2_STRINGS
};

EFI_STATUS
AddSmbiosType2 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS               Status;
  EFI_SMBIOS_HANDLE        SmbiosHandle, SmbiosHandleType4;
  EFI_SMBIOS_TABLE_HEADER  *Record;
  EFI_SMBIOS_TYPE          Type4 = SMBIOS_TYPE_PROCESSOR_INFORMATION;
  CIX_FW_VERSION_PROTOCOL  *pFwVerProtocol;
  CHAR16                   *pFwVerBuff;
  UINT32                   FwVerSize;
  EC_RESPONSE_BOARD_ID     *pBoardId;
  UINT16                   Sku;

  Status = gBS->LocateProtocol (
                  &gCixFwVersionProtocolGuid,
                  NULL,
                  (VOID **)&pFwVerProtocol
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fw ver protocol not found\n", __FUNCTION__));
  } else {
    Status = pFwVerProtocol->GetFwVersion (FwVerBoardId, &pFwVerBuff, &FwVerSize);
    if (!EFI_ERROR (Status)) {
      pBoardId = (EC_RESPONSE_BOARD_ID *)pFwVerBuff;
      Sku      = (pBoardId->Id.SkuExt << 3) + pBoardId->Id.Sku;
      switch (Sku) {
        case 0:
          mPlatformDefaultType2.Base.ProductName = 7;
          break;
        case 4:
          mPlatformDefaultType2.Base.ProductName = 8;
          break;
        default:
          break;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "%a: Board Id:NULL\n", __FUNCTION__));
    }
  }

  SmbiosHandleType4 = SMBIOS_HANDLE_PI_RESERVED;
  Status            = Smbios->GetNext (Smbios, &SmbiosHandleType4, &Type4, &Record, NULL);
  if (!EFI_ERROR (Status) && (SmbiosHandleType4 != SMBIOS_HANDLE_PI_RESERVED)) {
    mPlatformDefaultType2.Base.ContainedObjectHandles[0] = SmbiosHandleType4;
  } else {
    DEBUG ((DEBUG_ERROR, "Fail to locate the handle of Smbios Type4 entry!\n"));
  }

  SmbiosHandle = SMBIOS_HANDLE_MOTHERBOARD;
  Status       = Smbios->Add (
                           Smbios,
                           NULL,
                           &SmbiosHandle,
                           (EFI_SMBIOS_TABLE_HEADER *)&mPlatformDefaultType2
                           );

  if (EFI_ERROR (Status)) {
    DEBUG (
      (
       DEBUG_ERROR,
       "[%a]:[%dL] Smbios Type2 Table Log Failed! %r \n",
       __FUNCTION__,
       DEBUG_LINE_NUMBER,
       Status
      )
      );
  }

  return EFI_SUCCESS;
}
