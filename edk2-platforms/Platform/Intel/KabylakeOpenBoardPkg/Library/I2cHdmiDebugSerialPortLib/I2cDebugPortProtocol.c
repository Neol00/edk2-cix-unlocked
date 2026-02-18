/** @file
  I2C Debug Port Protocol Implementation

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>

#include <IgfxI2c.h>
#include <Gmbus.h>
#include <I2cDebugPortProtocol.h>

/**
  Writes data to the I2C debug port

  @param[in]  Buffer                      - The data to be written to the I2C debug port.
  @param[in]  Count                       - The number of bytes to write to the I2C debug port.

  @retval  EFI_SUCCESS                    - The data was successfully written.
  @retval  EFI_INVALID_PARAMETER          - One of the following conditions:
                                            * ByteCount is 0
                                            * Buffer is NULL
  @retval  EFI_TIMEOUT                    - The I2C controller did not respond within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the I2C controller.
**/
EFI_STATUS
I2cDebugPortWrite (
  IN  UINT8             *Buffer,
  IN  UINT32            Count
  )
{
  UINT8       WriteBuffer[I2C_DEBUG_PORT_MAX_DATA_SIZE + 1];
  EFI_STATUS  Status;
  UINT32      Index;
  UINT8       CurrentSize;
  UINT8       DdcBusPinPair;

  if (Count <= 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = GetGmbusBusPinPairForI2cDebugPort (&DdcBusPinPair);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  RaiseTplForI2cDebugPortAccess ();
  for (Index = 0; Index < Count; Index += I2C_DEBUG_PORT_MAX_DATA_SIZE) {
    MicroSecondDelay (3000);  //3ms stall to let the BusPirate catch up
    if ((Index + I2C_DEBUG_PORT_MAX_DATA_SIZE) >= Count) {
      CurrentSize = (UINT8) (Count - Index);
    } else {
      CurrentSize = I2C_DEBUG_PORT_MAX_DATA_SIZE;
    }
    WriteBuffer[0]  = (I2C_DEBUG_PORT_WRITE_COMMAND << I2C_DEBUG_PORT_COMMAND_BIT_POSITION) |
                      (CurrentSize & I2C_DEBUG_PORT_DATA_SIZE_BIT_MASK);
    CopyMem (&(WriteBuffer[1]), &(Buffer[Index]), CurrentSize);
    Status = GmbusWrite (DdcBusPinPair, I2C_DEBUG_PORT_WRITE_DEVICE_ADDRESS, TRUE, CurrentSize + 1, &(WriteBuffer[0]));
    if (EFI_ERROR (Status)) {
      break;
    }
  }
  RestoreTplAfterI2cDebugPortAccess ();

  return Status;
}

/**
  Reads data from the I2C debug port

  @param[out]     Buffer                  - The memory buffer to return the read data.
  @param[in, out] Count                   - The number of bytes to read from the I2C debug port.
                                            On output, the number of bytes actually read.

  @retval  EFI_SUCCESS                    - The data was successfully read.
  @retval  EFI_INVALID_PARAMETER          - One of the following conditions:
                                            * ByteCount is 0
                                            * Buffer is NULL
  @retval  EFI_TIMEOUT                    - The I2C controller did not respond within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the I2C controller.
**/
EFI_STATUS
I2cDebugPortRead (
  OUT     UINT8         *Buffer,
  IN OUT  UINT32        *Count
  )
{
  EFI_STATUS  Status;
  UINT32      Index;
  UINT32      BytesRead;
  UINT32      CurrentSize;
  UINT8       DdcBusPinPair;
  UINT8       GmbusIndexData;

  BytesRead = 0;
  if ((*Count) <= 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = GetGmbusBusPinPairForI2cDebugPort (&DdcBusPinPair);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  RaiseTplForI2cDebugPortAccess ();
  for (Index = 0; Index < (*Count); Index += I2C_DEBUG_PORT_MAX_DATA_SIZE) {
    MicroSecondDelay (3000);  //3ms stall to let the BusPirate catch up
    if ((Index + I2C_DEBUG_PORT_MAX_DATA_SIZE) >= (*Count)) {
      CurrentSize = (*Count) - Index;
    } else {
      CurrentSize = I2C_DEBUG_PORT_MAX_DATA_SIZE;
    }
    GmbusIndexData  = (I2C_DEBUG_PORT_READ_COMMAND << I2C_DEBUG_PORT_COMMAND_BIT_POSITION) |
                      (CurrentSize & I2C_DEBUG_PORT_DATA_SIZE_BIT_MASK);
    Status  = GmbusRead (
                DdcBusPinPair,
                I2C_DEBUG_PORT_READ_DEVICE_ADDRESS,
                TRUE,
                TRUE,
                GmbusIndexData,
                &CurrentSize,
                &(Buffer[Index])
                );
    if (EFI_ERROR (Status)) {
      break;
    }
    BytesRead += CurrentSize;
    if (((Index + I2C_DEBUG_PORT_MAX_DATA_SIZE) < (*Count)) && (CurrentSize < I2C_DEBUG_PORT_MAX_DATA_SIZE)) {
      break;
    }
  }
  RestoreTplAfterI2cDebugPortAccess ();

  (*Count) = BytesRead;
  return Status;
}

/**
  Queries the I2C debug port to see if there are any data waiting to be read

  @param[out] NumberOfBytesInFifoBuffer   - The number of bytes sitting in the I2C debug port's
                                            FIFO buffer waiting to be read

  @retval  EFI_SUCCESS                    - The I2C debug port was successfully queried
  @retval  EFI_INVALID_PARAMETER          - One of the following conditions:
                                            * NumberOfBytesInFifoBuffer is NULL
  @retval  EFI_TIMEOUT                    - The I2C controller did not respond within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the I2C controller.
**/
EFI_STATUS
I2cDebugPortReadyToRead (
  OUT     UINT8         *NumberOfBytesInFifoBuffer
  )
{
  EFI_STATUS  Status;
  UINT32      BytesRead;
  UINT8       DdcBusPinPair;
  UINT8       GmbusIndexData;

  BytesRead = 1;
  if (NumberOfBytesInFifoBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = GetGmbusBusPinPairForI2cDebugPort (&DdcBusPinPair);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  MicroSecondDelay (3000);  //3ms stall to let the BusPirate catch up
  GmbusIndexData  = (I2C_DEBUG_PORT_READY_TO_READ_COMMAND << I2C_DEBUG_PORT_COMMAND_BIT_POSITION) |
                    (1 & I2C_DEBUG_PORT_DATA_SIZE_BIT_MASK); //READY_TO_READ always returns 1 byte
  RaiseTplForI2cDebugPortAccess ();
  Status = GmbusRead (DdcBusPinPair, I2C_DEBUG_PORT_READ_DEVICE_ADDRESS, TRUE, TRUE, GmbusIndexData, &BytesRead, NumberOfBytesInFifoBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  RestoreTplAfterI2cDebugPortAccess ();
  if (BytesRead != 1) {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}
