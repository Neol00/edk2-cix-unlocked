/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/I2cLib.h>
#include <Library/EcLib.h>
#include <Library/TimerLib.h>
#include <Library/Ra8900ceRealTimeClockLib.h>
#include <Library/PlatformConfigParamsHookLib.h>

#define  RTC_RETRY_MAX  50

STATIC I2C_HOST_DESCRIPTOR  mHost;

EFI_STATUS
EFIAPI
PlatformConfigParamsHookLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  mHost.Bus     = FixedPcdGet8 (PcdRa8900ceI2cBus);
  mHost.MemBase = I2cGetMemBase (mHost.Bus);
  mHost.BusClk  = I2cGetBusClk (mHost.Bus);

  I2cEnvInit (mHost.Bus);
  Status = I2cInitialize (&mHost);

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
  UINT8                   Flag[2]        = { RA8900CE_FLAG_REG_OFFSET, 0 };
  UINT8                   Control[2]     = { RA8900CE_CTRL_REG_OFFSET, 0 };
  UINT8                   Time[8]        = { RA8900CE_DATA_REG_OFFSET, 0, 0, 0, 0x20, 0x15, 0x10, 0x21 };
  UINT8                   BackUp[2]      = { RA8900CE_BACKUP_REG_OFFSET, RA8900CE_BACKUP_REG_VDETOFF | RA8900CE_BACKUP_REG_SWOFF };
  UINT8                   Retry          = 0;

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
  RequestPacket->Operation[0].LengthInBytes = sizeof (Flag[0]);
  RequestPacket->Operation[0].Buffer        = (UINT8 *)&Flag[0];
  RequestPacket->Operation[1].Flags         = I2C_FLAG_READ;
  RequestPacket->Operation[1].LengthInBytes = sizeof (Flag[1]);
  RequestPacket->Operation[1].Buffer        = (UINT8 *)&Flag[1];

  Status = I2cMasterXfer (&mHost, FixedPcdGet8 (PcdRa8900ceI2cSlaveAddress), RequestPacket);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to get RTC info, status %r\n", __FUNCTION__, Status));
    goto EndReturn;
  }

  if (Flag[1] & RA8900CE_FLAG_REG_VLF) {
    DEBUG ((DEBUG_INFO, "%a: RTC voltage dropped\n", __FUNCTION__));
    *Dropped                                  = TRUE;
    Flag[1]                                  &= ~RA8900CE_FLAG_REG_VLF;
    RequestPacket->OperationCount             = I2C_WR_OP_COUNT;
    RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
    RequestPacket->Operation[0].LengthInBytes = sizeof (Flag);
    RequestPacket->Operation[0].Buffer        = (UINT8 *)&Flag[0];

    Status = I2cMasterXfer (&mHost, FixedPcdGet8 (PcdRa8900ceI2cSlaveAddress), RequestPacket);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: fail to clear RTC VLF flag, status %r\n", __FUNCTION__, Status));
      goto EndReturn;
    }

    Control[1]                                = RA8900CE_CTRL_REG_RESET;
    RequestPacket->OperationCount             = I2C_WR_OP_COUNT;
    RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
    RequestPacket->Operation[0].LengthInBytes = sizeof (Control);
    RequestPacket->Operation[0].Buffer        = (UINT8 *)&Control[0];

    Status = I2cMasterXfer (&mHost, FixedPcdGet8 (PcdRa8900ceI2cSlaveAddress), RequestPacket);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: fail to reset RTC, status %r\n", __FUNCTION__, Status));
      goto EndReturn;
    }

    while ((Control[1] & RA8900CE_CTRL_REG_RESET) && (Retry < RTC_RETRY_MAX)) {
      MicroSecondDelay (1);
      RequestPacket->OperationCount             = I2C_WR_THEN_RD_OP_COUNT;
      RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
      RequestPacket->Operation[0].LengthInBytes = sizeof (Control[0]);
      RequestPacket->Operation[0].Buffer        = (UINT8 *)&Control[0];
      RequestPacket->Operation[1].Flags         = I2C_FLAG_READ;
      RequestPacket->Operation[1].LengthInBytes = sizeof (Control[1]);
      RequestPacket->Operation[1].Buffer        = (UINT8 *)&Control[1];

      Status = I2cMasterXfer (&mHost, FixedPcdGet8 (PcdRa8900ceI2cSlaveAddress), RequestPacket);
      Retry++;
    }

    RequestPacket->OperationCount             = I2C_WR_OP_COUNT;
    RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
    RequestPacket->Operation[0].LengthInBytes = sizeof (BackUp);
    RequestPacket->Operation[0].Buffer        = (UINT8 *)&BackUp[0];

    Status = I2cMasterXfer (&mHost, FixedPcdGet8 (PcdRa8900ceI2cSlaveAddress), RequestPacket);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: fail to turn off backup function, status %r\n", __FUNCTION__, Status));
      goto EndReturn;
    }

    RequestPacket->OperationCount             = I2C_WR_OP_COUNT;
    RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
    RequestPacket->Operation[0].LengthInBytes = sizeof (Time);
    RequestPacket->Operation[0].Buffer        = (UINT8 *)&Time[0];

    Status = I2cMasterXfer (&mHost, FixedPcdGet8 (PcdRa8900ceI2cSlaveAddress), RequestPacket);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: fail to set time to RTC, status %r\n", __FUNCTION__, Status));
      goto EndReturn;
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
  EFI_STATUS            Status = EFI_SUCCESS;
  EC_RESPONSE_BOARD_ID  BoardIdInfo;

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
