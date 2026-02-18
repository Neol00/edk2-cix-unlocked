/** @file
  GMBUS I/O Registers and Functions

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
  - In general, SA registers are denoted by "_SA_" in register names
  - Registers / bits that are different between SA generations are denoted by
    "_SA_[generation_name]_" in register/bit names. e.g., "_SA_HSW_"
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SA generation will be just named
    as "_SA_" without [generation_name] inserted.
**/

#include <Uefi.h>
#include <SaRegs.h>

#define SA_SEG_NUM                                        0x0000000
#define R_SA_GTTMMADR_GMBUS0_CLKPRTSEL                    0x00C5100
#define B_SA_GTTMMADR_GMBUS0_PIN_PAIR_MASK                0x0000007
#define R_SA_GTTMMADR_GMBUS1_CMDSTS                       0x00C5104
#define B_SA_GTTMMADR_GMBUS1_SW_CLR_INT                   BIT31
#define B_SA_GTTMMADR_GMBUS1_SW_RDY                       BIT30
#define B_SA_GTTMMADR_GMBUS1_EN_TIMEOUT                   BIT29
#define B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_MASK           (BIT27 | BIT26 | BIT25)
/*
BUS_CYCLE_SEL Decoder Table
===========================
Value | Name                    | Description
------+-------------------------+------------------------------------------------------------------------------------
000b  | No cycle                | No GMBUS cycle is generated
001b  | No Index, No Stop, Wait | GMBUS cycle is generated without an INDEX, with no STOP, and ends with a WAIT
010b  | Reserved                | Reserved
011b  | Index, No Stop, Wait    | GMBUS cycle is generated with an INDEX, with no STOP, and ends with a WAIT
100b  | Gen Stop                | Generates a STOP if currently in a WAIT or after the completion of the current byte
101b  | No Index, Stop          | GMBUS cycle is generated without an INDEX and with a STOP
110b  | Reserved                | Reserved
111b  | Index, Stop             | GMBUS cycle is generated with an INDEX and with a STOP All
*/
#define B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_STOP           BIT27
#define B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_INDEX          BIT26
#define B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_START          BIT25
#define B_SA_GTTMMADR_GMBUS1_TOTAL_BYTE_COUNT_MASK        0x1FF0000
#define N_SA_GTTMMADR_GMBUS1_TOTAL_BYTE_COUNT             16
#define B_SA_GTTMMADR_GMBUS1_INDEX_MASK                   0x000FF00
#define N_SA_GTTMMADR_GMBUS1_INDEX                        8
#define R_SA_GTTMMADR_GMBUS2_STATUS                       0x00C5108
#define B_SA_GTTMMADR_GMBUS2_INUSE                        BIT15
#define B_SA_GTTMMADR_GMBUS2_SLAVE_STALL_TIMEOUT_ERROR    BIT13
#define B_SA_GTTMMADR_GMBUS2_HW_RDY                       BIT11
#define B_SA_GTTMMADR_GMBUS2_NACK_INDICATOR               BIT10
#define B_SA_GTTMMADR_GMBUS2_BUS_ACTIVE                   BIT9
#define R_SA_GTTMMADR_GMBUS3_DATA                         0x00C510C

#define GMBUS_MAX_BYTES                                   (0x1FF - 0x8)   //9-bits minus 8 bytes

#define GMBUS_CLOCK_RATE_100K                             0x0000000
#define GMBUS_CLOCK_RATE_50K                              0x0000001
#define GMBUS_CLOCK_RATE_400K                             0x0000002
#define GMBUS_CLOCK_RATE_1M                               0x0000003

#define GMBUS_TIMEOUT                                     0x00249F0

/**
  Gets the GttMmAdr BAR value

  @retval  The current value of the GTTMMADR BAR
**/
UINTN
GmbusGetGttMmAdr (
  VOID
  );

/**
  Reset Bus Master and Memory access on the IGD device to the initial state at
  the start of the current transaction.
**/
VOID
GmbusResetBusMaster (
  VOID
  );

/**
  Returns a flag indicating whether the IGD device bus master enable needs to
  be disabled at the end of the current transaction

  @retval  TRUE                           - IGD Bus Master Enable needs to be reset
  @retval  FALSE                          - IGD Bus Master Enable does not need to be reset
**/
BOOLEAN
GetIgdBusMasterReset (
  VOID
  );

