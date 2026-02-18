/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PdLib.h>
#include <Protocol/I2cMaster.h>
#include <Protocol/I2cDevicePath.h>
#include <Pi/PiI2c.h>

#define I2C_FLAG_WRITE           0
#define I2C_WR_THEN_RD_OP_COUNT  2

#define BYTE1_DATA_CONNECTION_PRESENT  BIT0
#define BYTE1_CONNECTION_ORIENTATION   BIT1
#define BYTE1_USB_3_2_CONNECTION       BIT5
#define BYTE2_DP_CONNECTION            BIT0

UINT8  *pPdDevBuses        = PcdGetPtr (PcdPdDevI2cBuses);
UINT8  *pPdDevSlaveAddress = PcdGetPtr (PcdPdDevI2cSlaveAddresses);

EFI_STATUS
I2C_Block_Read (
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster,
  UINTN                    SlaveAddress,
  UINT8                    *WriteData,
  UINT32                   WiteLength,
  UINT8                    *ReadData,
  UINT32                   ReadLength
  )
{
  EFI_STATUS              Status         = EFI_SUCCESS;
  EFI_I2C_REQUEST_PACKET  *RequestPacket = NULL;

  RequestPacket                 = (EFI_I2C_REQUEST_PACKET *)AllocateZeroPool (sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION));
  RequestPacket->OperationCount = I2C_WR_THEN_RD_OP_COUNT;

  RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
  RequestPacket->Operation[0].LengthInBytes = WiteLength;
  RequestPacket->Operation[0].Buffer        = WriteData;

  RequestPacket->Operation[1].Flags         = I2C_FLAG_READ;
  RequestPacket->Operation[1].LengthInBytes = ReadLength;
  RequestPacket->Operation[1].Buffer        = ReadData;

  Status = I2cMaster->StartRequest (I2cMaster, SlaveAddress, (void *)RequestPacket, NULL, NULL);

  if (RequestPacket) {
    FreePool (RequestPacket);
  }

  return Status;
}

EFI_STATUS
I2C_Block_Write (
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster,
  UINTN                    SlaveAddress,
  UINT8                    *WriteData,
  UINT32                   WiteLength
  )
{
  EFI_STATUS              Status         = EFI_SUCCESS;
  EFI_I2C_REQUEST_PACKET  *RequestPacket = NULL;

  RequestPacket                 = (EFI_I2C_REQUEST_PACKET *)AllocateZeroPool (sizeof (EFI_I2C_REQUEST_PACKET));
  RequestPacket->OperationCount = 1;

  RequestPacket->Operation[0].Flags         = I2C_FLAG_WRITE;
  RequestPacket->Operation[0].LengthInBytes = WiteLength;
  RequestPacket->Operation[0].Buffer        = WriteData;

  Status = I2cMaster->StartRequest (I2cMaster, SlaveAddress, (void *)RequestPacket, NULL, NULL);

  if (RequestPacket) {
    FreePool (RequestPacket);
  }

  return Status;
}

