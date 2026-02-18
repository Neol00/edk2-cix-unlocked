/** @file
  Serial I/O Port library implementation for the HDMI I2C Debug Port
  Generic Base Library implementation

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>

#include <IgfxI2c.h>
#include <I2cDebugPortProtocol.h>

/**
  Sets the control bits on a serial device.

  @param Control                Sets the bits of Control that are settable.

  @retval RETURN_SUCCESS        The new control bits were set on the serial device.
  @retval RETURN_UNSUPPORTED    The serial device does not support this operation.
  @retval RETURN_DEVICE_ERROR   The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortSetControl (
  IN UINT32 Control
  )
{
  //
  // check for invalid control parameters
  //
  if ((Control & (~(EFI_SERIAL_REQUEST_TO_SEND          |
                    EFI_SERIAL_DATA_TERMINAL_READY      |
                    EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE |
                    EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE |
                    EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE))) != 0 ) {
    return EFI_UNSUPPORTED;
  }
  return EFI_SUCCESS;
}

/**
  Retrieve the status of the control bits on a serial device.

  @param Control                A pointer to return the current control signals from the serial device.

  @retval RETURN_SUCCESS        The control bits were read from the serial device.
  @retval RETURN_UNSUPPORTED    The serial device does not support this operation.
  @retval RETURN_DEVICE_ERROR   The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortGetControl (
  OUT UINT32 *Control
  )
{
  EFI_STATUS  Status;
  UINT8       NumberOfBytesInFifoBuffer;

  Status = I2cDebugPortReadyToRead (&NumberOfBytesInFifoBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *Control = (EFI_SERIAL_CLEAR_TO_SEND | EFI_SERIAL_DATA_SET_READY |
              EFI_SERIAL_CARRIER_DETECT | EFI_SERIAL_OUTPUT_BUFFER_EMPTY);
  if (NumberOfBytesInFifoBuffer <= 0) {
    *Control |= EFI_SERIAL_INPUT_BUFFER_EMPTY;
  }
  return Status;
}

/**
  Returns the type of PCH on the system

  @retval   The PCH type.
**/
PCH_TYPE
GetPchType (
  VOID
  )
{
  return GetPchTypeInternal ();
}

/**
  Returns the GPIO pin pair to use for the I2C HDMI debug port

  @param[out] DdcBusPinPair               - The GPIO pin pair for the I2C HDMI debug port.

  @retval  EFI_SUCCESS                    - The GPIO pin pair was successfully determined
  @retval  EFI_INVALID_PARAMETER          - The given DDC I2C channel does not exist.
  @retval  EFI_UNSUPPORTED                - The platform is using a PCH that is not supported yet.
**/
EFI_STATUS
GetGmbusBusPinPairForI2cDebugPort (
  OUT UINT8             *DdcBusPinPair
  )
{
  return GetGmbusBusPinPair ((IGFX_I2C_CHANNEL) FixedPcdGet32 (PcdI2cHdmiDebugPortDdcI2cChannel), DdcBusPinPair);
}

/**
  Returns a flag indicating whether the IGD device bus master enable needs to
  be disabled at the end of the current transaction

  @retval  TRUE                           - IGD Bus Master Enable needs to be reset
  @retval  FALSE                          - IGD Bus Master Enable does not need to be reset
**/
BOOLEAN
GetIgdBusMasterReset (
  VOID
  )
{
  return TRUE;
}

/**
  Sets a flag indicating whether the IGD device bus master enable needs to
  be disabled at the end of the current transaction

  @param[in]  IgdBusMasterReset           - IGD device bus master enable flag
**/
VOID
SetIgdBusMasterReset (
  BOOLEAN IgdBusMasterReset
  )
{
}
