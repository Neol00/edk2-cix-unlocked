/** @file
  GMBUS I/O Implementation

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>

#include <IgfxI2c.h>
#include <Gmbus.h>

/**
  Gets the GttMmAdr BAR value

  @retval  The current value of the GTTMMADR BAR
**/
UINTN
GmbusGetGttMmAdr (
  VOID
  )
{
  UINTN                   GttMmPciAddress;
  UINT32                  GttMmAdr;

  //
  // Check if GTT Memory Mapped BAR has been already assigned, initialize if not
  //
  GttMmPciAddress = PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_SA_IGD_GTTMMADR);
  GttMmAdr = PciRead32 (GttMmPciAddress) & 0xFFFFFFF0;
  if (GttMmAdr == 0) {
    GttMmAdr = (UINT32) FixedPcdGet32 (PcdGttMmAddress);
    if (GttMmAdr == 0) {
      return 0;
    }
    //
    // Program and read back GTT Memory Mapped BAR
    //
    PciWrite32 (GttMmPciAddress, (UINT32) (GttMmAdr & 0xFF000000));
    GttMmAdr = PciRead32 (GttMmPciAddress) & 0xFFFFFFF0;
  }
  //
  // Check if Bus Master and Memory access on 0:2:0 is enabled, enable it if not
  //
  if ((PciRead16 (
        PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_SA_IGD_CMD)
        )
      & (BIT2 | BIT1)) != (BIT2 | BIT1)) {
    //
    // Enable Bus Master and Memory access on 0:2:0
    //
    PciOr16 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_SA_IGD_CMD), (BIT2 | BIT1));
    //
    // Set the Reset Bus Master flag so that it will be disabled when the current transaction is done
    //
    SetIgdBusMasterReset (TRUE);
  }

  return GttMmAdr;
}

/**
  Reset Bus Master and Memory access on the IGD device to the initial state at
  the start of the current transaction.
**/
VOID
GmbusResetBusMaster (
  VOID
  )
{
  if (GetIgdBusMasterReset ()) {
    //
    // Check if Bus Master and Memory access on 0:2:0 is enabled, disable it if so
    //
    if ((PciRead16 (
          PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_SA_IGD_CMD)
          )
        & (BIT2 | BIT1)) != 0) {
      //
      // Disable Bus Master and Memory access on 0:2:0
      //
      PciAnd16 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_SA_IGD_CMD), (UINT16) ~(BIT2 | BIT1));
    }
    //
    // Clear the Reset Bus Master flag
    //
    SetIgdBusMasterReset (FALSE);
  }
}

/**
  Writes to the GMBUS0 register (Clock/Port Select)

  @param[in]  GmbusClkPrtSel              - The value to write to GMBUS0

  @retval  EFI_SUCCESS                    - GMBUS0 was successfully written.
  @retval  EFI_DEVICE_ERROR               - An error occurred while writting to GMBUS0
**/
EFI_STATUS
SetGmbus0ClockPortSelect (
  IN  UINT32             GmbusClkPrtSel
  )
{
  UINTN                   GttMmAdr;

  GttMmAdr = GmbusGetGttMmAdr ();
  MmioWrite32 (GttMmAdr + R_SA_GTTMMADR_GMBUS0_CLKPRTSEL, GmbusClkPrtSel);
  return EFI_SUCCESS;
}

/**
  Writes to the GMBUS1 register (Command/Status)

  @param[in]  GmbusCmdSts                 - The value to write to GMBUS1

  @retval  EFI_SUCCESS                    - GMBUS1 was successfully written.
  @retval  EFI_DEVICE_ERROR               - An error occurred while writting to GMBUS1
**/
EFI_STATUS
SetGmbus1Command (
  IN  UINT32             GmbusCmdSts
  )
{
  UINTN                   GttMmAdr;

  GttMmAdr = GmbusGetGttMmAdr ();
  MmioWrite32 (GttMmAdr + R_SA_GTTMMADR_GMBUS1_CMDSTS, GmbusCmdSts);
  return EFI_SUCCESS;
}