EFI_STATUS
GetPdDevConfig (
  UINT8  PdDevIdx,
  UINT8  *I2cBusId,
  UINT8  *SlaveAddress
  )
{
  if (!pPdDevBuses || !pPdDevSlaveAddress) {
    return EFI_INVALID_PARAMETER;
  }

  *I2cBusId     = pPdDevBuses[PdDevIdx];
  *SlaveAddress = pPdDevSlaveAddress[PdDevIdx];

  if ((*I2cBusId == 0xFF) || (*SlaveAddress == 0xFF)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

BOOLEAN
IsPdDeviceValid (
  UINT8  PdDevIdx
  )
{
  EFI_STATUS  Status       = EFI_SUCCESS;
  UINT8       I2cBus       = 0xFF;
  UINT8       SlaveAddress = 0xFF;

  Status = GetPdDevConfig (PdDevIdx, &I2cBus, &SlaveAddress);

  DEBUG ((
    DEBUG_INFO,
    "Pd Device %d get device config, I2C bus: %x, Slave Address:%x\n",
    PdDevIdx,
    I2cBus,
    SlaveAddress
    ));

  return Status == EFI_SUCCESS;
}

EFI_STATUS
GetI2cDeviceHandle (
  IN  UINT8          I2cBus,
  IN OUT EFI_HANDLE  *DevHandle
  )
{
  EFI_STATUS                Status        = EFI_SUCCESS;
  UINTN                     HandleCount   = 0;
  EFI_HANDLE                *HandleBuffer = NULL;
  UINT32                    Index;
  EFI_DEVICE_PATH           *DevicePath;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiI2cMasterProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "I2c Bus %d Locate Handle Buffer %g fail, status %r\n",
      I2cBus,
      &gEfiI2cMasterProtocolGuid,
      Status
      ));
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    DevicePath = DevicePathFromHandle (HandleBuffer[Index]);
    if (!DevicePath) {
      continue;
    }

    if ((DevicePath->Type != HARDWARE_DEVICE_PATH) || (DevicePath->SubType != HW_VENDOR_DP)) {
      continue;
    }

    if (!CompareGuid (&(((VENDOR_DEVICE_PATH *)DevicePath)->Guid), &gCixI2cDevicePathGuid)) {
      continue;
    }

    if (((I2C_DEVICE_PATH*)DevicePath)->Bus == I2cBus) {
      *DevHandle = HandleBuffer[Index];
      break;
    }
  }

  if (Index == HandleCount) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: I2C bus %d is not found\n",
      __FUNCTION__,
      I2cBus
      ));
    Status = EFI_NOT_FOUND;
  }

  if (HandleBuffer) {
    FreePool (HandleBuffer);
  }

  return Status;
}

EFI_STATUS
EFIAPI
PdGetPortState (
  IN      UINT8             PdDevIdx,
  IN OUT  TYPEC_PORT_STATE  *PortState
  )
{
  EFI_STATUS               Status       = EFI_SUCCESS;
  UINT8                    I2cBus       = 0;
  UINT8                    SlaveAddress = 0;
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster   = NULL;
  EFI_HANDLE               I2cDevHandle;

  Status = GetPdDevConfig (PdDevIdx, &I2cBus, &SlaveAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Pd Device %d is not supported\n",
      __FUNCTION__,
      PdDevIdx
      ));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((
    DEBUG_INFO,
    "Pd Device %d I2C Bus %d, Slave Address %x, read data status\n",
    PdDevIdx,
    I2cBus,
    SlaveAddress
    ));

  Status = GetI2cDeviceHandle (I2cBus, &I2cDevHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d get I2c Bus %d device handle %r\n",
      I2cBus,
      Status
      ));
    return Status;
  }

  Status = gBS->HandleProtocol (I2cDevHandle, &gEfiI2cMasterProtocolGuid, (VOID **)&I2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d Handle Protocol %g status %r\n",
      PdDevIdx,
      &gEfiI2cMasterProtocolGuid,
      Status
      ));
    return Status;
  }

  UINT8  wdata[1] = { 0x5F };
  UINT8  rdata[6] = { 0 };

  Status = I2C_Block_Read (I2cMaster, SlaveAddress, wdata, 1, rdata, 6);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d read pd data status %r\n",
      PdDevIdx,
      Status
      ));
    return Status;
  }

  DEBUG ((
    DEBUG_INFO,
    "Pd Device %d read pd data status, byte1: %x, byte2: %x\n",
    PdDevIdx,
    rdata[1],
    rdata[2]
    ));

  if (!(rdata[1] & BYTE1_USB_3_2_CONNECTION) && !(rdata[2] & BYTE2_DP_CONNECTION)) {
    PortState->Orientation = TYPEC_ORIENTATION_NONE;
  } else {
    if (rdata[1] & BYTE1_CONNECTION_ORIENTATION) {
      PortState->Orientation = TYPEC_ORIENTATION_REVERSE;
    } else {
      PortState->Orientation = TYPEC_ORIENTATION_NORMAL;
    }
  }

  if ((rdata[1] & BYTE1_USB_3_2_CONNECTION) && (rdata[2] & BYTE2_DP_CONNECTION)) {
    // usb+dp
    PortState->Mode = TYPEC_DP_STATE_D;
  } else if (rdata[1] & BYTE1_USB_3_2_CONNECTION) {
    // usb only
    PortState->Mode = TYPEC_STATE_USB;
  } else if (rdata[2] & BYTE2_DP_CONNECTION) {
    // dp only
    PortState->Mode = TYPEC_DP_STATE_E;
  } else {
    if (!(rdata[1] & BYTE1_DATA_CONNECTION_PRESENT)) {
      PortState->Mode = TYPEC_STATE_DISCONNECT;
    } else {
      // disconnect
      PortState->Mode = TYPEC_STATE_SAFE;
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
PdEnterAlertInterrupt (
  IN  UINT8  PdDevIdx
  )
{
  EFI_STATUS               Status       = EFI_SUCCESS;
  UINT8                    I2cBus       = 0;
  UINT8                    SlaveAddress = 0;
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster   = NULL;
  EFI_HANDLE               I2cDevHandle;

  Status = GetPdDevConfig (PdDevIdx, &I2cBus, &SlaveAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Pd Device %d is not supported\n",
      __FUNCTION__,
      PdDevIdx
      ));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((
    DEBUG_INFO,
    "Pd Device %d, I2C Bus %d, Slave Address %x, set cpu ack for interrupt\n",
    PdDevIdx,
    I2cBus,
    SlaveAddress
    ));

  Status = GetI2cDeviceHandle (I2cBus, &I2cDevHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d get I2c Bus %d device path guid %r\n",
      PdDevIdx,
      I2cBus,
      Status
      ));
    return Status;
  }

  Status = gBS->HandleProtocol (I2cDevHandle, &gEfiI2cMasterProtocolGuid, (VOID **)&I2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d handle Protocol %g status %r\n",
      PdDevIdx,
      &gEfiI2cMasterProtocolGuid,
      Status
      ));
    return Status;
  }

  UINT8  wdata[4] = { 0x50, 2, BIT2, 0 };

  Status = I2C_Block_Write (I2cMaster, SlaveAddress, wdata, 4);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d write cpu ack for i2c interrupt status %r\n",
      PdDevIdx,
      Status
      ));
    return Status;
  }

  return Status;
}

