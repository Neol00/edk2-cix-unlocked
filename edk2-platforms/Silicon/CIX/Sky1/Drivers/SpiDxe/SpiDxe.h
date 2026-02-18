/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2023-2024, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SPI_DXE_H__
#define __SPI_DXE_H__

#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SpiLib.h>
#include <Library/CixPostCodeLib.h>
#include <Protocol/Spi.h>
#include <Protocol/ConfigParamsManageProtocol.h>
#include "Protocol/ClockId.h"
#include "IoConfig.h"
#include "MemoryMap.h"

typedef struct {
  UINTN                Signature;
  EFI_HANDLE           Handle;
  SPI_HOST_PROTOCOL    HostProtocol;
  SPI_DEVICE_PATH      DevicePath;
  UINTN                BaseAddr;
  UINTN                ClockRate;
  EFI_LOCK             Lock;
} SPI_HOST;

#endif // __SPI_DXE_H__
