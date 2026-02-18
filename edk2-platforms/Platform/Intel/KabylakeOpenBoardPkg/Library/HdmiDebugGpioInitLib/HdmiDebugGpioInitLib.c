/** @file
  GPIO initialization for the HDMI I2C Debug Port

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>

#include <Library/GpioLib.h>
#include <Library/HdmiDebugPchDetectionLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>

//GPIO Table Terminator
#define END_OF_GPIO_TABLE 0xFFFFFFFF

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

/*** SKL-LP ***/

// HDMI-B DDC GPIO Pins
GPIO_INIT_CONFIG mDebugGpioTableSklLpDdpB[] =
{
  {GPIO_SKL_LP_GPP_E18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPB_CTRLCLK
  {GPIO_SKL_LP_GPP_E19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPB_CTRLDATA
  {END_OF_GPIO_TABLE,   {GpioPadModeGpio,    GpioHostOwnGpio,    GpioDirNone,    GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};
UINT16 mDebugGpioTableSklLpDdpBSize = sizeof (mDebugGpioTableSklLpDdpB) / sizeof (GPIO_INIT_CONFIG) - 1;

// HDMI-C DDC GPIO Pins
GPIO_INIT_CONFIG mDebugGpioTableSklLpDdpC[] =
{
  {GPIO_SKL_LP_GPP_E20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPC_CTRLCLK
  {GPIO_SKL_LP_GPP_E21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPC_CTRLDATA
  {END_OF_GPIO_TABLE,   {GpioPadModeGpio,    GpioHostOwnGpio,    GpioDirNone,    GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};
UINT16 mDebugGpioTableSklLpDdpCSize = sizeof (mDebugGpioTableSklLpDdpC) / sizeof (GPIO_INIT_CONFIG) - 1;

// HDMI-D DDC GPIO Pins
GPIO_INIT_CONFIG mDebugGpioTableSklLpDdpD[] =
{
  {GPIO_SKL_LP_GPP_E22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPD_CTRLCLK
  {GPIO_SKL_LP_GPP_E23, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPD_CTRLDATA
  {END_OF_GPIO_TABLE,   {GpioPadModeGpio,    GpioHostOwnGpio,    GpioDirNone,    GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};
UINT16 mDebugGpioTableSklLpDdpDSize = sizeof (mDebugGpioTableSklLpDdpD) / sizeof (GPIO_INIT_CONFIG) - 1;

/*** SKL-H ***/

// HDMI-B DDC GPIO Pins
GPIO_INIT_CONFIG mDebugGpioTableSklHDdpB[] =
{
  {GPIO_SKL_H_GPP_I5,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPB_CTRLCLK
  {GPIO_SKL_H_GPP_I6,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPB_CTRLDATA
  {END_OF_GPIO_TABLE,   {GpioPadModeGpio,    GpioHostOwnGpio,    GpioDirNone,    GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};
UINT16 mDebugGpioTableSklHDdpBSize = sizeof (mDebugGpioTableSklHDdpB) / sizeof (GPIO_INIT_CONFIG) - 1;

// HDMI-C DDC GPIO Pins
GPIO_INIT_CONFIG mDebugGpioTableSklHDdpC[] =
{
  {GPIO_SKL_H_GPP_I7,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPC_CTRLCLK
  {GPIO_SKL_H_GPP_I8,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPC_CTRLDATA
  {END_OF_GPIO_TABLE,   {GpioPadModeGpio,    GpioHostOwnGpio,    GpioDirNone,    GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};
UINT16 mDebugGpioTableSklHDdpCSize = sizeof (mDebugGpioTableSklHDdpC) / sizeof (GPIO_INIT_CONFIG) - 1;

// HDMI-D DDC GPIO Pins
GPIO_INIT_CONFIG mDebugGpioTableSklHDdpD[] =
{
  {GPIO_SKL_H_GPP_I9,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPD_CTRLCLK
  {GPIO_SKL_H_GPP_I10,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}}, //DDPD_CTRLDATA
  {END_OF_GPIO_TABLE,   {GpioPadModeGpio,    GpioHostOwnGpio,    GpioDirNone,    GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};
UINT16 mDebugGpioTableSklHDdpDSize = sizeof (mDebugGpioTableSklHDdpD) / sizeof (GPIO_INIT_CONFIG) - 1;

/**
  Configures GPIO

  @param[in]  GpioTable       Point to Platform Gpio table
  @param[in]  GpioTableCount  Number of Gpio table entries

**/
VOID
HdmiDebugConfigureGpio (
  IN GPIO_INIT_CONFIG                 *GpioDefinition,
  IN UINT16                           GpioTableCount
  )
{
  EFI_STATUS          Status;

  Status = GpioConfigurePads (GpioTableCount, GpioDefinition);

}

/**
  Configures GPIOs to enable usage of the HDMI DDC I2C Bus

  @retval EFI_SUCCESS        The function completed successfully
  @retval EFI_UNSUPPORTED    The platform is using a PCH that is not supported yet.

**/
EFI_STATUS
HdmiDebugGpioInit (
  VOID
  )
{
  IGFX_I2C_CHANNEL  Channel;
  PCH_TYPE  PchType;

  PchType = GetPchTypeInternal ();
  Channel = (IGFX_I2C_CHANNEL) PcdGet32 (PcdI2cHdmiDebugPortDdcI2cChannel);
  switch (PchType) {
    case PchTypeSptLp:
      switch (Channel) {
        case EnumDdcB:
          HdmiDebugConfigureGpio (mDebugGpioTableSklLpDdpB, mDebugGpioTableSklLpDdpBSize);
          return EFI_SUCCESS;
        case EnumDdcC:
          HdmiDebugConfigureGpio (mDebugGpioTableSklLpDdpC, mDebugGpioTableSklLpDdpCSize);
          return EFI_SUCCESS;
        case EnumDdcD:
          HdmiDebugConfigureGpio (mDebugGpioTableSklLpDdpD, mDebugGpioTableSklLpDdpDSize);
          return EFI_SUCCESS;

        default:
          return EFI_UNSUPPORTED;
      }
      break;
    case PchTypeSptH:
    case PchTypeKbpH:
      switch (Channel) {
        case EnumDdcB:
          HdmiDebugConfigureGpio (mDebugGpioTableSklHDdpB, mDebugGpioTableSklHDdpBSize);
          return EFI_SUCCESS;
        case EnumDdcC:
          HdmiDebugConfigureGpio (mDebugGpioTableSklHDdpC, mDebugGpioTableSklHDdpCSize);
          return EFI_SUCCESS;
        case EnumDdcD:
          HdmiDebugConfigureGpio (mDebugGpioTableSklHDdpD, mDebugGpioTableSklHDdpDSize);
          return EFI_SUCCESS;

        default:
          return EFI_UNSUPPORTED;
      }
      break;
    default:
      return EFI_UNSUPPORTED;
  }
}
