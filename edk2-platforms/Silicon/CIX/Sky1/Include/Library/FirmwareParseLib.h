/** @file  FirmwareParseLib.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022-2023, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FIRMWARE_PARSE_LIB_H__
#define __FIRMWARE_PARSE_LIB_H__

#include <Base.h>
#include <PiDxe.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include "FirmwareLayout.h"

EFI_STATUS
EFIAPI
FindFirmwareRawData (
  IN  FIRMWARE_TYPE  Type,
  OUT UINT32         **Location,
  OUT UINTN          *Length
  );

EFI_STATUS
EFIAPI
GetFlashConfig (
  IN  UINT32       JedecId,
  OUT XSPI_CONFIG  **Configs
  );

EFI_STATUS
EFIAPI
FindFirmwareEntryInfo (
  IN  UINT8*        pImageBuff,
  IN  UINT32*       ImageLength,
  IN  FIRMWARE_TYPE FwType,
  OUT UINT32*       FwBase,
  OUT UINT32*       FwLength
  );

#endif /* __FIRMWARE_PARSE_LIB_H__ */
