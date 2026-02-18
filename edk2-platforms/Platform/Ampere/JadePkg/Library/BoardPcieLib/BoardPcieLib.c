/** @file
  Pcie board specific driver to handle asserting PERST signal to Endpoint
  card. PERST asserting is via group of GPIO pins to CPLD as Platform Specification.

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <Guid/RootComplexInfoHob.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/TimerLib.h>
#include <Platform/Ac01.h>

#define RCA_MAX_PERST_GROUPVAL          62
#define RCB_MAX_PERST_GROUPVAL          46
#define DEFAULT_SEGMENT_NUMBER          0x0F

VOID
BoardPcieReleaseAllPerst (
  IN UINT8 SocketId
  )
{
  UINT32 GpioIndex, GpioPin;

  // Write 1 to all GPIO[16..21] to release all PERST
  GpioPin = AC01_GPIO_PINS_PER_SOCKET * SocketId + 16;
  for (GpioIndex = 0; GpioIndex < 6; GpioIndex++) {
    GpioModeConfig (GpioPin + GpioIndex, GpioConfigOutHigh);
  }
}

/**
  Assert PERST of PCIe controller

  @param[in]  RootComplex           Root Complex instance.
  @param[in]  PcieIndex             PCIe controller index of input Root Complex.
  @param[in]  IsPullToHigh          Target status for the PERST.

  @retval RETURN_SUCCESS            The operation is successful.
  @retval Others                    An error occurred.
**/
RETURN_STATUS
EFIAPI
BoardPcieAssertPerst (
  IN AC01_ROOT_COMPLEX *RootComplex,
  IN UINT8             PcieIndex,
  IN BOOLEAN           IsPullToHigh
  )
{
  UINT32 GpioGroupVal, Val, GpioIndex, GpioPin;

  if (!IsPullToHigh) {
    if (RootComplex->Type == RootComplexTypeA) {
      //
      // RootComplexTypeA: RootComplex->ID: 0->3 ; PcieIndex: 0->3
      //
      GpioGroupVal = RCA_MAX_PERST_GROUPVAL - PcieIndex
                     - RootComplex->ID * MaxPcieControllerOfRootComplexA;
    } else {
      //
      // RootComplexTypeB: RootComplex->ID: 4->7 ; PcieIndex: 0->7
      //
      GpioGroupVal = RCB_MAX_PERST_GROUPVAL - PcieIndex
                     - (RootComplex->ID - MaxRootComplexA) * MaxPcieControllerOfRootComplexB;
    }

    // Update the value of GPIO[16..21]. Corresponding PERST line will be decoded by CPLD.
    GpioPin = AC01_GPIO_PINS_PER_SOCKET * RootComplex->Socket + 16;
    for (GpioIndex = 0; GpioIndex < 6; GpioIndex++) {
      Val = (GpioGroupVal & 0x3F) & (1 << GpioIndex);
      if (Val == 0) {
        GpioModeConfig (GpioPin + GpioIndex, GpioConfigOutLow);
      } else {
        GpioModeConfig (GpioPin + GpioIndex, GpioConfigOutHigh);
      }
    }

    // Keep reset as low as 100 ms as specification
    MicroSecondDelay (100 * 1000);
  } else {
    BoardPcieReleaseAllPerst (RootComplex->Socket);
  }

  return RETURN_SUCCESS;
}

/**
  Override the segment number for a root complex with a board specific number.

  @param[in]  RootComplex           Root Complex instance with properties.

  @retval Segment number corresponding to the input root complex.
          Default segment number is 0x0F.
**/
UINT16
BoardPcieGetSegmentNumber (
  IN  AC01_ROOT_COMPLEX *RootComplex
  )
{
  UINT8 Ac01BoardSegment[PLATFORM_CPU_MAX_SOCKET][AC01_PCIE_MAX_ROOT_COMPLEX] =
                          {
                            { 0x0C, 0x0D, 0x01, 0x00, 0x02, 0x03, 0x04, 0x05 },
                            { 0x10, 0x11, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B }
                          };

  if (RootComplex->Socket < PLATFORM_CPU_MAX_SOCKET
      && RootComplex->ID < AC01_PCIE_MAX_ROOT_COMPLEX) {
    return Ac01BoardSegment[RootComplex->Socket][RootComplex->ID];
  }

  return DEFAULT_SEGMENT_NUMBER;
}
