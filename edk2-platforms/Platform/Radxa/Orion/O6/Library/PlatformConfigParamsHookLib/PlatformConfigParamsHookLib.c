/**

  Copyright 2024 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
  Copyright 2023 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.
  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/I2cLib.h>
#include <Library/EcLib.h>
#include <Library/TimerLib.h>
#include <Library/PlatformConfigParamsHookLib.h>
#include "Pcf8563.h"

STATIC I2C_HOST_DESCRIPTOR  mHost;

EFI_STATUS
EFIAPI
PlatformConfigParamsHookLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  mHost.Bus     = FixedPcdGet8 (PcdRtc8563I2cBus);
  mHost.MemBase = I2cGetMemBase (mHost.Bus);
  mHost.BusClk  = I2cGetBusClk (mHost.Bus);

  I2cEnvInit (mHost.MemBase);
  Status = I2cInitialize (&mHost);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to initialize I2C bus %u (0x%p) at clock %u, status %r\n", __FUNCTION__, mHost.Bus, mHost.MemBase, mHost.BusClk, Status));
  } else {
    DEBUG ((DEBUG_INFO, "%a: initialize I2C bus %u (0x%p) at clock %u\n", __FUNCTION__, mHost.Bus, mHost.MemBase, mHost.BusClk));
  }

  return Status;
}

STATIC
EFI_STATUS
GetRtcVoltDropFlag (
  OUT BOOLEAN  *Dropped
  )
{
  EFI_STATUS              Status         = EFI_SUCCESS;
  EFI_I2C_REQUEST_PACKET  *RequestPacket = NULL;
  UINT8                   RtcTime[]      = { PCF8563_DATA_REG_OFFSET,
                                             DecimalToBcd8(0),
                                             DecimalToBcd8(0),
                                             DecimalToBcd8(0),
                                             DecimalToBcd8(30),
                                             4,
                                             DecimalToBcd8(8),
                                             DecimalToBcd8(24) };

  if (Dropped == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RequestPacket = (EFI_I2C_REQUEST_PACKET *)AllocateZeroPool (sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION));

  if (RequestPacket == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((DEBUG_ERROR, "%a: fail to allocate buffer, status %r\n", __FUNCTION__, Status));
    goto EndReturn;
  }

  RequestPacket->OperationCount             = I2C_WR_THEN_RD_OP_COUNT;
  RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
  RequestPacket->Operation[0].LengthInBytes = sizeof (RtcTime[0]);
  RequestPacket->Operation[0].Buffer        = &RtcTime[0];
  RequestPacket->Operation[1].Flags         = I2C_FLAG_READ;
  RequestPacket->Operation[1].LengthInBytes = sizeof (RtcTime[1]);
  RequestPacket->Operation[1].Buffer        = &RtcTime[1];

  Status = I2cMasterXfer (&mHost, FixedPcdGet8 (PcdI2cSlaveAddress), RequestPacket);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to get RTC info, status %r\n", __FUNCTION__, Status));
    goto EndReturn;
  } else {
    DEBUG ((DEBUG_INFO, "%a: RTC VL_seconds: 0x%x\n", __FUNCTION__, RtcTime[1]));
  }

  if (RtcTime[1] & PCF8563_CLOCK_INVALID) {
    DEBUG ((DEBUG_INFO, "%a: RTC voltage dropped\n", __FUNCTION__));
    *Dropped                                  = TRUE;

    RtcTime[1] = DecimalToBcd8(0);
    RequestPacket->OperationCount             = I2C_WR_OP_COUNT;
    RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
    RequestPacket->Operation[0].LengthInBytes = sizeof (RtcTime);
    RequestPacket->Operation[0].Buffer        = RtcTime;

    Status = I2cMasterXfer (&mHost, FixedPcdGet8 (PcdI2cSlaveAddress), RequestPacket);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: fail to set time to RTC, status %r\n", __FUNCTION__, Status));
      goto EndReturn;
    } else {
      DEBUG ((DEBUG_INFO, "%a: reset RTC to default time\n", __FUNCTION__));
    }
  } else {
    DEBUG ((DEBUG_INFO, "%a: RTC voltage no drop\n", __FUNCTION__));
    *Dropped = FALSE;
  }

EndReturn:
  if (RequestPacket) {
    FreePool ((VOID *)RequestPacket);
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetVoltDropFlag (
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  BOOLEAN     RtcVoltDrop;

  Status = GetRtcVoltDropFlag (&RtcVoltDrop);
  if (!EFI_ERROR (Status)) {
    ConfigData->RtcVoltDrop = RtcVoltDrop;
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetBoardId (
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS                 Status = EFI_SUCCESS;
  EC_RESPONSE_BOARD_ID       BoardIdInfo;

  DEBUG ((DEBUG_ERROR, "%a: enter \n", __FUNCTION__));
  Status = GetBoardId (&BoardIdInfo);

  if (!EFI_ERROR (Status)) {
    ConfigData->BoardId = BoardIdInfo.Value;
  }

  DEBUG ((DEBUG_ERROR, "%a: exit \n", __FUNCTION__));
  return Status;
}

STATIC PLATFORM_CONFIG_PARAMS_INIT_FUNC  mPlatformConfigParamsInitTable[] = {
  SetBoardId,
  SetVoltDropFlag,
  // add initializatrion routines for platform configuration parameters BEFORE this line, and they were invoked from top to down.
  NULL,
};

STATIC UINT32  mPlatformConfigParamsInitTableSize = ARRAY_SIZE (mPlatformConfigParamsInitTable);

VOID
EFIAPI
PlatformConfigParamsHook (
  IN OUT PLATFORM_CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      Index  = 0;

  if (ConfigData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: config params hook routine failed to get config data\n", __FUNCTION__));
    return;
  }

  while (Index < mPlatformConfigParamsInitTableSize) {
    if (mPlatformConfigParamsInitTable[Index] != NULL) {
      Status = mPlatformConfigParamsInitTable[Index](ConfigData);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: config params hook routine failed at index %d, status %r\n", __FUNCTION__, Index, Status));
      }
    }

    Index++;
  }
}
