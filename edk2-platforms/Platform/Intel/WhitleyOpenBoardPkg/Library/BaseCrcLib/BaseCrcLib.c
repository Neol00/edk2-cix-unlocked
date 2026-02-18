/** @file
  Base implementation of the CRC library class.

  @copyright
  Copyright 2016 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Library/CrcLib.h>

/**
  Calculate a 16-bit CRC.

  The algorithm used is MSB-first form of the ITU-T Recommendation V.41, which
  uses an initial value of 0x0000 and a polynomial of 0x1021. It is the same
  algorithm used by XMODEM.

  The output CRC location is not updated until the calculation is finished, so
  it is possible to pass a structure as the data, and the CRC field of the same
  structure as the output location for the calculated CRC. The CRC field should
  be set to zero before calling this function. Once the CRC field is updated by
  this function, running it again over the structure produces a CRC of zero.

  @param[in]  Data              A pointer to the target data.
  @param[in]  DataSize          The target data size.
  @param[out] CrcOut            A pointer to the return location of the CRC.

  @retval EFI_SUCCESS           The CRC was calculated successfully.
  @retval EFI_INVALID_PARAMETER A null pointer was provided.
**/
EFI_STATUS
CalculateCrc16 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT16  *CrcOut
  )
{
  UINT32  Crc;
  UINTN   Index;
  UINT8   *Byte;

  if (Data == NULL || CrcOut == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Crc = 0x0000;
  for (Byte = (UINT8 *) Data; Byte < (UINT8 *) Data + DataSize; Byte++) {
    //
    // XOR the next data byte into the CRC.
    //
    Crc ^= (UINT16) *Byte << 8;
    //
    // Shift out eight bits, feeding back based on the polynomial whenever a
    // 1 is shifted out of bit 15.
    //
    for (Index = 0; Index < 8; Index++) {
      Crc <<= 1;
      if (Crc & BIT16) {
        Crc ^= 0x1021;
      }
    }
  }

  //
  // Mask and return the 16-bit CRC.
  //
  *CrcOut = (UINT16) (Crc & 0xFFFF);
  return EFI_SUCCESS;
}