/**
  Reads from the GMBUS2 register (GMBUS Status)

  @param[out]  GmbusStatus                - The value read from GMBUS2

  @retval  EFI_SUCCESS                    - GMBUS2 was successfully read.
  @retval  EFI_DEVICE_ERROR               - An error occurred while reading from GMBUS2
**/
EFI_STATUS
GetGmbus2Status (
  OUT  UINT32             *GmbusStatus
  )
{
  UINTN                   GttMmAdr;

  GttMmAdr = GmbusGetGttMmAdr ();
  if (GttMmAdr == 0) {
    return EFI_UNSUPPORTED;
  }
  *GmbusStatus = MmioRead32 (GttMmAdr + R_SA_GTTMMADR_GMBUS2_STATUS);
  return EFI_SUCCESS;
}

/**
  Writes to the GMBUS2 register (GMBUS Status)

  @param[in]  GmbusStatus                 - The value to write to GMBUS2

  @retval  EFI_SUCCESS                    - GMBUS2 was successfully written.
  @retval  EFI_DEVICE_ERROR               - An error occurred while writting to GMBUS2
**/
EFI_STATUS
SetGmbus2Status (
  IN  UINT32             GmbusStatus
  )
{
  UINTN                   GttMmAdr;

  GttMmAdr = GmbusGetGttMmAdr ();
  if (GttMmAdr == 0) {
    return EFI_UNSUPPORTED;
  }
  MmioWrite32 (GttMmAdr + R_SA_GTTMMADR_GMBUS2_STATUS, GmbusStatus);
  return EFI_SUCCESS;
}

/**
  Reads from the GMBUS3 register (GMBUS Data Buffer)

  @param[out]  GmbusData                  - The value read from GMBUS3

  @retval  EFI_SUCCESS                    - GMBUS2 was successfully read.
  @retval  EFI_DEVICE_ERROR               - An error occurred while reading from GMBUS2
**/
EFI_STATUS
GetGmbus3Data (
  OUT  UINT32             *GmbusData
  )
{
  UINTN                   GttMmAdr;

  GttMmAdr = GmbusGetGttMmAdr ();
  if (GttMmAdr == 0) {
    return EFI_UNSUPPORTED;
  }
  *GmbusData = MmioRead32 (GttMmAdr + R_SA_GTTMMADR_GMBUS3_DATA);
  return EFI_SUCCESS;
}

/**
  Writes to the GMBUS3 register (GMBUS Data Buffer)

  @param[in]  GmbusData                   - The value to write to GMBUS3

  @retval  EFI_SUCCESS                    - GMBUS3 was successfully written.
  @retval  EFI_DEVICE_ERROR               - An error occurred while writting to GMBUS3
**/
EFI_STATUS
SetGmbus3Data (
  IN  UINT32             GmbusData
  )
{
  UINTN                   GttMmAdr;

  GttMmAdr = GmbusGetGttMmAdr ();
  if (GttMmAdr == 0) {
    return EFI_UNSUPPORTED;
  }
  MmioWrite32 (GttMmAdr + R_SA_GTTMMADR_GMBUS3_DATA, GmbusData);
  return EFI_SUCCESS;
}

