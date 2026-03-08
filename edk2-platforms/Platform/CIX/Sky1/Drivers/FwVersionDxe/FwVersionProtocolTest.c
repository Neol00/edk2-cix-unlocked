/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  Copyright (c) 2021, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/FwVersionProtocol.h>
#include <Protocol/EcPlatformProtocol.h>

STATIC
EFI_STATUS
EFIAPI
DumpPlatformFwVersions (
  )
{
  EFI_STATUS            Status;
  EC_PLATFORM_PROTOCOL  *Ec;
  EC_RESPONSE           EcResponse;

  Status = gBS->LocateProtocol (&gCixEcPlatformProtocolGuid, NULL, (VOID **)&Ec);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "EC platform not ready.\n"));
    return EFI_NOT_FOUND;
  }

  Status = Ec->Transfer (Ec, EC_COMMAND_GET_PD_VERSION, NULL, &EcResponse);
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " PD:failed\n");
  } else {
    DebugPrint (DEBUG_INFO, " PD:%d.%d %d.%d\n", EcResponse.PdVer.Pd2Ver & 0xFF, (EcResponse.PdVer.Pd2Ver & 0xFF00) >> 8, EcResponse.PdVer.Pd1Ver & 0xFF, (EcResponse.PdVer.Pd1Ver & 0xFF00) >> 8);
  }

  Status =  Ec->Transfer (Ec, EC_COMMAND_GET_PMIC_VERSION, NULL, &EcResponse);
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " PMIC:failed\n");
  } else {
    DebugPrint (DEBUG_INFO, " PMIC: 0x%02x 0x%02x 0x%02x \n", EcResponse.PmicVer.Pmic1Ver, EcResponse.PmicVer.Pmic2Ver, EcResponse.PmicVer.Pmic3Ver);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FwVersionProtocolTestEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS               Status;
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
    return EFI_NOT_FOUND;
  }

  DebugPrint (DEBUG_INFO, "------Begin firmware version dump------\n");
  Status = pFwVerProtocol->GetFwVersion (FwVerSE, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " SE:%s\n", pFwVerBuff);
  } else {
    DebugPrint (DEBUG_INFO, " SE:NULL\n");
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerPM, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " PM:%s\n", pFwVerBuff);
  } else {
    DebugPrint (DEBUG_INFO, " PM:NULL\n");
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerPBL, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " PBL:%s\n", pFwVerBuff);
  } else {
    DebugPrint (DEBUG_INFO, " PBL:NULL\n");
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerATF, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " ATF:%s\n", pFwVerBuff);
  } else {
    DebugPrint (DEBUG_INFO, " ATF:NULL\n");
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerTEE, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " TEE:%s\n", pFwVerBuff);
  } else {
    DebugPrint (DEBUG_INFO, " TEE:NULL\n");
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerUEFI, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " UEFI:%s\n", pFwVerBuff);
  } else {
    DebugPrint (DEBUG_INFO, " UEFI:NULL\n");
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerSTMM, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " STMM:%s\n", pFwVerBuff);
  } else {
    DebugPrint (DEBUG_INFO, " STMM:NULL\n");
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerEC, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " EC:%s\n", pFwVerBuff);
  } else {
    DebugPrint (DEBUG_INFO, " EC:NULL\n");
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerBoardId, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, " Board Id:%x", *pFwVerBuff);
    pBoardId = (EC_RESPONSE_BOARD_ID *)pFwVerBuff;
    Sku      = (pBoardId->Id.SkuExt << 3) + pBoardId->Id.Sku;
    switch (Sku) {
      case 0:
        DebugPrint (DEBUG_INFO, "(MERAK");
        break;
      case 1:
        DebugPrint (DEBUG_INFO, "(MIZAR");
        break;
      case 2:
        DebugPrint (DEBUG_INFO, "(MERAK DAP");
        break;
      case 3:
        DebugPrint (DEBUG_INFO, "(PHECDA SLT");
        break;
      case 4:
        DebugPrint (DEBUG_INFO, "(PHECDA");
        break;
      case 5:
        DebugPrint (DEBUG_INFO, "(MEGREZ");
        break;
      case 6:
        DebugPrint (DEBUG_INFO, "(PHECDA DAP");
        break;
      default:
        break;
    }

    switch (pBoardId->Id.Rev) {
      case 0:
        DebugPrint (DEBUG_INFO, "-Rev A)\n");
        break;
      case 1:
        DebugPrint (DEBUG_INFO, "-Rev B)\n");
        break;
      case 2:
        DebugPrint (DEBUG_INFO, "-Rev C)\n");
        break;
      default:
        break;
    }
  } else {
    DebugPrint (DEBUG_INFO, " Board Id:NULL\n");
  }

  DumpPlatformFwVersions ();

  DebugPrint (DEBUG_INFO, "------End firmware version dump------\n");
  return Status;
}
