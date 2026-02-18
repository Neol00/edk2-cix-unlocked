/** @file
  I2C Debug Port Protocol Implementation

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>

#define I2C_DEBUG_PORT_WRITE_DEVICE_ADDRESS 0xDE
#define I2C_DEBUG_PORT_READ_DEVICE_ADDRESS 0xDF

#define I2C_DEBUG_PORT_WRITE_COMMAND 0x00
#define I2C_DEBUG_PORT_READ_COMMAND 0x01
#define I2C_DEBUG_PORT_READY_TO_READ_COMMAND 0x02

#define I2C_DEBUG_PORT_COMMAND_BIT_POSITION 5
#define I2C_DEBUG_PORT_COMMAND_BIT_MASK 0x7
#define I2C_DEBUG_PORT_DATA_SIZE_BIT_MASK 0x1F
#define I2C_DEBUG_PORT_MAX_DATA_SIZE  I2C_DEBUG_PORT_DATA_SIZE_BIT_MASK

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
  );

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
  );

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
  );
