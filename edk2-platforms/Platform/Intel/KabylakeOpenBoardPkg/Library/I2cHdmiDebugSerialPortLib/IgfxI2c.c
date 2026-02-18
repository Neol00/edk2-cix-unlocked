/** @file
  Intel Graphics I2C Bus I/O

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <IgfxI2c.h>

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
  )
{
  PCH_TYPE  PchType;
  *DdcBusPinPair = 0;

  PchType = GetPchType ();
  switch (PchType) {
    // The PCH design lineage from SkyLake, KabyLake, AmberLake, & early CoffeeLake
    case PchTypeSptLp:
    case PchTypeSptH:
    case PchTypeKbpH:
      switch (Channel) {
        case EnumDdcB:
          *DdcBusPinPair = V_KBL_PCH_HDMI_DDC_B_PIN_PAIR;
          return EFI_SUCCESS;
        case EnumDdcC:
          *DdcBusPinPair = V_KBL_PCH_HDMI_DDC_C_PIN_PAIR;
          return EFI_SUCCESS;
        case EnumDdcD:
          *DdcBusPinPair = V_KBL_PCH_HDMI_DDC_D_PIN_PAIR;
          return EFI_SUCCESS;

        default:
          return EFI_INVALID_PARAMETER;
      }
      break;
    // The PCH design lineage from newer CoffeeLake & WhiskeyLake
    case PchTypeCnlLp:
    case PchTypeCnlH:
      switch (Channel) {
        case EnumDdcB:
          *DdcBusPinPair = V_CNL_PCH_HDMI_DDC_B_PIN_PAIR;
          return EFI_SUCCESS;
        case EnumDdcC:
          *DdcBusPinPair = V_CNL_PCH_HDMI_DDC_C_PIN_PAIR;
          return EFI_SUCCESS;
        case EnumDdcD:
          *DdcBusPinPair = V_CNL_PCH_HDMI_DDC_D_PIN_PAIR;
          return EFI_SUCCESS;
        case EnumDdcF:
          *DdcBusPinPair = V_CNL_PCH_HDMI_DDC_F_PIN_PAIR;
          return EFI_SUCCESS;

        default:
          return EFI_INVALID_PARAMETER;
      }
      break;
  }

  return EFI_UNSUPPORTED;
}