EFI_STATUS
EFIAPI
PdExitAlertInterrupt (
  IN  UINT8  PdDevIdx
  )
{
  EFI_STATUS               Status       = EFI_SUCCESS;
  UINT8                    I2cBus       = 0;
  UINT8                    SlaveAddress = 0;
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster   = NULL;
  EFI_HANDLE               I2cDevHandle;
  UINT8                    wdata[4] = { 0x5F };
  UINT8                    rdata[6] = { 0 };

  Status = GetPdDevConfig (PdDevIdx, &I2cBus, &SlaveAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Pd Device %d is not supported\n",
      __FUNCTION__,
      PdDevIdx
      ));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((
    DEBUG_INFO,
    "Pd Device %d, I2C Bus %d, Slave Address %x, set ack of HPD IRQ\n",
    PdDevIdx,
    I2cBus,
    SlaveAddress
    ));

  Status = GetI2cDeviceHandle (I2cBus, &I2cDevHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d get I2c Bus %d device path guid %r\n",
      PdDevIdx,
      I2cBus,
      Status
      ));
    return Status;
  }

  Status = gBS->HandleProtocol (I2cDevHandle, &gEfiI2cMasterProtocolGuid, (VOID **)&I2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d handle Protocol %g status %r\n",
      PdDevIdx,
      &gEfiI2cMasterProtocolGuid,
      Status
      ));
    return Status;
  }

  Status = I2C_Block_Read (I2cMaster, SlaveAddress, wdata, 1, rdata, 6);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d read pd data status %r\n",
      PdDevIdx,
      Status
      ));
    return Status;
  }

  DEBUG ((
    DEBUG_INFO,
    "Pd Device %d Read data status byte2: %x\n",
    PdDevIdx,
    rdata[2]
    ));

  // check id HPD_IRQ is set in data status
  if (!(rdata[2] & 0x40)) {
    // if not, skip
    DEBUG ((
      DEBUG_INFO,
      "Pd Device %d IRQ_HPD flag is not set, skip set ack of HPD IRQ\n",
      PdDevIdx
      ));

    return Status;
  }

  wdata[0] = 0x50;
  wdata[1] = 2;
  wdata[2] = 0;
  wdata[3] = BIT5; // Clear HPD_IRQ in data control
  Status   = I2C_Block_Write (I2cMaster, SlaveAddress, wdata, 4);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Pd Device %d write ack for HPD_IRQ fail, status %r\n",
      PdDevIdx,
      Status
      ));
    return Status;
  }

  return Status;
}