/**
  Sets a flag indicating whether the IGD device bus master enable needs to
  be disabled at the end of the current transaction

  @param[in]  IgdBusMasterReset           - IGD device bus master enable flag
**/
VOID
SetIgdBusMasterReset (
  BOOLEAN IgdBusMasterReset
  );

/**
  Writes to the GMBUS0 register (Clock/Port Select)

  @param[in]  GmbusClkPrtSel              - The value to write to GMBUS0

  @retval  EFI_SUCCESS                    - GMBUS0 was successfully written.
  @retval  EFI_DEVICE_ERROR               - An error occurred while writting to GMBUS0
**/
EFI_STATUS
SetGmbus0ClockPortSelect (
  IN  UINT32             GmbusClkPrtSel
  );

/**
  Writes to the GMBUS1 register (Command/Status)

  @param[in]  GmbusCmdSts                 - The value to write to GMBUS1

  @retval  EFI_SUCCESS                    - GMBUS1 was successfully written.
  @retval  EFI_DEVICE_ERROR               - An error occurred while writting to GMBUS1
**/
EFI_STATUS
SetGmbus1Command (
  IN  UINT32             GmbusCmdSts
  );

/**
  Reads from the GMBUS2 register (GMBUS Status)

  @param[out]  GmbusStatus                - The value read from GMBUS2

  @retval  EFI_SUCCESS                    - GMBUS2 was successfully read.
  @retval  EFI_DEVICE_ERROR               - An error occurred while reading from GMBUS2
**/
EFI_STATUS
GetGmbus2Status (
  OUT  UINT32             *GmbusStatus
  );

/**
  Writes to the GMBUS2 register (GMBUS Status)

  @param[in]  GmbusStatus                 - The value to write to GMBUS2

  @retval  EFI_SUCCESS                    - GMBUS2 was successfully written.
  @retval  EFI_DEVICE_ERROR               - An error occurred while writting to GMBUS2
**/
EFI_STATUS
SetGmbus2Status (
  IN  UINT32             GmbusStatus
  );

/**
  Reads from the GMBUS3 register (GMBUS Data Buffer)

  @param[out]  GmbusData                  - The value read from GMBUS3

  @retval  EFI_SUCCESS                    - GMBUS2 was successfully read.
  @retval  EFI_DEVICE_ERROR               - An error occurred while reading from GMBUS2
**/
EFI_STATUS
GetGmbus3Data (
  OUT  UINT32             *GmbusData
  );

/**
  Writes to the GMBUS3 register (GMBUS Data Buffer)

  @param[in]  GmbusData                   - The value to write to GMBUS3

  @retval  EFI_SUCCESS                    - GMBUS3 was successfully written.
  @retval  EFI_DEVICE_ERROR               - An error occurred while writting to GMBUS3
**/
EFI_STATUS
SetGmbus3Data (
  IN  UINT32             GmbusData
  );

/**
  Set and clear the software clear interrupt bit. This causes a local reset on the GMBUS controller.

  @retval  EFI_SUCCESS                    - The GMBUS error was successfully cleared.
  @retval  EFI_TIMEOUT                    - The GMBUS I2C controller did not respond within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the GMBUS I2C controller.
**/
EFI_STATUS
GmbusRecoverError (
  VOID
  );

/**
  Wait for a given bitmask of status bits to de-assert to zero.

  @param[in]  StatusBitMask               - A bitmask of status bits to be compared to the present value of GMBUS2
  @param[in]  WaitForAssertion            - If TRUE, the Status Bit indicated must be 1, otherwise it must be 0.

  @retval  EFI_SUCCESS                    - The GMBUS controller has cleared all of the bits in the bitmask.
  @retval  EFI_TIMEOUT                    - The GMBUS controller did not clear all of the bits in the bitmask
                                            within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the GMBUS I2C controller.
**/
EFI_STATUS
GmbusWaitForReady (
  IN  UINT32              StatusBitMask,
  IN  BOOLEAN             WaitForAssertion
  );

/**
  Initialize the GMBUS to use a given GPIO pin pair and clock speed in preparation
  for sending a I2C command to the GMBUS controller.

  @param[in]  BusSpeed                    - The clock rate for the I2C bus.
  @param[in]  DdcBusPinPair               - The GPIO pin pair the GMBUS controller should use.

  @retval  EFI_SUCCESS                    - The GMBUS has been initialized successfully.
  @retval  EFI_INVALID_PARAMETER          - The given BusSpeed does not match a valid clock rate value.
  @retval  EFI_TIMEOUT                    - The GMBUS I2C controller did not respond within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the GMBUS I2C controller.
**/
EFI_STATUS
GmbusPrepare (
  IN  UINT8             BusSpeed,
  IN  UINT8             DdcBusPinPair
  );

