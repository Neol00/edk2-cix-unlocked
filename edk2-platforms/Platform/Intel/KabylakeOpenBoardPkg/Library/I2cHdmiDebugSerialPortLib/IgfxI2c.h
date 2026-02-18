/** @file
  Intel Graphics I2C Bus I/O

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
**/

#include <Uefi/UefiBaseType.h>
#include <Library/HdmiDebugPchDetectionLib.h>

//
// HDMI DDC Pin Pairs
//
#define V_CNL_PCH_HDMI_DDC_B_PIN_PAIR                     0x01
#define V_CNL_PCH_HDMI_DDC_C_PIN_PAIR                     0x02
#define V_CNL_PCH_HDMI_DDC_D_PIN_PAIR                     0x04
#define V_CNL_PCH_HDMI_DDC_F_PIN_PAIR                     0x03

#define V_KBL_PCH_HDMI_DDC_B_PIN_PAIR                     0x05
#define V_KBL_PCH_HDMI_DDC_C_PIN_PAIR                     0x04
#define V_KBL_PCH_HDMI_DDC_D_PIN_PAIR                     0x06

typedef enum {
  EnumDdcUnknown = 0,
  EnumDdcA,
  EnumDdcB,
  EnumDdcC,
  EnumDdcD,
  EnumDdcE,
  EnumDdcF,
  EnumI2cChannelMax
} IGFX_I2C_CHANNEL;

/**
  Returns the type of PCH on the system

  @retval   The PCH type.
**/
PCH_TYPE
GetPchType (
  VOID
  );

/**
  Returns the GPIO pin pair to use for the given DDC channel

  @param[in]  Channel                     - The DDC I2C channel.
  @param[out] DdcBusPinPair               - The GPIO pin pair for the given DDC channel.

  @retval  EFI_SUCCESS                    - The GPIO pin pair was successfully determined
  @retval  EFI_INVALID_PARAMETER          - The given DDC I2C channel does not exist.
  @retval  EFI_UNSUPPORTED                - The platform is using a PCH that is not supported yet.
**/
EFI_STATUS
GetGmbusBusPinPair (
  IN  IGFX_I2C_CHANNEL  Channel,
  OUT UINT8             *DdcBusPinPair
  );

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
  );

/**
  For boot phases that utilize task priority levels (TPLs), this function raises
  the TPL to the appriopriate level needed to execute I/O to the I2C Debug Port
**/
VOID
RaiseTplForI2cDebugPortAccess (
  VOID
  );

/**
  For boot phases that utilize task priority levels (TPLs), this function
  restores the TPL to the previous level after I/O to the I2C Debug Port is
  complete
**/
VOID
RestoreTplAfterI2cDebugPortAccess (
  VOID
  );