/**
  Set and clear the software clear interrupt bit. This causes a local reset on the GMBUS controller.

  @retval  EFI_SUCCESS                    - The GMBUS error was successfully cleared.
  @retval  EFI_TIMEOUT                    - The GMBUS I2C controller did not respond within the required timeout period.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the GMBUS I2C controller.
**/
EFI_STATUS
GmbusRecoverError (
  VOID
  )
{
  EFI_STATUS              Status;

  //
  // Setting B_SA_GTTMMADR_GMBUS1_SW_CLR_INT
  // causes a local reset on the GMBUS controller
  //
  Status = SetGmbus1Command ((UINT32) B_SA_GTTMMADR_GMBUS1_SW_CLR_INT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = SetGmbus1Command (0);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Wait for reset to complete
  //
  Status = GmbusWaitForReady (B_SA_GTTMMADR_GMBUS2_BUS_ACTIVE, FALSE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

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
  )
{
  EFI_STATUS              Status;
  EFI_STATUS              Status2;
  UINTN                   Index;
  UINT32                  GmbusStatus;

  Status = EFI_TIMEOUT;
  for (Index = 0; Index < GMBUS_TIMEOUT; Index++) {
    Status2 = GetGmbus2Status (&GmbusStatus);
    if (EFI_ERROR (Status2)) {
      return Status2;
    }
    if (WaitForAssertion) {
      if (GmbusStatus & StatusBitMask) {
        Status = EFI_SUCCESS;
        break;
      }
    } else {
      if (!(GmbusStatus & StatusBitMask)) {
        Status = EFI_SUCCESS;
        break;
      }
    }
  }
  return Status;
}

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
  )
{
  EFI_STATUS              Status;
  UINT32                  GmbusClkPrtSel;
  UINT32                  GmbusStatus;

  //
  // Check that the user provided a valid bus speed
  //
  if ((BusSpeed != GMBUS_CLOCK_RATE_100K) && (BusSpeed != GMBUS_CLOCK_RATE_50K) &&
      (BusSpeed != GMBUS_CLOCK_RATE_400K) && (BusSpeed != GMBUS_CLOCK_RATE_1M)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Wait for GMBUS to complete any pending commands
  //
  Status = GmbusWaitForReady (B_SA_GTTMMADR_GMBUS2_INUSE, FALSE);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Program the GMBUS Port and Clock
  //
  GmbusClkPrtSel = (BusSpeed << 8) | DdcBusPinPair;
  Status = SetGmbus0ClockPortSelect (GmbusClkPrtSel);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check for a NACK that has not been cleared yet. Clear it if found.
  //
  Status = GetGmbus2Status (&GmbusStatus);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (GmbusStatus & B_SA_GTTMMADR_GMBUS2_NACK_INDICATOR) {
    Status = GmbusRecoverError ();
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return Status;
}

/**
  Release the GMBUS controller

  @retval  EFI_SUCCESS                    - The GMBUS has been released successfully.
  @retval  EFI_DEVICE_ERROR               - An error occurred while accessing the GMBUS I2C controller.
**/
EFI_STATUS
GmbusRelease (
  VOID
  )
{
  EFI_STATUS              Status;

  //
  // Clear the GMBUS Port and Clock
  //
  Status = SetGmbus0ClockPortSelect (0);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Setting the INUSE bit to 1 indicates that software has released the GMBUS resource.
  // The GMBUS controller will then reset the INUSE bit to 0.
  //
  Status = SetGmbus2Status (B_SA_GTTMMADR_GMBUS2_INUSE);
  if (EFI_ERROR (Status)) {
  }

  return Status;
}

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
  )
{
  EFI_STATUS              Status;
  EFI_STATUS              Status2;
  UINT32                  Index;
  UINT32                  GmbusCmdSts;
  UINT32                  GmbusStatus;
  UINT32                  GmbusData;
  UINT32                  BytesRead;

  Status      = EFI_SUCCESS;
  BytesRead   = 0;
  GmbusStatus = 0;

  //
  // Input Validation
  //
  if ((*ByteCount) <= 0) {
    return EFI_INVALID_PARAMETER;
  }
  if ((*ByteCount) > GMBUS_MAX_BYTES) {
    return EFI_INVALID_PARAMETER;
  }
  if (ReadBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if ((SlaveAddress & BIT0) != BIT0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Configure Gmbus port and clock speed
  //
  Status = GmbusPrepare (GMBUS_CLOCK_RATE_50K, (DdcBusPinPair & B_SA_GTTMMADR_GMBUS0_PIN_PAIR_MASK));
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Construct the command for the GMBUS controller
  //
  GmbusCmdSts     = ((UINT32) SlaveAddress)                   |
                    B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_START  |
                    B_SA_GTTMMADR_GMBUS1_SW_RDY               |
                    B_SA_GTTMMADR_GMBUS1_EN_TIMEOUT;
  GmbusCmdSts    |= (((*ByteCount) << N_SA_GTTMMADR_GMBUS1_TOTAL_BYTE_COUNT) & B_SA_GTTMMADR_GMBUS1_TOTAL_BYTE_COUNT_MASK);
  if (SendStopCondition) {
    GmbusCmdSts  |= B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_STOP;
  }
  if (SendIndexBeforeRead) {
    GmbusCmdSts  |= B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_INDEX;
    GmbusCmdSts  |= ((IndexData << N_SA_GTTMMADR_GMBUS1_INDEX) & B_SA_GTTMMADR_GMBUS1_INDEX_MASK);
  }

  //
  // Send the command to the GMBUS controller, this will cause the I2C transaction to begin immediately
  //
  Status = SetGmbus1Command (GmbusCmdSts);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Read the data from the GMBUS controller as it arrives
  //
  while (BytesRead < (*ByteCount)) {
    //
    // Wait for the GMBUS controller to set the HW_RDY bit to 1
    //
    // The HW_RDY bit is set under the following conditions:
    //
    // * After a reset
    // * When a transaction is aborted by the setting the SW_CLR_INT bit in the GMBUS1 register
    // * When an active GMBUS cycle has terminated with a STOP condition
    // * During a GMBUS write transaction, when the data register can accept another four bytes of data
    // * During a GMBUS read transaction, when the data register has four bytes of new data or when the read
    //   transaction DATA phase is complete and the data register contains all remaining data.
    //
    Status = GmbusWaitForReady (B_SA_GTTMMADR_GMBUS2_HW_RDY, TRUE);
    //
    // Check the GMBUS2 register for error conditions (NACK or Slave Stall Timeout)
    //
    Status2 = GetGmbus2Status (&GmbusStatus);
    if (EFI_ERROR (Status2)) {
      Status = Status2;
      goto Done;
    }
    if (EFI_ERROR (Status) && ((GmbusStatus & B_SA_GTTMMADR_GMBUS2_NACK_INDICATOR) == 0)) {
      Status = EFI_DEVICE_ERROR;
      goto Done;
    }
    if (((GmbusStatus & B_SA_GTTMMADR_GMBUS2_NACK_INDICATOR) != 0) ||
        ((GmbusStatus & B_SA_GTTMMADR_GMBUS2_SLAVE_STALL_TIMEOUT_ERROR) != 0)) {
      //
      // If a NACK or Slave Stall Timeout occurs, then a bus error has occurred.
      // In the event of a bus error, one must reset the GMBUS controller to resume normal operation.
      //
      Status = GmbusRecoverError ();
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      Status = EFI_DEVICE_ERROR;
      goto Done;
    }
    //
    // No error conditions were encountered, read the data and write it to the data buffer
    //
    Status = GetGmbus3Data (&GmbusData);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    for (Index = 0; (Index < sizeof (UINT32)) && (BytesRead < (*ByteCount)); Index++) {
      ReadBuffer[BytesRead] = (GmbusData >> (Index * 8)) & 0xFF;
      BytesRead++;
    }
  }

  //
  // Wait for the GMBUS controller to enter the IDLE state
  //
  Status = GmbusWaitForReady (B_SA_GTTMMADR_GMBUS2_BUS_ACTIVE, FALSE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

Done:
  Status2 = GmbusRelease ();
  if (EFI_ERROR (Status2)) {
    Status = Status2;
  }
  GmbusResetBusMaster ();

  (*ByteCount) = BytesRead;
  return Status;
}

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
  )
{
  EFI_STATUS              Status;
  EFI_STATUS              Status2;
  UINT32                  Index;
  UINT32                  GmbusCmdSts;
  UINT32                  GmbusStatus;
  UINT32                  GmbusData;
  UINT32                  BytesWritten;
  BOOLEAN                 FirstLoop;

  Status        = EFI_SUCCESS;
  BytesWritten  = 0;
  GmbusStatus   = 0;
  FirstLoop     = TRUE;

  //
  // Input Validation
  //
  if (ByteCount <= 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (ByteCount > GMBUS_MAX_BYTES) {
    return EFI_INVALID_PARAMETER;
  }
  if (WriteBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if ((SlaveAddress & BIT0) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Configure Gmbus port and clock speed
  Status = GmbusPrepare (GMBUS_CLOCK_RATE_50K, (DdcBusPinPair & B_SA_GTTMMADR_GMBUS0_PIN_PAIR_MASK));
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Construct the command for the GMBUS controller
  //
  GmbusCmdSts     = ((UINT32) SlaveAddress)                   |
                    B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_START  |
                    B_SA_GTTMMADR_GMBUS1_SW_RDY               |
                    B_SA_GTTMMADR_GMBUS1_EN_TIMEOUT;
  GmbusCmdSts    |= ((ByteCount << N_SA_GTTMMADR_GMBUS1_TOTAL_BYTE_COUNT) & B_SA_GTTMMADR_GMBUS1_TOTAL_BYTE_COUNT_MASK);
  if (SendStopCondition) {
    GmbusCmdSts  |= B_SA_GTTMMADR_GMBUS1_BUS_CYCLE_SEL_STOP;
  }

  //
  // Preload the first 4 bytes of data so that when we send the command to the GMBUS
  // controller the first 4 bytes of data are ready for transmission. The GMBUS controller requires this.
  //
  GmbusData = 0;
  for (Index = 0; (Index < sizeof (UINT32)) && (BytesWritten < ByteCount); Index++) {
    GmbusData |= (WriteBuffer[BytesWritten] << (Index * 8));
    BytesWritten++;
  }
  Status = SetGmbus3Data (GmbusData);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Send the command to the GMBUS controller, this will cause the I2C transaction to begin immediately
  //
  Status = SetGmbus1Command (GmbusCmdSts);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  while ((BytesWritten < ByteCount) || FirstLoop) {
    //
    // If this is not the first loop, load the next 4 bytes of data into the
    // GMBUS controller's data buffer.
    //
    if(!FirstLoop) {
      GmbusData = 0;
      for (Index = 0; (Index < sizeof (UINT32)) && (BytesWritten < ByteCount); Index++) {
        GmbusData |= (WriteBuffer[BytesWritten] << (Index * 8));
        BytesWritten++;
      }
      Status = SetGmbus3Data (GmbusData);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
    }
    FirstLoop = FALSE;

    //
    // Wait for the GMBUS controller to set the HW_RDY bit to 1
    //
    // The HW_RDY bit is set under the following conditions:
    //
    // * After a reset
    // * When a transaction is aborted by the setting the SW_CLR_INT bit in the GMBUS1 register
    // * When an active GMBUS cycle has terminated with a STOP condition
    // * During a GMBUS write transaction, when the data register can accept another four bytes of data
    // * During a GMBUS read transaction, when the data register has four bytes of new data or when the read
    //   transaction DATA phase is complete and the data register contains all remaining data.
    //
    Status = GmbusWaitForReady (B_SA_GTTMMADR_GMBUS2_HW_RDY, TRUE);
    if (EFI_ERROR (Status)) {
    }
    //
    // Check the GMBUS2 register for error conditions (NACK or Slave Stall Timeout)
    //
    Status2 = GetGmbus2Status (&GmbusStatus);
    if (EFI_ERROR (Status2)) {
      Status = Status2;
      goto Done;
    }
    if (EFI_ERROR (Status) && ((GmbusStatus & B_SA_GTTMMADR_GMBUS2_NACK_INDICATOR) == 0)) {
      Status = EFI_DEVICE_ERROR;
      goto Done;
    }
    if (((GmbusStatus & B_SA_GTTMMADR_GMBUS2_NACK_INDICATOR) != 0) ||
        ((GmbusStatus & B_SA_GTTMMADR_GMBUS2_SLAVE_STALL_TIMEOUT_ERROR) != 0)) {
      //
      // If a NACK or Slave Stall Timeout occurs, then a bus error has occurred.
      // In the event of a bus error, one must reset the GMBUS controller to resume normal operation.
      //
      Status = GmbusRecoverError ();
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      Status = EFI_DEVICE_ERROR;
      goto Done;
    }
  }

  //
  // Wait for the GMBUS controller to enter the IDLE state
  //
  Status = GmbusWaitForReady (B_SA_GTTMMADR_GMBUS2_BUS_ACTIVE, FALSE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

Done:
  Status2 = GmbusRelease ();
  if (EFI_ERROR (Status2)) {
    Status = Status2;
  }
  GmbusResetBusMaster ();

  return Status;
}
