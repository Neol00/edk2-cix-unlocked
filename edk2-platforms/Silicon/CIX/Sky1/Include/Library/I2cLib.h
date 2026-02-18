/** @file
  I2c Lib to control I2c controller.

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef I2C_LIB_H__
#define I2C_LIB_H__

#include <Uefi.h>
#include <Pi/PiI2c.h>
#include "IoConfig.h"
#include "MemoryMap.h"

#define I2C0_MMIO_BASE  I2C0_BASE
#define I2C1_MMIO_BASE  I2C1_BASE
#define I2C2_MMIO_BASE  I2C2_BASE
#define I2C3_MMIO_BASE  I2C3_BASE
#define I2C4_MMIO_BASE  I2C4_BASE
#define I2C5_MMIO_BASE  I2C5_BASE
#define I2C6_MMIO_BASE  I2C6_BASE
#define I2C7_MMIO_BASE  I2C7_BASE
#define I2C_MMIO_SIZE   0x14
#define I2C_CTRL_NUM    8

#define I2C_FLAG_WRITE           0
#define I2C_WR_OP_COUNT          1
#define I2C_RD_OP_COUNT          1
#define I2C_WR_THEN_RD_OP_COUNT  2

extern BOOLEAN  mI2cLibAtRuntime;

typedef struct _I2C_HOST_DESCRIPTOR {
  UINT8     Bus;
  UINTN     MemBase;
  UINT32    ErrStatus;
  UINT8     *SendBuf;
  UINT8     *RecvBuf;
  UINT32    SendCount;
  UINT32    RecvCount;
  UINT32    BusClk;
} I2C_HOST_DESCRIPTOR;

EFI_STATUS
I2cEnvInit (
  IN UINT8  Bus
  );

UINTN
I2cGetMemBase (
  IN UINT8  Bus
  );

UINT32
I2cGetBusClk (
  IN UINT8  Bus
  );

/**
  software reset of the entire I2C module.
  The module is reset.
  Status register fields are cleared.

  @param[in] Base       Base Address of I2c controller's registers

  @return  EFI_SUCCESS  successfuly reset the I2c module
**/
EFI_STATUS
I2cReset (
  IN UINTN  Base
  );

EFI_STATUS
I2cSetClk (
  IN I2C_HOST_DESCRIPTOR  *Host
  );

/**
  Configure I2c bus to operate at a given speed

  @param[in] Base         Descriptor of I2c controller

  @return  EFI_SUCCESS  successfuly setup the I2C bus
**/
EFI_STATUS
I2cInitialize (
  IN I2C_HOST_DESCRIPTOR  *Host
  );

/**
  Transfer data to/from I2c slave device

  @param[in] Host           Descriptor of I2c controller
  @param[in] SlaveAddress   Slave Address from which data is to be read
  @param[in] RequestPacket  Pointer to an EFI_I2C_REQUEST_PACKET structure
                            describing the I2C transaction

  @return  EFI_SUCCESS       successfuly transfer the data
  @return  EFI_DEVICE_ERROR  There was an error while transferring data through
                             I2c bus
  @return  EFI_NO_RESPONSE   There was no Ack from i2c device
  @return  EFI_TIMEOUT       I2c Bus is busy
  @return  EFI_NOT_READY     I2c Bus Arbitration lost
**/
EFI_STATUS
I2cMasterXfer (
  IN I2C_HOST_DESCRIPTOR     *Host,
  IN UINT32                  SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET  *RequestPacket
  );

#endif // I2C_LIB_H__