/**
  Release the GMBUS controller

  @retval  EFI_SUCCESS                    - The GMBUS has been released successfully.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the GMBUS I2C controller.
**/
EFI_STATUS
GmbusRelease (
  VOID
  );

/**
  Reads data from the I2C bus using the GMBUS I2C controller

  @param[in]  DdcBusPinPair               - The GPIO pin pair to use for the read operation
  @param[in]  SlaveAddress                - The I2C device address to read data from
  @param[in]  SendStopCondition           - TRUE:  After the read is complete, send a STOP condition to the I2C bus
                                            FALSE: Don't send a STOP after the read is complete, this allows one to
                                                   immediately send a repeated START condition to the I2C bus after
                                                   GmbusRead() exits by calling either GmbusRead() or GmbusWrite()
                                                   immediately after this function returns.
  @param[in]  SendIndexBeforeRead         - TRUE:  Before executing the read on the I2C bus, first send a WRITE to the
                                                   I2C bus using the same SlaveAddress (but with BIT0 set to 0 because
                                                   the operation is a write) the write will contain a single byte, that
                                                   byte is the data given in the IndexData parameter.
                                            FALSE: Just send a read to the I2C bus, the IndexData parameter is ignored.
  @param[in]  IndexData                   - If SendIndexBeforeRead is TRUE, this byte of data will be written to the I2C
                                            bus before the I2C bus is read. If SendIndexBeforeRead is FALSE, this
                                            parameter is ignored.
  @param[in, out]  ByteCount              - The number of bytes to read from the I2C bus. On output, the number of bytes
                                            actually read.
  @param[out] ReadBuffer                  - The memory buffer to return the read data.

  @retval  EFI_SUCCESS                    - The data was successfully read.
  @retval  EFI_INVALID_PARAMETER          - One of the following conditions:
                                            * ByteCount is 0 or >GMBUS_MAX_BYTES.
                                            * ReadBuffer is NULL
                                            * SlaveAddress does not have BIT0 set (required for reads.)
  @retval  EFI_TIMEOUT                    - The GMBUS I2C controller did not respond within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the GMBUS I2C controller.
**/
EFI_STATUS
GmbusRead (
  IN      UINT8         DdcBusPinPair,
  IN      UINT8         SlaveAddress,
  IN      BOOLEAN       SendStopCondition,
  IN      BOOLEAN       SendIndexBeforeRead,
  IN      UINT8         IndexData,
  IN OUT  UINT32        *ByteCount,
  OUT     UINT8         *ReadBuffer
  );

/**
  Writes data to the I2C bus using the GMBUS I2C controller

  @param[in]  DdcBusPinPair               - The GPIO pin pair to use for the write operation
  @param[in]  SlaveAddress                - The I2C device address to write data to
  @param[in]  SendStopCondition           - TRUE:  After the write is complete, send a STOP condition to the I2C bus
                                            FALSE: Don't send a STOP after the write is complete, this allows one to
                                                   immediately send a repeated START condition to the I2C bus after
                                                   GmbusRead() exits by calling either GmbusRead() or GmbusWrite()
                                                   immediately after this function returns.
  @param[in]  ByteCount                   - The number of bytes to write to the I2C bus.
  @param[in]  WriteBuffer                 - The data to be written to the I2C bus.

  @retval  EFI_SUCCESS                    - The data was successfully written.
  @retval  EFI_INVALID_PARAMETER          - One of the following conditions:
                                            * ByteCount is 0 or >GMBUS_MAX_BYTES.
                                            * WriteBuffer is NULL
                                            * SlaveAddress does not have BIT0 cleared (required for writes.)
  @retval  EFI_TIMEOUT                    - The GMBUS I2C controller did not respond within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the GMBUS I2C controller.
**/
EFI_STATUS
GmbusWrite (
  IN  UINT8             DdcBusPinPair,
  IN  UINT8             SlaveAddress,
  IN  BOOLEAN           SendStopCondition,
  IN  UINT32            ByteCount,
  IN  UINT8             *WriteBuffer
  );
